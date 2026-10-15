/* ROM address wiring and output fields.       */
/* Slop-coded with love by an LLM.             */
/* Sorry, no way I was doing this manually...  */

#ifndef KD11EA_ROM_WIRING_H
#define KD11EA_ROM_WIRING_H

#include <stdint.h>
#include "roms/rom_images.h"

static inline uint8_t e29_addr(uint8_t priority, uint8_t halt,
                               uint8_t br7, uint8_t br6,
                               uint8_t br5, uint8_t br4)
{
    return ((priority >> 2) & 1) |
           ((br5 & 1) << 1) |
           ((br7 & 1) << 2) |
           ((br4 & 1) << 3) |
           (((priority >> 1) & 1) << 4) |
           ((priority & 1) << 5) |
           ((halt & 1) << 6) |
           ((br6 & 1) << 7);
}

#define E29_BG6(d) ((d) & 1)
#define E29_BG5(d) (((d) >> 1) & 1)
#define E29_BG4(d) (((d) >> 2) & 1)
#define E29_BG7(d) (((d) >> 3) & 1)

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

#define BMODE_HOLD          0
#define BMODE_SHIFT_R       1
#define BMODE_SHIFT_L       2
#define BMODE_LOAD          3

static inline uint8_t e102_addr(uint8_t but_field) {
    return ((but_field >> 1) & 1) << 4 |
           ((but_field >> 3) & 1) << 3 |
           ((but_field >> 2) & 1) << 2 |
           ((but_field >> 0) & 1) << 1 |
           1;
}

#define E102_BUT_ALU_OUT_MPC7(d)  (((d) >> 0) & 1)
#define E102_BUT_CC_N_MPC6(d)     (((d) >> 1) & 1)
#define E102_BUT_SP15_MPC3(d)     (((d) >> 2) & 1)
#define E102_BUT_BXREG01_MPC5(d)  (((d) >> 3) & 1)
#define E102_BUT_BXREG00_MPC4(d)  (((d) >> 4) & 1)
#define E102_BUT_COUNT05_MPC2(d)  (((d) >> 5) & 1)
#define E102_BUT_CC_Z_MPC1(d)     (((d) >> 6) & 1)
#define E102_BUT_IR09_MPC0(d)     (((d) >> 7) & 1)

static inline uint8_t e59_e60_addr(uint16_t ir) {
    return (((ir >> 15) & 1)     ) |
           (((ir >> 11) & 1) << 1) |
           (((ir >> 10) & 1) << 2) |
           (((ir >>  9) & 1) << 3) |
           (((ir >>  8) & 1) << 4) |
           (((ir >>  7) & 1) << 5) |
           (((ir >>  6) & 1) << 6) |
           (1              << 7);
}

static inline uint8_t e63_addr(uint16_t ir) {
    return ((ir     ) & 1)        |
           (((ir >> 4) & 1) << 1) |
           (((ir >> 3) & 1) << 2) |
           (((ir >> 2) & 1) << 3) |
           (((ir >> 1) & 1) << 4) |
           (((ir >> 7) & 1) << 5) |
           (((ir >> 5) & 1) << 6) |
           (((ir >> 6) & 1) << 7);
}

static inline uint16_t e68_addr(uint16_t ir) {
    return (((ir >> 15) & 1)     ) |
           (((ir >> 14) & 1) << 1) |
           (((ir >> 13) & 1) << 2) |
           (((ir >> 12) & 1) << 3) |
           (((ir >> 10) & 1) << 4) |
           (((ir >>  9) & 1) << 5) |
           (((ir >>  8) & 1) << 6) |
           (((ir >>  7) & 1) << 7) |
           (((ir >>  6) & 1) << 8);
}

static inline uint8_t e69_addr(uint16_t ir, int but_dest_l, int ir_decode_h) {
    uint8_t dm0 = ((ir >> 3) & 7) == 0 ? 1 : 0;
    uint8_t sm0 = ((ir >> 9) & 7) == 0 ? 1 : 0;
    return (((ir >> 15) & 1)     ) |
           (((ir >> 14) & 1) << 1) |
           (((ir >> 13) & 1) << 2) |
           (((ir >> 12) & 1) << 3) |
           ((but_dest_l & 1) << 4) |
           ((ir_decode_h & 1) << 5) |
           (dm0 << 6)               |
           (sm0 << 7);
}

static inline uint16_t e70_addr(uint16_t ir, int but_dest_l, int ir_decode_h) {
    return e69_addr(ir, but_dest_l, ir_decode_h) | (1 << 8);
}

static inline uint8_t e71_addr(uint16_t ir, uint16_t psw) {
    return (psw & 0xF)                 |
           (((ir >> 8) & 1)  << 4)    |
           (((ir >> 9) & 1)  << 5)    |
           (((ir >> 10) & 1) << 6)    |
           (((ir >> 15) & 1) << 7);
}

static inline uint8_t e74_addr(uint16_t ir, int ir_decode_h) {
    uint8_t nand_121314 = !(((ir >> 14) & 1) && ((ir >> 13) & 1) && ((ir >> 12) & 1)) ? 1 : 0;
    return (((ir >> 9)  & 1)     ) |
           (((ir >> 10) & 1) << 1) |
           (((ir >> 11) & 1) << 2) |
           (nand_121314 << 3)      |
           ((ir_decode_h & 1) << 4);
}

static inline uint8_t e107_addr(uint8_t r_msb, uint8_t s_msb, uint8_t d_msb,
                                uint8_t rot_cbit, uint8_t psw_cbit, uint8_t cc_code) {
    return (r_msb & 1)            |
           ((s_msb & 1) << 1)     |
           ((d_msb & 1) << 2)     |
           ((rot_cbit & 1) << 3)  |
           ((psw_cbit & 1) << 4)  |
           (((cc_code >> 2) & 1) << 5) |
           (((cc_code >> 1) & 1) << 6) |
           ((cc_code & 1) << 7);
}

#define E68_BYTE_L(d)    ((d) & 1)
#define E68_CC_CODE(d)   (((d) >> 1) & 7)

#define DOP_ALU_PACK(s, m, cin)  ((s) | ((m) << 4) | ((cin) << 5))
#define DOP_ALU_S(d)     ((d) & 0xF)
#define DOP_ALU_MODE(d)  (((d) >> 4) & 1)
#define DOP_ALU_CIN(d)   (((d) >> 5) & 1)

static const uint8_t dop_alu[16] = {
     0,
     DOP_ALU_PACK(0xF, 1, 0),
     DOP_ALU_PACK(0x6, 0, 1),
     DOP_ALU_PACK(0xB, 1, 0),
     DOP_ALU_PACK(0x2, 1, 0),
     DOP_ALU_PACK(0xE, 1, 0),
     DOP_ALU_PACK(0x9, 0, 0),
     DOP_ALU_PACK(0x6, 1, 0),
     0,
     DOP_ALU_PACK(0xF, 1, 0),
     DOP_ALU_PACK(0x6, 0, 1),
     DOP_ALU_PACK(0xB, 1, 0),
     DOP_ALU_PACK(0x2, 1, 0),
     DOP_ALU_PACK(0xE, 1, 0),
     DOP_ALU_PACK(0x6, 0, 1),
     0,
};

static inline uint8_t e61_addr(uint16_t ir, uint8_t cbit, uint8_t nbit) {
    return (((ir >> 10) & 1)     ) |
           (((ir >>  9) & 1) << 1) |
           (((ir >>  8) & 1) << 2) |
           (((ir >>  7) & 1) << 3) |
           (((ir >>  6) & 1) << 4) |
           ((cbit & 1) << 5)       |
           ((nbit & 1) << 6)       |
           (((ir >> 15) & 1) << 7);
}

static inline uint8_t e62_addr(uint16_t ir, uint8_t breg0, uint8_t cbit, uint8_t cc_n) {
    return ((cc_n & 1)           ) |
           ((cbit & 1)      << 1) |
           ((breg0 & 1)     << 2) |
           (((ir >> 6) & 1) << 3) |
           (((ir >> 7) & 1) << 4) |
           (((ir >> 8) & 1) << 5) |
           (((ir >> 9) & 1) << 6) |
           (((ir >> 10)& 1) << 7);
}

#define E62_B_MODE_01_L(d)   ((d) & 1)
#define E62_B_MODE_00_L(d)   (((d) >> 1) & 1)
#define E62_SERIAL_SHIFT(d)  (((d) >> 2) & 1)
#define E62_ROT_CBIT(d)      (((d) >> 3) & 1)

static inline uint8_t E62_B_MODE(uint8_t d) {
    return ((!(d & 1)) << 1) | ((!((d >> 1) & 1)));
}

static inline uint8_t e80_addr(uint8_t ss01, uint8_t ss00, uint8_t bdt,
                                uint8_t aux, uint8_t byte_h, uint8_t vba00_bdt,
                                uint8_t mov_l) {
    return ((ss01 & 1) << 7) | ((ss00 & 1) << 6) | ((bdt & 1) << 5) |
           ((aux & 1) << 4) | ((byte_h & 1) << 3) | ((vba00_bdt & 1) << 2) |
           (0 << 1) | (mov_l & 1);
}

static inline uint8_t E80_SWAP(uint8_t d)          { return d & 1; }
static inline uint8_t E80_SEX(uint8_t d)           { return (d >> 1) & 1; }
static inline uint8_t E80_DISABLE_UPPER(uint8_t d) { return (d >> 3) & 1; }

static inline uint8_t e53_addr(uint16_t ir, uint8_t user_mode) {

    return ((ir >> 0) & 1)              |
           (((ir >> 4) & 1) << 1)       |
           (((ir >> 3) & 1) << 2)       |
           (((ir >> 2) & 1) << 3)       |
           (((ir >> 1) & 1) << 4)       |
           (((ir >> 5) & 1) << 5)       |
           (((ir >> 7) & 1) << 6)       |
           ((user_mode & 1) << 7);
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
#define E53_IR_CODE_02_L(d) (((d) >> 1) & 1)
#define E53_IR_CODE_01_L(d) (((d) >> 2) & 1)
#define E53_IR_CODE_00_L(d) (((d) >> 3) & 1)

static inline uint8_t e53_ir_code(uint8_t d) {
    return ((!E53_IR_CODE_02_L(d)) << 2) |
           ((!E53_IR_CODE_01_L(d)) << 1) |
           (!E53_IR_CODE_00_L(d));
}

static inline uint8_t e59_ir_code(uint8_t d) {
    return (!(d & 0x01) << 2) |
           (!(d & 0x02) << 1) |
           (!(d & 0x04));
}

#define E74_IR_CODE_00_L(d)  ((d) & 1)

static inline uint8_t e54_addr(uint16_t ir, uint8_t user_mode) {

    uint8_t dm0 = (((ir >> 3) & 7) == 0) ? 1 : 0;
    uint8_t psw15_l = user_mode ? 0 : 1;
    uint8_t nor_08_15 = (((ir >> 8) & 1) == 0 && ((ir >> 15) & 1) == 0) ? 1 : 0;

    return (((ir >> 6) & 1))             |
           (((ir >> 0) & 1) << 1)        |
           (((ir >> 1) & 1) << 2)        |
           (((ir >> 2) & 1) << 3)        |
           ((dm0 & 1) << 4)              |
           (((ir >> 7) & 1) << 5)        |
           ((psw15_l & 1) << 6)          |
           ((nor_08_15 & 1) << 7);
}

#define E54_START_RESET(d)      ((d) & 1)
#define E54_ENAB_TBIT(d)        (((d) >> 1) & 1)
#define E54_DISABLE_LOAD_PSW(d) (((d) >> 2) & 1)

#define E52_CODE_NONE 0x0
#define E52_CODE_KTE  0xB

static inline uint16_t e52_addr(uint8_t tbit_and_enab, uint8_t stov,
                                uint8_t be, uint8_t pe, uint8_t kte,
                                uint8_t pfail, uint8_t ir_code_2_0,
                                uint8_t svc_br_pfail)
{

    uint8_t irc02l = (~(ir_code_2_0 >> 2)) & 1;
    uint8_t irc01l = (~(ir_code_2_0 >> 1)) & 1;
    uint8_t irc00l = (~(ir_code_2_0 >> 0)) & 1;

    uint8_t sbpfl  = (~svc_br_pfail) & 1;

    return ((tbit_and_enab & 1))      |
           ((stov & 1)  << 1)         |
           ((be & 1)    << 2)         |
           ((pe & 1)    << 3)         |
           ((kte & 1)   << 4)         |
           ((pfail & 1) << 5)         |
           ((uint16_t)irc02l << 6)    |
           ((uint16_t)irc01l << 7)    |
           ((uint16_t)irc00l << 8)    |
           ((uint16_t)sbpfl  << 9);
}

#define E51_PFAIL_SERV(d)   ((d) & 1)
#define E51_STOV_SERV(d)    (((d) >> 2) & 1)
#define E51_C4(d)           (((d) >> 4) & 1)
#define E51_C3(d)           (((d) >> 5) & 1)
#define E51_C2(d)           (((d) >> 6) & 1)
#define E51_ENAB_GRANTS(d)  (((d) >> 7) & 1)
#define E51_VECTOR(d)       ((E51_C4(d) << 4) | (E51_C3(d) << 3) | (E51_C2(d) << 2))

static inline uint8_t e52_to_e51_addr(uint8_t e52_out) {

    uint8_t o0 = (e52_out >> 0) & 1;
    uint8_t o1 = (e52_out >> 1) & 1;
    uint8_t o2 = (e52_out >> 2) & 1;
    uint8_t o3 = (e52_out >> 3) & 1;
    return (o0 << 4) | (o3 << 3) | (o2 << 2) | (o1 << 1);
}

static inline uint8_t m8265_e74_e75_addr(uint32_t pba, uint8_t c1,
                                         uint8_t c0, uint8_t e74)
{
    return ((e74 & 1)               ) |
           ((c0 & 1)            << 1) |
           (((pba >> 0) & 1)    << 2) |
           (((pba >> 7) & 1)    << 3) |
           (((pba >> 3) & 1)    << 4) |
           (((pba >> 1) & 1)    << 5) |
           (((pba >> 2) & 1)    << 6) |
           ((c1 & 1)            << 7);
}

#define M8265_E74_KT_MUX_S0_L(d)    ((d) & 1)
#define M8265_E74_INT_SSYN_L(d)      (((d) >> 2) & 1)
#define M8265_E75_LOAD_SR0_LOW_L(d)  ((d) & 1)
#define M8265_E75_LOAD_SR0_HIGH_L(d) (((d) >> 3) & 1)

static inline uint8_t m8265_e76_e77_addr(uint32_t pba, uint8_t c1,
                                         uint8_t c0)
{
    return (((pba >> 5) & 1)       ) |
           ((c0 & 1)            << 1) |
           (((pba >> 0) & 1)    << 2) |
           (((pba >> 11) & 1)   << 3) |
           (((pba >> 9) & 1)    << 4) |
           (((pba >> 6) & 1)    << 5) |
           (((pba >> 8) & 1)    << 6) |
           ((c1 & 1)            << 7);
}

#define M8265_E76_KT_MUX_S0_L(d) ((d) & 1)
#define M8265_E76_PAR_PDR_L(d)    (((d) >> 2) & 1)
#define M8265_E77_LOAD_PDR_LOW_L(d)  ((d) & 1)
#define M8265_E77_LOAD_PAR_HIGH_L(d) (((d) >> 1) & 1)
#define M8265_E77_LOAD_PAR_LOW_L(d)  (((d) >> 2) & 1)
#define M8265_E77_LOAD_PDR_HIGH_L(d) (((d) >> 3) & 1)

static inline uint8_t m8265_e83_addr(uint8_t c1, uint8_t c0,
                                     uint8_t disable_msyn_l,
                                     uint8_t illegal_mode, uint8_t acf,
                                     uint8_t compare, uint8_t ed)
{
    return ((ed & 1)                    ) |
           ((c0 & 1)                << 1) |
           ((disable_msyn_l & 1)    << 2) |
           ((illegal_mode & 1)      << 3) |
           (((acf >> 1) & 1)        << 4) |
           ((compare & 1)           << 5) |
           ((acf & 1)               << 6) |
           ((c1 & 1)                << 7);
}

#define M8265_E83_NR(d)        ((d) & 1)
#define M8265_E83_RO(d)        (((d) >> 2) & 1)
#define M8265_E83_PL(d)        (((d) >> 3) & 1)

static inline uint8_t m8265_e86_addr(uint32_t pba, uint8_t c1,
                                     uint8_t sack_ret)
{
    return (c1 & 1) |
           (((pba >> 8) & 1) << 1) |
           (((pba >> 7) & 1) << 2) |
           (((pba >> 6) & 1) << 3) |
           (((pba >> 5) & 1) << 4) |
           ((sack_ret & 1) << 5) |
           (((pba >> 11) & 1) << 6) |
           (((pba >> 9) & 1) << 7);
}

#define M8265_E86_INT_SSYN_L(d) ((d) & 1)
#define M8265_E86_GEN_REG_L(d)  (((d) >> 1) & 1)
#define M8265_E86_SP_WRITE_L(d)  (((d) >> 2) & 1)
#define M8265_E86_INT_DATI_L(d)  (((d) >> 3) & 1)

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
