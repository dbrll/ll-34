/*
 * kd11ea.h: KD11-EA (PDP-11/34A) microcode-level emulator
 */

#ifndef KD11EA_H
#define KD11EA_H

#include <stdint.h>
#include "../unibus/unibus.h"
#include "combo_roms.h"
#include "clockgen.h"
#include "int.h"
#include "debug.h"
#include "mmu.h"

/* Micro-word field extraction (48-bit, stored in uint64_t).
 * Multi-bit fields have MSB at the LOWEST bit position (reversed).
 * ufr() handles the bit reversal. */

#define UB(w, bit)     (((w) >> (bit)) & 1)

static inline uint8_t ufr(uint64_t w, int hi, int lo) {
    int n = hi - lo + 1;
    uint8_t val = 0;
    for (int i = 0; i < n; i++)
        val |= (uint8_t)(((w >> (lo + i)) & 1) << (n - 1 - i));
    return val;
}

/* Next MPC (bits 8:0, reversed) */
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
/* Bits 16/17 swapped vs DEC docs -- confirmed correct with Verilog model */
#define UW_LONG_CYCLE_L(w)  UB(w, 16)
#define UW_LOAD_BA(w)       UB(w, 17)
#define UW_AUX_CONTROL(w)   UB(w, 18)

/* ALU function code: 5 bits [23:19] -> E82 (raw, no reversal) */
#define UW_ALU_FUNC(w)      ((uint8_t)(((w) >> 19) & 0x1F))

/* B/BX/OVX/DBE: 4 bits [27:24] -> E87 (raw) */
#define UW_BBXOVXDBE(w)     ((uint8_t)(((w) >> 24) & 0xF))

#define UW_SSMUX(w)         ufr(w, 29, 28)
#define SSMUX_STRAIGHT      0
#define SSMUX_SIGN_EXT      1
#define SSMUX_SWAP_BYTES    2
#define SSMUX_EXT_DATA      3

/* AMUX (active low) */
#define UW_AMUX_L(w)        ufr(w, 31, 30)
#define AMUX_UNIBUS         0
#define AMUX_CONSTANTS      1
#define AMUX_ALU            2
#define AMUX_PSW            3

/* BUT bits [35:32] -> E102 (raw) */
#define UW_BUT_BITS(w)      ((uint8_t)(((w) >> 32) & 0xF))

/* Scratchpad addressing */
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


/* 74S181 ALU (4 slices, 16-bit) */
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
        uint32_t aa = a, bb = b;
        switch (s) {
        case 0x0: result = aa + (cin ? 1 : 0); break;                      /* A */
        case 0x1: result = (aa | bb) + (cin ? 1 : 0); break;               /* A|B */
        case 0x2: result = (aa | (~bb & 0xFFFF)) + (cin ? 1 : 0); break;   /* A|~B */
        case 0x3: result = 0xFFFF + (cin ? 1 : 0); break;                  /* -1 */
        case 0x4: result = aa + (aa & (~bb & 0xFFFF)) + (cin ? 1 : 0); break;
        case 0x5: result = (aa | bb) + (aa & (~bb & 0xFFFF)) + (cin ? 1 : 0); break;
        case 0x6: result = aa + (~bb & 0xFFFF) + (cin ? 1 : 0); break;       /* A+~B+CIN (= A-B-~CIN) */
        case 0x7: result = (aa & (~bb & 0xFFFF)) + 0xFFFF + (cin ? 1 : 0); break;
        case 0x8: result = aa + (aa & bb) + (cin ? 1 : 0); break;
        case 0x9: result = aa + bb + (cin ? 1 : 0); break;                 /* A+B */
        case 0xA: result = (aa | (~bb & 0xFFFF)) + (aa & bb) + (cin ? 1 : 0); break;
        case 0xB: result = (aa & bb) + 0xFFFF + (cin ? 1 : 0); break;
        case 0xC: result = aa + aa + (cin ? 1 : 0); break;                 /* 2A */
        case 0xD: result = (aa | bb) + aa + (cin ? 1 : 0); break;
        case 0xE: result = (aa | (~bb & 0xFFFF)) + aa + (cin ? 1 : 0); break;
        case 0xF: result = aa + 0xFFFF + (cin ? 1 : 0); break;              /* A-1+CIN */
        default:  result = 0; break;
        }
        if (cout) *cout = (result >> 16) & 1;
    }
    return (uint16_t)(result & 0xFFFF);
}


typedef struct KD11EA {
    uint16_t mpc;           /* Micro Program Counter (9 bits) */

    /* Scratchpad (85S68): slot 15-8 = R0-R7, slot 7-0 = temps.
     * R6/SP: slot 9 (kernel), slot 1 (user). */
    uint16_t sp[16];
#define REG(cpu, n)  ((cpu)->sp[15 - (n)])  /* user reg Rn */
#define PC(cpu)      ((cpu)->sp[8])         /* R7/PC = slot 8 */

    uint16_t ir;
    uint16_t b_reg;         /* 74194 shift register */
    uint16_t bx_reg;        /* 74194 shift register */
    uint32_t ba;            /* Bus Address (18-bit PBA) */
    uint16_t alu_out;
    uint8_t  alu_cout;
    uint16_t psw;
    uint16_t unibus_data;
    uint8_t  bus_pending;
    uint8_t  cc_n, cc_z, cc_v, cc_c;
    uint8_t  count;         /* 6-bit EIS counter */
    uint8_t  ovx;           /* OVX flip-flop (E100), cleared at LOAD IR */

    /* Trap decode latches (E53/E54, K2-6), latched at LOAD IR */
    uint8_t  ir_code;       /* 3-bit IR CODE from E53 */
    uint8_t  halt_rqst;
    uint8_t  enab_tbit;     /* from E54 */
    uint8_t  tbit_trap;     /* TBIT AND ENAB_TBIT -> E52 A0 */
    uint8_t  service_vector;
    uint8_t  service_trap;
    uint8_t  bus_error;     /* NXM latch */
    int      nxm_trace_count;

    MMU      mmu;
    uint8_t  mmu_abort;
    uint8_t  mmu_kte;

    uint8_t  halted;
    uint8_t  waiting;
    uint8_t  ir_valid;
    uint64_t ns_elapsed;
    Bus *bus;
    IntController intc;
    DbgRing dbg;
    struct Probe *probe;
} KD11EA;



void kd11ea_reset(KD11EA *cpu);
uint32_t kd11ea_ustep(KD11EA *cpu);  /* one micro-cycle, returns ns (0=halted) */
uint32_t kd11ea_step(KD11EA *cpu);   /* one instruction, returns ns (0=halted) */

#endif /* KD11EA_H */
