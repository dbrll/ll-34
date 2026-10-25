/*
 * probe.c: Virtual logic analyzer backend
 *
 * Signal table, ring buffer capture with trigger, and value readout.
 * All output formatting is done by the front ends.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "probe.h"
#include "../kd11ea/kd11ea.h"

/* Level 2 (derived) signals */

/* E58:8 (K2-6): NAND(IR 14-12) */
static uint32_t eval_e85_pin8(const ProbeSnapshot *snap) {
    return ((snap->ir >> 12) & 7) != 7;
}

/* E53 enabled: IR 06 low and E85:6 low (IR 14-12 = 0, SM0, IR 08 and 15 low) */
static uint32_t eval_e53_enable(const ProbeSnapshot *snap) {
    return !(snap->ir & 0x0040)
        && ((snap->ir >> 12) & 7) == 0
        && ((snap->ir >> 9) & 7) == 0
        && !((snap->ir & 0x0100) || (snap->ir & 0x8000));
}

/* E50:11 (K2-3): T AND ENAB TBIT */
static uint32_t eval_e50_pin3(const ProbeSnapshot *snap) {
    return ((snap->psw >> 4) & 1) && ((snap->e54_out >> 2) & 1);
}

/* E75:12 (K2-5): IR 12-14 = 0 */
static uint32_t eval_ir12_14_zero(const ProbeSnapshot *snap) {
    return ((snap->ir >> 12) & 7) == 0;
}

/* K2-3 INT VECTOR L (E3:8): BUT SERVICE (1) and no ENAB GRANTS */
static uint32_t eval_int_vector_l(const ProbeSnapshot *snap) {
    return !(((snap->uw >> 42) & 1) && !((snap->e51_out >> 7) & 1));
}

/* K2-3 USER MODE H (E93:10): PSW 15 and no BE */
static uint32_t eval_user_mode(const ProbeSnapshot *snap) {
    return ((snap->psw >> 15) & 1) && !snap->bus_error;
}

/* K2-2 BG INH L (E4:8): low while a grant or RESET holds the clock */
static uint32_t eval_bg_inh_l(const ProbeSnapshot *snap) {
    return !(snap->bus_grant || snap->reset_pulse);
}

/* BUS BGn H (E77): E29 output gated by BG (1) H (E28) */
static uint32_t eval_bus_bg7(const ProbeSnapshot *snap) { return snap->bus_grant && ((snap->e29_out >> 1) & 1); }
static uint32_t eval_bus_bg6(const ProbeSnapshot *snap) { return snap->bus_grant && (snap->e29_out & 1); }
static uint32_t eval_bus_bg5(const ProbeSnapshot *snap) { return snap->bus_grant && ((snap->e29_out >> 3) & 1); }
static uint32_t eval_bus_bg4(const ProbeSnapshot *snap) { return snap->bus_grant && ((snap->e29_out >> 2) & 1); }

/* K2-6 wire-OR lines (open collector, low wins) */
static uint32_t eval_irc00(const ProbeSnapshot *s) {
    return ((s->e53_out >> 1) & 1) && ((s->e59_out >> 1) & 1) && (s->e70_out & 1) && ((s->e74_out >> 5) & 1);
}
static uint32_t eval_irc01(const ProbeSnapshot *s) { return ((s->e53_out >> 2) & 1) && ((s->e59_out >> 2) & 1); }
static uint32_t eval_irc02(const ProbeSnapshot *s) { return ((s->e53_out >> 3) & 1) && ((s->e59_out >> 3) & 1); }
static uint32_t eval_dest_l(const ProbeSnapshot *s) { return (s->e59_out & 1) && ((s->e74_out >> 6) & 1); }
static uint32_t eval_dm0(const ProbeSnapshot *s) { return ((s->ir >> 3) & 7) == 0; }
static uint32_t eval_sm0(const ProbeSnapshot *s) { return ((s->ir >> 9) & 7) == 0; }

/* K2-3 R6+7 L (E37:3): NAND(SPA 02, SPA 01) */
static uint32_t eval_high(const ProbeSnapshot *s) { (void)s; return 1; }
static uint32_t eval_r6_7_l(const ProbeSnapshot *s) { return !(((s->spa >> 2) & 1) && ((s->spa >> 1) & 1)); }

/* E73 (K2-6): NOR(IR08, IR15) */
static uint32_t eval_e73(const ProbeSnapshot *snap) {
    return !((snap->ir & 0x0100) || (snap->ir & 0x8000));
}


static uint32_t eval_e107_d(const ProbeSnapshot *s) { return (s->b_leg >> (s->byte_mux ? 7 : 15)) & 1; }
static uint32_t eval_e107_s(const ProbeSnapshot *s) { return (s->a_leg >> (s->byte_mux ? 7 : 15)) & 1; }
static uint32_t eval_psw_clock(const ProbeSnapshot *s) {
    return UW_MISC(s->uw) == MISC_LOAD_CC || UW_MISC(s->uw) == MISC_LOAD_PSW;
}

/* Signal definition table (K1 = M8265 data path, K2 = M8266 control) */

#define D(chip, alias_, desc_, fld, sz, bit_) \
    { .name = chip, .alias = alias_, .desc = desc_, .kind = PROBE_SNAP, \
      .offset = offsetof(ProbeSnapshot, fld), .size = sz, .bit = bit_ }

#define D_WHOLE(chip, alias_, desc_, fld, sz) \
    D(chip, alias_, desc_, fld, sz, PROBE_BIT_WHOLE)

#define DERIVED(chip, alias_, desc_, fn) \
    { .name = chip, .alias = alias_, .desc = desc_, .kind = PROBE_EVAL, .eval = fn }

#define ALIAS(group_, alias_, desc_, fld, sz) \
    { .alias = alias_, .desc = desc_, .kind = PROBE_SNAP, \
      .offset = offsetof(ProbeSnapshot, fld), .size = sz, .bit = PROBE_BIT_WHOLE, \
      .group = group_ }

/* Signals named after the schematics: DEC name, chip:pin of the driver */
#define SIG(name_, loc_, alias_, desc_, fld, sz, bit_, inv_) \
    { .name = name_, .loc = loc_, .alias = alias_, .desc = desc_, .kind = PROBE_SNAP, \
      .offset = offsetof(ProbeSnapshot, fld), .size = sz, .bit = bit_, .inv = inv_ }

/* Latched control store output: bit of the micro-word being executed */
#define UWB(name_, loc_, desc_, bit_, inv_) \
    { .name = name_, .loc = loc_, .desc = desc_, .kind = PROBE_UW, .bit = bit_, .inv = inv_ }

/* Control store ROM output before the latches: bit of the next micro-word */
#define UWN(name_, loc_, desc_, bit_) \
    { .name = name_, .loc = loc_, .desc = desc_, .kind = PROBE_UW_NEXT, .bit = bit_ }

/* 8815 zero detect: 1 when the 4 bits of the field from bit up are 0 */
#define ZERO4(name_, loc_, desc_, fld, bit_) \
    { .name = name_, .loc = loc_, .desc = desc_, .kind = PROBE_ZERO4, \
      .offset = offsetof(ProbeSnapshot, fld), .size = 2, .bit = bit_ }

/* K2-7 E89 decoder output, low when the MISC field equals code */
#define MISC(name_, loc_, desc_, code) \
    { .name = name_, .loc = loc_, .desc = desc_, .kind = PROBE_MISC, .bit = code }

#define PIN(sheet_, loc_, desc_, fld, sz, bit_) \
    SIG(sheet_ " " desc_, loc_, NULL, desc_, fld, sz, bit_, 0)

static const ProbeDef probe_defs[] = {
    /* Physical levels used by the conformance clips, before inverter stages. */
    UWN("K2-8 E97 O1", "E97:14", "E97 O1", 27),
    UWN("K2-8 E97 O2", "E97:13", "E97 O2", 26),
    PIN("K2-8", "E82:1", "E82 O1 before E81", e82_out, 1, 0),
    PIN("K2-8", "E82:2", "E82 O2 before E81", e82_out, 1, 1),
    PIN("K2-8", "E82:4", "E82 O4 before E81", e82_out, 1, 3),
    PIN("K2-8", "E82:5", "E82 O5 before E81", e82_out, 1, 4),
    PIN("K2-8", "E87:1", "E87 O1 before E91 and E62", e87_out, 1, 0),
    PIN("K2-8", "E87:2", "E87 O2 before E91 and E62", e87_out, 1, 1),
    { .name = "K1-10 D SIGN H", .loc = "E107:2", .desc = "D sign selected by BYTE MUX",
      .kind = PROBE_EVAL, .eval = eval_e107_d },
    { .name = "K1-10 S SIGN H", .loc = "E107:1", .desc = "S sign selected by BYTE MUX",
      .kind = PROBE_EVAL, .eval = eval_e107_s },
    DERIVED("K1-1 PSW CLK H", NULL, "LOAD CC or LOAD PSW clock pulse", eval_psw_clock),

    PIN("K2-3", "E52:5", "T AND ENAB TBIT", e52_inputs, 2, 0),
    PIN("K2-3", "E52:6", "BE at E52", e52_inputs, 2, 1),
    PIN("K2-3", "E52:7", "STOV at E52", e52_inputs, 2, 2),
    PIN("K2-3", "E52:4", "PE at E52", e52_inputs, 2, 3),
    PIN("K2-3", "E52:3", "KTE at E52", e52_inputs, 2, 4),
    PIN("K2-3", "E52:2", "PFAIL at E52", e52_inputs, 2, 5),
    PIN("K2-3", "E52:1", "IR CODE 02 L at service", e52_inputs, 2, 6),
    PIN("K2-3", "E52:17", "IR CODE 01 L at service", e52_inputs, 2, 7),
    PIN("K2-3", "E52:16", "IR CODE 00 L at service", e52_inputs, 2, 8),
    PIN("K2-3", "E52:15", "SERVICE BR PFAIL L", e52_inputs, 2, 9),
    UWB("K2-3 BUT SERVICE L", "E52:8", "BUT SERVICE L", 42, 1),
    PIN("K2-6", "E53:5", "IR00 at E53", e53_inputs, 1, 0),
    PIN("K2-6", "E53:6", "IR05 at E53", e53_inputs, 1, 1),
    PIN("K2-6", "E53:7", "IR07 at E53", e53_inputs, 1, 2),
    PIN("K2-6", "E53:4", "IR01 at E53", e53_inputs, 1, 3),
    PIN("K2-6", "E53:3", "IR02 at E53", e53_inputs, 1, 4),
    PIN("K2-6", "E53:2", "IR03 at E53", e53_inputs, 1, 5),
    PIN("K2-6", "E53:1", "IR04 at E53", e53_inputs, 1, 6),
    PIN("K2-6", "E53:15", "USER MODE at E53", e53_inputs, 1, 7),
    PIN("K2-6", "E53:13", "IR06 select", e53_selects, 1, 0),
    PIN("K2-6", "E53:14", "E85 select network", e53_selects, 1, 1),
    PIN("K2-6", "E54:7", "DM0 at E54", e54_inputs, 1, 2),
    PIN("K2-6", "E54:1", "PSW15 L at E54", e54_inputs, 1, 6),
    PIN("K2-6", "E54:15", "E85-6 at E54", e54_inputs, 1, 7),
    PIN("K2-8", "E80:5", "BYTE H at E80", e80_inputs, 1, 0),
    PIN("K2-8", "E80:6", "VBA00 AND requested BDT", e80_inputs, 1, 1),
    PIN("K2-8", "E80:7", "MOV L at E80", e80_inputs, 1, 2),
    PIN("K2-8", "E80:4", "AUX at E80", e80_inputs, 1, 3),
    PIN("K2-8", "E80:3", "Requested BDT before MSYN inhibition", e80_inputs, 1, 4),
    PIN("K2-8", "E80:2", "SS01 at E80", e80_inputs, 1, 5),
    PIN("K2-8", "E80:15", "SS00 at E80", e80_inputs, 1, 7),



    D("K2:E102:1", NULL,  "BUT ALU COUT→MPC 07",   e102_out, 1, 0),
    D("K2:E102:2", NULL,  "BUT CC N→MPC 06",       e102_out, 1, 1),
    D("K2:E102:3", NULL,  "BUT SP15→MPC 03",       e102_out, 1, 2),
    D("K2:E102:4", NULL,  "BUT BX REG 01→MPC 05",  e102_out, 1, 3),
    D("K2:E102:5", NULL,  "BUT BX REG 00→MPC 04",  e102_out, 1, 4),
    D("K2:E102:6", NULL,  "BUT COUNT 05→MPC 02",   e102_out, 1, 5),
    D("K2:E102:7", NULL,  "BUT CC Z→MPC 01",       e102_out, 1, 6),
    D("K2:E102:9", NULL,  "BUT IR 09→MPC 00",      e102_out, 1, 7),

    /* K1-1 Data path 0-3 */
    SIG("K1-1 SP 03 (1) H", "E8:11", NULL, "scratchpad output, ALU A input", a_leg, 2, 3, 0),
    SIG("K1-1 SP 02 (1) H", "E8:10", NULL, "scratchpad output, ALU A input", a_leg, 2, 2, 0),
    SIG("K1-1 SP 01 (1) H", "E8:8", NULL, "scratchpad output, ALU A input", a_leg, 2, 1, 0),
    SIG("K1-1 SP 00 (1) H", "E8:7", NULL, "scratchpad output, ALU A input", a_leg, 2, 0, 0),
    SIG("K1-1 BREG 03 (1) H", "E10:15", NULL, "B register", b_reg, 2, 3, 0),
    SIG("K1-1 BREG 02 (1) H", "E10:14", NULL, "B register", b_reg, 2, 2, 0),
    SIG("K1-1 BREG 01 (1) H", "E10:13", NULL, "B register", b_reg, 2, 1, 0),
    SIG("K1-1 BREG 00 (1) H", "E10:12", NULL, "B register", b_reg, 2, 0, 0),
    SIG("K1-1 BXREG 03 (1) H", "E11:15", NULL, "BX register", bx_reg, 2, 3, 0),
    SIG("K1-1 BXREG 02 (1) H", "E11:14", NULL, "BX register", bx_reg, 2, 2, 0),
    SIG("K1-1 BXREG 01 (1) H", "E11:13", NULL, "BX register", bx_reg, 2, 1, 0),
    SIG("K1-1 BXREG 00 (1) H", "E11:12", NULL, "BX register", bx_reg, 2, 0, 0),
    SIG("K1-1 AMUX 03 H", "E3:7", NULL, "AMUX output", amux_out, 2, 3, 0),
    SIG("K1-1 AMUX 02 H", "E3:9", NULL, "AMUX output", amux_out, 2, 2, 0),
    SIG("K1-1 AMUX 01 H", "E4:7", NULL, "AMUX output", amux_out, 2, 1, 0),
    SIG("K1-1 AMUX 00 H", "E4:9", NULL, "AMUX output", amux_out, 2, 0, 0),
    SIG("K1-1 SSMUX 03 H", "E2:4", NULL, "SSMUX output", ss_out, 2, 3, 0),
    SIG("K1-1 SSMUX 02 H", "E2:12", NULL, "SSMUX output", ss_out, 2, 2, 0),
    SIG("K1-1 SSMUX 01 H", "E2:7", NULL, "SSMUX output", ss_out, 2, 1, 0),
    SIG("K1-1 SSMUX 00 H", "E2:9", NULL, "SSMUX output", ss_out, 2, 0, 0),
    ZERO4("K1-1 0-3=0 H", "E17:6", "ALU bits 0-3 all zero", alu_out, 0),
    SIG("K1-1 ALU COUT 3 L", "E27:12", NULL, "carry out of bit 3, from the 74S182", slice_cout, 1, 0, 1),

    /* K1-2 Data path 4-7 */
    SIG("K1-2 SP 07 (1) H", "E29:11", NULL, "scratchpad output, ALU A input", a_leg, 2, 7, 0),
    SIG("K1-2 SP 06 (1) H", "E29:10", NULL, "scratchpad output, ALU A input", a_leg, 2, 6, 0),
    SIG("K1-2 SP 05 (1) H", "E29:8", NULL, "scratchpad output, ALU A input", a_leg, 2, 5, 0),
    SIG("K1-2 SP 04 (1) H", "E29:7", NULL, "scratchpad output, ALU A input", a_leg, 2, 4, 0),
    SIG("K1-2 BREG 07 (1) H", "E31:15", NULL, "B register", b_reg, 2, 7, 0),
    SIG("K1-2 BREG 06 (1) H", "E31:14", NULL, "B register", b_reg, 2, 6, 0),
    SIG("K1-2 BREG 05 (1) H", "E31:13", NULL, "B register", b_reg, 2, 5, 0),
    SIG("K1-2 BREG 04 (1) H", "E31:12", NULL, "B register", b_reg, 2, 4, 0),
    SIG("K1-2 BXREG 07 (1) H", "E32:15", NULL, "BX register", bx_reg, 2, 7, 0),
    SIG("K1-2 BXREG 06 (1) H", "E32:14", NULL, "BX register", bx_reg, 2, 6, 0),
    SIG("K1-2 BXREG 05 (1) H", "E32:13", NULL, "BX register", bx_reg, 2, 5, 0),
    SIG("K1-2 BXREG 04 (1) H", "E32:12", NULL, "BX register", bx_reg, 2, 4, 0),
    SIG("K1-2 AMUX 07 H", "E25:7", NULL, "AMUX output", amux_out, 2, 7, 0),
    SIG("K1-2 AMUX 06 H", "E25:9", NULL, "AMUX output", amux_out, 2, 6, 0),
    SIG("K1-2 AMUX 05 H", "E26:7", NULL, "AMUX output", amux_out, 2, 5, 0),
    SIG("K1-2 AMUX 04 H", "E26:9", NULL, "AMUX output", amux_out, 2, 4, 0),
    SIG("K1-2 SSMUX 07 H", "E24:4", NULL, "SSMUX output", ss_out, 2, 7, 0),
    SIG("K1-2 SSMUX 06 H", "E24:12", NULL, "SSMUX output", ss_out, 2, 6, 0),
    SIG("K1-2 SSMUX 05 H", "E24:7", NULL, "SSMUX output", ss_out, 2, 5, 0),
    SIG("K1-2 SSMUX 04 H", "E24:9", NULL, "SSMUX output", ss_out, 2, 4, 0),
    ZERO4("K1-2 4-7=0 H", "E37:6", "ALU bits 4-7 all zero", alu_out, 4),
    SIG("K1-2 ALU COUT 7 L", "E27:11", NULL, "carry out of bit 7, from the 74S182", slice_cout, 1, 1, 1),

    /* K1-3 Data path 8-11 */
    SIG("K1-3 SP 11 (1) H", "E19:11", NULL, "scratchpad output, ALU A input", a_leg, 2, 11, 0),
    SIG("K1-3 SP 10 (1) H", "E19:10", NULL, "scratchpad output, ALU A input", a_leg, 2, 10, 0),
    SIG("K1-3 SP 09 (1) H", "E19:8", NULL, "scratchpad output, ALU A input", a_leg, 2, 9, 0),
    SIG("K1-3 SP 08 (1) H", "E19:7", NULL, "scratchpad output, ALU A input", a_leg, 2, 8, 0),
    SIG("K1-3 BREG 11 (1) H", "E21:15", NULL, "B register", b_reg, 2, 11, 0),
    SIG("K1-3 BREG 10 (1) H", "E21:14", NULL, "B register", b_reg, 2, 10, 0),
    SIG("K1-3 BREG 09 (1) H", "E21:13", NULL, "B register", b_reg, 2, 9, 0),
    SIG("K1-3 BREG 08 (1) H", "E21:12", NULL, "B register", b_reg, 2, 8, 0),
    SIG("K1-3 BXREG 11 (1) H", "E22:15", NULL, "BX register", bx_reg, 2, 11, 0),
    SIG("K1-3 BXREG 10 (1) H", "E22:14", NULL, "BX register", bx_reg, 2, 10, 0),
    SIG("K1-3 BXREG 09 (1) H", "E22:13", NULL, "BX register", bx_reg, 2, 9, 0),
    SIG("K1-3 BXREG 08 (1) H", "E22:12", NULL, "BX register", bx_reg, 2, 8, 0),
    SIG("K1-3 AMUX 11 H", "E15:7", NULL, "AMUX output", amux_out, 2, 11, 0),
    SIG("K1-3 AMUX 10 H", "E15:9", NULL, "AMUX output", amux_out, 2, 10, 0),
    SIG("K1-3 AMUX 09 H", "E5:7", NULL, "AMUX output", amux_out, 2, 9, 0),
    SIG("K1-3 AMUX 08 H", "E5:9", NULL, "AMUX output", amux_out, 2, 8, 0),
    SIG("K1-3 SSMUX 11 H", "E13:7", NULL, "SSMUX output", ss_out, 2, 11, 0),
    SIG("K1-3 SSMUX 10 H", "E13:9", NULL, "SSMUX output", ss_out, 2, 10, 0),
    SIG("K1-3 SSMUX 09 H", "E14:7", NULL, "SSMUX output", ss_out, 2, 9, 0),
    SIG("K1-3 SSMUX 08 H", "E14:9", NULL, "SSMUX output", ss_out, 2, 8, 0),
    ZERO4("K1-3 8-11=0 H", "E17:8", "ALU bits 8-11 all zero", alu_out, 8),
    SIG("K1-3 ALU COUT 11 L", "E27:9", NULL, "carry out of bit 11, from the 74S182", slice_cout, 1, 2, 1),

    /* K1-4 Data path 12-15 */
    SIG("K1-4 SP 15 (1) H", "E39:11", NULL, "scratchpad output, ALU A input", a_leg, 2, 15, 0),
    SIG("K1-4 SP 14 (1) H", "E39:10", NULL, "scratchpad output, ALU A input", a_leg, 2, 14, 0),
    SIG("K1-4 SP 13 (1) H", "E39:8", NULL, "scratchpad output, ALU A input", a_leg, 2, 13, 0),
    SIG("K1-4 SP 12 (1) H", "E39:7", NULL, "scratchpad output, ALU A input", a_leg, 2, 12, 0),
    SIG("K1-4 BREG 15 (1) H", "E41:15", NULL, "B register", b_reg, 2, 15, 0),
    SIG("K1-4 BREG 14 (1) H", "E41:14", NULL, "B register", b_reg, 2, 14, 0),
    SIG("K1-4 BREG 13 (1) H", "E41:13", NULL, "B register", b_reg, 2, 13, 0),
    SIG("K1-4 BREG 12 (1) H", "E41:12", NULL, "B register", b_reg, 2, 12, 0),
    SIG("K1-4 BXREG 15 (1) H", "E52:15", NULL, "BX register", bx_reg, 2, 15, 0),
    SIG("K1-4 BXREG 14 (1) H", "E52:14", NULL, "BX register", bx_reg, 2, 14, 0),
    SIG("K1-4 BXREG 13 (1) H", "E52:13", NULL, "BX register", bx_reg, 2, 13, 0),
    SIG("K1-4 BXREG 12 (1) H", "E52:12", NULL, "BX register", bx_reg, 2, 12, 0),
    SIG("K1-4 AMUX 15 H", "E36:7", NULL, "AMUX output", amux_out, 2, 15, 0),
    SIG("K1-4 AMUX 14 H", "E36:9", NULL, "AMUX output", amux_out, 2, 14, 0),
    SIG("K1-4 AMUX 13 H", "E46:7", NULL, "AMUX output", amux_out, 2, 13, 0),
    SIG("K1-4 AMUX 12 H", "E46:9", NULL, "AMUX output", amux_out, 2, 12, 0),
    SIG("K1-4 SSMUX 15 H", "E34:7", NULL, "SSMUX output", ss_out, 2, 15, 0),
    SIG("K1-4 SSMUX 14 H", "E34:9", NULL, "SSMUX output", ss_out, 2, 14, 0),
    SIG("K1-4 SSMUX 13 H", "E35:7", NULL, "SSMUX output", ss_out, 2, 13, 0),
    SIG("K1-4 SSMUX 12 H", "E35:9", NULL, "SSMUX output", ss_out, 2, 12, 0),
    ZERO4("K1-4 12-15=0 H", "E37:8", "ALU bits 12-15 all zero", alu_out, 12),

    /* K1-6 Unibus address */
    SIG("K1-6 VBA 15 (1) H", "E49", NULL, "virtual bus address latch", vba, 2, 15, 0),
    SIG("K1-6 VBA 14 (1) H", "E49", NULL, "virtual bus address latch", vba, 2, 14, 0),
    SIG("K1-6 VBA 13 (1) H", "E49", NULL, "virtual bus address latch", vba, 2, 13, 0),
    SIG("K1-6 VBA 12 (1) H", "E49", NULL, "virtual bus address latch", vba, 2, 12, 0),
    SIG("K1-6 VBA 11 (1) H", "E49", NULL, "virtual bus address latch", vba, 2, 11, 0),
    SIG("K1-6 VBA 10 (1) H", "E49", NULL, "virtual bus address latch", vba, 2, 10, 0),
    SIG("K1-6 VBA 09 (1) H", "E48", NULL, "virtual bus address latch", vba, 2, 9, 0),
    SIG("K1-6 VBA 08 (1) H", "E48", NULL, "virtual bus address latch", vba, 2, 8, 0),
    SIG("K1-6 VBA 07 (1) H", "E48", NULL, "virtual bus address latch", vba, 2, 7, 0),
    SIG("K1-6 VBA 06 (1) H", "E48", NULL, "virtual bus address latch", vba, 2, 6, 0),
    SIG("K1-6 VBA 05 (1) H", "E48", NULL, "virtual bus address latch", vba, 2, 5, 0),
    SIG("K1-6 VBA 04 (1) H", "E48", NULL, "virtual bus address latch", vba, 2, 4, 0),
    SIG("K1-6 VBA 03 (1) H", "E47", NULL, "virtual bus address latch", vba, 2, 3, 0),
    SIG("K1-6 VBA 02 (1) H", "E47", NULL, "virtual bus address latch", vba, 2, 2, 0),
    SIG("K1-6 VBA 01 (1) H", "E47", NULL, "virtual bus address latch", vba, 2, 1, 0),
    SIG("K1-6 VBA 00 (1) H", "E47", NULL, "virtual bus address latch", vba, 2, 0, 0),
    SIG("K1-6 BA 17 (1) H", "E44", NULL, "bus address register (relocated)", ba, 4, 17, 0),
    SIG("K1-6 BA 16 (1) H", "E44", NULL, "bus address register (relocated)", ba, 4, 16, 0),
    SIG("K1-6 BA 15 (1) H", "E44", NULL, "bus address register (relocated)", ba, 4, 15, 0),
    SIG("K1-6 BA 14 (1) H", "E44", NULL, "bus address register (relocated)", ba, 4, 14, 0),
    SIG("K1-6 BA 13 (1) H", "E55", NULL, "bus address register (relocated)", ba, 4, 13, 0),
    SIG("K1-6 BA 12 (1) H", "E55", NULL, "bus address register (relocated)", ba, 4, 12, 0),
    SIG("K1-6 BA 11 (1) H", "E55", NULL, "bus address register (relocated)", ba, 4, 11, 0),
    SIG("K1-6 BA 10 (1) H", "E55", NULL, "bus address register (relocated)", ba, 4, 10, 0),
    SIG("K1-6 BA 09 (1) H", "E65", NULL, "bus address register (relocated)", ba, 4, 9, 0),
    SIG("K1-6 BA 08 (1) H", "E65", NULL, "bus address register (relocated)", ba, 4, 8, 0),
    SIG("K1-6 BA 07 (1) H", "E65", NULL, "bus address register (relocated)", ba, 4, 7, 0),
    SIG("K1-6 BA 06 (1) H", "E65", NULL, "bus address register (relocated)", ba, 4, 6, 0),
    SIG("K1-6 PBA 17 H", "E43", NULL, "physical bus address (8641 receiver)", ba, 4, 17, 0),
    SIG("K1-6 PBA 16 H", "E43", NULL, "physical bus address (8641 receiver)", ba, 4, 16, 0),
    SIG("K1-6 PBA 15 H", "E43", NULL, "physical bus address (8641 receiver)", ba, 4, 15, 0),
    SIG("K1-6 PBA 14 H", "E43", NULL, "physical bus address (8641 receiver)", ba, 4, 14, 0),
    SIG("K1-6 PBA 13 H", "E53", NULL, "physical bus address (8641 receiver)", ba, 4, 13, 0),
    SIG("K1-6 PBA 12 H", "E53", NULL, "physical bus address (8641 receiver)", ba, 4, 12, 0),
    SIG("K1-6 PBA 11 H", "E53", NULL, "physical bus address (8641 receiver)", ba, 4, 11, 0),
    SIG("K1-6 PBA 10 H", "E53", NULL, "physical bus address (8641 receiver)", ba, 4, 10, 0),
    SIG("K1-6 PBA 09 H", "E64", NULL, "physical bus address (8641 receiver)", ba, 4, 9, 0),
    SIG("K1-6 PBA 08 H", "E64", NULL, "physical bus address (8641 receiver)", ba, 4, 8, 0),
    SIG("K1-6 PBA 07 H", "E64", NULL, "physical bus address (8641 receiver)", ba, 4, 7, 0),
    SIG("K1-6 PBA 06 H", "E64", NULL, "physical bus address (8641 receiver)", ba, 4, 6, 0),
    SIG("K1-6 PBA 05 H", "E42", NULL, "physical bus address (8641 receiver)", ba, 4, 5, 0),
    SIG("K1-6 PBA 04 H", "E42", NULL, "physical bus address (8641 receiver)", ba, 4, 4, 0),
    SIG("K1-6 PBA 03 H", "E42", NULL, "physical bus address (8641 receiver)", ba, 4, 3, 0),
    SIG("K1-6 PBA 02 H", "E42", NULL, "physical bus address (8641 receiver)", ba, 4, 2, 0),
    SIG("K1-6 PBA 01 H", "E63", NULL, "physical bus address (8641 receiver)", ba, 4, 1, 0),
    SIG("K1-6 PBA 00 H", "E63", NULL, "physical bus address (8641 receiver)", ba, 4, 0, 0),

    /* K1-7 PAR + PDR (entry addressed by mode and VBA 15-13) */
    SIG("K1-7 PAR 11 (1) H", "E80:11", NULL, "page address register", par_out, 2, 11, 0),
    SIG("K1-7 PAR 10 (1) H", "E80:10", NULL, "page address register", par_out, 2, 10, 0),
    SIG("K1-7 PAR 09 (1) H", "E80:8", NULL, "page address register", par_out, 2, 9, 0),
    SIG("K1-7 PAR 08 (1) H", "E80:7", NULL, "page address register", par_out, 2, 8, 0),
    SIG("K1-7 PAR 07 (1) H", "E79:11", NULL, "page address register", par_out, 2, 7, 0),
    SIG("K1-7 PAR 06 (1) H", "E79:10", NULL, "page address register", par_out, 2, 6, 0),
    SIG("K1-7 PAR 05 (1) H", "E79:8", NULL, "page address register", par_out, 2, 5, 0),
    SIG("K1-7 PAR 04 (1) H", "E79:7", NULL, "page address register", par_out, 2, 4, 0),
    SIG("K1-7 PAR 03 (1) H", "E78:11", NULL, "page address register", par_out, 2, 3, 0),
    SIG("K1-7 PAR 02 (1) H", "E78:10", NULL, "page address register", par_out, 2, 2, 0),
    SIG("K1-7 PAR 01 (1) H", "E78:8", NULL, "page address register", par_out, 2, 1, 0),
    SIG("K1-7 PAR 00 (1) H", "E78:7", NULL, "page address register", par_out, 2, 0, 0),
    SIG("K1-7 PDR 14 (1) H", "E89:10", NULL, "page length field", pdr_out, 2, 14, 0),
    SIG("K1-7 PDR 13 (1) H", "E89:8", NULL, "page length field", pdr_out, 2, 13, 0),
    SIG("K1-7 PDR 12 (1) H", "E89:7", NULL, "page length field", pdr_out, 2, 12, 0),
    SIG("K1-7 PDR 11 (1) H", "E90:11", NULL, "page length field", pdr_out, 2, 11, 0),
    SIG("K1-7 PDR 10 (1) H", "E90:10", NULL, "page length field", pdr_out, 2, 10, 0),
    SIG("K1-7 PDR 09 (1) H", "E90:8", NULL, "page length field", pdr_out, 2, 9, 0),
    SIG("K1-7 PDR 08 (1) H", "E90:7", NULL, "page length field", pdr_out, 2, 8, 0),
    SIG("K1-7 ED (1) H",   "E88:11", NULL, "expansion direction (1 = down)", pdr_out, 2, 3, 0),
    SIG("K1-7 ACF2 (1) H", "E88:10", NULL, "access control field", pdr_out, 2, 2, 0),
    SIG("K1-7 ACF1 (1) H", "E88:8",  NULL, "access control field", pdr_out, 2, 1, 0),
    SIG("K1-7 WBIT H",     "E117:10", NULL, "page written", pdr_out, 2, 6, 0),

    /* K1-8 SR0 and KT error */
    SIG("K1-8 RELOCATE ENB (1) H", "E100", NULL, "SR0 00, memory management enabled", sr0, 2, 0, 0),
    SIG("K1-8 MAINT (1) H", "E103:2",  NULL, "SR0 08, maintenance", sr0, 2, 8, 0),
    SIG("K1-8 NR (1) H",    "E103:7",  NULL, "SR0 15, abort non-resident", sr0, 2, 15, 0),
    SIG("K1-8 PL (1) H",    "E103:10", NULL, "SR0 14, abort page length", sr0, 2, 14, 0),
    SIG("K1-8 RO (1) H",    "E103:15", NULL, "SR0 13, abort read-only", sr0, 2, 13, 0),
    SIG("K1-8 SR0 06 (1) H", "E73:5",  NULL, "SR0 mode of the abort", sr0, 2, 6, 0),
    SIG("K1-8 SR0 05 (1) H", "E73:12", NULL, "SR0 mode of the abort", sr0, 2, 5, 0),
    SIG("K1-8 SR0 03 (1) H", "E73:2",  NULL, "SR0 page of the abort", sr0, 2, 3, 0),
    SIG("K1-8 SR0 02 (1) H", "E73:15", NULL, "SR0 page of the abort", sr0, 2, 2, 0),
    SIG("K1-8 SR0 01 (1) H", "E73:7",  NULL, "SR0 page of the abort", sr0, 2, 1, 0),
    SIG("K1-8 MODE 01 H",   "E71:4",  NULL, "mode for PAR/PDR: current, previous or kernel", mmu_mode, 1, 1, 0),
    SIG("K1-8 MODE 00 H",   "E71:12", NULL, "mode for PAR/PDR: current, previous or kernel", mmu_mode, 1, 0, 0),

    /* K1-9 SR2 and KT MUX */
    SIG("K1-9 SR2 07 (1) H", "E51:10", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 7, 0),
    SIG("K1-9 SR2 06 (1) H", "E51:15", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 6, 0),
    SIG("K1-9 SR2 05 (1) H", "E51:7", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 5, 0),
    SIG("K1-9 SR2 04 (1) H", "E51:12", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 4, 0),
    SIG("K1-9 SR2 03 (1) H", "E50:10", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 3, 0),
    SIG("K1-9 SR2 02 (1) H", "E50:7", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 2, 0),
    SIG("K1-9 SR2 01 (1) H", "E50:2", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 1, 0),
    SIG("K1-9 SR2 00 (1) H", "E50:5", NULL, "SR2, virtual address of the last instruction fetch", sr2, 2, 0, 0),

    /* K1-10 CC decode, address detection */
    SIG("K1-10 CC N H",        "E108:7",  "CC_N", "condition code N, bit 7 for bytes (BYTE MUX)", cc_wires, 1, 3, 0),
    SIG("K1-10 CC Z H",        "E121:8",  "CC_Z", "condition code Z, low byte only for bytes", cc_wires, 1, 2, 0),
    SIG("K1-10 CC V H",        "E107:9",  "CC_V", "condition code V, C+V decode ROM", cc_wires, 1, 1, 0),
    SIG("K1-10 CC C H",        "E107:10", "CC_C", "condition code C, C+V decode ROM", cc_wires, 1, 0, 0),
    SIG("K1-10 SHIFT IN B H",  "E119:7",  NULL, "serial input of the B register", shift_in, 1, 0, 0),
    SIG("K1-10 SHIFT IN BX H", "E119:9",  NULL, "serial input of the BX register", shift_in, 1, 1, 0),
    SIG("K1-10 SHIFT IN 07 H", "E108:4",  NULL, "bit 7 input of right shifts: serial for bytes, BREG 08 for words", shift_in, 1, 2, 0),
    SIG("K1-10 OVX (1) H",     "E100:5",  NULL, "overflow extension flip-flop", ovx, 1, 0, 0),
    UWB("K1-10 AMUX S1 H",     "E120:11", "AMUX select (micro-word, internal register reads not shown)", 30, 1),
    UWB("K1-10 AMUX S0 H",     "E123:8",  "AMUX select (micro-word, internal register reads not shown)", 31, 1),

    /* Named signals of the slices */
    SIG("K1-4 ALU COUT H",  "E118:3", NULL, "carry out of the top slice (E38:16 inverted)", alu_cout, 1, 0, 0),
    SIG("K1-2 ALU 07 H",    "E28:13", NULL, "ALU output bit 7", alu_out, 2, 7, 0),
    SIG("K1-4 ALU 15 H",    "E38:13", NULL, "ALU output bit 15", alu_out, 2, 15, 0),
    SIG("K1-2 BLEG 07 H",   NULL,     NULL, "B leg (BMUX output) bit 7", b_leg, 2, 7, 0),
    SIG("K1-4 BLEG 15 H",   NULL,     NULL, "B leg (BMUX output) bit 15", b_leg, 2, 15, 0),
    SIG("K1-1 N BIT (1) H", "E97:2",  NULL, "PSW N", psw, 2, 3, 0),
    SIG("K1-1 Z BIT (1) H", "E97:7",  NULL, "PSW Z", psw, 2, 2, 0),
    SIG("K1-1 V BIT (1) H", "E97:10", NULL, "PSW V", psw, 2, 1, 0),
    SIG("K1-1 C BIT (1) H", "E97:15", NULL, "PSW C", psw, 2, 0, 0),
    SIG("K1-2 TBIT (1) H",  "E109:5", NULL, "PSW T (trace)", psw, 2, 4, 0),
    SIG("K1-2 PSW 07 (1) H", "E99:2",  NULL, "PSW priority", psw, 2, 7, 0),
    SIG("K1-2 PSW 06 (1) H", "E99:7",  NULL, "PSW priority", psw, 2, 6, 0),
    SIG("K1-2 PSW 05 (1) H", "E99:10", NULL, "PSW priority", psw, 2, 5, 0),
    SIG("K1-4 PSW 15 (1) H", "E82:2",  NULL, "PSW current mode", psw, 2, 15, 0),
    SIG("K1-4 PSW 15 (1) L", "E82:3",  NULL, "PSW current mode", psw, 2, 15, 1),
    SIG("K1-4 PSW 14 (1) H", "E82:7",  NULL, "PSW current mode", psw, 2, 14, 0),
    SIG("K1-4 PSW 13 (1) H", "E82:10", NULL, "PSW previous mode", psw, 2, 13, 0),
    SIG("K1-4 PSW 12 (1) H", "E82:15", NULL, "PSW previous mode", psw, 2, 12, 0),

    /* K2-5 IR, AUX control, CC code */
    SIG("K2-5 IR 15 (1) H", "E33:9",  NULL, "instruction register", ir, 2, 15, 0),
    SIG("K2-5 IR 14 (1) H", "E66:2",  NULL, "instruction register", ir, 2, 14, 0),
    SIG("K2-5 IR 13 (1) H", "E66:15", NULL, "instruction register", ir, 2, 13, 0),
    SIG("K2-5 IR 12 (1) H", "E66:5",  NULL, "instruction register", ir, 2, 12, 0),
    SIG("K2-5 IR 11 (1) H", "E67:2",  NULL, "instruction register", ir, 2, 11, 0),
    SIG("K2-5 IR 11 (1) L", "E67:3",  NULL, "instruction register", ir, 2, 11, 1),
    SIG("K2-5 IR 10 (1) H", "E66:7",  NULL, "instruction register", ir, 2, 10, 0),
    SIG("K2-5 IR 09 (1) H", "E66:10", NULL, "instruction register", ir, 2, 9, 0),
    SIG("K2-5 IR 08 (1) H", "E67:15", NULL, "instruction register", ir, 2, 8, 0),
    SIG("K2-5 IR 07 (1) H", "E56:2",  NULL, "instruction register", ir, 2, 7, 0),
    SIG("K2-5 IR 06 (1) H", "E67:7",  NULL, "instruction register", ir, 2, 6, 0),
    SIG("K2-5 IR 06 (1) L", "E67:6",  NULL, "instruction register", ir, 2, 6, 1),
    SIG("K2-5 IR 05 (1) H", "E56:15", NULL, "instruction register", ir, 2, 5, 0),
    SIG("K2-5 IR 04 (1) H", "E56:5",  NULL, "instruction register", ir, 2, 4, 0),
    SIG("K2-5 IR 03 (1) H", "E56:12", NULL, "instruction register", ir, 2, 3, 0),
    SIG("K2-5 IR 02 (1) H", "E56:7",  NULL, "instruction register", ir, 2, 2, 0),
    SIG("K2-5 IR 01 (1) H", "E56:10", NULL, "instruction register", ir, 2, 1, 0),
    SIG("K2-5 IR 00 (1) H", "E67:10", NULL, "instruction register", ir, 2, 0, 0),
    SIG("K2-5 IR 00 (1) L", "E67:11", NULL, "instruction register", ir, 2, 0, 1),
    { .name = "K2-5 IR 12-14=0 H", .loc = "E75:12", .desc = "IR 14-12 all zero",
      .kind = PROBE_EVAL, .eval = eval_ir12_14_zero },
    SIG("K2-5 BYTE L",       "E68:9",  NULL, "byte instruction, BYTE MUX select", e68_out, 1, 3, 0),
    SIG("K2-5 CC CODE 02 H", "E68:10", NULL, "instruction category for E107", e68_out, 1, 2, 0),
    SIG("K2-5 CC CODE 01 H", "E68:11", NULL, "instruction category for E107", e68_out, 1, 1, 0),
    SIG("K2-5 CC CODE 00 H", "E68:12", NULL, "instruction category for E107", e68_out, 1, 0, 0),
    SIG("K2-8 FUNC CODE 03 H", "E83:9/E61:9",  NULL, "AUX ALU function, latched by E71", func_code, 1, 3, 0),
    SIG("K2-8 FUNC CODE 02 H", "E83:1/E61:10", NULL, "AUX ALU function, latched by E71", func_code, 1, 2, 0),
    SIG("K2-8 FUNC CODE 01 H", "E83:2/E61:11", NULL, "AUX ALU function, latched by E71", func_code, 1, 1, 0),
    SIG("K2-8 FUNC CODE 00 H", "E83:3/E61:12", NULL, "AUX ALU function, latched by E71", func_code, 1, 0, 0),
    SIG("K2-5 SERIAL SHIFT H", "E62:12", NULL, "serial input for rotates and shifts", e62_out, 1, 0, 0),
    SIG("K2-5 ROT CBIT (1) H", "E55:15", NULL, "carry from the rotate or shift, to E107", rot_cbit, 1, 0, 0),

    /* K2-7 MPC BUT field: next micro-word address, and the MISC decoder */
    SIG("K2-7 MPC 08 H", "E111:2",  NULL, "next micro-word address", next_mpc, 2, 8, 0),
    SIG("K2-7 MPC 07 H", "E111:8",  NULL, "next micro-word address", next_mpc, 2, 7, 0),
    SIG("K2-7 MPC 06 H", "E111:10", NULL, "next micro-word address", next_mpc, 2, 6, 0),
    SIG("K2-7 MPC 05 H", "E111:12", NULL, "next micro-word address", next_mpc, 2, 5, 0),
    SIG("K2-7 MPC 04 H", "E111:6",  NULL, "next micro-word address", next_mpc, 2, 4, 0),
    SIG("K2-7 MPC 03 H", "E111:4",  NULL, "next micro-word address", next_mpc, 2, 3, 0),
    SIG("K2-7 MPC 02 H", "E112:2",  NULL, "next micro-word address", next_mpc, 2, 2, 0),
    SIG("K2-7 MPC 01 H", "E112:4",  NULL, "next micro-word address", next_mpc, 2, 1, 0),
    SIG("K2-7 MPC 00 H", "E112:12", NULL, "next micro-word address", next_mpc, 2, 0, 0),
    SIG("K2-7 MPC 08 L", "E101:6",  NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 8, 1),
    SIG("K2-7 MPC 07 L", "E101:12", NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 7, 1),
    SIG("K2-7 MPC 06 L", "E101:10", NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 6, 1),
    SIG("K2-7 MPC 05 L", "E101:8",  NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 5, 1),
    SIG("K2-7 MPC 04 L", "E101:2",  NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 4, 1),
    SIG("K2-7 MPC 03 L", "E101:4",  NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 3, 1),
    SIG("K2-7 MPC 02 L", "E91:6",   NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 2, 1),
    SIG("K2-7 MPC 01 L", "E91:4",   NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 1, 1),
    SIG("K2-7 MPC 00 L", "E91:8",   NULL, "next micro-word address, wired-OR with BUT", next_mpc, 2, 0, 1),
    MISC("K2-7 LOAD IR L",    "E89:14", "load the instruction register", 1),
    MISC("K2-7 LOAD PSW L",   "E89:13", "load the PSW", 2),
    MISC("K2-7 LOAD CC L",    "E89:12", "load the condition codes", 3),
    MISC("K2-7 BUT DEST L",   "E89:11", "branch on destination mode", 4),
    MISC("K2-7 ENAB STOV L",  "E89:10", "enable stack overflow check", 5),
    MISC("K2-7 LOAD COUNT L", "E89:9",  "load the EIS counter", 6),
    MISC("K2-7 CLK COUNT L",  "E89:7",  "clock the EIS counter", 7),

    /* K2-8 Micro store */
    UWB("K2-8 BUF DAT TRAN (1) H", "E96:10", "bus data transfer", 12, 0),
    UWB("K2-8 BUF C1 (1) H",       "E96:7",  "bus cycle type C1", 13, 0),
    UWB("K2-8 BUF C0 (1) H",       "E96:12", "bus cycle type C0", 14, 0),
    UWB("K2-8 ENAB MAINT (1) H",   "E96:5",  "maintenance", 15, 0),
    UWB("K2-8 LOAD BA (1) H",      "E96:15", "load the bus address", 17, 0),
    UWB("K2-8 LONG CYCLE (1) L",   "E96:2",  "long machine cycle", 16, 0),
    UWN("K2-8 AUX SETUP H",        "E106:13", "AUX CONTROL of the next micro-word", 18),
    UWB("K2-8 AUX CONTROL (1) H",  "E55:10", "auxiliary ALU control", 18, 0),
    UWB("K2-8 AUX CONTROL (1) L",  "E55:11", "auxiliary ALU control", 18, 1),
    SIG("K2-8 ALU S3 H",    "E81:3",  "ALU_S[3]", "ALU function, from E82", e82_out, 1, 0, 1),
    SIG("K2-8 ALU S2 H",    "E81:11", "ALU_S[2]", "ALU function, from E82", e82_out, 1, 1, 1),
    SIG("K2-8 ALU S1 H",    "E81:6",  "ALU_S[1]", "ALU function, from E82", e82_out, 1, 3, 1),
    SIG("K2-8 ALU S0 H",    "E81:8",  "ALU_S[0]", "ALU function, from E82", e82_out, 1, 4, 1),
    SIG("K2-8 ALU MODE H",  "E82:3",  NULL, "ALU mode (logic when high)", e82_out, 1, 2, 0),
    SIG("K2-8 ALU CIN L",   "E82:6",  NULL, "ALU carry in", e82_out, 1, 5, 0),
    SIG("K2-8 BLEG 01 H",   "E82:7",  NULL, "B leg select", e82_out, 1, 6, 0),
    SIG("K2-8 BLEG 00 H",   "E82:9",  NULL, "B leg select", e82_out, 1, 7, 0),
    SIG("K2-8 B MODE 01 L", "E91:2",  NULL, "B register mode, E87:1 via E91, wired with E62:11", b_mode, 1, 0, 0),
    SIG("K2-8 B MODE 00 L", "E91:10", NULL, "B register mode, E87:2 via E91, wired with E62:10", b_mode, 1, 1, 0),
    SIG("K2-8 BX MODE 01 L",   "E87:3", NULL, "BX register mode", e87_out, 1, 2, 0),
    SIG("K2-8 BX MODE 00 L",   "E87:4", NULL, "BX register mode", e87_out, 1, 3, 0),
    SIG("K2-8 SHIFT MUX 01 L", "E87:5", NULL, "shift multiplexer", e87_out, 1, 4, 0),
    SIG("K2-8 SHIFT MUX 00 L", "E87:6", NULL, "shift multiplexer", e87_out, 1, 5, 0),
    SIG("K2-8 ENAB OVX L",     "E87:7", NULL, "enable overflow extension", e87_out, 1, 6, 0),
    SIG("K2-8 ENAB DBE L",     "E87:9", NULL, "enable double bus error", e87_out, 1, 7, 0),
    SIG("K2-8 SWAP H",         "E80:9",  "SWAP", "swap bytes", e80_out, 1, 3, 0),
    SIG("K2-8 SEX H",          "E80:10", "SEX",  "sign extension", e80_out, 1, 2, 0),
    SIG("K2-8 DISAB MSYN L",   "E80:11", NULL, "disable MSYN", e80_out, 1, 1, 0),
    SIG("K2-8 DISABLE UPPER BYTE WRITE H", "E80:12", NULL, "byte write to the low byte", e80_out, 1, 0, 0),

    /* K2-9 Micro code */
    UWN("K2-9 SS 01 H",               "E98:14", "SSMUX select of the next micro-word", 28),
    UWN("K2-9 SS 00 H",               "E98:13", "SSMUX select of the next micro-word", 29),
    UWB("K2-9 AMUX S1 (1) L",         "E93:15", "AMUX select", 30, 0),
    UWB("K2-9 AMUX S0 (1) L",         "E93:2",  "AMUX select", 31, 0),
    UWB("K2-9 SPA SRC SEL 1 (1) H",   "E49:5",  "scratchpad source select", 36, 0),
    UWB("K2-9 SPA SRC SEL 0 (1) H",   "E49:9",  "scratchpad source select", 37, 0),
    UWB("K2-9 SPA DST SEL 1 (1) H",   "E38:12", "scratchpad destination select", 38, 0),
    UWB("K2-9 SPA DST SEL 0 (1) H",   "E38:2",  "scratchpad destination select", 39, 0),
    UWB("K2-9 FORCE RS+1 (1) L",      "E38:15", "force register set 1", 40, 0),
    UWB("K2-9 PREVIOUS MODE (1) L",   "E38:5",  "use the previous mode", 41, 0),
    UWB("K2-9 BUT SERVICE (1) H",     "E38:7",  "branch on service (traps)", 42, 0),
    UWB("K2-9 FORCE KERNEL (1) H",    "E38:10", "force kernel mode", 43, 0),

    /* K2-10 ROM SPA counter */
    SIG("K2-10 COUNT 05 (1) H", "E79:2", NULL, "EIS counter bit 5", count, 1, 5, 0),
    UWB("K2-10 ROM SPA 03 (1) H", "E48:2",  "scratchpad address from the micro-word", 44, 0),
    UWB("K2-10 ROM SPA 02 (1) H", "E48:6",  "scratchpad address from the micro-word", 45, 0),
    UWB("K2-10 ROM SPA 01 (1) H", "E48:11", "scratchpad address from the micro-word", 46, 0),
    UWB("K2-10 ROM SPA 00 (1) H", "E48:14", "scratchpad address from the micro-word", 47, 0),
    UWB("K2-10 ROM SPA 00 (1) L", "E48:15", "scratchpad address from the micro-word", 47, 1),

    /* K2-6 IR decode: named lines, then each PROM output pin */
    SIG("K2-6 IR DECODE (1) H", "E55:2", NULL, "decode in the cycle after LOAD IR", ir_valid, 1, 0, 0),
    SIG("K2-6 IR DECODE (1) L", "E55:3", NULL, "decode in the cycle after LOAD IR", ir_valid, 1, 0, 1),
    { .name = "K2-6 DM0 H", .loc = "E75:8", .desc = "destination mode 0", .kind = PROBE_EVAL, .eval = eval_dm0 },
    { .name = "K2-6 SM0 H", .loc = "E75:6", .desc = "source mode 0", .kind = PROBE_EVAL, .eval = eval_sm0 },
    SIG("K2-6 SRC H",        "E69:12", NULL, "source mode on MPC 02-00", e69_out, 1, 0, 0),
    SIG("K2-6 MOV L",        "E69:11", NULL, "MOV instruction", e69_out, 1, 1, 0),
    { .name = "K2-6 DEST L", .loc = "E74:7/E59:12", .desc = "destination mode on MPC 02-00",
      .kind = PROBE_EVAL, .eval = eval_dest_l },
    D("K2:E59:11", NULL, "E59 IR CODE 00 L", e59_out, 1, 1),
    D("K2:E59:10", NULL, "E59 IR CODE 01 L", e59_out, 1, 2),
    D("K2:E59:9",  NULL, "E59 IR CODE 02 L", e59_out, 1, 3),
    D("K2:E60:12", NULL, "E60 MPC 03 L", e60_out, 1, 0),
    D("K2:E60:11", NULL, "E60 MPC 04 L", e60_out, 1, 1),
    D("K2:E60:10", NULL, "E60 MPC 05 L", e60_out, 1, 2),
    D("K2:E60:9",  NULL, "E60 MPC 06 L", e60_out, 1, 3),
    D("K2:E63:12", NULL, "E63 MPC 03 L", e63_out, 1, 0),
    D("K2:E63:11", NULL, "E63 MPC 02 L", e63_out, 1, 1),
    D("K2:E63:10", NULL, "E63 MPC 01 L", e63_out, 1, 2),
    D("K2:E63:9",  NULL, "E63 MPC 00 L", e63_out, 1, 3),
    D("K2:E69:10", NULL, "E69 MPC 03 L", e69_out, 1, 2),
    D("K2:E69:9",  NULL, "E69 MPC 04 L", e69_out, 1, 3),
    D("K2:E70:12", NULL, "E70 IR CODE 00 L", e70_out, 1, 0),
    D("K2:E70:11", NULL, "E70 MPC 05 L", e70_out, 1, 1),
    D("K2:E70:10", NULL, "E70 MPC 06 L", e70_out, 1, 2),
    D("K2:E70:9",  NULL, "E70 MPC 07 L", e70_out, 1, 3),
    D("K2:E71:12", NULL, "E71 (E72) MPC 07 L, branch", e71_out, 1, 0),
    D("K2:E74:2",  NULL, "E74 MPC 03 L", e74_out, 1, 1),
    D("K2:E74:3",  NULL, "E74 MPC 04 L", e74_out, 1, 2),
    D("K2:E74:4",  NULL, "E74 MPC 05 L", e74_out, 1, 3),
    D("K2:E74:5",  NULL, "E74 MPC 06 L", e74_out, 1, 4),
    D("K2:E74:6",  NULL, "E74 IR CODE 00 L", e74_out, 1, 5),
    D("K2:E74:9",  NULL, "E74 MPC 07 L", e74_out, 1, 7),

    /* K2-2 Priority arbitration */
    SIG("BUS BR7 L",         "E17:11", NULL, "bus request 7", br_lines, 1, 7, 1),
    SIG("BUS BR6 L",         "E17:5",  NULL, "bus request 6", br_lines, 1, 6, 1),
    SIG("BUS BR5 L",         "E17:13", NULL, "bus request 5", br_lines, 1, 5, 1),
    SIG("BUS BR4 L",         "E17:3",  NULL, "bus request 4", br_lines, 1, 4, 1),
    SIG("K2-2 BG7 H",        "E29:11", NULL, "grant level from E29", e29_out, 1, 1, 0),
    SIG("K2-2 BG6 H",        "E29:12", NULL, "grant level from E29", e29_out, 1, 0, 0),
    SIG("K2-2 BG5 H",        "E29:9",  NULL, "grant level from E29", e29_out, 1, 3, 0),
    SIG("K2-2 BG4 H",        "E29:10", NULL, "grant level from E29", e29_out, 1, 2, 0),
    UWB("K2-2 BG ENAB H",    "E21:6",  "grants enabled during BUT SERVICE", 42, 0),
    SIG("K2-2 BG (1) H",     "E12:9",  NULL, "grant sent", bus_grant, 1, 0, 0),
    { .name = "BUS BG7 H", .loc = "E77:10", .desc = "bus grant 7", .kind = PROBE_EVAL, .eval = eval_bus_bg7 },
    { .name = "BUS BG6 H", .loc = "E77:4",  .desc = "bus grant 6", .kind = PROBE_EVAL, .eval = eval_bus_bg6 },
    { .name = "BUS BG5 H", .loc = "E77:13", .desc = "bus grant 5", .kind = PROBE_EVAL, .eval = eval_bus_bg5 },
    { .name = "BUS BG4 H", .loc = "E77:1",  .desc = "bus grant 4", .kind = PROBE_EVAL, .eval = eval_bus_bg4 },
    SIG("BUS SACK L",        NULL,     NULL, "grant acknowledged by the device", bus_grant, 1, 0, 1),
    { .name = "K2-2 BG INH L", .loc = "E4:8", .desc = "clock held during a grant or RESET",
      .kind = PROBE_EVAL, .eval = eval_bg_inh_l },
    SIG("K2-2 RESET (1) H",  "E16",    NULL, "RESET instruction one-shot", reset_pulse, 1, 0, 0),
    SIG("BUS INIT L",        "E27:4",  NULL, "Unibus initialize", bus_init_l, 1, 0, 0),
    { .name = "BUS AC LO L", .desc = "power fail warning, not modelled: high, as with good power",
      .kind = PROBE_EVAL, .eval = eval_high },
    { .name = "BUS DC LO L", .desc = "power not ready, not modelled: high, as with good power",
      .kind = PROBE_EVAL, .eval = eval_high },

    /* K2-1 DAT TRAN */
    SIG("K2-1 UBUS C1 (1) H", "E13:15", NULL, "transfer type", bus_c, 1, 1, 0),
    SIG("K2-1 UBUS C0 (1) H", "E13:2",  NULL, "transfer type (byte)", bus_c, 1, 0, 0),
    SIG("BUS MSYN L",        "E27",    NULL, "master sync", msyn, 1, 0, 1),
    SIG("BUS SSYN L",        "E27",    NULL, "slave sync", ssyn, 1, 0, 1),
    SIG("K2-1 TRAN INH L",   "E90:12", NULL, "clock stopped for the transfer", msyn, 1, 0, 1),
    SIG("K2-1 TO (1) H",     "E32",    NULL, "no SSYN: bus timeout", timeout, 1, 0, 0),
    SIG("K2-1 ABORT H",      "E41:8",  NULL, "transfer aborted", abort_h, 1, 0, 0),
    SIG("K2-1 SET BE L",     "E42:8",  NULL, "sets BE", set_be, 1, 0, 1),
    SIG("K2-4 DISABLE MSYN+1 L", "E112:6", NULL, "no MSYN, byte increment by 1", dis_msyn_p1, 1, 0, 1),
    SIG("K2-4 SPA 03 H",     NULL,     NULL, "scratchpad address (read)", spa, 1, 3, 0),
    SIG("K2-4 SPA 02 H",     NULL,     NULL, "scratchpad address (read)", spa, 1, 2, 0),
    SIG("K2-4 SPA 01 H",     NULL,     NULL, "scratchpad address (read)", spa, 1, 1, 0),
    SIG("K2-4 SPA 00 H",     NULL,     NULL, "scratchpad address (read)", spa, 1, 0, 0),
    SIG("K2-4 SP WRITE L",   NULL,     NULL, "scratchpad write", sp_write, 1, 0, 1),
    { .name = "K2-3 R6+7 L", .loc = "E37:3", .desc = "source register is R6 or R7",
      .kind = PROBE_EVAL, .eval = eval_r6_7_l },
    { .name = "K1-5 PROC CLK L", .desc = "processor clock, high when sampled (a .sr export draws its low pulse)",
      .kind = PROBE_EVAL, .eval = eval_high },
    SIG("K1-5 LONG CYCLE",   "E30",    NULL, "long cycle selected (LONG CYCLE L or LOAD BA with relocation)", clk_long, 1, 0, 0),

    /* K2-3 Service */
    SIG("BUS INTR L",        NULL,     NULL, "interrupt, vector on the D lines", intr, 1, 0, 1),
    SIG("K2-3 INTR H",       "E1:6",   NULL, "interrupt received", intr, 1, 0, 0),
    SIG("K2-3 ENAB GRANTS H", "E51:9", NULL, "no trap pending", e51_out, 1, 7, 0),
    SIG("K2-3 PFAIL SERV H", "E51:1",  NULL, "power fail serviced", e51_out, 1, 0, 0),
    SIG("K2-3 STOV SERV H",  "E51:3",  NULL, "stack overflow serviced", e51_out, 1, 2, 0),
    SIG("K2-3 C4 H",         "E51:5",  NULL, "trap vector bit 4", e51_out, 1, 4, 0),
    SIG("K2-3 C3 H",         "E51:6",  NULL, "trap vector bit 3", e51_out, 1, 5, 0),
    SIG("K2-3 C2 H",         "E51:7",  NULL, "trap vector bit 2", e51_out, 1, 6, 0),
    { .name = "K2-3 INT VECTOR L", .loc = "E3:8", .desc = "trap vector onto the AMUX constants",
      .kind = PROBE_EVAL, .eval = eval_int_vector_l },
    UWB("K2-3 SERV IR H",    "E28:10", "clears the IR after service", 42, 0),
    SIG("K2-1 BE (1) H",     "E33:5",  NULL, "bus error", bus_error, 1, 0, 0),
    SIG("K2-1 KTE (1) H",    NULL,     NULL, "memory management error", kte, 1, 0, 0),
    SIG("K2-3 DBE H",        "E93:4",  NULL, "double bus error", dbe, 1, 0, 0),
    { .name = "K2-3 USER MODE H", .loc = "E93:10", .desc = "user mode, forced off by BE",
      .kind = PROBE_EVAL, .eval = eval_user_mode },
    D("K2:E52:14", NULL,  "E52 O1 (E51 A3)",       e52_code, 1, 0),
    D("K2:E52:13", NULL,  "E52 O2 (E51 A2)",       e52_code, 1, 1),
    D("K2:E52:12", NULL,  "E52 O3 (E51 A1)",       e52_code, 1, 2),
    D("K2:E52:11", NULL,  "E52 O4 (E51 A0)",       e52_code, 1, 3),

    /* K2-6 trap decode */
    { .name = "K2-6 IR CODE 02 L", .loc = "E53:9/E59:9", .desc = "trap code, wire-OR",
      .kind = PROBE_EVAL, .eval = eval_irc02 },
    { .name = "K2-6 IR CODE 01 L", .loc = "E53:10/E59:10", .desc = "trap code, wire-OR",
      .kind = PROBE_EVAL, .eval = eval_irc01 },
    { .name = "K2-6 IR CODE 00 L", .loc = "E53:11/E59:11/E70:12/E74:6", .desc = "trap code, wire-OR",
      .kind = PROBE_EVAL, .eval = eval_irc00 },
    SIG("K2-6 HALT RQST L",  "E53:12", NULL, "HALT instruction in kernel mode", e53_out, 1, 0, 0),
    SIG("K2-6 DISABLE LOAD PSW H", "E54:11", NULL, "PSW 15-12 kept", e54_out, 1, 1, 0),
    SIG("K2-6 ENAB TBIT H",  "E54:10", NULL, "trace trap allowed", e54_out, 1, 2, 0),
    SIG("K2-6 START RESET H", "E54:9", NULL, "RESET instruction in kernel mode", e54_out, 1, 3, 0),

    /* The slices take their carries (ALU COUT 3/7/11 L) from the 74S182
     * look-ahead generator E27. The carry out of the top slice is
     * K1-4 ALU COUT H, used by the MPC 07 branch condition (K2-9). */
    ALIAS("ALU", "ALU_OUT",   "ALU F outputs, K1 E7 E28 E18 E38",   alu_out, 2),
    ALIAS("ALU", "ALU_COUT",  "carry out of the top slice (K1-4 ALU COUT H)", alu_cout, 1),

    DERIVED("K2:E58:8",  NULL,       "NAND(IR 14-12), E74 A4",    eval_e85_pin8),
    DERIVED("K2:E53:EN", "E53_EN",   "E53 enabled",               eval_e53_enable),
    DERIVED("K2:E50:11", "TBIT_TRAP","T AND ENAB TBIT",           eval_e50_pin3),
    DERIVED("K2:E73:4",  NULL,       "NOR(IR 08, IR 15)",          eval_e73),

    ALIAS("Microsequencer", "MPC",       "microprogram counter, K2-7 E111 E112",   mpc, 2),
    ALIAS("Microsequencer", "NEXT_MPC",  "next MPC (9-bit)",               next_mpc, 2),
    ALIAS("Registers", "IR",        "instruction register, K2-5 E33 E56 E66 E67",   ir, 2),
    ALIAS("Registers", "PSW",       "processor status word, K1-1 E97 (NZVC), K1-2 E99 (7:5), K1-4 E82 (15:12)",           psw, 2),
    ALIAS("ALU", "ALU_S",     "ALU function S3:S0",              alu_s, 1),
    ALIAS("ALU", "ALU_M",     "ALU mode",                        alu_mode, 1),
    ALIAS("ALU", "A_LEG",     "ALU A inputs, K1 E7 E28 E18 E38",                     a_leg, 2),
    ALIAS("ALU", "B_LEG",     "ALU B inputs, K1 E7 E28 E18 E38",                     b_leg, 2),
    ALIAS("Unibus", "BA",        "bus address, K1-6 BA E44 E55 E65, VBA E47 E48",             ba, 4),
    ALIAS("Unibus", "DATA",      "UNIBUS data, K1 E1 E23 E12 E33 (8641)",                     unibus_data, 2),
    ALIAS("Registers", "BREG",      "B shift register, K1 E10 E31 E21 E41",                b_reg, 2),
    ALIAS("Registers", "BXREG",     "BX shift register, K1 E11 E32 E22 E52",               bx_reg, 2),
    ALIAS("Registers", "PC",        "R7/PC",                            pc, 2),
    ALIAS("Registers", "SP",        "R6/SP (kernel)",                   sp, 2),
    ALIAS("Registers", "R0",        "R0",                               r0, 2),
    ALIAS("Registers", "R1",        "R1",                               r1, 2),
    ALIAS("Registers", "R2",        "R2",                               r2, 2),
    ALIAS("Registers", "R3",        "R3",                               r3, 2),
    ALIAS("Registers", "R4",        "R4",                               r4, 2),
    ALIAS("Registers", "R5",        "R5",                               r5, 2),
    ALIAS("Registers", "SP_USER",   "R6/SP (user)",                     sp_user, 2),
    ALIAS("Decode & traps", "IR_CODE",   "trap code (3-bit)",                ir_code, 1),
    ALIAS("Decode & traps", "SVC_TRAP",  "service trap flag",                service_trap, 1),
    ALIAS("Unibus", "BUS_ERR",   "bus error (NXM)",                  bus_error, 1),
    ALIAS("Unibus", "BUS_OP",    "bus operation (0/1/2)",            bus_op, 1),
    ALIAS("Decode & traps", "HALTED",    "CPU halted flag",                  halted, 1),
    ALIAS("Unibus", "LONG_CYC",  "long cycle active",                long_cycle, 1),
    ALIAS("Unibus", "BUF_TRAN",  "BUF_DAT_TRAN",                    buf_dat_tran, 1),
    ALIAS("Decode & traps", "IR_DECODE", "K2-6 IR DECODE (1) H (E55)",        ir_valid, 1),
    ALIAS("Emulator", "CYCLE_NS",  "cycle duration (ns)",              cycle_ns, 4),
    ALIAS("Emulator", "SSYN_NS",   "slave SSYN response (ns)",        ssyn_ns, 2),
    ALIAS("Emulator", "EOT_NS",    "EOT delay (ns)",                  eot_ns, 2),
    ALIAS("Emulator", "NS",        "simulation time (ns)",             ns, 8),
};

#define PROBE_DEF_COUNT  (int)(sizeof(probe_defs) / sizeof(probe_defs[0]))
#define PROBE_DEF_COUNT_MAX 1024



int probe_def_count(void) {
    return PROBE_DEF_COUNT;
}

const ProbeDef *probe_def_get(int idx) {
    if (idx < 0 || idx >= PROBE_DEF_COUNT)
        return NULL;
    return &probe_defs[idx];
}

const char *probe_display_name(const ProbeDef *def) {
    if (def->name)
        return def->name;
    return def->alias;
}

/* Upper case, single spaces, no leading or trailing space */
static void probe_norm(const char *in, char *out, size_t max) {
    size_t n = 0;
    int space = 0;
    while (*in == ' ' || *in == '\t') in++;
    for (; *in && n + 1 < max; in++) {
        if (*in == ' ' || *in == '\t') { space = 1; continue; }
        if (space && n + 1 < max) out[n++] = ' ';
        space = 0;
        out[n++] = (char)toupper((unsigned char)*in);
    }
    out[n] = '\0';
}

/* Board of a signal from its sheet: "K1" (M8265) or "K2" (M8266) */
const char *probe_def_board(const ProbeDef *d) {
    if (!d->name) return NULL;
    if (!strncasecmp(d->name, "K1", 2) && (d->name[2] == '-' || d->name[2] == ':')) return "K1";
    if (!strncasecmp(d->name, "K2", 2) && (d->name[2] == '-' || d->name[2] == ':')) return "K2";
    return NULL;
}

/* Board-qualified chip and pin, "K2:E97:1", or NULL */
const char *probe_def_qloc(int idx) {
    static char q[PROBE_DEF_COUNT_MAX][20];
    if (idx < 0 || idx >= PROBE_DEF_COUNT || idx >= PROBE_DEF_COUNT_MAX) return NULL;
    const ProbeDef *d = &probe_defs[idx];
    const char *b = probe_def_board(d);
    if (!b || !d->loc || !strchr(d->loc, ':')) return NULL;
    if (!q[idx][0]) snprintf(q[idx], sizeof q[idx], "%s:%s", b, d->loc);
    return q[idx];
}

/* Signal by DEC name, alias, chip and pin ("E97:1", unique) or board, chip
 * and pin ("K2:E97:1"). Spaces and case do not matter. -1 unknown,
 * -2 ambiguous (the same chip number on both boards). */
int probe_find_signal(const char *name) {
    char want[96], have[96];
    probe_norm(name, want, sizeof want);
    for (int i = 0; i < PROBE_DEF_COUNT; i++) {
        if (probe_defs[i].name) {
            probe_norm(probe_defs[i].name, have, sizeof have);
            if (!strcmp(want, have)) return i;
        }
        if (probe_defs[i].alias && !strcasecmp(want, probe_defs[i].alias))
            return i;
        const char *q = probe_def_qloc(i);
        if (q && !strcasecmp(want, q)) return i;
    }
    int found = -1;
    for (int i = 0; i < PROBE_DEF_COUNT; i++) {
        if (probe_defs[i].loc && strchr(probe_defs[i].loc, ':') &&
            probe_def_board(&probe_defs[i]) && !strcasecmp(want, probe_defs[i].loc)) {
            if (found >= 0) return -2;
            found = i;
        }
    }
    return found;
}



uint32_t probe_read_value(const ProbeSnapshot *snap, const ProbeDef *def) {
    uint32_t val;

    switch (def->kind) {
    case PROBE_EVAL:
        return def->eval ? def->eval(snap) : 0;
    case PROBE_UW:
        val = (uint32_t)((snap->uw >> def->bit) & 1);
        break;
    case PROBE_UW_NEXT:
        val = (uint32_t)((snap->uw_next >> def->bit) & 1);
        break;
    case PROBE_MISC:
        return UW_MISC(snap->uw) == def->bit ? 0 : 1;   /* active low */
    case PROBE_ZERO4:
        return ((*(const uint16_t *)((const uint8_t *)snap + def->offset) >> def->bit) & 0xF) == 0;
    default: {
        const uint8_t *base = (const uint8_t *)snap + def->offset;
        switch (def->size) {
        case 1: val = *(const uint8_t *)base; break;
        case 2: val = *(const uint16_t *)base; break;
        case 4: val = *(const uint32_t *)base; break;
        case 8: val = (uint32_t)*(const uint64_t *)base; break;
        default: val = 0; break;
        }
        if (def->bit == PROBE_BIT_WHOLE)
            return val;
        val = (val >> def->bit) & 1;
        break;
    }
    }
    return def->inv ? val ^ 1 : val;
}

int probe_is_1bit(const ProbeDef *def) {
    switch (def->kind) {
    case PROBE_UW:
    case PROBE_UW_NEXT:
    case PROBE_MISC:
    case PROBE_ZERO4:
        return 1;
    case PROBE_SNAP:
        return def->bit != PROBE_BIT_WHOLE;
    default:
        return 0;
    }
}



void probe_init(Probe *p, ProbeSnapshot *buf, uint32_t depth) {
    memset(p, 0, sizeof(*p));
    p->buf = buf;
    p->capacity = depth;
    probe_set_depth(p, depth);
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
    /* Never allow the ring to address beyond the caller's buffer. */
    if (depth > PROBE_MAX_DEPTH)
        depth = PROBE_MAX_DEPTH;
    if (depth > p->capacity)
        depth = p->capacity;
    /* Round down to power of 2. Initialization requires a nonempty buffer. */
    uint32_t d = 1;
    while (d * 2 <= depth)
        d *= 2;
    if (d != p->depth)
        probe_reset(p);
    p->depth = d;
    p->mask = d - 1;
}

void probe_set_trigger_edge(Probe *p, int edge) {
    p->trigger_edge = edge ? 1 : 0;
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
    p->trigger_prev = -1;
    p->triggered = 0;
    p->trigger_abs = 0;

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
        int cond = (val & p->trigger_mask) == (p->trigger_value & p->trigger_mask);
        int triggered = p->trigger_edge ? (cond && p->trigger_prev == 0) : cond;
        p->trigger_prev = cond;

        /* Store sample only on divider tick */
        int stored = 0;
        if (--p->div_counter == 0) {
            p->div_counter = p->divider;
            p->buf[p->head & p->mask] = *snap;
            p->head++;
            p->count++;
            stored = 1;
        }

        /* Like a hardware analyzer, ignore the trigger until the
         * pre-trigger part of the buffer is full */
        if (triggered && p->count < p->pre_trigger_count)
            triggered = 0;

        if (triggered) {
            p->state = PROBE_CAPTURING;
            p->trigger_sample = ((p->head > 0 ? p->head : 1) - 1) & p->mask;
            p->triggered = 1;
            p->trigger_abs = p->head > 0 ? p->head - 1 : 0;
            /* The trigger sample, if stored, is the first post-trigger one */
            p->post_trigger_remaining = p->depth - p->pre_trigger_count;
            if (stored && p->post_trigger_remaining > 0)
                p->post_trigger_remaining--;
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

uint32_t probe_valid_count(const Probe *p) {
    return p->count < p->depth ? p->count : p->depth;
}

const ProbeSnapshot *probe_get(const Probe *p, uint32_t idx) {
    uint32_t n = probe_valid_count(p);
    if (idx >= n)
        return NULL;
    return &p->buf[(p->head - n + idx) & p->mask];
}

int probe_trigger_index(const Probe *p) {
    if (!p->triggered)
        return -1;
    uint32_t first = p->head - probe_valid_count(p);
    if (p->trigger_abs < first)
        return -1;
    return (int)(p->trigger_abs - first);
}

/* Chip names, from pdp11/kd11ea/roms/README.md for the ROMs, and sheet
 * titles (in parentheses on each drawing) */
static const struct { const char *chip, *name; } probe_chips[] = {
    { "K1-1",    "Data path 0-3" },
    { "K1-2",    "Data path 4-7" },
    { "K1-3",    "Data path 8-11" },
    { "K1-4",    "Data path 12-15" },
    { "K1-5",    "CPU clock" },
    { "K1-6",    "Unibus address" },
    { "K1-7",    "PAR + PDR" },
    { "K1-8",    "SR0 and KT error" },
    { "K1-9",    "SR2 and KT MUX" },
    { "K1-10",   "CC decode, address detection" },
    { "K2-1",    "Data transfer" },
    { "K2-2",    "Priority arbitration" },
    { "K2-3",    "Power fail, auto restart, BUT SERVICE" },
    { "K2-4",    "Scratchpad address multiplexer" },
    { "K2-5",    "IR, AUX control, CC code" },
    { "K2-6",    "IR decode" },
    { "BUS",     "Unibus" },
    { "K2:E58",  "IR 14-12 NAND gate" },
    { "K2-7",    "MPC BUT field" },
    { "K2-8",    "Micro store" },
    { "K2-9",    "Micro code" },
    { "K2-10",   "ROM SPA counter" },
    { "K2:E50",  "7408 AND gates" },
    { "K2:E51",  "Service dispatch and trap-vector selection" },
    { "K2:E52",  "Service and trap priority encoder" },
    { "K2:E53",  "Instruction trap and HALT decoder" },
    { "K2:E54",  "RESET and trace-trap control" },
    { "K2:E59",  "Single-operand instruction decode, low part" },
    { "K2:E60",  "Single-operand instruction decode, high part" },
    { "K2:E63",  "Operate, RTS and condition-code instruction decode" },
    { "K2:E69",  "Double-operand instruction decode, low part" },
    { "K2:E70",  "Double-operand instruction decode, high part" },
    { "K2:E71",  "Conditional branch decode" },
    { "K2:E73",  "74S02 NOR gates" },
    { "K2:E74",  "Extended Instruction Set decode" },
    { "K2:E80",  "Sign EXtension, byte swapping and byte-write control" },
    { "K2:E82",  "Primary microcode ALU-function expansion" },
    { "K2:E85",  "74S10 NAND gates" },
    { "K2:E87",  "B/BX shift-register mode expansion" },
    { "K2:E102", "Micro-branch (BUT) expansion" },
};

const char *probe_chip_name(const char *chip) {
    for (size_t i = 0; i < sizeof(probe_chips) / sizeof(probe_chips[0]); i++)
        if (strcasecmp(chip, probe_chips[i].chip) == 0)
            return probe_chips[i].name;
    return NULL;
}
