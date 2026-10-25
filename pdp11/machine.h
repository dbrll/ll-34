/*
 * machine.h -- PDP-11/34A system assembly for ll-34
 *
 * Builds the machine (CPU, bus, memory, boot ROM, peripherals, panel)
 * and provides the operations shared by all frontends. The frontend
 * owns the execution loop and the host I/O.
 */

#ifndef MACHINE_H
#define MACHINE_H

#include <stdint.h>
#include "unibus/unibus.h"
#include "unibus/ram.h"
#include "kd11ea/kd11ea.h"
#include "ky11/console.h"
#include "m9301/rom.h"
#include "dl11/dl11.h"
#include "rk11/rk11.h"
#include "rl11/rl11.h"
#include "kw11/kw11.h"
#include "pc11/pc11.h"

#define MACHINE_TICK_USTEPS  256   /* micro-steps between device ticks */
#define MACHINE_MAX_BP       16
#define MACHINE_TRACE_SIZE   512   /* power of 2 */

typedef struct {
    unsigned mem_kb;    /* memory size, 248 KB max */
    TimingKind timing;  /* zero-init = nominal (DEC values), AGED = measured */
    unsigned rtc_hz;    /* line clock, 50 or 60, 0 = 60 */
} MachineConfig;

/* Why machine_run() returned */
typedef enum {
    MACHINE_STOP_BUDGET,        /* time budget used up */
    MACHINE_STOP_HALT,          /* CPU halted by itself (HALT, error) */
    MACHINE_STOP_BREAKPOINT,    /* breakpoint hit, slot in bp_hit */
    MACHINE_STOP_STEP,          /* instruction or micro-step count reached */
    MACHINE_STOP_HOST,          /* host_tick asked to stop (instruction end) */
} MachineStop;

typedef struct {
    KD11EA    cpu;
    Console   con;      /* KY11-LB programmer console */
    Bus       bus;
    Ram       ram;
    Rom       rom;      /* M9301-YF */
    DL11      dl;
    RK05      rk;
    RL11      rl;
    KW11      kw;
    PC11      pc;

    /* Called before each device tick, from machine_run(). The frontend
     * polls its host I/O here. A non-zero return stops the run at the end
     * of the current instruction, as the front panel HALT does. */
    int     (*host_tick)(void *ctx);
    void     *host_ctx;
    int       tick_counter;
    int       host_stop;            /* stop asked, waiting for the boundary */
    uint32_t  host_stop_usteps;     /* micro-steps since then */

    /* Debugger */
    uint16_t  bp_addr[MACHINE_MAX_BP];
    uint8_t   bp_used[MACHINE_MAX_BP];
    int       bp_hit;               /* slot of the last breakpoint hit */
    int       step_instr;           /* stop after N instructions */
    int       step_usteps;          /* stop after N micro-steps */

    /* Instruction trace: PC and IR of the last instructions fetched */
    uint16_t  trace_pc[MACHINE_TRACE_SIZE];
    uint16_t  trace_ir[MACHINE_TRACE_SIZE];
    int       trace_head;
    int       trace_count;
} Machine;

/* Build the machine and register all devices on the bus.
 * Returns 0, or -1 on error. */
int  machine_init(Machine *m, const MachineConfig *cfg);

/* BUS INIT (BINIT L): reset the peripherals and the interrupt requests. */
void machine_bus_init(Machine *m);

/* Reset the CPU state and cancel any pending step. Simulated time,
 * the probe, breakpoints and trace are kept. */
void machine_cpu_reset(Machine *m);
/* Administrative selection, CPU halted, no register/RAM/time reset. */
int machine_set_timing_profile(Machine *m, TimingKind timing);
int machine_set_rtc_hz(Machine *m, unsigned hz);

/* Power-up sequence: fetch the PC/PS vector from the M9301 (773024). */
void machine_power_on(Machine *m);

/* Set the M9301 S1 boot switches. */
void machine_set_s1(Machine *m, uint16_t s1);

/* Advance the peripherals to the current CPU time.
 * Called every MACHINE_TICK_USTEPS micro-steps. */
void machine_tick(Machine *m);

/* Read a word at an 18-bit physical address without side effects
 * on the bus error flag. Returns 0, or -1 on NXM. */
int  machine_examine(Machine *m, uint32_t pa, uint16_t *data);

/* Run the CPU for up to budget_ns of simulated time. Breakpoints and
 * instruction steps are checked at instruction boundaries (MPC 000 before
 * the fetch), so PC holds the address of the next instruction when the
 * run stops there. On every stop other than BUDGET and HOST, the CPU is
 * left halted. */
MachineStop machine_run(Machine *m, uint64_t budget_ns);

/* While halted: let the peripherals make progress. */
void machine_idle(Machine *m);

void machine_halt(Machine *m);
void machine_resume(Machine *m);
void machine_step(Machine *m, int n_instr);    /* run N instructions */
void machine_ustep(Machine *m, int n_usteps);  /* run N micro-steps */

/* Breakpoints on 16-bit virtual PC values. machine_bp_set() returns the
 * slot (an existing one if the address is already set), or -1 if full. */
int  machine_bp_set(Machine *m, uint16_t addr);
int  machine_bp_find(const Machine *m, uint16_t addr);
int  machine_bp_del(Machine *m, int slot);
void machine_bp_clear(Machine *m);

/* Trace entry n, 0 = oldest. Returns -1 if out of range. */
int  machine_trace_get(const Machine *m, int n, uint16_t *pc, uint16_t *ir);
void machine_trace_clear(Machine *m);

/* Run 1M micro-steps in ODT and return the host time per micro-step in ns.
 * The machine is reset afterwards, ready to boot with the given S1. */
double machine_benchmark(Machine *m, uint16_t s1);

#endif /* MACHINE_H */
