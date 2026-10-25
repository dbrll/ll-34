/* KD11-EA, the PDP-11/34A CPU, modelled micro-cycle by micro-cycle. */

#ifndef KD11EA_H
#define KD11EA_H

#include <stdint.h>
#include "../unibus/unibus.h"
#include "rom_wiring.h"
#include "clockgen.h"
#include "int.h"
#include "debug.h"
#include "mmu.h"

/* The micro-word has 48 bits. In most multi-bit fields the most
 * significant bit is at the lowest position, and ufr() reverses them. */

#define UB(w, bit)     (((w) >> (bit)) & 1)

static inline uint8_t ufr(uint64_t w, int hi, int lo) {
    int n = hi - lo + 1;
    uint8_t val = 0;
    for (int i = 0; i < n; i++)
        val |= (uint8_t)(((w >> (lo + i)) & 1) << (n - 1 - i));
    return val;
}

/* The next MPC is in bits 8-0, reversed as well. */
static inline uint16_t uw_next_mpc(uint64_t w) {
    uint16_t val = 0;
    for (int i = 0; i < 9; i++)
        val |= ((w >> i) & 1) << (8 - i);
    return val;
}

#define UW_MISC(w)          ufr(w, 11, 9)
#define MISC_NOP            0
#define MISC_LOAD_IR        1
#define MISC_LOAD_PSW       2
#define MISC_LOAD_CC        3
#define MISC_BUT_DEST       4
#define MISC_ENAB_STOV      5
#define MISC_LOAD_COUNT     6
#define MISC_CLK_COUNT      7

#define UW_BUF_DAT_TRAN(w) UB(w, 12)
#define UW_BUS_CTL(w)       ufr(w, 14, 13)
#define BUS_DATI            0
#define BUS_DATIP           1
#define BUS_DATO            2
#define BUS_DATOB           3

#define UW_ENAB_MAINT(w)   UB(w, 15)
/* Bits 16 and 17 are the other way round in the DEC documents. The
 * order here is the one that works, checked against a Verilog model. */
#define UW_LONG_CYCLE_L(w)  UB(w, 16)
#define UW_LOAD_BA(w)       UB(w, 17)
#define UW_AUX_CONTROL(w)   UB(w, 18)

/* The ALU function goes to E82, and B, BX, OVX and DBE go to E87. Neither
 * field is reversed. */
#define UW_ALU_FUNC(w)      ((uint8_t)(((w) >> 19) & 0x1F))
#define UW_BBXOVXDBE(w)     ((uint8_t)(((w) >> 24) & 0xF))

#define UW_SSMUX(w)         ufr(w, 29, 28)
#define SSMUX_STRAIGHT      0
#define SSMUX_SIGN_EXT      1
#define SSMUX_SWAP_BYTES    2
#define SSMUX_EXT_DATA      3

/* AMUX is active low. */
#define UW_AMUX_L(w)        ufr(w, 31, 30)
#define AMUX_UNIBUS         0
#define AMUX_CONSTANTS      1
#define AMUX_ALU            2
#define AMUX_PSW            3

/* The BUT bits go to E102, not reversed. */
#define UW_BUT_BITS(w)      ((uint8_t)(((w) >> 32) & 0xF))

#define UW_SPA_SRC_SEL(w)   ufr(w, 37, 36)
#define UW_SPA_DST_SEL(w)   ufr(w, 39, 38)
#define SPA_ROM             0
#define SPA_RS              1
#define SPA_RD              2
#define SPA_RBA             3

#define UW_FORCE_RSV1_L(w)  UB(w, 40)
#define UW_PREV_MODE_L(w)   UB(w, 41)
#define UW_BUT_SERVICE(w)   UB(w, 42)
#define UW_FORCE_KERNEL(w)  UB(w, 43)

#define UW_ROM_SPA(w)       ufr(w, 47, 44)


/* 74S181 arithmetic on the bits in mask. The 16-bit result uses 0xFFFF,
 * and the carries between slices use 0xF, 0xFF and 0xFFF. */
static inline uint32_t alu_74s181_arith(uint32_t aa, uint32_t bb, uint8_t s,
                                        uint8_t cin, uint32_t mask) {
    uint32_t result;
    aa &= mask;
    bb &= mask;
    switch (s) {
    case 0x0: result = aa + (cin ? 1 : 0); break;                      /* A */
    case 0x1: result = (aa | bb) + (cin ? 1 : 0); break;               /* A|B */
    case 0x2: result = (aa | (~bb & mask)) + (cin ? 1 : 0); break;   /* A|~B */
    case 0x3: result = mask + (cin ? 1 : 0); break;                  /* -1 */
    case 0x4: result = aa + (aa & (~bb & mask)) + (cin ? 1 : 0); break;
    case 0x5: result = (aa | bb) + (aa & (~bb & mask)) + (cin ? 1 : 0); break;
    case 0x6: result = aa + (~bb & mask) + (cin ? 1 : 0); break;       /* A+~B+CIN (= A-B-~CIN) */
    case 0x7: result = (aa & (~bb & mask)) + mask + (cin ? 1 : 0); break;
    case 0x8: result = aa + (aa & bb) + (cin ? 1 : 0); break;
    case 0x9: result = aa + bb + (cin ? 1 : 0); break;                 /* A+B */
    case 0xA: result = (aa | (~bb & mask)) + (aa & bb) + (cin ? 1 : 0); break;
    case 0xB: result = (aa & bb) + mask + (cin ? 1 : 0); break;
    case 0xC: result = aa + aa + (cin ? 1 : 0); break;                 /* 2A */
    case 0xD: result = (aa | bb) + aa + (cin ? 1 : 0); break;
    case 0xE: result = (aa | (~bb & mask)) + aa + (cin ? 1 : 0); break;
    case 0xF: result = aa + mask + (cin ? 1 : 0); break;              /* A-1+CIN */
    default:  result = 0; break;
    }
    return result;
}

/* The four 74S181 slices of the ALU. */
static inline uint16_t alu_74s181(uint16_t a, uint16_t b,
                                   uint8_t s, uint8_t mode, uint8_t cin,
                                   uint8_t *cout) {
    uint32_t result;
    if (mode) {
        switch (s) {
        case 0x0: result = ~a; break;
        case 0x1: result = ~(a | b); break;
        case 0x2: result = (~a) & b; break;
        case 0x3: result = 0; break;
        case 0x4: result = ~(a & b); break;
        case 0x5: result = ~b; break;
        case 0x6: result = a ^ b; break;
        case 0x7: result = a & (~b); break;
        case 0x8: result = (~a) | b; break;
        case 0x9: result = ~(a ^ b); break;
        case 0xA: result = b; break;
        case 0xB: result = a & b; break;
        case 0xC: result = 0xFFFF; break;
        case 0xD: result = a | (~b); break;
        case 0xE: result = a | b; break;
        case 0xF: result = a; break;
        default:  result = 0; break;
        }
        if (cout) *cout = 0;
    } else {
        result = alu_74s181_arith(a, b, s, cin, 0xFFFF);
        if (cout) *cout = (result >> 16) & 1;
    }
    return (uint16_t)(result & 0xFFFF);
}

/* Carries out of bits 3, 7 and 11, which the 74S182 E27 gives as ALU COUT
 * 3, 7 and 11. They are returned in bits 0-2. Logic mode has no carry. */
static inline uint8_t alu_74s181_slice_carries(uint16_t a, uint16_t b,
                                               uint8_t s, uint8_t mode, uint8_t cin) {
    if (mode)
        return 0;
    return (uint8_t)(((alu_74s181_arith(a, b, s, cin, 0xF) >> 4) & 1) |
                     (((alu_74s181_arith(a, b, s, cin, 0xFF) >> 8) & 1) << 1) |
                     (((alu_74s181_arith(a, b, s, cin, 0xFFF) >> 12) & 1) << 2));
}


typedef struct KD11EA {
    uint16_t mpc;

    /* The 85S68 scratchpad. Slots 15-8 hold R0-R7 and slots 7-0 the
     * temporaries. The kernel SP is slot 9 and the user SP slot 1. */
    uint16_t sp[16];
#define REG(cpu, n)  ((cpu)->sp[15 - (n)])
#define PC(cpu)      ((cpu)->sp[8])

    uint16_t ir;            /* the instruction of the last LOAD IR */
    uint8_t  serv_ir;       /* SERV IR (K2-3) has cleared the IR pins */
    uint16_t b_reg;         /* 74194 shift registers */
    uint16_t bx_reg;
    uint32_t ba;            /* 18-bit physical address */
    uint16_t vba;           /* virtual address latch, K1-6 E47-E49 */
    uint16_t alu_out;
    uint8_t  alu_cout;
    uint16_t psw;
    uint16_t unibus_data;
    uint8_t  bus_pending;
    uint8_t  cc_n, cc_z, cc_v, cc_c;
    uint8_t  count;         /* 6-bit EIS counter */
    uint8_t  ovx;           /* E100, cleared by LOAD IR */
    uint8_t  rot_cbit;      /* E55 on K2-5, set from E62 */
    uint8_t  svc_e52, svc_e51;  /* E52 and E51 at the last BUT SERVICE */
    uint8_t  reset_inst;    /* E13 on K2-1, which drives E16 on K2-2 */

    /* E53 and E54 (K2-6) are latched at LOAD IR. */
    uint8_t  ir_code;
    uint8_t  halt_rqst;
    uint8_t  enab_tbit;
    uint8_t  tbit_trap;     /* T bit and ENAB TBIT, to E52 A0 */
    uint8_t  stov;          /* E29 on K2-3 */
    uint8_t  stov_en;       /* E45, which holds E29 clear while low */
    uint16_t service_vector;   /* from the constants for a trap, from the
                                  D lines for an interrupt */
    uint8_t  service_trap;
    uint8_t  service_intr;
    uint8_t  bus_error;     /* E33 on K2-1, set by a timeout or DISABLE MSYN */
    int      nxm_trace_count;

    MMU      mmu;
    uint8_t  mmu_abort;
    uint8_t  mmu_kte;

    uint8_t  halted;
    uint8_t  ir_valid;
    uint64_t ns_elapsed;
    Bus *bus;
    const ClockTiming *timing;  /* NULL means the DEC nominal values */
    IntController intc;
    DbgRing dbg;
    struct Probe *probe;
} KD11EA;



/* On SERV IR, E22 (K2-5) clears IR14-00 and E33 sets IR15. The pins are
 * computed here, so that ir keeps the last instruction for the debugger. */
static inline uint16_t kd11ea_ir_pins(const KD11EA *cpu) {
    return cpu->serv_ir ? 0100000 : cpu->ir;
}

void kd11ea_reset(KD11EA *cpu);
/* One micro-cycle, or one instruction. Both return the time taken in ns,
 * or 0 if the CPU is halted. */
uint32_t kd11ea_ustep(KD11EA *cpu);
uint32_t kd11ea_step(KD11EA *cpu);

#endif
