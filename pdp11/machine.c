/*
 * machine.c -- PDP-11/34A system assembly for ll-34
 */

#define _DEFAULT_SOURCE   /* clock_gettime on musl/glibc */

#include <string.h>
#include <time.h>
#include "machine.h"
#include "kd11ea/int.h"
#include "kd11ea/mmu.h"

#define PSW_ADDR    0x3FFFE  /* 777776 */
#define SWREG_ADDR  0x3FF78  /* 777570 */

static int psw_read(void *dev, uint32_t addr, uint16_t *data) {
    Machine *m = dev;
    (void)addr;
    *data = m->cpu.psw;
    return 0;
}

static int psw_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    Machine *m = dev;
    (void)addr;
    if (is_byte) {
        /* DATOB: low byte only */
        m->cpu.psw = (m->cpu.psw & 0xFF00) | (data & 0xFF);
    } else {
        m->cpu.psw = data;
    }
    return 0;
}

static int swreg_read(void *dev, uint32_t addr, uint16_t *data) {
    Machine *m = dev;
    (void)addr;
    *data = m->con.switch_reg;
    return 0;
}

static int swreg_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    Machine *m = dev;
    (void)addr; (void)is_byte;
    m->con.switch_reg = data;
    return 0;
}

static void cpu_irq_set(void *ctx, uint16_t vector, uint8_t priority) {
    KD11EA *cpu = ctx;
    int_request(&cpu->intc, vector, priority);
}

static void cpu_irq_clr(void *ctx, uint16_t vector) {
    KD11EA *cpu = ctx;
    int_cancel(&cpu->intc, vector);
}

int machine_init(Machine *m, const MachineConfig *cfg) {
    memset(m, 0, sizeof(*m));

    bus_init(&m->bus);
    ram_init(&m->ram, cfg->mem_kb * 512u);
    rom_init(&m->rom);
    dl11_init(&m->dl, 9600);
    rk05_init(&m->rk);
    rl11_init(&m->rl);
    kw11_init(&m->kw, 60);
    pc11_init(&m->pc);

    ram_register(&m->ram, &m->bus);
    rom_register(&m->rom, &m->bus);

    dl11_register(&m->dl, &m->bus);
    rk05_register(&m->rk, &m->bus);
    rl11_register(&m->rl, &m->bus);
    kw11_register(&m->kw, &m->bus);
    pc11_register(&m->pc, &m->bus);
    bus_register(&m->bus, PSW_ADDR, PSW_ADDR + 1,
                 m, psw_read, psw_write, "PSW", 100);
    bus_register(&m->bus, SWREG_ADDR, SWREG_ADDR + 1,
                 m, swreg_read, swreg_write, "SWITCH", 100);

    /* MMU registers */
    mmu_init(&m->cpu.mmu);
    bus_register(&m->bus, 0x3F480, 0x3F5FE,   /* Kernel+Super 772200-772776 */
                 &m->cpu.mmu, mmu_read, mmu_write, "KS PAR/PDR", 100);
    bus_register(&m->bus, 0x3FF7A, 0x3FF7E,   /* SR0-SR2 777572-777576 */
                 &m->cpu.mmu, mmu_read, mmu_write, "MMU SR", 100);
    bus_register(&m->bus, 0x3FF80, 0x3FFBE,   /* User 777600-777676 */
                 &m->cpu.mmu, mmu_read, mmu_write, "U PAR/PDR", 100);

    m->dl.clock_ptr = &m->cpu.ns_elapsed;
    m->dl.irq_set   = cpu_irq_set;
    m->dl.irq_ctx   = &m->cpu;

    m->rk.irq_set = cpu_irq_set;
    m->rk.irq_clr = cpu_irq_clr;
    m->rk.irq_ctx = &m->cpu;

    m->rl.irq_set = cpu_irq_set;
    m->rl.irq_clr = cpu_irq_clr;
    m->rl.irq_ctx = &m->cpu;

    m->kw.clock_ptr = &m->cpu.ns_elapsed;
    m->kw.irq_set   = cpu_irq_set;
    m->kw.irq_ctx   = &m->cpu;

    m->pc.clock_ptr = &m->cpu.ns_elapsed;
    m->pc.irq_set   = cpu_irq_set;
    m->pc.irq_clr   = cpu_irq_clr;
    m->pc.irq_ctx   = &m->cpu;

    kd11ea_reset(&m->cpu);
    m->cpu.bus = &m->bus;

    console_init(&m->con);
    m->con.bus = &m->bus;
    m->con.cpu = &m->cpu;
    return 0;
}

void machine_bus_init(Machine *m) {
    m->dl.rcsr = 0;
    m->dl.rbuf = 0;
    m->dl.xcsr = DL11_XCSR_RDY;
    m->dl.tx_done_at_ns = 0;

    /* RK05: clear errors, controller done, preserve disk state */
    m->rk.rker = 0;
    m->rk.rkcs = RKCS_DONE;
    m->rk.rkwc = 0;
    m->rk.rkba = 0;
    m->rk.rkda = 0;

    /* KW11-L: clear status */
    m->kw.lks = 0;

    m->bus.nxm = 0;
    int_init(&m->cpu.intc);
}

void machine_cpu_reset(Machine *m) {
    /* Simulated time keeps running across a CPU reset: the peripherals
     * hold absolute deadlines (the KW11 line clock runs off the mains) */
    struct Probe *probe = m->cpu.probe;
    uint64_t now = m->cpu.ns_elapsed;
    kd11ea_reset(&m->cpu);
    m->cpu.bus   = &m->bus;
    m->cpu.probe = probe;
    m->cpu.ns_elapsed = now;
    m->step_instr  = 0;
    m->step_usteps = 0;
}

/* Power-on bootstrap: microcode copies PC to R5, then loads vector */
void machine_power_on(Machine *m) {
    uint16_t pc, ps;
    REG(&m->cpu, 5) = 0;  /* flow 26-N: PC cleared before copy to R5 */

    bus_read(&m->bus, 0x3F614, &pc);   /* 773024: prototype PC (OR'd with S1) */
    bus_read(&m->bus, 0x3F616, &ps);   /* 773026: prototype PS */
    PC(&m->cpu) = pc;
    m->cpu.psw = ps;
    m->cpu.mpc = 0;
    m->cpu.halted = 0;
}

void machine_set_s1(Machine *m, uint16_t s1) {
    rom_set_s1(&m->rom, s1);
}

void machine_tick(Machine *m) {
    uint64_t now = m->cpu.ns_elapsed;
    dl11_tick(&m->dl, now);
    kw11_tick(&m->kw, now);
    pc11_tick(&m->pc, now);
}

int machine_examine(Machine *m, uint32_t pa, uint16_t *data) {
    uint8_t saved_nxm = m->bus.nxm;
    int rc = bus_read(&m->bus, pa & ~1u, data);
    m->bus.nxm = saved_nxm;
    return rc < 0 ? -1 : 0;
}

/* ---- Execution ---- */

static int bp_check(Machine *m, uint16_t pc) {
    for (int i = 0; i < MACHINE_MAX_BP; i++) {
        if (m->bp_used[i] && m->bp_addr[i] == pc) {
            m->bp_hit = i;
            return 1;
        }
    }
    return 0;
}

static MachineStop stop_halted(Machine *m, MachineStop why) {
    m->cpu.halted  = 1;
    m->step_instr  = 0;
    m->step_usteps = 0;
    return why;
}

MachineStop machine_run(Machine *m, uint64_t budget_ns) {
    KD11EA *cpu = &m->cpu;
    uint64_t target_ns = cpu->ns_elapsed + budget_ns;

    while (cpu->ns_elapsed < target_ns) {
        int was_valid = cpu->ir_valid;
        if (kd11ea_ustep(cpu) == 0 || cpu->halted)
            return stop_halted(m, MACHINE_STOP_HALT);

        /* IR just loaded: PC has already moved past the instruction word */
        if (!was_valid && cpu->ir_valid) {
            m->trace_pc[m->trace_head] = PC(cpu) - 2;
            m->trace_ir[m->trace_head] = cpu->ir;
            m->trace_head = (m->trace_head + 1) & (MACHINE_TRACE_SIZE - 1);
            if (m->trace_count < MACHINE_TRACE_SIZE)
                m->trace_count++;
        }

        /* Instruction boundary: MPC 000 before the fetch */
        if (cpu->mpc == 0 && !cpu->ir_valid) {
            if (bp_check(m, PC(cpu)))
                return stop_halted(m, MACHINE_STOP_BREAKPOINT);
            if (m->step_instr > 0 && --m->step_instr == 0)
                return stop_halted(m, MACHINE_STOP_STEP);
        }

        if (m->step_usteps > 0 && --m->step_usteps == 0)
            return stop_halted(m, MACHINE_STOP_STEP);

        if (++m->tick_counter >= MACHINE_TICK_USTEPS) {
            m->tick_counter = 0;
            int stop = m->host_tick ? m->host_tick(m->host_ctx) : 0;
            machine_tick(m);
            if (stop)
                return MACHINE_STOP_HOST;
        }
    }
    return MACHINE_STOP_BUDGET;
}

/* The probe is not fed while halted: the CPU clock is stopped, so a
 * capture armed now waits until the CPU runs again. */
void machine_idle(Machine *m) {
    dl11_tick(&m->dl, m->cpu.ns_elapsed);
}

void machine_halt(Machine *m) {
    stop_halted(m, MACHINE_STOP_HALT);
}

void machine_resume(Machine *m) {
    m->step_instr  = 0;
    m->step_usteps = 0;
    m->cpu.halted  = 0;
}

void machine_step(Machine *m, int n_instr) {
    machine_resume(m);
    m->step_instr = n_instr > 0 ? n_instr : 0;
}

void machine_ustep(Machine *m, int n_usteps) {
    machine_resume(m);
    m->step_usteps = n_usteps > 0 ? n_usteps : 0;
}

/* ---- Breakpoints ---- */

int machine_bp_find(const Machine *m, uint16_t addr) {
    for (int i = 0; i < MACHINE_MAX_BP; i++)
        if (m->bp_used[i] && m->bp_addr[i] == addr)
            return i;
    return -1;
}

int machine_bp_set(Machine *m, uint16_t addr) {
    int slot = machine_bp_find(m, addr);
    if (slot >= 0)
        return slot;
    for (int i = 0; i < MACHINE_MAX_BP; i++) {
        if (!m->bp_used[i]) {
            m->bp_addr[i] = addr;
            m->bp_used[i] = 1;
            return i;
        }
    }
    return -1;
}

int machine_bp_del(Machine *m, int slot) {
    if (slot < 0 || slot >= MACHINE_MAX_BP || !m->bp_used[slot])
        return -1;
    m->bp_used[slot] = 0;
    return 0;
}

void machine_bp_clear(Machine *m) {
    memset(m->bp_used, 0, sizeof(m->bp_used));
}

/* ---- Instruction trace ---- */

int machine_trace_get(const Machine *m, int n, uint16_t *pc, uint16_t *ir) {
    if (n < 0 || n >= m->trace_count)
        return -1;
    int idx = (m->trace_head - m->trace_count + n) & (MACHINE_TRACE_SIZE - 1);
    *pc = m->trace_pc[idx];
    *ir = m->trace_ir[idx];
    return 0;
}

void machine_trace_clear(Machine *m) {
    m->trace_head  = 0;
    m->trace_count = 0;
}

double machine_benchmark(Machine *m, uint16_t s1) {
    machine_cpu_reset(m);
    machine_set_s1(m, 0);
    machine_power_on(m);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (long i = 0; i < 1000000L; i++)
        kd11ea_ustep(&m->cpu);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    /* The CPU clock restarts at 0: restart the time-based devices too */
    machine_cpu_reset(m);
    machine_set_s1(m, s1);
    machine_bus_init(m);
    m->kw.next_tick_ns = 0;

    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) * 1e-9;
    return elapsed / 1e6 * 1e9;
}
