/* ROM address wiring and output fields.       */
/* Slop-coded with love by an LLM.             */
/* Sorry, no way I was doing this manually...  */

#ifndef KD11EA_ROM_WIRING_H
#define KD11EA_ROM_WIRING_H

#include <stdint.h>
#include "roms/rom_images.h"

/* Address bit n is PROM input An, data bit n is output O(n+1).
 *   82S129/131: A0-A8 pins 5 6 7 4 3 2 1 15 14, O1-O4 pins 12 11 10 9
 *   82S137:     A0-A9 pins 5 6 7 4 3 2 1 17 16 15, O1-O4 pins 14 13 12 11
 *   82S123:     A0-A4 pins 10-14, O1-O8 pins 1-7, 9
 * Inputs and outputs below are listed A0 upwards and O1 upwards. */

/* E29 (K2-2, 23-160A2) BR arbitration: PSW07 PSW06 HLT RQST BR6 BR4 BR7
 * BR5 PSW05 (BRs latched in E19) -> BG6 BG7 BG4 BG5 */
static inline uint8_t e29_addr(uint8_t priority, uint8_t halt,
                               uint8_t br7, uint8_t br6,
                               uint8_t br5, uint8_t br4)
{
    return ((priority >> 2) & 1) |
           (((priority >> 1) & 1) << 1) |
           ((halt & 1) << 2) |
           ((br6 & 1) << 3) |
           ((br4 & 1) << 4) |
           ((br7 & 1) << 5) |
           ((br5 & 1) << 6) |
           ((priority & 1) << 7);
}

#define E29_BG6_H(d) ((d) & 1)
#define E29_BG7_H(d) (((d) >> 1) & 1)
#define E29_BG4_H(d) (((d) >> 2) & 1)
#define E29_BG5_H(d) (((d) >> 3) & 1)

static inline uint8_t e82_addr(uint8_t alu_field) {
    return ((alu_field >> 4) & 1) << 4 |
           ((alu_field >> 1) & 1) << 3 |
           ((alu_field >> 2) & 1) << 2 |
           ((alu_field >> 0) & 1) << 1 |
           ((alu_field >> 3) & 1) << 0;
}

#define E82_BLEG(d)         (((((d) >> 6) & 1) << 1) | (((d) >> 7) & 1))
#define E82_CIN_L(d)        (((d) >> 5) & 1)
#define E82_MODE(d)         (((d) >> 2) & 1)
static inline uint8_t E82_ALU_S(uint8_t d) {
    return ((~d & 1) << 3) |
           (((~d >> 1) & 1) << 2) |
           (((~d >> 3) & 1) << 1) |
           (((~d >> 4) & 1));
}

#define BLEG_BREG           0
#define BLEG_BXREG          1
#define BLEG_PLUS16         2
#define BLEG_PLUS1          3

static inline uint8_t e87_addr(uint8_t bbx_field) {
    return ((bbx_field >> 1) & 1) << 4 |
           ((bbx_field >> 3) & 1) << 3 |
           ((bbx_field >> 2) & 1) << 2 |
           ((bbx_field >> 0) & 1) << 1 |
           0;
}

static inline uint8_t E87_B_MODE(uint8_t d) {
    return (((~d >> 1) & 1) << 1) | ((~d) & 1);
}
static inline uint8_t E87_BX_MODE(uint8_t d) {
    return (((d >> 3) & 1) << 1) | ((d >> 2) & 1);
}
static inline uint8_t E87_SHIFT_MUX_L(uint8_t d) {
    return (((d >> 4) & 1) << 1) | ((d >> 5) & 1);
}
#define E87_ENAB_OVX_L(d)   (((d) >> 6) & 1)
#define E87_ENAB_DBE_L(d)   (((d) >> 7) & 1)

/* 74194 modes, (S0 << 1) | S1 */
#define BMODE_HOLD          0
#define BMODE_SHIFT_L       1
#define BMODE_SHIFT_R       2
#define BMODE_LOAD          3

static inline uint8_t e102_addr(uint8_t but_field) {
    return ((but_field >> 1) & 1) << 4 |
           ((but_field >> 3) & 1) << 3 |
           ((but_field >> 2) & 1) << 2 |
           ((but_field >> 0) & 1) << 1 |
           1;
}

#define E102_BUT_ALU_COUT_MPC7(d) (((d) >> 0) & 1)
#define E102_BUT_CC_N_MPC6(d)     (((d) >> 1) & 1)
#define E102_BUT_SP15_MPC3(d)     (((d) >> 2) & 1)
#define E102_BUT_BXREG01_MPC5(d)  (((d) >> 3) & 1)
#define E102_BUT_BXREG00_MPC4(d)  (((d) >> 4) & 1)
#define E102_BUT_COUNT05_MPC2(d)  (((d) >> 5) & 1)
#define E102_BUT_CC_Z_MPC1(d)     (((d) >> 6) & 1)
#define E102_BUT_IR09_MPC0(d)     (((d) >> 7) & 1)

/* E59/E60 (K2-6) single operand: DM0 IR06-IR11 IR15.
 * E59 -> DEST L, IR CODE 00-02 L. E60 -> MPC 03-06 L. */
static inline uint8_t e59_e60_addr(uint16_t ir) {
    uint8_t dm0 = ((ir >> 3) & 7) == 0;
    return (uint8_t)(dm0 | (((ir >> 6) & 1) << 1) | (((ir >> 7) & 1) << 2) |
                     (((ir >> 8) & 1) << 3) | (((ir >> 9) & 1) << 4) |
                     (((ir >> 10) & 1) << 5) | (((ir >> 11) & 1) << 6) |
                     (((ir >> 15) & 1) << 7));
}
#define E59_DEST_L(d)    ((d) & 1)
#define E60_MPC03_L(d)   ((d) & 1)
#define E60_MPC04_L(d)   (((d) >> 1) & 1)
#define E60_MPC05_L(d)   (((d) >> 2) & 1)
#define E60_MPC06_L(d)   (((d) >> 3) & 1)

/* E63 (K2-6, 23-176A2) operate group: IR00 IR05 IR07 IR01-IR04 IR06
 * -> MPC 03 02 01 00 L */
static inline uint8_t e63_addr(uint16_t ir) {
    return (ir & 1)                |
           (((ir >> 5) & 1) << 1) |
           (((ir >> 7) & 1) << 2) |
           (((ir >> 1) & 1) << 3) |
           (((ir >> 2) & 1) << 4) |
           (((ir >> 3) & 1) << 5) |
           (((ir >> 4) & 1) << 6) |
           (((ir >> 6) & 1) << 7);
}
#define E63_MPC03_L(d)   ((d) & 1)
#define E63_MPC02_L(d)   (((d) >> 1) & 1)
#define E63_MPC01_L(d)   (((d) >> 2) & 1)
#define E63_MPC00_L(d)   (((d) >> 3) & 1)

/* E68 (K2-5, 23-014A9) CC category: IR12 IR09 IR10 IR07 IR06 IR14 IR15
 * IR13 IR08 -> CC CODE 00-02, BYTE L */
static inline uint16_t e68_addr(uint16_t ir) {
    return (uint16_t)((((ir >> 12) & 1) << 0) | (((ir >>  9) & 1) << 1) |
                      (((ir >> 10) & 1) << 2) | (((ir >>  7) & 1) << 3) |
                      (((ir >>  6) & 1) << 4) | (((ir >> 14) & 1) << 5) |
                      (((ir >> 15) & 1) << 6) | (((ir >> 13) & 1) << 7) |
                      (((ir >>  8) & 1) << 8));
}

/* E69/E70 (K2-6) double operand: IR12 DM0 SM0 IR DECODE H BUT DEST L IR14
 * IR15 IR13 (E70: FP11 ATTACHED L).
 * E69 -> SRC H, MOV L, MPC 03-04 L. E70 -> IR CODE 00 L, MPC 05-07 L. */
static inline uint8_t e69_addr(uint16_t ir, int but_dest_l, int ir_decode_h) {
    uint8_t dm0 = ((ir >> 3) & 7) == 0;
    uint8_t sm0 = ((ir >> 9) & 7) == 0;
    return (uint8_t)((((ir >> 12) & 1) << 0) | (dm0 << 1) | (sm0 << 2) |
                     ((ir_decode_h & 1) << 3) | ((but_dest_l & 1) << 4) |
                     (((ir >> 14) & 1) << 5) | (((ir >> 15) & 1) << 6) |
                     (((ir >> 13) & 1) << 7));
}

static inline uint16_t e70_addr(uint16_t ir, int but_dest_l, int ir_decode_h) {
    return (uint16_t)(e69_addr(ir, but_dest_l, ir_decode_h) | (1 << 8)); /* no FP11 */
}
#define E69_SRC_H(d)        ((d) & 1)
#define E69_MOV_L(d)        (((d) >> 1) & 1)
#define E69_MPC03_L(d)      (((d) >> 2) & 1)
#define E69_MPC04_L(d)      (((d) >> 3) & 1)
#define E70_IR_CODE_00_L(d) ((d) & 1)
#define E70_MPC05_L(d)      (((d) >> 1) & 1)
#define E70_MPC06_L(d)      (((d) >> 2) & 1)
#define E70_MPC07_L(d)      (((d) >> 3) & 1)

/* E71 (E72, 23-022A2, on K2-6) branch: IR08-IR10 IR15 V Z N C -> MPC 07 L */
static inline uint8_t e71_addr(uint16_t ir, uint16_t psw) {
    return ((ir >> 8) & 7)            |
           (((ir >> 15) & 1) << 3)   |
           (((psw >> 1) & 1) << 4)   |
           (((psw >> 2) & 1) << 5)   |
           (((psw >> 3) & 1) << 6)   |
           ((psw & 1) << 7);
}
#define E71_MPC07_L(d)   ((d) & 1)

/* E74 (K2-6, 82S123) EIS: IR09-IR11 IR DECODE H NAND(IR14-12)
 * -> -, MPC 03-06 L, IR CODE 00 L, DEST L, MPC 07 L */
static inline uint8_t e74_addr(uint16_t ir, int ir_decode_h) {
    uint8_t nand_121314 = !(((ir >> 14) & 1) && ((ir >> 13) & 1) && ((ir >> 12) & 1));
    return (uint8_t)(((ir >> 9) & 1) | (((ir >> 10) & 1) << 1) | (((ir >> 11) & 1) << 2) |
                     ((ir_decode_h & 1) << 3) | (nand_121314 << 4));
}
#define E74_MPC03_L(d)      (((d) >> 1) & 1)
#define E74_MPC04_L(d)      (((d) >> 2) & 1)
#define E74_MPC05_L(d)      (((d) >> 3) & 1)
#define E74_MPC06_L(d)      (((d) >> 4) & 1)
#define E74_IR_CODE_00_L(d) (((d) >> 5) & 1)
#define E74_DEST_L(d)       (((d) >> 6) & 1)
#define E74_MPC07_L(d)      (((d) >> 7) & 1)

/* E107 (K1-10) C and V: CC CODE 00-02 CBIT ROT CBIT D S R signs -> -, -, C, V */
static inline uint8_t e107_addr(uint8_t r_msb, uint8_t s_msb, uint8_t d_msb,
                                uint8_t rot_cbit, uint8_t psw_cbit, uint8_t cc_code) {
    return (uint8_t)((cc_code & 7) | ((psw_cbit & 1) << 3) | ((rot_cbit & 1) << 4) |
                     ((d_msb & 1) << 5) | ((s_msb & 1) << 6) | ((r_msb & 1) << 7));
}
#define E107_CC_V(d)     (((d) >> 3) & 1)
#define E107_CC_C(d)     (((d) >> 2) & 1)

#define E68_CC_CODE(d)   ((d) & 7)
#define E68_BYTE_L(d)    (((d) >> 3) & 1)

/* E83 (K2-5, 82S123) DOP ALU function: GND IR12-IR15
 * -> FUNC CODE 02 01 00, ..., FUNC CODE 03 */
static inline uint8_t e83_addr(uint16_t ir) {
    return (uint8_t)((((ir >> 12) & 1) << 1) | (((ir >> 13) & 1) << 2) |
                     (((ir >> 14) & 1) << 3) | (((ir >> 15) & 1) << 4));
}

#define E83_FUNC_CODE(d)   ((uint8_t)((((d) >> 7) & 1) << 3 | ((d) & 1) << 2 | \
                                      (((d) >> 1) & 1) << 1 | (((d) >> 2) & 1)))

/* E61 (K2-5) SOP ALU function: IR15 IR06-IR10 N C -> FUNC CODE 00-03 */
static inline uint8_t e61_addr(uint16_t ir, uint8_t cbit, uint8_t nbit) {
    return (uint8_t)((((ir >> 15) & 1) << 0) | (((ir >> 6) & 1) << 1) | (((ir >> 7) & 1) << 2) |
                     (((ir >> 8) & 1) << 3) | (((ir >> 9) & 1) << 4) | (((ir >> 10) & 1) << 5) |
                     ((nbit & 1) << 6) | ((cbit & 1) << 7));
}

/* E62 (K2-5) rotate/shift: IR10 CBIT B00 CC N IR09 IR06 IR07 IR08
 * -> SERIAL SHIFT, B MODE 01 L, B MODE 00 L, ROT CBIT */
static inline uint8_t e62_addr(uint16_t ir, uint8_t breg0, uint8_t cbit, uint8_t cc_n) {
    return (uint8_t)((((ir >> 10) & 1) << 0) | ((cbit & 1) << 1) | ((breg0 & 1) << 2) |
                     ((cc_n & 1) << 3) | (((ir >> 9) & 1) << 4) | (((ir >> 6) & 1) << 5) |
                     (((ir >> 7) & 1) << 6) | (((ir >> 8) & 1) << 7));
}

#define E62_SERIAL_SHIFT(d)  ((d) & 1)
#define E62_B_MODE_01_L(d)   (((d) >> 1) & 1)
#define E62_B_MODE_00_L(d)   (((d) >> 2) & 1)
#define E62_ROT_CBIT(d)      (((d) >> 3) & 1)


/* E80 (K2-8, 23-163A2) byte and write control: BYTE H, VBA00 AND BDT,
 * MOV L, AUX, BDT, SS01, GND, SS00 -> DISABLE UPPER, DISAB MSYN L, SEX, SWAP */
static inline uint8_t e80_addr(uint8_t ss01, uint8_t ss00, uint8_t bdt,
                                uint8_t aux, uint8_t byte_h, uint8_t vba00_bdt,
                                uint8_t mov_l) {
    return (uint8_t)((byte_h & 1) | ((vba00_bdt & 1) << 1) | ((mov_l & 1) << 2) |
                     ((aux & 1) << 3) | ((bdt & 1) << 4) | ((ss01 & 1) << 5) |
                     ((ss00 & 1) << 7));
}

#define E80_DISABLE_UPPER(d) ((d) & 1)
#define E80_DISAB_MSYN_L(d)  (((d) >> 1) & 1)
#define E80_SEX(d)           (((d) >> 2) & 1)
#define E80_SWAP(d)          (((d) >> 3) & 1)

/* E53 (K2-6) trap decode: IR00 IR05 IR07 IR01-IR04 USER MODE
 * -> HALT RQST L, IR CODE 00-02 L. Enabled by IR06 low and E85. */
static inline uint8_t e53_addr(uint16_t ir, uint8_t user_mode) {
    return (uint8_t)((ir & 1) | (((ir >> 5) & 1) << 1) | (((ir >> 7) & 1) << 2) |
                     (((ir >> 1) & 1) << 3) | (((ir >> 2) & 1) << 4) |
                     (((ir >> 3) & 1) << 5) | (((ir >> 4) & 1) << 6) |
                     ((user_mode & 1) << 7));
}

static inline int e53_enabled(uint16_t ir) {
    if ((ir >> 6) & 1) return 0;
    if (((ir >> 12) & 7) != 0) return 0;
    if (((ir >> 9) & 7) != 0) return 0;
    if ((ir >> 8) & 1) return 0;
    if ((ir >> 15) & 1) return 0;
    return 1;
}

#define E53_HALT_RQST_L(d)  ((d) & 0x01)
#define E53_IR_CODE_00_L(d) (((d) >> 1) & 1)
#define E53_IR_CODE_01_L(d) (((d) >> 2) & 1)
#define E53_IR_CODE_02_L(d) (((d) >> 3) & 1)

static inline uint8_t e53_ir_code(uint8_t d) {
    return ((!E53_IR_CODE_02_L(d)) << 2) |
           ((!E53_IR_CODE_01_L(d)) << 1) |
           (!E53_IR_CODE_00_L(d));
}

static inline uint8_t e59_ir_code(uint8_t d) {
    return (uint8_t)((!((d >> 3) & 1) << 2) | (!((d >> 2) & 1) << 1) | !((d >> 1) & 1));
}


/* E54 (K2-6) reset/trap control: IR00 IR06 DM0 IR01 IR02 IR07 PSW15 L E85
 * -> -, DISABLE LOAD PSW, ENAB TBIT, START RESET */
static inline uint8_t e54_addr(uint16_t ir, uint8_t user_mode) {
    uint8_t dm0 = ((ir >> 3) & 7) == 0;
    uint8_t e85_l = !((ir & 0xFF00) == 0);
    return (uint8_t)((ir & 1) | (((ir >> 6) & 1) << 1) | (dm0 << 2) |
                     (((ir >> 1) & 1) << 3) | (((ir >> 2) & 1) << 4) |
                     (((ir >> 7) & 1) << 5) | ((!user_mode) << 6) | (e85_l << 7));
}

#define E54_DISABLE_LOAD_PSW(d) (((d) >> 1) & 1)
#define E54_ENAB_TBIT(d)        (((d) >> 2) & 1)
#define E54_START_RESET(d)      (((d) >> 3) & 1)

/* IR CODE 02-00 (TM 4.5.3) */
#define IR_CODE_RESERVED 1      /* vector 010 */
#define IR_CODE_ILLEGAL  2      /* vector 010, JMP/JSR mode 0 */
#define IR_CODE_BPT      3      /* vector 014 */
#define IR_CODE_IOT      4      /* vector 020 */
#define IR_CODE_EMT      5      /* vector 030 */
#define IR_CODE_TRAP     6      /* vector 034 */

/* E52 (K2-3, 82S137) service priority: T, BE, STOV, PE, KTE, PFAIL,
 * IR CODE 02-00 L, FP11 SERVICE BR PFAIL L -> E51 A3-A0 */
static inline uint16_t e52_addr(uint8_t tbit_and_enab, uint8_t stov,
                                uint8_t be, uint8_t pe, uint8_t kte,
                                uint8_t pfail, uint8_t ir_code,
                                uint8_t service_br_pfail_l)
{
    return (uint16_t)((tbit_and_enab & 1) | ((be & 1) << 1) | ((stov & 1) << 2) |
                      ((pe & 1) << 3) | ((kte & 1) << 4) | ((pfail & 1) << 5) |
                      (!((ir_code >> 2) & 1) << 6) | (!((ir_code >> 1) & 1) << 7) |
                      (!(ir_code & 1) << 8) | ((service_br_pfail_l & 1) << 9));
}

/* E51 (K2-3, 82S123) service vector: E52 O4-O1, BUT SERVICE (1)
 * -> PFAIL SERV, -, STOV SERV, -, C4, C3, C2, ENAB GRANTS */
static inline uint8_t e51_addr(uint8_t e52_d, uint8_t but_service_1) {
    return (uint8_t)(((e52_d >> 3) & 1) | (((e52_d >> 2) & 1) << 1) |
                     (((e52_d >> 1) & 1) << 2) | ((e52_d & 1) << 3) |
                     ((but_service_1 & 1) << 4));
}
#define E51_PFAIL_SERV(d)   ((d) & 1)
#define E51_STOV_SERV(d)    (((d) >> 2) & 1)
#define E51_C4(d)           (((d) >> 4) & 1)
#define E51_C3(d)           (((d) >> 5) & 1)
#define E51_C2(d)           (((d) >> 6) & 1)
#define E51_ENAB_GRANTS(d)  (((d) >> 7) & 1)
/* Trap vector on the AMUX constants input (K1-1, K1-2) */
#define SERVICE_VECTOR(d, kte, pe) \
    ((E51_C2(d) << 2) | (E51_C3(d) << 3) | (E51_C4(d) << 4) | \
     (((kte) & 1) << 5) | (((pe) & 1) << 6) | (((kte) & 1) << 7))

/* M8265 E74/E75 (K1-10, both 23-169A2): A0 (+3V on E74, GND on E75),
 * PBA01-03, PBA07, PBA00, C0, C1.
 * E74 -> KT MUX S0 L, PSW DATI L, INT SSYN L, INT DATI L.
 * E75 -> LOAD SR0 LOW/HIGH L, LOAD PSW LOW/HIGH L. */
static inline uint8_t m8265_e74_e75_addr(uint32_t pba, uint8_t c1,
                                         uint8_t c0, uint8_t e74)
{
    return (e74 & 1)                  |
           (((pba >> 1) & 1)    << 1) |
           (((pba >> 2) & 1)    << 2) |
           (((pba >> 3) & 1)    << 3) |
           (((pba >> 7) & 1)    << 4) |
           ((pba & 1)          << 5) |
           ((c0 & 1)           << 6) |
           ((c1 & 1)           << 7);
}

#define M8265_E74_KT_MUX_S0_L(d)    ((d) & 1)
#define M8265_E74_INT_SSYN_L(d)      (((d) >> 2) & 1)
#define M8265_E75_LOAD_SR0_LOW_L(d)  ((d) & 1)
#define M8265_E75_LOAD_SR0_HIGH_L(d) (((d) >> 1) & 1)
#define M8265_E74_PSW_DATI_L(d)      (((d) >> 1) & 1)
#define M8265_E74_INT_DATI_L(d)      (((d) >> 3) & 1)
#define M8265_E75_LOAD_PSW_LOW_L(d)  (((d) >> 2) & 1)
#define M8265_E75_LOAD_PSW_HIGH_L(d) (((d) >> 3) & 1)

/* M8265 E76/E77 (K1-10): PBA05 PBA06 PBA08 PBA09 PBA11 PBA00 C0 C1.
 * E76 -> KT MUX S0 L, -, PAR+PDR L, INT DATI L.
 * E77 -> LOAD PDR LOW/HIGH L, LOAD PAR LOW/HIGH L. */
static inline uint8_t m8265_e76_e77_addr(uint32_t pba, uint8_t c1,
                                         uint8_t c0)
{
    return ((pba >> 5) & 1)           |
           (((pba >> 6) & 1)    << 1) |
           (((pba >> 8) & 1)    << 2) |
           (((pba >> 9) & 1)    << 3) |
           (((pba >> 11) & 1)   << 4) |
           ((pba & 1)          << 5) |
           ((c0 & 1)           << 6) |
           ((c1 & 1)           << 7);
}

#define M8265_E76_KT_MUX_S0_L(d) ((d) & 1)
#define M8265_E76_PAR_PDR_L(d)    (((d) >> 2) & 1)
#define M8265_E77_LOAD_PDR_LOW_L(d)  ((d) & 1)
#define M8265_E77_LOAD_PAR_HIGH_L(d) (((d) >> 3) & 1)
#define M8265_E77_LOAD_PAR_LOW_L(d)  (((d) >> 2) & 1)
#define M8265_E77_LOAD_PDR_HIGH_L(d) (((d) >> 1) & 1)
#define M8265_E76_INT_DATI_L(d)     (((d) >> 3) & 1)

/* M8265 E83 (K1-8) SR0 errors: ED, compare, ACF1, ACF2, illegal mode,
 * DISABLE MSYN L, C0, C1 -> NR, PL, RO, SR0 MUX select */
static inline uint8_t m8265_e83_addr(uint8_t c1, uint8_t c0,
                                     uint8_t disable_msyn_l,
                                     uint8_t illegal_mode, uint8_t acf,
                                     uint8_t compare, uint8_t ed)
{
    return (ed & 1)                      |
           ((compare & 1)           << 1) |
           ((acf & 1)               << 2) |
           (((acf >> 1) & 1)        << 3) |
           ((illegal_mode & 1)      << 4) |
           ((disable_msyn_l & 1)    << 5) |
           ((c0 & 1)                << 6) |
           ((c1 & 1)                << 7);
}

#define M8265_E83_NR(d)        ((d) & 1)
#define M8265_E83_RO(d)        (((d) >> 2) & 1)
#define M8265_E83_PL(d)        (((d) >> 1) & 1)
#define M8265_E83_SR0_MUX_S(d) (((d) >> 3) & 1)

/* M8265 E86 (K1-10) general registers: C1, SACK RET, PBA11, PBA05-09
 * -> INT SSYN L, INT DATI L, SP WRITE L, GEN REG L */
static inline uint8_t m8265_e86_addr(uint32_t pba, uint8_t c1,
                                     uint8_t sack_ret)
{
    return (c1 & 1) |
           ((sack_ret & 1) << 1) |
           (((pba >> 11) & 1) << 2) |
           (((pba >> 5) & 1) << 3) |
           (((pba >> 6) & 1) << 4) |
           (((pba >> 7) & 1) << 5) |
           (((pba >> 8) & 1) << 6) |
           (((pba >> 9) & 1) << 7);
}

#define M8265_E86_INT_SSYN_L(d) ((d) & 1)
#define M8265_E86_GEN_REG_L(d)  (((d) >> 3) & 1)
#define M8265_E86_SP_WRITE_L(d)  (((d) >> 2) & 1)
#define M8265_E86_INT_DATI_L(d)  (((d) >> 1) & 1)

static inline uint8_t m8265_e54_io_page_enabled(uint32_t pba)
{
    return (pba & 0x3f400u) == 0x3f400u;
}

static inline uint8_t m8265_e81_general_enabled(uint32_t pba)
{
    return (pba & 0x0b70u) == 0x0b70u;
}

static inline uint8_t m8265_e86_enabled(uint32_t pba)
{
    return m8265_e54_io_page_enabled(pba) && !(pba & 0x10u);
}

static inline uint8_t m8265_e118_par_pdr_enabled(uint32_t pba)
{
    return (pba & 0x0090u) == 0x0080u;
}

#endif
