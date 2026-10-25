/*
 * frontends/wasm/main_wasm.c -- WebAssembly entry point for ll-34
 * 
 * Initialises the PDP-11/34A system and drives the main loop through
 * emscripten_set_main_loop(). The simulation is paced against wall clock
 * time so the KW11 60 Hz line clock fires at the correct rate.
 *
 * Note: This code is LLM-generated, as the rest of the WASM UI.
 *
 * JavaScript API: system:
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
 * JavaScript API: debug console:
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
 * JavaScript API: MMU:
 *   wasm_get_mmu_sr0()             -- SR0 (MMU status / enable)
 *   wasm_get_mmu_sr2()             -- SR2 (virtual PC at last abort)
 *   wasm_get_mmu_par(n)            -- PAR[n] (0-7=kernel, 8-15=user)
 *   wasm_get_mmu_pdr(n)            -- PDR[n] (0-7=kernel, 8-15=user)
 *
 * JavaScript API: breakpoints (max 16):
 *   wasm_bp_set(addr)              -- add breakpoint at 16-bit virtual address
 *   wasm_bp_del(addr)              -- remove breakpoint
 *   wasm_bp_clear()                -- remove all breakpoints
 *   wasm_bp_count()                -- number of active breakpoints
 *   wasm_bp_addr(i)                -- address of breakpoint i
 *
 * JavaScript API: instruction trace ring:
 *   wasm_get_trace_count()         -- entries in ring (0..512)
 *   wasm_get_trace_pc(n)           -- PC of entry n (0=oldest)
 *   wasm_get_trace_ir(n)           -- IR of entry n
 *   wasm_trace_clear()             -- clear the ring
 *
 * JavaScript API: logic analyzer (virtual probe):
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

#include "pdp11/machine.h"
#include "frontends/tty/tty.h"
#include "pdp11/kd11ea/mmu.h"
#include "pdp11/unibus/loader.h"
#include "pdp11/debug/disasm.h"
#include "pdp11/probe/probe.h"

/* System state */
static Machine  m;
static TTY     *tty;

static uint16_t direct_boot_pc = 0;
static int      direct_boot    = 0;

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
    m.cpu.mpc    = 0;
    m.cpu.halted = 0;
    if (direct_boot) {
        /* LDA/ASC program loaded directly, skip M9301, jump to entry point */
        PC(&m.cpu) = direct_boot_pc;
        m.cpu.psw  = 0;
        direct_boot = 0;
        return;
    }
    /* Normal M9301-YF boot */
    machine_power_on(&m);
}

/* Debug bus read (preserves NXM flag) */
static int debug_read_word(uint16_t vaddr, void *ctx) {
    (void)ctx;
    uint32_t phys = (vaddr >= 0xE000u) ? (0x30000u | vaddr) : (uint32_t)vaddr;
    uint16_t val = 0;
    if (machine_examine(&m, phys, &val) < 0) return -1;
    return (int)val;
}

static int halt_pending        = 0;  /* set on halt, cleared by wasm_poll_halt() */
static int halt_spontaneous    = 0;  /* 1 = HALT instr/crash, 0 = debugger action */

#define PROBE_DEFAULT_DEPTH  2048   /* max capture depth (88 bytes each = 176 KB) */
static ProbeSnapshot probe_buf[PROBE_DEFAULT_DEPTH];
static Probe         probe_inst;

/* Main simulation loop (~60 fps) */
#define MAX_SIM_NS_PER_FRAME  50000000ULL

static double last_frame_ms;

static int wasm_host_tick(void *ctx) {
    (void)ctx;
    tty->tick(tty);
    return 0;
}

static void wasm_loop(void) {
    /* Tick console LEDs regardless of halted state */
    console_tick(&m.con);

    /* Handle console-initiated boot/init */
    if (m.con.boot_requested) {
        m.con.boot_requested = 0;
        machine_bus_init(&m);
        machine_cpu_reset(&m);
        power_on();
    }
    if (m.con.init_requested) {
        m.con.init_requested = 0;
        machine_bus_init(&m);
    }

    if (m.cpu.halted) {
        tty->tick(tty);
        machine_idle(&m);
        return;
    }

    double now_ms   = emscripten_get_now();
    double delta_ms = now_ms - last_frame_ms;
    last_frame_ms   = now_ms;
    if (delta_ms <= 0.0 || delta_ms > 200.0) delta_ms = 16.0;

    uint64_t budget_ns = (uint64_t)(delta_ms * 1.0e6);
    if (budget_ns > MAX_SIM_NS_PER_FRAME) budget_ns = MAX_SIM_NS_PER_FRAME;

    switch (machine_run(&m, budget_ns)) {
    case MACHINE_STOP_HALT:
        halt_pending     = 1;
        halt_spontaneous = 1;
        break;
    case MACHINE_STOP_BREAKPOINT:
    case MACHINE_STOP_STEP:
        halt_pending = 1;
        break;
    default:
        break;
    }
}

/* --- JS API: system --- */

EMSCRIPTEN_KEEPALIVE
void wasm_power_on(void) {
    halt_pending      = 0;
    memset(m.ram.mem, 0, m.ram.size_words * sizeof(m.ram.mem[0]));
    machine_bus_init(&m);
    machine_cpu_reset(&m);
    console_reset(&m.con);   /* KY11-LB power-up clear (§5.3.2) */
    power_on();
}

EMSCRIPTEN_KEEPALIVE
void wasm_power_off(void) {
    m.cpu.halted   = 1;
    halt_pending = 0;
    console_reset(&m.con);
}

EMSCRIPTEN_KEEPALIVE
void wasm_reset(void) {
    halt_pending      = 0;
    machine_bus_init(&m);
    machine_cpu_reset(&m);
    power_on();
}

EMSCRIPTEN_KEEPALIVE
void wasm_set_odt(void) {
    /* Prepare ODT boot (S1=0), cancels any pending direct boot. */
    m.con.switch_reg  = 0;
    direct_boot = 0;
    machine_set_s1(&m, 0);
}

EMSCRIPTEN_KEEPALIVE
void wasm_load_rk_image(int drive, const char *memfs_path) {
    if (drive < 0 || drive >= RK_NUMDR) return;
    rk05_attach(&m.rk, drive, memfs_path, 0);
    m.con.switch_reg  = 0146;   /* M9301-YF: boot DK, skip diagnostics */
    direct_boot = 0;
    machine_set_s1(&m, m.con.switch_reg);
}

EMSCRIPTEN_KEEPALIVE
void wasm_load_rl_image(int drive, const char *memfs_path) {
    if (drive < 0 || drive >= RL_NUMDR) return;
    rl11_attach(&m.rl, drive, memfs_path, 0);
    /* No M9301-YF boot code for RL, use direct boot or chain from RK */
}

EMSCRIPTEN_KEEPALIVE
int32_t wasm_load_lda(const char *memfs_path) {
    uint16_t start = 0;
    if (loader_lda(&m.ram, memfs_path, &start) < 0) return -1;
    direct_boot_pc = start;
    direct_boot    = 1;
    m.con.switch_reg = 0;
    machine_set_s1(&m, 0);
    return (int32_t)start;
}

EMSCRIPTEN_KEEPALIVE
int32_t wasm_load_asc(const char *memfs_path) {
    uint16_t start = 0;
    if (loader_asc(&m.ram, memfs_path, &start) < 0) return -1;
    direct_boot_pc = start;
    direct_boot    = 1;
    m.con.switch_reg = 0;
    machine_set_s1(&m, 0);
    return (int32_t)start;
}

EMSCRIPTEN_KEEPALIVE
void wasm_console_key(int key) {
    console_key(&m.con, key);
}

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_get_display_addr(void) {
    return m.con.display;
}

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_display_data(void) {
    return m.con.display & 0xFFFF;  /* compat, same as display low 16 bits */
}

/* LED bitmask: bit0=RUN, bit1=BUS_ERR, bit2=SR_DISP, bit3=MAINT, bit4=CNTRL */
EMSCRIPTEN_KEEPALIVE
uint8_t wasm_get_leds(void) {
    return (m.con.led_run      ? 1  : 0)
         | (m.con.led_bus_err  ? 2  : 0)
         | (m.con.led_sr_disp  ? 4  : 0)
         | (m.con.led_maint    ? 8  : 0)
         | (m.con.cntrl_active ? 16 : 0);
}

/* wasm_rx_push() and wasm_tx_poll() are in frontends/tty/tty_wasm.c */

/* --- JS API: debug console --- */
EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_reg(int n) { return REG(&m.cpu, n & 7); }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_psw(void)  { return m.cpu.psw; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_ir(void)   { return m.cpu.ir; }

EMSCRIPTEN_KEEPALIVE
int wasm_is_halted(void)     { return m.cpu.halted; }

EMSCRIPTEN_KEEPALIVE
double wasm_get_us(void)     { return (double)(m.cpu.ns_elapsed / 1000); }

EMSCRIPTEN_KEEPALIVE
void wasm_halt(void) {
    machine_halt(&m);
    halt_pending = 1;
}

EMSCRIPTEN_KEEPALIVE
void wasm_run(void) {
    halt_pending = 0;
    machine_resume(&m);
}

EMSCRIPTEN_KEEPALIVE
void wasm_step(void) {
    halt_pending = 0;
    machine_step(&m, 1);
}

EMSCRIPTEN_KEEPALIVE
void wasm_ustep_n(int n) {
    halt_pending = 0;
    machine_ustep(&m, n);
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
uint16_t *wasm_ram_ptr(void) { return m.ram.mem; }
EMSCRIPTEN_KEEPALIVE
uint32_t wasm_ram_words(void) { return m.ram.size_words; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_examine(uint32_t addr) {
    uint16_t val = 0;
    machine_examine(&m, addr & 0x3FFFFu, &val);
    return val;
}

EMSCRIPTEN_KEEPALIVE
void wasm_deposit(uint32_t addr, uint16_t val) {
    bus_write(&m.bus, addr & 0x3FFFFu, val, BUS_DATO);
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
uint16_t wasm_get_mpc(void)    { return m.cpu.mpc; }

#include "pdp11/kd11ea/ucode_labels.h"
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
uint32_t wasm_get_ba(void)     { return m.cpu.ba; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_b_reg(void)  { return m.cpu.b_reg; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_bx_reg(void) { return m.cpu.bx_reg; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_alu_out(void) { return m.cpu.alu_out; }

EMSCRIPTEN_KEEPALIVE
uint8_t wasm_get_alu_cout(void) { return m.cpu.alu_cout; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_udata(void)  { return m.cpu.unibus_data; }

/* --- JS API: MMU --- */

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_sr0(void) { return m.cpu.mmu.sr0; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_sr2(void) { return m.cpu.mmu.sr2; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_par(int n) {
    if (n < 0 || n > 15) return 0;
    return m.cpu.mmu.par[n];
}

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_mmu_pdr(int n) {
    if (n < 0 || n > 15) return 0;
    return m.cpu.mmu.pdr[n];
}

/* --- JS API: breakpoints --- */
EMSCRIPTEN_KEEPALIVE
int wasm_bp_set(uint16_t addr) {
    return machine_bp_set(&m, addr) < 0 ? -1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_bp_del(uint16_t addr) {
    return machine_bp_del(&m, machine_bp_find(&m, addr));
}

EMSCRIPTEN_KEEPALIVE
void wasm_bp_clear(void) { machine_bp_clear(&m); }

EMSCRIPTEN_KEEPALIVE
int wasm_bp_count(void) {
    int n = 0;
    for (int i = 0; i < MACHINE_MAX_BP; i++)
        n += m.bp_used[i];
    return n;
}

/* Address of the i-th breakpoint set */
EMSCRIPTEN_KEEPALIVE
uint16_t wasm_bp_addr(int i) {
    for (int slot = 0; slot < MACHINE_MAX_BP; slot++)
        if (m.bp_used[slot] && i-- == 0)
            return m.bp_addr[slot];
    return 0;
}

/* --- JS API: instruction trace --- */
EMSCRIPTEN_KEEPALIVE
int wasm_get_trace_count(void) { return m.trace_count; }

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_trace_pc(int n) {
    uint16_t pc = 0, ir = 0;
    machine_trace_get(&m, n, &pc, &ir);
    return pc;
}

EMSCRIPTEN_KEEPALIVE
uint16_t wasm_get_trace_ir(int n) {
    uint16_t pc = 0, ir = 0;
    machine_trace_get(&m, n, &pc, &ir);
    return ir;
}

EMSCRIPTEN_KEEPALIVE
void wasm_trace_clear(void) { machine_trace_clear(&m); }

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
const char *wasm_probe_chip_name(const char *chip) {
    const char *n = probe_chip_name(chip);
    return n ? n : "";
}

EMSCRIPTEN_KEEPALIVE
const char *wasm_probe_sig_group(int i) {
    const ProbeDef *d = probe_def_get(i);
    return (d && d->group) ? d->group : "";
}

/* Chip and pin of the signal driver ("E81:3"), or "" */
EMSCRIPTEN_KEEPALIVE
const char *wasm_probe_sig_loc(int i) {
    const ProbeDef *d = probe_def_get(i);
    return (d && d->loc) ? d->loc : "";
}

EMSCRIPTEN_KEEPALIVE
const char *wasm_probe_sig_alias(int i) {
    const ProbeDef *d = probe_def_get(i);
    return (d && d->alias) ? d->alias : "";
}

static int def_is_1bit(const ProbeDef *d) {
    return d && probe_is_1bit(d);
}

EMSCRIPTEN_KEEPALIVE
int wasm_probe_sig_is_1bit(int i) { return def_is_1bit(probe_def_get(i)); }

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
    return def_is_1bit(probe_def_get(probe_inst.signal_idx[i]));
}

EMSCRIPTEN_KEEPALIVE
int wasm_probe_set_trigger(const char *name, uint32_t value, uint32_t mask) {
    return probe_set_trigger(&probe_inst, name, value, mask);
}

EMSCRIPTEN_KEEPALIVE
void wasm_probe_clear_trigger(void) { probe_inst.trigger_signal = -1; }

/* 1 = fire when the condition becomes true, 0 = while it is true */
EMSCRIPTEN_KEEPALIVE
void wasm_probe_set_trigger_edge(int edge) { probe_set_trigger_edge(&probe_inst, edge); }

/* Index of the trigger sample (0 = oldest), -1 if none */
EMSCRIPTEN_KEEPALIVE
int wasm_probe_trigger_index(void) { return probe_trigger_index(&probe_inst); }

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
int wasm_probe_sample_count(void) { return (int)probe_valid_count(&probe_inst); }

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
    if (sample_idx < 0 || sig_idx < 0 || sig_idx >= probe_inst.nsignals) return 0;
    const ProbeSnapshot *snap = probe_get(&probe_inst, (uint32_t)sample_idx);
    if (!snap) return 0;

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
    const ProbeSnapshot *first = probe_get(&probe_inst, 0);
    const ProbeSnapshot *snap  = sample_idx < 0 ? NULL
                               : probe_get(&probe_inst, (uint32_t)sample_idx);
    if (!first || !snap) return 0.0;
    return (double)(snap->ns - first->ns);
}

/* Call while powered off/halted. Does not rescale time already elapsed. */
EMSCRIPTEN_KEEPALIVE
int wasm_set_timing_profile(int kind) {
    if (kind != TIMING_NOMINAL && kind != TIMING_AGED) return -1;
    return machine_set_timing_profile(&m, (TimingKind)kind);
}
EMSCRIPTEN_KEEPALIVE
int wasm_get_timing_profile(void) {
    return m.cpu.timing == &clock_aged ? TIMING_AGED : TIMING_NOMINAL;
}

int main(void) {
    MachineConfig cfg = { .mem_kb = 248 };
    if (machine_init(&m, &cfg) < 0)
        return 1;

    tty = tty_wasm_create();

    m.dl.rx_ready = dl11_rx_ready;
    m.dl.rx_read  = dl11_rx_read;
    m.dl.tx_write = dl11_tx_write;
    m.dl.io_ctx   = tty;
    m.host_tick = wasm_host_tick;

    probe_init(&probe_inst, probe_buf, PROBE_DEFAULT_DEPTH);
    m.cpu.probe = &probe_inst;

    m.cpu.halted = 1;

    last_frame_ms = emscripten_get_now();
    emscripten_set_main_loop(wasm_loop, 0, 1);
    return 0;
}
