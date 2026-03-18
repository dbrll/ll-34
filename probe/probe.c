/*
 * probe.c: Virtual logic analyzer backend
 *
 * Signal table, ring buffer capture with trigger, and value readout.
 * All output formatting is done by the CLI in main.c.
 */

#include <string.h>
#include <strings.h>
#include "probe.h"

/* Level 2 (derived) signals */

/* E85:8 (K2-6): NAND(IR[14:12]) */
static uint32_t eval_e85_pin8(const ProbeSnapshot *snap) {
    return ((snap->ir >> 12) & 7) != 7;
}

/* E53 combined enable: IR[14:12]=0, IR[6]=0, SM0, NOR(IR08,IR15) */
static uint32_t eval_e53_enable(const ProbeSnapshot *snap) {
    return !(snap->ir & 0x0040)
        && ((snap->ir >> 12) & 7) == 0
        && ((snap->ir >> 9) & 7) == 0
        && !((snap->ir & 0x0100) || (snap->ir & 0x8000));
}

/* E50:3 (K2-3): TBIT AND ENAB_TBIT */
static uint32_t eval_e50_pin3(const ProbeSnapshot *snap) {
    return ((snap->psw >> 4) & 1) && snap->enab_tbit;
}

/* E73 (K2-6): NOR(IR08, IR15) */
static uint32_t eval_e73(const ProbeSnapshot *snap) {
    return !((snap->ir & 0x0100) || (snap->ir & 0x8000));
}


/* Signal definition table (K1 = M8265 data path, K2 = M8266 control) */

#define D(chip, alias, desc, fld, sz, bit) \
    { chip, alias, desc, 1, offsetof(ProbeSnapshot, fld), sz, bit, NULL }

#define D_WHOLE(chip, alias, desc, fld, sz) \
    { chip, alias, desc, 1, offsetof(ProbeSnapshot, fld), sz, PROBE_BIT_WHOLE, NULL }

#define DERIVED(chip, alias, desc, fn) \
    { chip, alias, desc, 2, 0, 0, 0, fn }

#define ALIAS(name, desc, fld, sz) \
    { NULL, name, desc, 1, offsetof(ProbeSnapshot, fld), sz, PROBE_BIT_WHOLE, NULL }

static const ProbeDef probe_defs[] = {

    D("K2:E82:1", "ALU_S[0]",  "ALU S0 (via E81)",      alu_s, 1, 0),
    D("K2:E82:2", "ALU_S[1]",  "ALU S1 (via E81)",      alu_s, 1, 1),
    D("K2:E82:3", "ALU_S[2]",  "ALU S2 (via E81)",      alu_s, 1, 2),
    D("K2:E82:4", "ALU_S[3]",  "ALU S3 (via E81)",      alu_s, 1, 3),
    D("K2:E82:5", "ALU_M",     "ALU mode",               alu_mode, 1, 0),
    D("K2:E82:6", "ALU_CIN",   "ALU carry in (inv)",     alu_cin, 1, 0),
    D_WHOLE("K2:E82:*", NULL,   "E82 raw output",        e82_out, 1),

    D_WHOLE("K2:E87:*", NULL,   "E87 raw output",        e87_out, 1),

    D("K2:E102:1", NULL,  "BUT ALU_OUT→MPC[7]",   e102_out, 1, 0),
    D("K2:E102:2", NULL,  "BUT CC_N→MPC[6]",      e102_out, 1, 1),
    D("K2:E102:3", NULL,  "BUT SP15→MPC[3]",      e102_out, 1, 2),
    D("K2:E102:4", NULL,  "BUT BXREG01→MPC[5]",   e102_out, 1, 3),
    D("K2:E102:5", NULL,  "BUT BXREG00→MPC[4]",   e102_out, 1, 4),
    D("K2:E102:6", NULL,  "BUT COUNT05→MPC[2]",    e102_out, 1, 5),
    D("K2:E102:7", NULL,  "BUT CC_Z→MPC[1]",       e102_out, 1, 6),
    D("K2:E102:9", NULL,  "BUT IR09→MPC[0]",       e102_out, 1, 7),

    D("K2:E59:1", NULL,   "E59 O1 (ir_code[0])",   e59_out, 1, 0),
    D("K2:E59:2", NULL,   "E59 O2 (ir_code[1])",   e59_out, 1, 1),
    D("K2:E59:4", NULL,   "E59 O3 (ir_code[2])",   e59_out, 1, 2),
    D("K2:E59:5", NULL,   "E59 O4 (dest_gate)",    e59_out, 1, 3),

    D("K2:E60:1", "MPC[3]",  "SOP MPC bit 3",      e60_out, 1, 0),
    D("K2:E60:2", "MPC[4]",  "SOP MPC bit 4",      e60_out, 1, 1),
    D("K2:E60:4", "MPC[5]",  "SOP MPC bit 5",      e60_out, 1, 2),
    D("K2:E60:5", "MPC[6]",  "SOP MPC bit 6",      e60_out, 1, 3),

    D_WHOLE("K2:E63:*", NULL,  "E63 raw output",    e63_out, 1),

    D("K2:E68:1", "BYTE_L",   "E68 BYTE L",        e68_out, 1, 0),
    D("K2:E68:2", NULL,       "E68 CC_CODE[0]",    e68_out, 1, 1),
    D("K2:E68:4", NULL,       "E68 CC_CODE[1]",    e68_out, 1, 2),
    D("K2:E68:5", NULL,       "E68 CC_CODE[2]",    e68_out, 1, 3),

    D("K2:E69:1", NULL,   "E69 MPC[3]",            e69_out, 1, 0),
    D("K2:E69:2", NULL,   "E69 MPC[4]",            e69_out, 1, 1),
    D("K2:E69:4", "MOV_L","E69 MOV L",             e69_out, 1, 2),
    D("K2:E69:5", "SRC_H","E69 SRC H",             e69_out, 1, 3),

    D("K2:E70:1", NULL,   "E70 MPC[5]",            e70_out, 1, 0),
    D("K2:E70:2", NULL,   "E70 MPC[6]",            e70_out, 1, 1),
    D("K2:E70:4", NULL,   "E70 MPC[7]",            e70_out, 1, 2),
    D("K2:E70:5", NULL,   "E70 IR_CODE_00_L",      e70_out, 1, 3),

    D("K2:E71:1", NULL,   "E71 MPC[7] branch",     e71_out, 1, 0),

    D_WHOLE("K2:E74:*", NULL,  "E74 raw output",    e74_out, 1),

    D_WHOLE("K2:E61:*", NULL,  "E61 raw output",    e61_out, 1),

    D("K2:E62:1", NULL,   "E62 B_MODE_01_L",       e62_out, 1, 0),
    D("K2:E62:2", NULL,   "E62 B_MODE_00_L",       e62_out, 1, 1),
    D("K2:E62:4", NULL,   "E62 SERIAL_SHIFT",      e62_out, 1, 2),
    D("K2:E62:5", NULL,   "E62 ROT_CBIT",          e62_out, 1, 3),

    D_WHOLE("K2:E83:*", NULL,  "E83 raw output",    e83_out, 1),

    D("K2:E53:1", "IR_CODE[0]", "E53 ir_code[0]",  e53_out, 1, 0),
    D("K2:E53:2", "IR_CODE[1]", "E53 ir_code[1]",  e53_out, 1, 1),
    D("K2:E53:4", "IR_CODE[2]", "E53 ir_code[2]",  e53_out, 1, 2),
    D("K2:E53:5", "HALT_RQST",  "E53 HALT RQST L", e53_out, 1, 3),

    D("K2:E54:1", NULL,        "E54 START RESET",   e54_out, 1, 0),
    D("K2:E54:2", "ENAB_TBIT", "E54 ENAB TBIT",    enab_tbit, 1, 0),
    D("K2:E54:4", NULL,        "E54 DIS LOAD PSW",  e54_out, 1, 2),

    D("K2:E52:12", NULL,  "E52 code[0]",           e52_code, 1, 0),
    D("K2:E52:11", NULL,  "E52 code[1]",           e52_code, 1, 1),
    D("K2:E52:10", NULL,  "E52 code[2]",           e52_code, 1, 2),
    D("K2:E52:9",  NULL,  "E52 code[3]",           e52_code, 1, 3),

    D("K2:E51:1", NULL,   "E51 PFAIL SERV",        e51_out, 1, 0),
    D("K2:E51:3", NULL,   "E51 STOV SERV",         e51_out, 1, 2),
    D("K2:E51:5", NULL,   "E51 C4 (vec bit 4)",    e51_out, 1, 4),
    D("K2:E51:6", NULL,   "E51 C3 (vec bit 3)",    e51_out, 1, 5),
    D("K2:E51:7", NULL,   "E51 C2 (vec bit 2)",    e51_out, 1, 6),
    D("K2:E51:9", NULL,   "E51 ENAB GRANTS",       e51_out, 1, 7),

    D("K2:E80:1", "SWAP",  "E80 SWAP H",           e80_out, 1, 0),
    D("K2:E80:2", "SEX",   "E80 SEX H",            e80_out, 1, 1),
    D("K2:E80:4", NULL,    "E80 DISAB MSYN L",     e80_out, 1, 2),
    D("K2:E80:5", NULL,    "E80 DIS UPPER BYTE",   e80_out, 1, 3),

    D("K1:E107:1", "CC_V",  "E107 overflow",       e107_out, 1, 0),
    D("K1:E107:2", "CC_C",  "E107 carry",          e107_out, 1, 1),

    D_WHOLE("K1:E1-4:F", "ALU_OUT", "ALU result (16-bit)", alu_out, 2),
    D("K1:E5:7",  "ALU_COUT", "ALU carry out (CLA)", alu_cout, 1, 0),

    DERIVED("K2:E85:8",  NULL,       "NAND(IR14,13,12) E53 /E2",  eval_e85_pin8),
    DERIVED("K2:E85:*",  "E53_EN",   "E53 full enable",           eval_e53_enable),
    DERIVED("K2:E50:3",  "TBIT_TRAP","TBIT AND ENAB_TBIT",        eval_e50_pin3),
    DERIVED("K2:E73:*",  NULL,       "NOR(IR08, IR15)",            eval_e73),

    ALIAS("MPC",       "microprogram counter (9-bit)",   mpc, 2),
    ALIAS("NEXT_MPC",  "next MPC (9-bit)",               next_mpc, 2),
    ALIAS("IR",        "instruction register (16-bit)",   ir, 2),
    ALIAS("PSW",       "processor status word",           psw, 2),
    ALIAS("ALU_S",     "ALU function S3:S0",              alu_s, 1),
    ALIAS("ALU_M",     "ALU mode",                        alu_mode, 1),
    ALIAS("A_LEG",     "ALU A input",                     a_leg, 2),
    ALIAS("B_LEG",     "ALU B input",                     b_leg, 2),
    ALIAS("BA",        "bus address (18-bit)",             ba, 4),
    ALIAS("DATA",      "UNIBUS data",                     unibus_data, 2),
    ALIAS("BREG",      "B shift register",                b_reg, 2),
    ALIAS("BXREG",     "BX shift register",               bx_reg, 2),
    ALIAS("PC",        "R7/PC",                            pc, 2),
    ALIAS("SP",        "R6/SP (kernel)",                   sp, 2),
    ALIAS("R0",        "R0",                               r0, 2),
    ALIAS("R1",        "R1",                               r1, 2),
    ALIAS("IR_CODE",   "trap code (3-bit)",                ir_code, 1),
    ALIAS("SVC_TRAP",  "service trap flag",                service_trap, 1),
    ALIAS("BUS_ERR",   "bus error (NXM)",                  bus_error, 1),
    ALIAS("BUS_OP",    "bus operation (0/1/2)",            bus_op, 1),
    ALIAS("HALTED",    "CPU halted flag",                  halted, 1),
    ALIAS("LONG_CYC",  "long cycle active",                long_cycle, 1),
    ALIAS("BUF_TRAN",  "BUF_DAT_TRAN",                    buf_dat_tran, 1),
    ALIAS("IR_VALID",  "IR valid for decode",              ir_valid, 1),
    ALIAS("CYCLE_NS",  "cycle duration (ns)",              cycle_ns, 4),
    ALIAS("SSYN_NS",   "slave SSYN response (ns)",        ssyn_ns, 2),
    ALIAS("EOT_NS",    "EOT delay (ns)",                  eot_ns, 2),
    ALIAS("NS",        "simulation time (ns)",             ns, 8),
};

#define PROBE_DEF_COUNT  (int)(sizeof(probe_defs) / sizeof(probe_defs[0]))



int probe_def_count(void) {
    return PROBE_DEF_COUNT;
}

const ProbeDef *probe_def_get(int idx) {
    if (idx < 0 || idx >= PROBE_DEF_COUNT)
        return NULL;
    return &probe_defs[idx];
}

const char *probe_display_name(const ProbeDef *def) {
    if (def->chip_pin)
        return def->chip_pin;
    return def->alias;
}

int probe_find_signal(const char *name) {
    for (int i = 0; i < PROBE_DEF_COUNT; i++) {
        if (probe_defs[i].chip_pin && strcasecmp(name, probe_defs[i].chip_pin) == 0)
            return i;
        if (probe_defs[i].alias && strcasecmp(name, probe_defs[i].alias) == 0)
            return i;
    }
    return -1;
}



uint32_t probe_read_value(const ProbeSnapshot *snap, const ProbeDef *def) {
    if (def->level == 2) {
        if (def->eval)
            return def->eval(snap);
        return 0;
    }

    /* Level 1: direct read from snapshot */
    const uint8_t *base = (const uint8_t *)snap + def->offset;
    uint32_t val;

    switch (def->size) {
    case 1: val = *(const uint8_t *)base; break;
    case 2: val = *(const uint16_t *)base; break;
    case 4: val = *(const uint32_t *)base; break;
    default: val = 0; break;
    }

    if (def->bit != PROBE_BIT_WHOLE)
        val = (val >> def->bit) & 1;

    return val;
}



void probe_init(Probe *p, ProbeSnapshot *buf, uint32_t depth) {
    memset(p, 0, sizeof(*p));
    p->buf = buf;
    p->depth = depth;
    p->mask = depth - 1;
    p->trigger_signal = -1;
    p->trigger_pos_pct = 50;
    p->divider = 1;
    p->state = PROBE_IDLE;
}

void probe_reset(Probe *p) {
    p->state = PROBE_IDLE;
    p->head = 0;
    p->count = 0;
    p->trigger_sample = 0;
    p->post_trigger_remaining = 0;
}



int probe_add_signal(Probe *p, const char *name) {
    if (p->nsignals >= PROBE_MAX_SIGNALS)
        return -1;
    int idx = probe_find_signal(name);
    if (idx < 0)
        return -1;
    /* Check for duplicates */
    for (int i = 0; i < p->nsignals; i++) {
        if (p->signal_idx[i] == idx)
            return 0;  /* already added */
    }
    p->signal_idx[p->nsignals++] = idx;
    return 0;
}

int probe_rm_signal(Probe *p, const char *name) {
    int idx = probe_find_signal(name);
    if (idx < 0)
        return -1;
    for (int i = 0; i < p->nsignals; i++) {
        if (p->signal_idx[i] == idx) {
            /* Shift remaining signals down */
            for (int j = i; j < p->nsignals - 1; j++)
                p->signal_idx[j] = p->signal_idx[j + 1];
            p->nsignals--;
            return 0;
        }
    }
    return -1;
}

void probe_clear_signals(Probe *p) {
    p->nsignals = 0;
}

int probe_set_trigger(Probe *p, const char *signal, uint32_t value, uint32_t mask) {
    int idx = probe_find_signal(signal);
    if (idx < 0)
        return -1;
    p->trigger_signal = idx;
    p->trigger_value = value;
    p->trigger_mask = mask;
    return 0;
}

void probe_set_depth(Probe *p, uint32_t depth) {
    /* Clamp to power of 2 and max */
    if (depth > PROBE_MAX_DEPTH)
        depth = PROBE_MAX_DEPTH;
    /* Round down to power of 2 */
    uint32_t d = 1;
    while (d * 2 <= depth)
        d *= 2;
    p->depth = d;
    p->mask = d - 1;
}

void probe_set_trigger_pos(Probe *p, int pct) {
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    p->trigger_pos_pct = pct;
}



void probe_arm(Probe *p) {
    p->head = 0;
    p->count = 0;
    p->trigger_sample = 0;
    p->pre_trigger_count = (p->depth * p->trigger_pos_pct) / 100;
    p->post_trigger_remaining = 0;

    p->div_counter = p->divider;

    if (p->trigger_signal < 0) {
        /* No trigger, immediate capture (free run) */
        p->state = PROBE_CAPTURING;
        p->post_trigger_remaining = p->depth;
    } else {
        p->state = PROBE_ARMED;
    }
}

void probe_disarm(Probe *p) {
    p->state = PROBE_IDLE;
}



void probe_sample_slow(Probe *p, const ProbeSnapshot *snap) {
    if (p->state == PROBE_ARMED) {
        /* Check trigger every ustep (don't miss it) */
        const ProbeDef *trig = &probe_defs[p->trigger_signal];
        uint32_t val = probe_read_value(snap, trig);
        int triggered = (val & p->trigger_mask) == (p->trigger_value & p->trigger_mask);

        /* Store sample only on divider tick */
        if (--p->div_counter == 0) {
            p->div_counter = p->divider;
            p->buf[p->head & p->mask] = *snap;
            p->head++;
            p->count++;
        }

        if (triggered) {
            p->state = PROBE_CAPTURING;
            p->trigger_sample = ((p->head > 0 ? p->head : 1) - 1) & p->mask;
            p->post_trigger_remaining = p->depth - p->pre_trigger_count;
            p->div_counter = p->divider;  /* reset divider on trigger */
            if (p->post_trigger_remaining == 0) {
                p->state = PROBE_DONE;
            }
        }
        return;
    }

    if (p->state == PROBE_CAPTURING) {
        if (--p->div_counter == 0) {
            p->div_counter = p->divider;
            p->buf[p->head & p->mask] = *snap;
            p->head++;
            p->count++;
            p->post_trigger_remaining--;

            if (p->post_trigger_remaining == 0) {
                p->state = PROBE_DONE;
            }
        }
        return;
    }
}
