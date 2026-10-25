/*
 * probe.h: Virtual logic analyzer for ll-34
 *
 * Ring buffer capture with trigger. Signals in KD1:Exx:pin notation.
 * Level 1 = direct (snapshot fields), Level 2 = derived (glue logic).
 * Backend only. The front ends (frontends/cli, frontends/wasm) display it.
 */

#ifndef PROBE_H
#define PROBE_H

#include <stdint.h>
#include <stddef.h>

/* ProbeSnapshot: the wires just before the PROC CLK edge ending the
 * micro-cycle. Registers show their value before that edge, while pc, sp,
 * r0-r5 are read after the step. */

typedef struct {
    /* Timestamp */
    uint64_t ns;                /* absolute simulation time */

    /* Control store: micro-word being executed (latched outputs) and the
     * one at the next MPC (ROM outputs before the latches) */
    uint64_t uw;
    uint64_t uw_next;
    uint8_t  count;             /* EIS counter (K2-10 E79/E78) */

    /* Data path (K1-1 to K1-4) */
    uint16_t amux_out;          /* AMUX output */
    uint16_t ss_out;            /* SSMUX output (scratchpad and B/BX input) */
    uint8_t  slice_cout;        /* carries out of bits 3, 7, 11 (bits 0-2) */

    /* K1-10 */
    uint8_t  byte_mux;          /* BYTE MUX on bit 7 (K2-5 BYTE L asserted) */
    uint8_t  ovx;               /* OVX flip-flop */
    uint8_t  shift_in;          /* SHIFT IN B (0), SHIFT IN BX (1), SHIFT IN 07 (2) */
    uint8_t  b_mode;            /* B MODE 01 L (0), B MODE 00 L (1): line levels */
    uint8_t  func_code;         /* K2-8 FUNC CODE 03-00 (bits 3-0), 0xF when not driven */
    uint8_t  rot_cbit;          /* K2-5 ROT CBIT (1) H, E55 */
    uint8_t  cc_wires;          /* CC N, Z, V, C H (bits 3-0) */

    /* K1-6 to K1-9: bus address and memory management */
    uint16_t vba;               /* virtual bus address latch */
    uint8_t  mmu_mode;          /* K1-8 MODE 01/00 */
    uint16_t par_out;           /* PAR of the page addressed by mode and VBA 15-13 */
    uint16_t pdr_out;           /* PDR of the same page */
    uint16_t sr0;
    uint16_t sr2;

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
    uint16_t ir;                /* instruction register (K2-5 E33 E56 E66 E67) */
    uint16_t psw;               /* processor status word */
    uint16_t b_reg;             /* B shift register (K1 E10 E31 E21 E41) */
    uint16_t bx_reg;            /* BX shift register (K1 E11 E32 E22 E52) */
    uint16_t pc;                /* R7 */
    uint16_t sp;                /* R6 (kernel) */
    uint16_t sp_user;           /* R6 (user) */
    uint16_t r0, r1, r2, r3, r4, r5;  /* general registers */

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
    uint8_t  kte;               /* KTE (1) H at the start of the cycle */
    uint8_t  dbe;               /* DBE H (K2-3) */

    /* Priority arbitration (K2-2) */
    uint8_t  br_lines;          /* BR7-BR4 latched in E19, bits 7-4 */
    uint8_t  e29_out;           /* E29 outputs */
    uint8_t  bus_grant;         /* BG (1) H: a grant was sent this cycle */
    uint8_t  intr;              /* BUS INTR L asserted (vector on D lines) */
    uint8_t  bus_init_l;        /* BUS INIT L level after the cycle */
    uint8_t  reset_pulse;       /* RESET (1) H fired in this cycle */

    /* DAT TRAN (K2-1) and clock (K1-5) */
    uint8_t  bus_c;             /* UBUS C1/C0 of the transfer */
    uint8_t  msyn;              /* BUS MSYN L asserted */
    uint8_t  ssyn;              /* BUS SSYN L returned */
    uint8_t  timeout;           /* TO (1) H */
    uint8_t  abort_h;           /* ABORT H */
    uint8_t  set_be;            /* SET BE L asserted */
    uint8_t  dis_msyn_p1;       /* DISABLE MSYN+1 L asserted (K2-4) */
    uint8_t  clk_long;          /* long cycle selected (E95/E30) */

    /* Scratchpad address (K2-4) */
    uint8_t  spa;               /* SPA 03-00 during the read, 15 - slot */
    uint8_t  spa_wr;            /* SPA 03-00 during the write */
    uint8_t  sp_write;          /* SP WRITE L asserted */

    /* Cycle timing (K1-5 delay line model) */
    uint32_t cycle_ns;
    uint16_t ssyn_ns;           /* slave SSYN response time (0 if no bus) */
    uint16_t eot_ns;            /* EOT delay: 100 for DATI, 0 for DATO */

    /* ROM input pin levels, recorded before service clears its inputs. */
    uint16_t e52_inputs;
    uint8_t  e53_inputs;
    uint8_t  e53_selects;
    uint8_t  e54_inputs;
    uint8_t  e80_inputs;

    /* ROM raw outputs, for pin-level probing */
    uint8_t  e51_out;           /* E51 service dispatch (K2-3) */
    uint8_t  e52_code;          /* E52 service priority (K2-3) */
    uint8_t  e53_out;           /* E53 trap decode (K2-6) */
    uint8_t  e54_out;           /* E54 reset/trap ctrl (K2-6) */
    uint8_t  e59_out;           /* E59 SOP decode low (K2-5) */
    uint8_t  e60_out;           /* E60 SOP decode high (K2-5) */
    uint8_t  e62_out;           /* E62 ROT/SHIFT (K2-5) */
    uint8_t  e63_out;           /* E63 operates (K2-5) */
    uint8_t  e68_out;           /* E68 CC categorizing (K2-5) */
    uint8_t  e69_out;           /* E69 DOP low (K2-5) */
    uint8_t  e70_out;           /* E70 DOP high (K2-5) */
    uint8_t  e71_out;           /* E71 branch decode (K2-6) */
    uint8_t  e74_out;           /* E74 EIS decode (K2-5) */
    uint8_t  e80_out;           /* E80 byte control (K2-8) */
    uint8_t  e82_out;           /* E82 ALU decode (K2-8) */
    uint8_t  e87_out;           /* E87 B/BX mode (K2-4) */
    uint8_t  e102_out;          /* E102 BUT decode (K2-2) */
    uint8_t  e107_out;          /* E107 CC V+C (K1-10) */
} ProbeSnapshot;


/* ProbeDef: one per probe point */

#define PROBE_BIT_WHOLE  0xFF   /* whole field, not a single bit */

typedef enum {
    PROBE_SNAP,     /* snapshot field, whole or one bit */
    PROBE_EVAL,     /* computed by eval() */
    PROBE_UW,       /* bit of the micro-word being executed */
    PROBE_UW_NEXT,  /* bit of the micro-word at the next MPC */
    PROBE_MISC,     /* K2-7 E89 output: low when the MISC field equals bit */
    PROBE_ZERO4,    /* 1 when the 4 bits of the field from bit up are all 0 */
} ProbeKind;

typedef struct {
    const char *name;           /* DEC signal name "K2-8 ALU S3 H", or chip
                                 * output "K2:E102:1", or NULL (alias only) */
    const char *loc;            /* chip and pin, e.g. "E81:3", or NULL */
    const char *alias;          /* "ALU_S[3]" or NULL */
    const char *desc;           /* short description */
    uint8_t    kind;            /* ProbeKind */

    /* PROBE_SNAP: offset and size of the field, bit or PROBE_BIT_WHOLE.
     * PROBE_UW, PROBE_UW_NEXT: bit of the micro-word. PROBE_MISC: code. */
    uint16_t   offset;
    uint8_t    size;            /* 1, 2, 4 or 8 bytes */
    uint8_t    bit;
    uint8_t    inv;             /* one-bit signal read inverted */

    /* PROBE_EVAL */
    uint32_t (*eval)(const ProbeSnapshot *snap);

    /* Group of an alias-only signal ("Registers", "Unibus"...), NULL for
     * signals named after the schematics */
    const char *group;
} ProbeDef;


/* Probe: analyzer state */

#define PROBE_MAX_SIGNALS  16
#define PROBE_MAX_DEPTH    262144  /* must be power of 2 */

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
    int          trigger_edge;      /* 1 = fire when the condition becomes true */
    int          trigger_pos_pct;   /* 0..100, default 50 */
    uint32_t     divider;           /* sample every N-th ustep (1=all) */

    /* State */
    ProbeState   state;
    uint32_t     trigger_sample;
    uint32_t     pre_trigger_count;
    uint32_t     post_trigger_remaining;
    uint32_t     div_counter;       /* counts down to next sample */
    int          trigger_prev;      /* condition at the previous ustep, -1 = unknown */
    int          triggered;
    uint32_t     trigger_abs;       /* sample number (head count) at trigger */

    /* Ring buffer (externally allocated) */
    ProbeSnapshot *buf;
    uint32_t     capacity;          /* allocated snapshots, independent of depth */
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
/* Clamp to allocated capacity and the global limit, then round down to a
 * power of two. A depth change discards the capture and disarms the probe. */
void probe_set_depth(Probe *p, uint32_t depth);
void probe_set_trigger_pos(Probe *p, int pct);
void probe_set_trigger_edge(Probe *p, int edge);
void probe_arm(Probe *p);
void probe_disarm(Probe *p);

void probe_sample_slow(Probe *p, const ProbeSnapshot *snap);

static inline void probe_sample(Probe *p, const ProbeSnapshot *snap) {
    if (p->state == PROBE_IDLE || p->state == PROBE_DONE)
        return;     /* fast exit, ~1ns */
    probe_sample_slow(p, snap);
}

/* Captured samples: count, access by index (0 = oldest), and index of the
 * trigger sample (-1 if no trigger or if it was overwritten). */
uint32_t probe_valid_count(const Probe *p);
const ProbeSnapshot *probe_get(const Probe *p, uint32_t idx);
int probe_trigger_index(const Probe *p);

/* Name of a probed chip ("K2:E62" -> "Rotate and shift control") or title
 * of a sheet ("K2-8" -> "Micro store"), or NULL */
const char *probe_chip_name(const char *chip);

int probe_def_count(void);
const ProbeDef *probe_def_get(int idx);
int probe_find_signal(const char *name);  /* -1 unknown, -2 ambiguous */
const char *probe_def_board(const ProbeDef *def);  /* "K1", "K2" or NULL */
const char *probe_def_qloc(int idx);       /* "K2:E97:1" or NULL */
uint32_t probe_read_value(const ProbeSnapshot *snap, const ProbeDef *def);
int probe_is_1bit(const ProbeDef *def);
const char *probe_display_name(const ProbeDef *def);

/* Write the capture as a sigrok session file at rate_hz (0 = 50 MHz), or as
 * VCD. Return the number of samples (sigrok) or micro-cycles (VCD) written,
 * or -1 with a message in err. */
int probe_export_sr(const Probe *p, const char *path, uint32_t rate_hz, char *err, int errlen);
int probe_export_vcd(const Probe *p, const char *path, char *err, int errlen);

#endif /* PROBE_H */
