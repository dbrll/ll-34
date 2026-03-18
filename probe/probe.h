/*
 * probe.h: Virtual logic analyzer for ll-34
 *
 * Ring buffer capture with trigger. Signals in KD1:Exx:pin notation.
 * Level 1 = direct (snapshot fields), Level 2 = derived (glue logic).
 * Backend only, CLI in main.c.
 */

#ifndef PROBE_H
#define PROBE_H

#include <stdint.h>
#include <stddef.h>

/* ProbeSnapshot: ~88 bytes per micro-step, 64K entries = ~5.6 MB */

typedef struct {
    /* Timestamp */
    uint64_t ns;                /* absolute simulation time */

    /* Micro-sequencer */
    uint16_t mpc;               /* MPC at entry */
    uint16_t next_mpc;          /* MPC at exit */

    /* ALU (E1-E4 74S181 × 4, E5 74S182 CLA) */
    uint16_t a_leg;             /* ALU A input (from scratchpad) */
    uint16_t b_leg;             /* ALU B input (from BMUX) */
    uint16_t alu_out;           /* ALU result */

    /* Bus */
    uint32_t ba;                /* bus address (18-bit PBA) */
    uint16_t unibus_data;       /* bus data */

    /* Registers */
    uint16_t ir;                /* instruction register (E46/E47) */
    uint16_t psw;               /* processor status word */
    uint16_t b_reg;             /* B shift register (E12-E15) */
    uint16_t bx_reg;            /* BX shift register (E7-E10) */
    uint16_t pc;                /* R7 */
    uint16_t sp;                /* R6 (kernel) */
    uint16_t r0, r1;            /* general registers */

    /* ALU control (from E82 decode) */
    uint8_t  alu_s;             /* ALU function S3:S0 */
    uint8_t  alu_mode;          /* ALU M (0=arith, 1=logic) */
    uint8_t  alu_cin;           /* ALU carry in */
    uint8_t  alu_cout;          /* ALU carry out */

    /* Bus control */
    uint8_t  bus_op;            /* 0=none, 1=DATI, 2=DATO */
    uint8_t  long_cycle;        /* LONG_CYCLE_L active (1=long) */
    uint8_t  buf_dat_tran;      /* BUF_DAT_TRAN */

    /* Trap/decode */
    uint8_t  ir_valid;
    uint8_t  ir_code;           /* E53 ir_code[2:0] */
    uint8_t  service_trap;
    uint8_t  bus_error;
    uint8_t  halted;
    uint8_t  enab_tbit;         /* E54 ENAB TBIT */

    /* Cycle timing (K1-5 delay line model) */
    uint32_t cycle_ns;
    uint16_t ssyn_ns;           /* slave SSYN response time (0 if no bus) */
    uint16_t eot_ns;            /* EOT delay: 100 for DATI, 0 for DATO */

    /* ROM raw outputs, for pin-level probing */
    uint8_t  e51_out;           /* E51 service dispatch (K2-3) */
    uint8_t  e52_code;          /* E52 service priority (K2-3) */
    uint8_t  e53_out;           /* E53 trap decode (K2-6) */
    uint8_t  e54_out;           /* E54 reset/trap ctrl (K2-6) */
    uint8_t  e59_out;           /* E59 SOP decode low (K2-5) */
    uint8_t  e60_out;           /* E60 SOP decode high (K2-5) */
    uint8_t  e61_out;           /* E61 SOP func code (K2-5) */
    uint8_t  e62_out;           /* E62 ROT/SHIFT (K2-5) */
    uint8_t  e63_out;           /* E63 operates (K2-5) */
    uint8_t  e68_out;           /* E68 CC categorizing (K2-6) */
    uint8_t  e69_out;           /* E69 DOP low (K2-5) */
    uint8_t  e70_out;           /* E70 DOP high (K2-5) */
    uint8_t  e71_out;           /* E71 branch decode (K2-6) */
    uint8_t  e74_out;           /* E74 EIS decode (K2-5) */
    uint8_t  e80_out;           /* E80 byte control (K2-8) */
    uint8_t  e82_out;           /* E82 ALU decode (K2-8) */
    uint8_t  e83_out;           /* E83 DOP func code (K2-5) */
    uint8_t  e87_out;           /* E87 B/BX mode (K2-4) */
    uint8_t  e102_out;          /* E102 BUT decode (K2-2) */
    uint8_t  e107_out;          /* E107 CC V+C (K1-10) */
} ProbeSnapshot;


/* ProbeDef: one per probe point */

#define PROBE_BIT_WHOLE  0xFF   /* whole field, not a single bit */

typedef struct {
    const char *chip_pin;       /* "KD1:E60:5" or NULL (alias-only) */
    const char *alias;          /* "MPC[3]" or NULL */
    const char *desc;           /* short description */
    uint8_t    level;           /* 1=direct, 2=derived */

    /* Level 1: direct read from snapshot */
    uint16_t   offset;          /* offsetof(ProbeSnapshot, field) */
    uint8_t    size;            /* 1, 2, or 4 bytes */
    uint8_t    bit;             /* bit position, or PROBE_BIT_WHOLE */

    /* Level 2: derived, eval function pointer */
    uint32_t (*eval)(const ProbeSnapshot *snap);
} ProbeDef;


/* Probe: analyzer state */

#define PROBE_MAX_SIGNALS  16
#define PROBE_MAX_DEPTH    65536  /* must be power of 2 */

typedef enum {
    PROBE_IDLE,             /* no capture configured */
    PROBE_ARMED,            /* waiting for trigger */
    PROBE_CAPTURING,        /* trigger fired, filling post-trigger */
    PROBE_DONE              /* buffer full, ready for dump */
} ProbeState;

typedef struct Probe {
    /* Configuration */
    int          signal_idx[PROBE_MAX_SIGNALS];
    int          nsignals;
    uint32_t     depth;
    int          trigger_signal;    /* index in probe_defs[], -1 = free run */
    uint32_t     trigger_value;
    uint32_t     trigger_mask;
    int          trigger_pos_pct;   /* 0..100, default 50 */
    uint32_t     divider;           /* sample every N-th ustep (1=all) */

    /* State */
    ProbeState   state;
    uint32_t     trigger_sample;
    uint32_t     pre_trigger_count;
    uint32_t     post_trigger_remaining;
    uint32_t     div_counter;       /* counts down to next sample */

    /* Ring buffer (externally allocated) */
    ProbeSnapshot *buf;
    uint32_t     head;
    uint32_t     count;
    uint32_t     mask;              /* depth - 1 (for ring wrap) */
} Probe;


/* Initialize probe (buf must hold depth ProbeSnapshots) */
void probe_init(Probe *p, ProbeSnapshot *buf, uint32_t depth);

void probe_reset(Probe *p);

int  probe_add_signal(Probe *p, const char *name);

int  probe_rm_signal(Probe *p, const char *name);
void probe_clear_signals(Probe *p);
int  probe_set_trigger(Probe *p, const char *signal, uint32_t value, uint32_t mask);
void probe_set_depth(Probe *p, uint32_t depth);
void probe_set_trigger_pos(Probe *p, int pct);
void probe_arm(Probe *p);
void probe_disarm(Probe *p);

void probe_sample_slow(Probe *p, const ProbeSnapshot *snap);

static inline void probe_sample(Probe *p, const ProbeSnapshot *snap) {
    if (p->state == PROBE_IDLE || p->state == PROBE_DONE)
        return;     /* fast exit, ~1ns */
    probe_sample_slow(p, snap);
}

int probe_def_count(void);
const ProbeDef *probe_def_get(int idx);
int probe_find_signal(const char *name);
uint32_t probe_read_value(const ProbeSnapshot *snap, const ProbeDef *def);
const char *probe_display_name(const ProbeDef *def);

#endif /* PROBE_H */
