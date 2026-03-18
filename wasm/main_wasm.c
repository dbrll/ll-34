/*
 * wasm/main_wasm.c -- WebAssembly entry point for ll-34
 *
 * Initialises the complete PDP-11/34A system and drives the main loop
 * via emscripten_set_main_loop().  The simulation is paced against wall
 * clock time so the KW11 60 Hz line clock fires at the correct rate.
 *
 * JavaScript API,system:
 *   wasm_power_on()                -- power on (boot from M9301)
 *   wasm_power_off()               -- power off (halt CPU)
 *   wasm_reset()                   -- power-cycle
 *   wasm_set_odt()                 -- prepare ODT boot (S1=0)
 *   wasm_load_rk_image(drv, path)  -- attach RK05 image from MEMFS
 *   wasm_load_lda(path)            -- load PDP-11 LDA binary from MEMFS
 *   wasm_load_asc(path)            -- load ASCII octal loader from MEMFS
 *   wasm_console_key(key)          -- front-panel key press
 *   wasm_get_display_addr()        -- 18-bit display value (single 6-digit octal)
 *   wasm_get_display_data()        -- compat alias (display low 16 bits)
 *   wasm_get_leds()                -- LED bitmask (bit0=RUN, bit1=BUS_ERR)
 *   wasm_rx_push(ch)               -- inject keypress into DL11 RX
 *   wasm_tx_poll()                 -- drain one DL11 TX byte (-1 = empty)
 *
 * JavaScript API,debug console:
 *   wasm_get_reg(n)                -- read register Rn (n=0..7)
 *   wasm_get_psw()                 -- processor status word
 *   wasm_get_ir()                  -- instruction register
 *   wasm_get_us()                  -- simulation time in microseconds
 *   wasm_is_halted()               -- 1 if CPU is halted
 *   wasm_halt()                    -- halt CPU immediately
 *   wasm_run()                     -- resume CPU
 *   wasm_step()                    -- execute one instruction then re-halt
 *   wasm_ustep_n(n)                -- execute N micro-steps then re-halt
 *   wasm_poll_halt()               -- returns 1 (once) when CPU just halted
 *   wasm_examine(addr)             -- read word at 18-bit physical address
 *   wasm_deposit(addr, val)        -- write word at 18-bit physical address
 *   wasm_disasm(vaddr)             -- disassemble at 16-bit virtual address
 *   wasm_disasm_len()              -- byte count from last wasm_disasm()
 *
 * JavaScript API,MMU:
 *   wasm_get_mmu_sr0()             -- SR0 (MMU status / enable)
 *   wasm_get_mmu_sr2()             -- SR2 (virtual PC at last abort)
 *   wasm_get_mmu_par(n)            -- PAR[n] (0-7=kernel, 8-15=user)
 *   wasm_get_mmu_pdr(n)            -- PDR[n] (0-7=kernel, 8-15=user)
 *
 * JavaScript API,breakpoints (max 8):
 *   wasm_bp_set(addr)              -- add breakpoint at 16-bit virtual address
 *   wasm_bp_del(addr)              -- remove breakpoint
 *   wasm_bp_clear()                -- remove all breakpoints
 *   wasm_bp_count()                -- number of active breakpoints
 *   wasm_bp_addr(i)                -- address of breakpoint i
 *
 * JavaScript API,instruction trace ring:
 *   wasm_get_trace_count()         -- entries in ring (0..512)
 *   wasm_get_trace_pc(n)           -- PC of entry n (0=oldest)
 *   wasm_get_trace_ir(n)           -- IR of entry n
 *   wasm_trace_clear()             -- clear the ring
 *
 * JavaScript API,logic analyzer (virtual probe):
 *   wasm_probe_sig_count()         -- total number of available signals
 *   wasm_probe_sig_name(i)         -- display name of signal i
 *   wasm_probe_sig_desc(i)         -- description of signal i
 *   wasm_probe_add(name)           -- add signal to capture set
 *   wasm_probe_rm(name)            -- remove signal from capture set
 *   wasm_probe_clear_sigs()        -- clear all selected signals
 *   wasm_probe_sel_count()         -- number of selected signals
 *   wasm_probe_sel_name(i)         -- name of selected signal i
 *   wasm_probe_set_trigger(name, val, mask) -- configure trigger
 *   wasm_probe_set_depth(n)        -- capture depth (power of 2, max 2048)
 *   wasm_probe_set_rate(n)         -- sample every N-th micro-step
 *   wasm_probe_set_pos(pct)        -- trigger position 0..100 (default 50)
 *   wasm_probe_arm()               -- arm the analyzer
 *   wasm_probe_disarm()            -- disarm without clearing
 *   wasm_probe_state()             -- 0=idle,1=armed,2=capturing,3=done
 *   wasm_probe_sample_count()      -- number of captured samples
 *   wasm_probe_read(sample, sig)   -- read signal value at sample index
 */

#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../unibus/unibus.h"
#include "../unibus/ram.h"
#include "../tty/tty.h"
#include "../m9301/rom.h"
#include "../dl11/dl11.h"
#include "../rk11/rk11.h"
#include "../rl11/rl11.h"
#include "../kd11ea/kd11ea.h"
#include "../kd11ea/mmu.h"
#include "../kw11/kw11.h"
#include "../pc11/pc11.h"
#include "../console/console.h"
#include "../unibus/loader.h"
#include "../debug/disasm.h"
#include "../probe/probe.h"

#define PSW_ADDR   0x3FFFE   /* 777776 octal */
#define SWREG_ADDR 0x3FF78   /* 777570 octal */

/* System state */
static KD11EA   cpu;
static Bus      bus;
static Ram      ram;
static Rom      rom;
static DL11     dl;
static RK05     rk;
static RL11     rl;
static KW11     kw;
static PC11     pc;
static Console  con;
static TTY     *tty;

static uint16_t direct_boot_pc = 0;
static int      direct_boot    = 0;

static int psw_read(void *dev, uint32_t addr, uint16_t *data) {
    (void)dev; (void)addr;
    *data = cpu.psw;
    return 0;
}

static int psw_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    (void)dev; (void)addr;
    if (is_byte)
        cpu.psw = (cpu.psw & 0xFF00) | (data & 0xFF);
    else
        cpu.psw = data;
    return 0;
}

static int swreg_read(void *dev, uint32_t addr, uint16_t *data) {
    (void)dev; (void)addr;
    *data = con.switch_reg;
    return 0;
}

static int swreg_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    (void)dev; (void)addr; (void)is_byte;
    con.switch_reg = data;
    return 0;
}

static void cpu_irq_set(void *ctx, uint16_t vector, uint8_t priority) {
    (void)ctx;
    int_request(&cpu.intc, vector, priority);
}

static void cpu_irq_clr(void *ctx, uint16_t vector) {
    (void)ctx;
    int_cancel(&cpu.intc, vector);
}

static int dl11_rx_ready(void *ctx) {
    return ((TTY *)ctx)->rx_ready((TTY *)ctx);
}

static int dl11_rx_read(void *ctx) {
    TTY *t = (TTY *)ctx;
    return t->rx_read(t);
}

static void dl11_tx_write(void *ctx, uint8_t ch) {
    TTY *t = (TTY *)ctx;
    t->tx_write(t, ch);
}

static void power_on(void) {
    cpu.mpc    = 0;
    cpu.halted = 0;
    if (direct_boot) {
        /* LDA/ASC program loaded directly, skip M9301, jump to entry point */
        PC(&cpu) = direct_boot_pc;
        cpu.psw  = 0;
        direct_boot = 0;
        return;
    }
    /* Normal M9301-YF boot */
    uint16_t pc_addr, ps;
    REG(&cpu, 5) = 0;
    bus_read(&bus, 0x3F614, &pc_addr);
    bus_read(&bus, 0x3F616, &ps);
    PC(&cpu) = pc_addr;
    cpu.psw  = ps;
}

static void do_bus_reset(void) {
    dl.rcsr = 0;
    dl.rbuf = 0;
    dl.xcsr = DL11_XCSR_RDY;
    dl.tx_done_at_ns = 0;

    rk.rker = 0;
    rk.rkcs = RKCS_DONE;
    rk.rkwc = 0;
    rk.rkba = 0;
    rk.rkda = 0;

    kw.lks = 0;

    bus.nxm = 0;
    int_init(&cpu.intc);
}

/* Debug bus read (preserves NXM flag) */
static int debug_read_word(uint16_t vaddr, void *ctx) {
    (void)ctx;
    uint32_t phys = (vaddr >= 0xE000u) ? (0x30000u | vaddr) : (uint32_t)vaddr;
    uint16_t val = 0;
    int saved_nxm = bus.nxm;
    int rc = bus_read(&bus, phys & ~1u, &val);
    bus.nxm = saved_nxm;   /* debug reads must not trigger NXM traps */
    if (rc < 0) return -1;
    return (int)val;
}

/* Instruction trace ring buffer */
#define ITRACE_SIZE  512
#define ITRACE_MASK  (ITRACE_SIZE - 1)

static uint16_t itrace_pc[ITRACE_SIZE];
static uint16_t itrace_ir[ITRACE_SIZE];
static int      itrace_head  = 0;
static int      itrace_count = 0;

static int step_instructions = 0;  /* halt after N instruction fetches */
static int step_usteps        = 0;  /* halt after N micro-steps */
static int halt_pending        = 0;  /* set when CPU just halted; cleared by poll */
static int halt_spontaneous    = 0;  /* 1 = HALT instr/crash, 0 = debugger action */

/* Breakpoints (max 8) */
#define MAX_BKPTS 8
static uint16_t bkpt_addr[MAX_BKPTS];
static int      bkpt_en[MAX_BKPTS];
static int      bkpt_count = 0;

static int bkpt_hit(uint16_t pc_val) {
    for (int i = 0; i < bkpt_count; i++) {
        if (bkpt_en[i] && bkpt_addr[i] == pc_val)
            return 1;
    }
    return 0;
}

#define PROBE_DEFAULT_DEPTH  2048   /* max capture depth (88 bytes each = 176 KB) */
static ProbeSnapshot probe_buf[PROBE_DEFAULT_DEPTH];
static Probe         probe_inst;

/* Main simulation loop (~60 fps) */
#define MAX_SIM_NS_PER_FRAME  50000000ULL
#define POLL_INTERVAL         256

static double last_frame_ms;
static int    poll_counter;

static void wasm_loop(void) {
    /* Tick console LEDs regardless of halted state */
    console_tick(&con);

    /* Handle console-initiated boot/init */
    if (con.boot_requested) {
        con.boot_requested = 0;
        do_bus_reset();
        kd11ea_reset(&cpu);
        cpu.bus   = &bus;
        cpu.probe = &probe_inst;
        power_on();
    }
    if (con.init_requested) {
        con.init_requested = 0;
        do_bus_reset();
    }

    if (cpu.halted) {
        /* Even when halted, feed the probe so the LA can complete captures.
         * We provide snapshots of the frozen CPU state (bus quiet). */
        if (cpu.probe && (cpu.probe->state == PROBE_ARMED ||
                          cpu.probe->state == PROBE_CAPTURING)) {
            ProbeSnapshot idle = {
                .ns           = cpu.ns_elapsed,
                .mpc          = cpu.mpc,
                .ir           = cpu.ir,
                .psw          = cpu.psw,
                .pc           = PC(&cpu),
                .sp           = cpu.sp[9],
                .r0           = REG(&cpu, 0),
                .r1           = REG(&cpu, 1),
                .b_reg        = cpu.b_reg,
                .bx_reg       = cpu.bx_reg,
                .halted       = 1,
            };
            /* Feed up to depth samples per frame to complete capture quickly */
            for (uint32_t i = 0; i < cpu.probe->depth &&
                    cpu.probe->state != PROBE_DONE; i++) {
                probe_sample(cpu.probe, &idle);
            }
        }
        tty->tick(tty);
        dl11_tick(&dl, cpu.ns_elapsed);
        return;
    }

    double now_ms   = emscripten_get_now();
    double delta_ms = now_ms - last_frame_ms;
    last_frame_ms   = now_ms;
    if (delta_ms <= 0.0 || delta_ms > 200.0) delta_ms = 16.0;

    uint64_t budget_ns = (uint64_t)(delta_ms * 1.0e6);
    if (budget_ns > MAX_SIM_NS_PER_FRAME) budget_ns = MAX_SIM_NS_PER_FRAME;
    uint64_t target_ns = cpu.ns_elapsed + budget_ns;

    int cpu_was_running = !cpu.halted;
    while (cpu.ns_elapsed < target_ns && !cpu.halted) {
        int was_valid = cpu.ir_valid;
        kd11ea_ustep(&cpu);

        /* Instruction trace: capture on ir_valid 0->1 (MISC_LOAD_IR fired).
         * At that point PC has been auto-incremented past the instruction word,
         * so the instruction address is PC-2. */
        if (!was_valid && cpu.ir_valid) {
            uint16_t ipc = PC(&cpu) - 2;

            itrace_pc[itrace_head] = ipc;
            itrace_ir[itrace_head] = cpu.ir;
            itrace_head = (itrace_head + 1) & ITRACE_MASK;
            if (itrace_count < ITRACE_SIZE) itrace_count++;

            /* Breakpoint check */
            if (bkpt_hit(ipc)) {
                cpu.halted = 1;
                halt_pending = 1;
                break;
            }

            /* Instruction-level step */
            if (step_instructions > 0) {
                if (--step_instructions == 0) {
                    cpu.halted = 1;
                    halt_pending = 1;
                    break;
                }
            }
        }

        /* Micro-step */
        if (step_usteps > 0) {
            if (--step_usteps == 0) {
                cpu.halted = 1;
                halt_pending = 1;
                break;
            }
        }

        if (++poll_counter >= POLL_INTERVAL) {
            poll_counter = 0;
            tty->tick(tty);
            dl11_tick(&dl, cpu.ns_elapsed);
            kw11_tick(&kw, cpu.ns_elapsed);
            pc11_tick(&pc, cpu.ns_elapsed);
        }
    }

    /* Detect spontaneous CPU halt (HALT instruction, illegal op, etc.)
     * that was not already signalled by a breakpoint or step completion. */
    if (cpu_was_running && cpu.halted && !halt_pending) {
        halt_pending = 1;
        halt_spontaneous = 1;
    }
}

/* --- JS API: system --- */

EMSCRIPTEN_KEEPALIVE
void wasm_power_on(void) {
    step_instructions = 0;
    step_usteps       = 0;
    halt_pending      = 0;
    memset(ram.mem, 0, ram.size_words * sizeof(ram.mem[0]));
    do_bus_reset();
    kd11ea_reset(&cpu);
    cpu.bus   = &bus;
    cpu.probe = &probe_inst;
    console_reset(&con);   /* KY11-LB power-up clear (§5.3.2) */
    power_on();
}

EMSCRIPTEN_KEEPALIVE
void wasm_power_off(void) {
    cpu.halted   = 1;
    halt_pending = 0;
    console_reset(&con);
}

EMSCRIPTEN_KEEPALIVE
void wasm_reset(void) {
    step_instructions = 0;
    step_usteps       = 0;
    halt_pending      = 0;
    do_bus_reset();
    kd11ea_reset(&cpu);
    cpu.bus   = &bus;
    cpu.probe = &probe_inst;
    power_on();
}

EMSCRIPTEN_KEEPALIVE
void wasm_set_odt(void) {
    /* Prepare ODT boot (S1=0), cancels any pending direct boot. */
    con.switch_reg  = 0;
    direct_boot = 0;
    rom_set_s1(&rom, 0);
}

EMSCRIPTEN_KEEPALIVE
void wasm_load_rk_image(int drive, const char *memfs_path) {
    if (drive < 0 || drive >= RK_NUMDR) return;
    rk05_attach(&rk, drive, memfs_path, 0);
    con.switch_reg  = 0146;   /* M9301-YF: boot DK, skip diagnostics */
    direct_boot = 0;
    rom_set_s1(&rom, con.switch_reg);
}

EMSCRIPTEN_KEEPALIVE
void wasm_load_rl_image(int drive, const char *memfs_path) {
    if (drive < 0 || drive >= RL_NUMDR) return;
    rl11_attach(&rl, drive, memfs_path, 0);
    /* No M9301-YF boot code for RL, use direct boot or chain from RK */
}

EMSCRIPTEN_KEEPALIVE
int32_t wasm_load_lda(const char *memfs_path) {
    uint16_t start = 0;
    if (loader_lda(&ram, memfs_path, &start) < 0) return -1;
    direct_boot_pc = start;
    direct_boot    = 1;
    con.switch_reg = 0;
    rom_set_s1(&rom, 0);
    return (int32_t)start;
}

EMSCRIPTEN_KEEPALIVE
int32_t wasm_load_asc(const char *memfs_path) {
    uint16_t start = 0;
    if (loader_asc(&ram, memfs_path, &start) < 0) return -1;
    direct_boot_pc = start;
    direct_boot    = 1;
    con.switch_reg = 0;
    rom_set_s1(&rom, 0);
    return (int32_t)start;
}

EMSCRIPTEN_KEEPALIVE
void wasm_console_key(int key) {
    console_key(&con, key);
}

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_get_display_addr(void) {
    return con.display;
}

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_display_data(void) {
    return con.display & 0xFFFF;  /* compat, same as display low 16 bits */
}

/* LED bitmask: bit0=RUN, bit1=BUS_ERR, bit2=SR_DISP, bit3=MAINT, bit4=CNTRL */
EMSCRIPTEN_KEEPALIVE
uint8_t wasm_get_leds(void) {
    return (con.led_run      ? 1  : 0)
         | (con.led_bus_err  ? 2  : 0)
         | (con.led_sr_disp  ? 4  : 0)
         | (con.led_maint    ? 8  : 0)
         | (con.cntrl_active ? 16 : 0);
}

/* wasm_rx_push() and wasm_tx_poll() are in tty/tty_wasm.c */

/* --- JS API: debug console --- */
EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_reg(int n) { return REG(&cpu, n & 7); }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_psw(void)  { return cpu.psw; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_ir(void)   { return cpu.ir; }

EMSCRIPTEN_KEEPALIVE
int wasm_is_halted(void)     { return cpu.halted; }

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_get_us(void)   { return (uint32_t)(cpu.ns_elapsed / 1000); }

EMSCRIPTEN_KEEPALIVE
void wasm_halt(void) {
    cpu.halted        = 1;
    step_instructions = 0;
    step_usteps       = 0;
    halt_pending      = 1;
}

EMSCRIPTEN_KEEPALIVE
void wasm_run(void) {
    step_instructions = 0;
    step_usteps       = 0;
    halt_pending      = 0;
    cpu.halted        = 0;
}

EMSCRIPTEN_KEEPALIVE
void wasm_step(void) {
    step_instructions = 1;
    step_usteps       = 0;
    halt_pending      = 0;
    cpu.halted        = 0;
}

EMSCRIPTEN_KEEPALIVE
void wasm_ustep_n(int n) {
    step_instructions = 0;
    step_usteps       = (n > 0) ? n : 0;
    halt_pending      = 0;
    cpu.halted        = 0;
}

/* Returns halt reason once: 0=none, 1=debugger, 2=spontaneous */
EMSCRIPTEN_KEEPALIVE
int wasm_poll_halt(void) {
    if (halt_pending) {
        halt_pending = 0;
        int reason = halt_spontaneous ? 2 : 1;
        halt_spontaneous = 0;
        return reason;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE
uint16_t *wasm_ram_ptr(void) { return ram.mem; }
EMSCRIPTEN_KEEPALIVE
uint32_t wasm_ram_words(void) { return ram.size_words; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_examine(uint32_t addr) {
    uint16_t val = 0;
    int saved_nxm = bus.nxm;
    bus_read(&bus, addr & 0x3FFFFu, &val);
    bus.nxm = saved_nxm;
    return val;
}

EMSCRIPTEN_KEEPALIVE
void wasm_deposit(uint32_t addr, uint16_t val) {
    bus_write(&bus, addr & 0x3FFFFu, val, BUS_DATO);
}

static char     disasm_buf[128];
static uint32_t disasm_last_len = 2;

EMSCRIPTEN_KEEPALIVE
const char *wasm_disasm(uint32_t vaddr) {
    disasm_buf[0] = '\0';
    int n = pdp11_disasm((uint16_t)vaddr, debug_read_word, NULL,
                         disasm_buf, sizeof(disasm_buf));
    disasm_last_len = (n > 0) ? (uint32_t)n : 2;
    return disasm_buf;
}

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_disasm_len(void) { return disasm_last_len; }

/* --- JS API: micro-step state --- */

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mpc(void)    { return cpu.mpc; }

#include "../kd11ea/ucode_labels.h"
EMSCRIPTEN_KEEPALIVE
const char *wasm_get_mpc_label(int mpc) {
    if (mpc < 0 || mpc >= 512) return "";
    return ucode_labels[mpc].op;
}

EMSCRIPTEN_KEEPALIVE
const char *wasm_get_mpc_desc(int mpc) {
    if (mpc < 0 || mpc >= 512) return "";
    const char *d = ucode_labels[mpc].desc;
    return d ? d : "";
}

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_get_ba(void)     { return cpu.ba; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_b_reg(void)  { return cpu.b_reg; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_bx_reg(void) { return cpu.bx_reg; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_alu_out(void) { return cpu.alu_out; }

EMSCRIPTEN_KEEPALIVE
uint8_t wasm_get_alu_cout(void) { return cpu.alu_cout; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_udata(void)  { return cpu.unibus_data; }

/* --- JS API: MMU --- */

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_sr0(void) { return cpu.mmu.sr0; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_sr2(void) { return cpu.mmu.sr2; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_par(int n) {
    if (n < 0 || n > 15) return 0;
    return cpu.mmu.par[n];
}

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_pdr(int n) {
    if (n < 0 || n > 15) return 0;
    return cpu.mmu.pdr[n];
}

/* --- JS API: breakpoints --- */
EMSCRIPTEN_KEEPALIVE
int wasm_bp_set(uint16_t addr) {
    /* Update existing slot first */
    for (int i = 0; i < bkpt_count; i++) {
        if (bkpt_addr[i] == addr) {
            bkpt_en[i] = 1;
            return 0;
        }
    }
    if (bkpt_count >= MAX_BKPTS) return -1;
    bkpt_addr[bkpt_count] = addr;
    bkpt_en[bkpt_count]   = 1;
    bkpt_count++;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_bp_del(uint16_t addr) {
    for (int i = 0; i < bkpt_count; i++) {
        if (bkpt_addr[i] == addr) {
            /* Compact the array */
            for (int j = i; j < bkpt_count - 1; j++) {
                bkpt_addr[j] = bkpt_addr[j + 1];
                bkpt_en[j]   = bkpt_en[j + 1];
            }
            bkpt_count--;
            return 0;
        }
    }
    return -1;
}

EMSCRIPTEN_KEEPALIVE
void wasm_bp_clear(void) { bkpt_count = 0; }

EMSCRIPTEN_KEEPALIVE
int wasm_bp_count(void)  { return bkpt_count; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_bp_addr(int i) {
    if (i < 0 || i >= bkpt_count) return 0;
    return bkpt_addr[i];
}

/* --- JS API: instruction trace --- */
EMSCRIPTEN_KEEPALIVE
int wasm_get_trace_count(void) { return itrace_count; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_trace_pc(int n) {
    if (n < 0 || n >= itrace_count) return 0;
    int idx = (itrace_head - itrace_count + n + ITRACE_SIZE) & ITRACE_MASK;
    return itrace_pc[idx];
}

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_trace_ir(int n) {
    if (n < 0 || n >= itrace_count) return 0;
    int idx = (itrace_head - itrace_count + n + ITRACE_SIZE) & ITRACE_MASK;
    return itrace_ir[idx];
}

EMSCRIPTEN_KEEPALIVE
void wasm_trace_clear(void) { itrace_head = 0; itrace_count = 0; }

/* --- JS API: logic analyzer --- */
EMSCRIPTEN_KEEPALIVE
int wasm_probe_sig_count(void) { return probe_def_count(); }

EMSCRIPTEN_KEEPALIVE
const char *wasm_probe_sig_name(int i) {
    const ProbeDef *d = probe_def_get(i);
    if (!d) return "";
    return probe_display_name(d);
}

EMSCRIPTEN_KEEPALIVE
const char *wasm_probe_sig_desc(int i) {
    const ProbeDef *d = probe_def_get(i);
    if (!d) return "";
    return d->desc ? d->desc : "";
}

EMSCRIPTEN_KEEPALIVE
int wasm_probe_add(const char *name)    { return probe_add_signal(&probe_inst, name); }

EMSCRIPTEN_KEEPALIVE
int wasm_probe_rm(const char *name)     { return probe_rm_signal(&probe_inst, name); }

EMSCRIPTEN_KEEPALIVE
void wasm_probe_clear_sigs(void)        { probe_clear_signals(&probe_inst); }

EMSCRIPTEN_KEEPALIVE
int wasm_probe_sel_count(void)          { return probe_inst.nsignals; }

EMSCRIPTEN_KEEPALIVE
const char *wasm_probe_sel_name(int i) {
    if (i < 0 || i >= probe_inst.nsignals) return "";
    int idx = probe_inst.signal_idx[i];
    const ProbeDef *d = probe_def_get(idx);
    if (!d) return "";
    return probe_display_name(d);
}

EMSCRIPTEN_KEEPALIVE
int wasm_probe_sel_is_1bit(int i) {
    if (i < 0 || i >= probe_inst.nsignals) return 0;
    int idx = probe_inst.signal_idx[i];
    const ProbeDef *d = probe_def_get(idx);
    if (!d) return 0;
    return (d->level == 1 && d->bit != PROBE_BIT_WHOLE) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_probe_set_trigger(const char *name, uint32_t value, uint32_t mask) {
    return probe_set_trigger(&probe_inst, name, value, mask);
}

EMSCRIPTEN_KEEPALIVE
void wasm_probe_set_depth(int n) {
    if (n < 1) n = 1;
    if (n > PROBE_DEFAULT_DEPTH) n = PROBE_DEFAULT_DEPTH;
    /* Round down to previous power of 2 */
    int d = 1;
    while (d * 2 <= n) d *= 2;
    probe_set_depth(&probe_inst, (uint32_t)d);
}

EMSCRIPTEN_KEEPALIVE
void wasm_probe_set_rate(int n) {
    probe_inst.divider = (n > 1) ? (uint32_t)n : 1;
}

EMSCRIPTEN_KEEPALIVE
void wasm_probe_set_pos(int pct) {
    probe_set_trigger_pos(&probe_inst, pct);
}

EMSCRIPTEN_KEEPALIVE
void wasm_probe_arm(void)    { probe_arm(&probe_inst); }

EMSCRIPTEN_KEEPALIVE
void wasm_probe_disarm(void) { probe_disarm(&probe_inst); }

EMSCRIPTEN_KEEPALIVE
int wasm_probe_state(void)   { return (int)probe_inst.state; }

EMSCRIPTEN_KEEPALIVE
int wasm_probe_sample_count(void) { return (int)probe_inst.count; }

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_probe_dbg_head(void)  { return probe_inst.head; }
EMSCRIPTEN_KEEPALIVE
uint32_t wasm_probe_dbg_ptr(void)   { return probe_inst.post_trigger_remaining; }
EMSCRIPTEN_KEEPALIVE
uint32_t wasm_probe_dbg_depth(void) { return probe_inst.depth; }
EMSCRIPTEN_KEEPALIVE
uint32_t wasm_probe_dbg_div(void)   { return probe_inst.divider; }

/* Read signal value from captured sample.
 * sample_idx: 0=oldest, count-1=newest.
 * sig_idx: index into the *selected* signal list (0..nsignals-1). */
EMSCRIPTEN_KEEPALIVE
uint32_t wasm_probe_read(int sample_idx, int sig_idx) {
    if (sample_idx < 0 || sample_idx >= (int)probe_inst.count) return 0;
    if (sig_idx < 0 || sig_idx >= probe_inst.nsignals) return 0;

    /* Locate the sample in the ring buffer */
    uint32_t ring_idx = (probe_inst.head - probe_inst.count
                         + (uint32_t)sample_idx) & probe_inst.mask;
    const ProbeSnapshot *snap = &probe_buf[ring_idx];

    /* Read the selected signal */
    int def_idx = probe_inst.signal_idx[sig_idx];
    const ProbeDef *def = probe_def_get(def_idx);
    if (!def) return 0;
    return probe_read_value(snap, def);
}

/* Read timestamp (ns) for a captured sample.  Returns relative ns
 * from first sample (double, since uint64 is awkward in JS). */
EMSCRIPTEN_KEEPALIVE
double wasm_probe_read_ns(int sample_idx) {
    if (sample_idx < 0 || sample_idx >= (int)probe_inst.count) return 0.0;
    uint32_t ring0 = (probe_inst.head - probe_inst.count) & probe_inst.mask;
    uint64_t ns0   = probe_buf[ring0].ns;
    uint32_t ringi = (probe_inst.head - probe_inst.count
                      + (uint32_t)sample_idx) & probe_inst.mask;
    return (double)(probe_buf[ringi].ns - ns0);
}

int main(void) {
    bus_init(&bus);
    ram_init(&ram, 248u * 512u);
    rom_init(&rom);
    dl11_init(&dl, 9600);
    rk05_init(&rk);
    rl11_init(&rl);
    kw11_init(&kw, 60);
    pc11_init(&pc);

    con.switch_reg = 0;
    rom_set_s1(&rom, 0);

    ram_register(&ram, &bus);
    rom_register(&rom, &bus);
    dl11_register(&dl, &bus);
    rk05_register(&rk, &bus);
    rl11_register(&rl, &bus);
    kw11_register(&kw, &bus);
    pc11_register(&pc, &bus);
    bus_register(&bus, PSW_ADDR,   PSW_ADDR + 1,
                 NULL, psw_read,   psw_write,   "PSW",    100);
    bus_register(&bus, SWREG_ADDR, SWREG_ADDR + 1,
                 NULL, swreg_read, swreg_write, "SWITCH", 100);

    mmu_init(&cpu.mmu);
    bus_register(&bus, 0x3F480, 0x3F5FE,
                 &cpu.mmu, mmu_read, mmu_write, "KS PAR/PDR", 100);
    bus_register(&bus, 0x3FF7A, 0x3FF7E,
                 &cpu.mmu, mmu_read, mmu_write, "MMU SR",     100);
    bus_register(&bus, 0x3FF80, 0x3FFBE,
                 &cpu.mmu, mmu_read, mmu_write, "U PAR/PDR",  100);

    tty = tty_wasm_create();

    dl.rx_ready  = dl11_rx_ready;
    dl.rx_read   = dl11_rx_read;
    dl.tx_write  = dl11_tx_write;
    dl.io_ctx    = tty;
    dl.clock_ptr = &cpu.ns_elapsed;
    dl.irq_set   = cpu_irq_set;
    dl.irq_ctx   = &cpu;

    rk.irq_set = cpu_irq_set;
    rk.irq_clr = cpu_irq_clr;
    rk.irq_ctx = &cpu;

    rl.irq_set = cpu_irq_set;
    rl.irq_clr = cpu_irq_clr;
    rl.irq_ctx = &cpu;

    kw.clock_ptr = &cpu.ns_elapsed;
    kw.irq_set   = cpu_irq_set;
    kw.irq_ctx   = &cpu;

    pc.clock_ptr = &cpu.ns_elapsed;
    pc.irq_set   = cpu_irq_set;
    pc.irq_clr   = cpu_irq_clr;
    pc.irq_ctx   = &cpu;

    kd11ea_reset(&cpu);
    cpu.bus = &bus;

    probe_init(&probe_inst, probe_buf, PROBE_DEFAULT_DEPTH);
    cpu.probe = &probe_inst;

    console_init(&con);
    con.bus = &bus;
    con.cpu = &cpu;

    cpu.halted = 1;

    last_frame_ms = emscripten_get_now();
    emscripten_set_main_loop(wasm_loop, 0, 1);
    return 0;
}
