#define _DEFAULT_SOURCE
/*
 * KD11-EA, the PDP-11/34A CPU, at the circuit level.
 *
 * The original microcode (512 words of 48 bits) runs one word per cycle.
 * The other PROMs are images in physical pin order, and this code only
 * models the gates and the wiring between them, after the MP00190
 * schematics. The data path is described in chapter 4 of the KD11-E
 * maintenance manual, and the microcode in the KD11-E flow diagrams.
 *
 * Damien Boureille, 2026
 * MIT Licence
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include "kd11ea.h"
#include "ucode_rom.h"
#include "bus_wiring.h"
#include "../trace.h"
#include "../probe/probe.h"

/* AMUX: post-ALU mux, feeds SSMUX (E22/E27, 74S157, K1-6/K1-7) */
static uint16_t amux_select(KD11EA *cpu, uint8_t amux_l, uint16_t alu_result) {
    uint8_t sel = ~amux_l & 3;
    switch (sel) {
    case 0: return cpu->unibus_data;  /* UNIBUS data (E16/E21) */
    case 1: return 0;                 /* constants, the vector is set later */
    case 2: return alu_result;        /* ALU output */
    case 3: return cpu->psw;          /* PSW register */
    }
    return 0;
}

/* SSMUX: swap/sign-extend (E18/E23, 74S157, K1-5/K1-6).
 * Controlled by E80 ROM outputs (not raw SS[1:0] from microcode). */
static uint16_t ssmux_select(uint16_t amux_data, uint8_t swap, uint8_t sex) {
    if (swap)
        return ((amux_data >> 8) & 0xFF) | ((amux_data & 0xFF) << 8);
    if (sex)
        return (amux_data & 0x80) ? (amux_data | 0xFF00) : (amux_data & 0x00FF);  /* E19 (74S86) */
    return amux_data;
}

/* BMUX: ALU B-input select (E38/E39, 74S153, K1-3/K1-4) */
static uint16_t bmux_select(KD11EA *cpu, uint8_t bleg) {
    switch (bleg) {
    case BLEG_BREG:    return cpu->b_reg;
    case BLEG_BXREG:   return cpu->bx_reg;
    case BLEG_PLUS16:  return 14;            /* 16 less 2, for the counter timing */
    case BLEG_PLUS1:   return 1;
    }
    return 0;
}

/* Rn -> sp[15-n] (K2-4 SPA MUX, E57/E97).
 * E57/E97: user-mode R6 maps to slot 1 (USP) instead of slot 9 (KSP). */
static uint8_t reg_to_spa(uint8_t reg3, int user_mode) {
    if (reg3 == 6 && user_mode)
        return 1;
    return 15 - reg3;
}

static uint8_t spa_resolve(KD11EA *cpu, uint8_t sel, uint64_t uw) {
    /* MODE = PSW[14] (current), or PSW[12] when PREVIOUS_MODE_L active */
    int user_mode;
    if (!UW_PREV_MODE_L(uw))
        user_mode = (cpu->psw >> 12) & 1;  /* previous mode */
    else
        user_mode = (cpu->psw >> 14) & 1;  /* current mode */

    switch (sel) {
    case SPA_ROM: {
        uint8_t slot = UW_ROM_SPA(uw);
        /* E57/E97: slot 9 = R6/SP, remap to slot 1 in user mode.
         * SPA_ROM bypasses the RS/RD remapping in reg_to_spa(),
         * but the same E57/E97 mode-dependent logic applies.
         * This isn't obvious from the schematics and prevented V6
         * from booting. Never again such tedious debugging...
         */
        if (slot == 9 && user_mode) return 1;
        return slot;
    }
    case SPA_RS: {
        uint8_t reg = (cpu->ir >> 6) & 7;
        if (!UW_FORCE_RSV1_L(uw))  /* bit 40 (E103), active low, sets RS bit 0 */
            reg |= 1;
        return reg_to_spa(reg, user_mode);
    }
    case SPA_RD:  return reg_to_spa(cpu->ir & 7, user_mode);  /* IR[2:0] */
    case SPA_RBA: return (cpu->ba >> 1) & 0xF;            /* BA[4:1] */
    }
    return 0;
}

/* B/BX register update (74194 shift registers).
 * Bits are wired reversed on KD11-EA: physical SHIFT_R = logical left,
 * physical SHIFT_L = logical right. We store MSB=bit15, so we invert.
 * On a logical right shift, bit 7 takes SHIFT IN 07 (K1-10). */
static void breg_update(uint16_t *reg, uint8_t mode, uint16_t load_data,
                         uint8_t shift_in, uint8_t shift_in_07) {
    switch (mode) {
    case BMODE_HOLD:    break;
    case BMODE_SHIFT_L: /* x2, shift_in into bit 0 */
        *reg = (*reg << 1) | (shift_in & 1); break;
    case BMODE_SHIFT_R: /* /2, shift_in into bit 15 */
        *reg = (uint16_t)(((*reg >> 1) & ~0x0080) | ((shift_in_07 & 1) << 7) |
                          ((uint16_t)(shift_in & 1) << 15)); break;
    case BMODE_LOAD:    *reg = load_data; break;
    }
}

/* The BYTE MUX select of E108 (K1-10) is BYTE L, E68 pin 9. Low means a
 * byte. */
static int byte_mux_ir(uint16_t ir) {
    return !E68_BYTE_L(e68_rom[e68_addr(ir)]);
}

static int byte_mux(const KD11EA *cpu) {
    return byte_mux_ir(cpu->ir);
}

/* MODE 01 and 00 H (K1-8, SR0 MUX E71). They give the current mode, the
 * previous mode, or kernel. */
static uint8_t mmu_mode_of(uint64_t uw, uint16_t psw) {
    if (UW_FORCE_KERNEL(uw))
        return 0;
    return (uint8_t)(!UW_PREV_MODE_L(uw) ? (psw >> 12) & 3 : (psw >> 14) & 3);
}

/* The PAR and PDR entry is addressed by the mode and VBA 15-13 (E91,
 * K1-7). */
static int mmu_index(uint64_t uw, uint16_t psw, uint16_t vba) {
    return (mmu_mode_of(uw, psw) == 3 ? 8 : 0) + (vba >> 13);
}

/* CC N, Z, V and C (K1-10, from E108, E121 and E107) in bits 3-0, for the
 * probe. */
static uint8_t cc_wires(uint16_t ir, uint16_t a_leg, uint16_t b_leg,
                        uint16_t alu, uint16_t psw, uint8_t rot_cbit) {
    int byte = byte_mux_ir(ir);
    uint8_t sign = byte ? 7 : 15;
    uint8_t n = (alu >> sign) & 1;
    uint8_t z = (alu & (byte ? 0xFF : 0xFFFF)) == 0;
    uint8_t vc = e107_rom[e107_addr(n, (a_leg >> sign) & 1, (b_leg >> sign) & 1,
                                    rot_cbit, psw & 1,
                                    E68_CC_CODE(e68_rom[e68_addr(ir)]))];
    return (uint8_t)((n << 3) | (z << 2) | (E107_CC_V(vc) << 1) | E107_CC_C(vc));
}

/* The service ROMs of K2-3. Returns the outputs of E51. */
static uint8_t service_roms(KD11EA *cpu) {
    uint8_t e54_d = e54_rom[e54_addr(kd11ea_ir_pins(cpu), (cpu->psw >> 15) & 1)];
    cpu->enab_tbit = E54_ENAB_TBIT(e54_d);
    cpu->tbit_trap = ((cpu->psw >> 4) & 1) & cpu->enab_tbit;
    uint16_t e52_a = e52_addr(
        cpu->tbit_trap,                     /* T AND ENAB TBIT (E50) */
        cpu->stov,
        cpu->bus_error,
        0,                                  /* PE: not modeled */
        cpu->mmu_kte,
        0,                                  /* PFAIL: not modeled */
        cpu->ir_code,
        1);                                 /* FP11 SERVICE BR PFAIL L */
    cpu->svc_e52 = e52_rom[e52_a];
    cpu->svc_e51 = e51_rom[e51_addr(cpu->svc_e52, 1)];
    return cpu->svc_e51;
}

/* MPC next-address: NEXT field OR'd with branch signals via E102 */
static uint16_t compute_next_mpc(KD11EA *cpu, uint64_t uw, uint8_t count_at_start, uint16_t bx_at_start, uint16_t sp_out) {
    uint16_t next = uw_next_mpc(uw);
    uint8_t misc = UW_MISC(uw);

    uint8_t but_bits = UW_BUT_BITS(uw);
    uint8_t e102_a = e102_addr(but_bits);
    uint8_t e102_d = e102_rom[e102_a];

    /* The branches test ALU COUT (K1-4), and N and Z of the ALU result, not
     * those of the PSW. */
    if (E102_BUT_ALU_COUT_MPC7(e102_d))
        next |= (cpu->alu_cout & 1) << 7;
    int byte = byte_mux(cpu);
    if (E102_BUT_CC_N_MPC6(e102_d))
        next |= ((cpu->alu_out >> (byte ? 7 : 15)) & 1) << 6;
    if (E102_BUT_BXREG01_MPC5(e102_d))
        next |= ((bx_at_start >> 1) & 1) << 5;
    if (E102_BUT_BXREG00_MPC4(e102_d))
        next |= (bx_at_start & 1) << 4;
    if (E102_BUT_SP15_MPC3(e102_d))
        next |= ((sp_out >> 15) & 1) << 3;
    if (E102_BUT_COUNT05_MPC2(e102_d))
        next |= ((count_at_start >> 5) & 1) << 2;
    if (E102_BUT_CC_Z_MPC1(e102_d))
        next |= ((cpu->alu_out & (byte ? 0xFF : 0xFFFF)) == 0 ? 1 : 0) << 1;
    if (E102_BUT_IR09_MPC0(e102_d))
        next |= ((cpu->ir >> 9) & 1);

    /* BUT SERVICE (K2-3): E52 priority-encodes trap conditions,
     * E51 decodes to vector bits. E3 forces MPC bit 0 -> MPC 001. */
    if (UW_BUT_SERVICE(uw)) {
        uint8_t e51_d = service_roms(cpu);
        /* E45 toggles on the STOV service, so E29 stays clear during its
         * trap sequence. J is BUT SERVICE and K is STOV SERV. */
        cpu->stov_en = E51_STOV_SERV(e51_d) ? !cpu->stov_en : 1;
        if (!cpu->stov_en)
            cpu->stov = 0;
        if (!E51_ENAB_GRANTS(e51_d)) {
            next |= 1;
            cpu->service_vector = SERVICE_VECTOR(e51_d, cpu->mmu_kte, 0);
            cpu->service_trap = 1;
            cpu->service_intr = 0;

            cpu->bus_error = 0;
            cpu->tbit_trap = 0;
            cpu->mmu_kte = 0;

            trace("  BUT SERVICE: vec=%03o%s%s", cpu->service_vector,
                  E51_PFAIL_SERV(e51_d) ? " PFAIL_SERV" : "",
                  E51_STOV_SERV(e51_d) ? " STOV_SERV" : "");
        } else if (cpu->halt_rqst) {
            /* HALT detected (E53 HALT RQST, not through E52 priority).
             * On real hardware, drives RUN flip-flop low. */
            cpu->halted = 1;
            trace("  BUT SERVICE: HALT");
        } else {
            /* Grant the highest BR above the CPU priority (K2-2). */
            uint8_t cpu_pri = (cpu->psw >> 5) & 7;
            if (int_pending(&cpu->intc, cpu_pri) >= 0) {
                IntRequest irq = int_ack(&cpu->intc, cpu_pri);
                next |= 1;
                cpu->service_vector = irq.vector;
                cpu->service_trap = 1;
                cpu->service_intr = 1;
                trace("  BUT SERVICE: INTR vec=%03o BR%d", irq.vector, irq.priority);
            }
        }
    }

    /* BUT DEST (K2-6). E69 and E70 drive MPC 07-03, and IR 5-3 drive
     * MPC 02-00. */
    if (misc == MISC_BUT_DEST) {
        uint8_t d69 = e69_rom[e69_addr(cpu->ir, /*but_dest_l=*/0, /*ir_decode_h=*/0)];
        uint8_t d70 = e70_rom[e70_addr(cpu->ir, /*but_dest_l=*/0, /*ir_decode_h=*/0)];
        next |= (!E69_MPC03_L(d69)) << 3;
        next |= (!E69_MPC04_L(d69)) << 4;
        next |= (!E70_MPC05_L(d70)) << 5;
        next |= (!E70_MPC06_L(d70)) << 6;
        next |= (!E70_MPC07_L(d70)) << 7;
        next |= (cpu->ir >> 3) & 7;
    }

    return next & 0x1FF;
}

/* IR Decode: wire-OR of E59/E60/E63/E69/E70/E71/E74 (K2-5, K2-6).
 * OC bit=0 -> MPC bit=1. Trap dispatch is in compute_next_mpc(). */
static uint16_t ir_decode_entry(uint16_t ir, uint16_t psw) {
    uint16_t mpc = 0;
    uint8_t ir1412 = (ir >> 12) & 7;   /* IR[14:12] */
    uint8_t ir15 = (ir >> 15) & 1;

    /* E69 and E70 decode the double operand instructions. */
    {
        uint8_t d69 = e69_rom[e69_addr(ir, /*but_dest_l=*/1, /*ir_decode_h=*/1)];
        mpc |= (!E69_MPC04_L(d69)) << 4;
        mpc |= (!E69_MPC03_L(d69)) << 3;
        if (!E69_SRC_H(d69))                /* IR 11-9 on MPC 02-00 */
            mpc |= (ir >> 9) & 7;

        uint8_t d70 = e70_rom[e70_addr(ir, /*but_dest_l=*/1, /*ir_decode_h=*/1)];
        mpc |= (!E70_MPC07_L(d70)) << 7;
        mpc |= (!E70_MPC06_L(d70)) << 6;
        mpc |= (!E70_MPC05_L(d70)) << 5;
    }

    /* E60 decodes the single operand instructions. */
    if (ir1412 == 0) {
        uint8_t d60 = e60_rom[e59_e60_addr(ir)];
        mpc |= (!E60_MPC06_L(d60)) << 6;
        mpc |= (!E60_MPC05_L(d60)) << 5;
        mpc |= (!E60_MPC04_L(d60)) << 4;
        mpc |= (!E60_MPC03_L(d60)) << 3;
    }

    /* E59 DEST puts IR 5-3 on MPC 02-00. */
    if (ir1412 == 0) {
        uint8_t d59 = e59_rom[e59_e60_addr(ir)];
        if (!E59_DEST_L(d59))
            mpc |= (ir >> 3) & 7;
    }

    /* E63: operates MPC[3:0] (HALT, WAIT, RTI, BPT, IOT, RTS, CC, JMP, SWAB) */
    if (ir1412 == 0 && ((ir >> 8) & 0xF) == 0 && !ir15) {
        uint8_t a63 = e63_addr(ir);
        uint8_t d63 = e63_rom[a63];
        mpc |= (!E63_MPC00_L(d63));
        mpc |= (!E63_MPC01_L(d63)) << 1;
        mpc |= (!E63_MPC02_L(d63)) << 2;
        mpc |= (!E63_MPC03_L(d63)) << 3;
    }

    /* E74: EIS decode (IR15=0 only, active when IR[14:12]=111) */
    if (!ir15) {
        uint8_t d74 = e74_rom[e74_addr(ir, /*ir_decode_h=*/1)];
        mpc |= (!E74_MPC07_L(d74)) << 7;
        mpc |= (!E74_MPC06_L(d74)) << 6;
        mpc |= (!E74_MPC05_L(d74)) << 5;
        mpc |= (!E74_MPC04_L(d74)) << 4;
        mpc |= (!E74_MPC03_L(d74)) << 3;
        if (!E74_DEST_L(d74))
            mpc |= (ir >> 3) & 7;
    }

    /* E71 (E72 on the schematic): branch condition, MPC07 L pulled low when taken */
    if (ir1412 == 0 && !((ir >> 11) & 1)) {
        uint8_t a71 = e71_addr(ir, psw);
        uint8_t d71 = e71_rom[a71];
        mpc |= (!E71_MPC07_L(d71)) << 7;
    }

    /* JSR: handled by E60+E59. EMT/TRAP/BPT/IOT/illegal/HALT: mpc=0 here,
     * E53 sets IR CODE or HALT RQST, BUT SERVICE overrides to MPC 001. */
    return mpc & 0x1FF;
}


/* Decode ROM outputs on their pins, for the probe. A disabled open
 * collector output reads high. */
typedef struct { uint8_t e53, e59, e60, e63, e69, e70, e71, e74; } DecodePins;

static DecodePins decode_pins(uint16_t ir, uint16_t psw, int ir_decode_h, int but_dest_l) {
    DecodePins d;
    int ir1412_0 = ((ir >> 12) & 7) == 0;
    int user = (psw >> 15) & 1;
    d.e53 = e53_enabled(ir) ? e53_rom[e53_addr(ir, user)] : 0xF;
    d.e59 = (ir_decode_h && ir1412_0) ? e59_rom[e59_e60_addr(ir)] : 0xF;
    d.e60 = (ir_decode_h && ir1412_0) ? e60_rom[e59_e60_addr(ir)] : 0xF;
    d.e63 = (ir_decode_h && (ir & 0xFF00) == 0) ? e63_rom[e63_addr(ir)] : 0xF;
    d.e69 = e69_rom[e69_addr(ir, but_dest_l, ir_decode_h)];
    d.e70 = e70_rom[e70_addr(ir, but_dest_l, ir_decode_h)];
    d.e71 = (ir_decode_h && (ir & 0x7800) == 0) ? e71_rom[e71_addr(ir, psw)] : 0xF;
    d.e74 = !(ir & 0x8000) ? e74_rom[e74_addr(ir, ir_decode_h)] : 0xFF;
    return d;
}

/* Main micro-cycle execution */

uint32_t kd11ea_ustep(KD11EA *cpu) {
    if (cpu->halted)
        return 0;

    uint64_t uw = ucode_read(cpu->mpc);
    /* IR DECODE (1) H (E55) decodes in the cycle after LOAD IR. */
    uint8_t ir_decode_1 = cpu->ir_valid;
    uint16_t psw_before = cpu->psw;
    uint8_t  ovx_before = cpu->ovx;
    uint8_t  rot_cbit_before = cpu->rot_cbit;
    uint16_t ir_before = cpu->ir;
    uint16_t ir_pins_before = kd11ea_ir_pins(cpu);
    uint16_t b_before = cpu->b_reg;
    uint16_t bx_before = cpu->bx_reg;
    uint8_t  kte_before = cpu->mmu_kte;
    uint8_t  br_lines = int_br_lines(&cpu->intc);
    uint8_t  e29_d = e29_rom[e29_addr((psw_before >> 5) & 7, 0, (br_lines >> 7) & 1,
                                      (br_lines >> 6) & 1, (br_lines >> 5) & 1,
                                      (br_lines >> 4) & 1)];
    uint8_t  dbe = 0;
    uint8_t  msyn = 0, ssyn = 0, timeout = 0;
    int nxm_abort = 0;                  /* a bus abort sends MPC to 000 */

    trace("MPC=%03o  uw=%012llX", cpu->mpc, (unsigned long long)uw);

    uint8_t alu_field = UW_ALU_FUNC(uw);
    uint8_t e82_a = e82_addr(alu_field);
    uint8_t e82_d = e82_rom[e82_a];
    uint8_t alu_s    = E82_ALU_S(e82_d);
    uint8_t alu_mode = E82_MODE(e82_d);
    uint8_t alu_cin  = !E82_CIN_L(e82_d);
    uint8_t bleg     = E82_BLEG(e82_d);

    uint8_t bbx_field = UW_BBXOVXDBE(uw);
    uint8_t e87_a = e87_addr(bbx_field);
    uint8_t e87_d = e87_rom[e87_a];
    uint8_t b_mode  = E87_B_MODE(e87_d);
    uint8_t bx_mode = E87_BX_MODE(e87_d);

    /* ROM outputs for the probe, 0xFF when not evaluated. */
    uint8_t prb_e54 = 0xFF;
    prb_e54 = e54_rom[e54_addr(kd11ea_ir_pins(cpu), (cpu->psw >> 15) & 1)];
    uint8_t prb_e107 = 0xFF;
    uint8_t prb_e51 = 0xFF, prb_e52 = 0xFF;

    /* Scratchpad read (ALU A-input). DST resolved after LOAD_BA. */
    uint8_t spa_src = spa_resolve(cpu, UW_SPA_SRC_SEL(uw), uw);
    uint8_t spa_dst_sel = UW_SPA_DST_SEL(uw);
    uint8_t aux_control = UW_AUX_CONTROL(uw);

    /* AUX CONTROL: E83 (DOP) or E61/E62 (SOP) override E99 ALU function.
     * Note: TM-001 calls E83 "E81" but E81 is the 7437 buffer on the board. */
    if (aux_control) {
        uint8_t fc;
        if (((cpu->ir >> 12) & 7) == 0)
            fc = e61_rom[e61_addr(cpu->ir, cpu->psw & 1, (cpu->psw >> 3) & 1)] & 0xF;
        else
            fc = E83_FUNC_CODE(e83_rom[e83_addr(cpu->ir)]);
        uint8_t e82_field = (alu_field & 1) |
                            (((fc >> 3) & 1) << 1) |
                            (((fc >> 2) & 1) << 2) |
                            (((fc >> 1) & 1) << 3) |
                            ((fc & 1) << 4);
        e82_d    = e82_rom[e82_addr(e82_field)];
        alu_s    = E82_ALU_S(e82_d);
        alu_mode = E82_MODE(e82_d);
        alu_cin  = !E82_CIN_L(e82_d);
        bleg     = E82_BLEG(e82_d);
    }

    uint16_t a_leg = cpu->sp[spa_src & 0xF];

    /* BYTE H (E64, K2-5) is NAND of IR DECODE (1) L and E68 BYTE L. */
    uint8_t ir_decode_h = ir_decode_1;
    uint8_t byte_h = !E68_BYTE_L(e68_rom[e68_addr(kd11ea_ir_pins(cpu))]) || ir_decode_h;

    /* E80 controls bytes and writes (K2-8). */
    uint8_t buf_dat_tran = UW_BUF_DAT_TRAN(uw);
    uint8_t ssmux_ctl = UW_SSMUX(uw);
    uint8_t vba00 = UW_LOAD_BA(uw) ? (a_leg & 1) : (cpu->vba & 1);
    uint8_t mov_l = E69_MOV_L(e69_rom[e69_addr(kd11ea_ir_pins(cpu), UW_MISC(uw) != MISC_BUT_DEST,
                                               ir_decode_h)]);
    uint8_t e86_11 = !(vba00 & buf_dat_tran);
    uint8_t e112_10 = !e86_11;
    uint8_t e80_a = e80_addr((ssmux_ctl >> 1) & 1, ssmux_ctl & 1, buf_dat_tran,
                             aux_control, byte_h, e112_10, mov_l);
    uint8_t e80_d = e80_rom[e80_a];
    uint8_t e80_swap = E80_SWAP(e80_d);
    uint8_t e80_sex  = E80_SEX(e80_d);

    /* A byte autoincrement steps by 1, except on R6 and R7 (K2-4). */
    uint8_t r6_7 = (((15 - spa_src) >> 1) & 3) == 3;
    uint8_t disable_msyn_plus1_l = k2_4_disable_msyn_plus1_l(
        E80_DISAB_MSYN_L(e80_d), !r6_7, buf_dat_tran);
    uint16_t b_leg = (bleg == BLEG_PLUS1 && !disable_msyn_plus1_l) ? 0 : bmux_select(cpu, bleg);

    uint8_t cout = 0;
    uint16_t alu_result = alu_74s181(a_leg, b_leg, alu_s, alu_mode, alu_cin, &cout);

    cpu->alu_out = alu_result;
    cpu->alu_cout = cout;

    trace("  ALU: A=%06o(%s[%o]) B=%06o S=%X M=%d Cin=%d -> %06o Cout=%d",
          a_leg,
          (const char*[]){"ROM","RS","RD","RBA"}[UW_SPA_SRC_SEL(uw)],
          spa_src,
          b_leg, alu_s, alu_mode, alu_cin, alu_result, cout);

    /* E62 acts on the step before an AUX step. */
    uint8_t e62_a = e62_addr(ir_before, b_before & 1, psw_before & 1,
                             (alu_result >> (byte_mux_ir(ir_before) ? 7 : 15)) & 1);
    uint8_t e62_d = 0xF;                /* disabled outputs are pulled up */

    /* VBA register (E49/E40/E47, K1-6): loaded from scratchpad read bus
     * (not ALU output), so it captures the pre-increment value. */
    uint8_t load_ba = UW_LOAD_BA(uw);
    /* RELOCATE H (E113, K1-8) comes from the SR0 enable, or from
     * maintenance mode with ENAB MAINT. */
    int relocate = (cpu->mmu.sr0 & SR0_ENABLE) ||
                   ((cpu->mmu.sr0 & SR0_MAINT) && UW_ENAB_MAINT(uw));
    if (load_ba) {
        uint16_t vba = a_leg & 0xFFFF;
        cpu->vba = vba;

        int mmu_mode = (cpu->psw >> 14) & 3;
        if (UW_FORCE_KERNEL(uw))
            mmu_mode = 0;
        else if (!UW_PREV_MODE_L(uw))
            mmu_mode = (cpu->psw >> 12) & 3;

        uint32_t pba;
        mmu_translate(&cpu->mmu, vba, mmu_mode, MMU_MAP, relocate, PC(cpu), &pba);
        cpu->ba = pba;
        trace("  BA <- %06o (VBA=%06o)", cpu->ba, vba);
    }

    /* E95 and E30 (K1-5) choose a short or long cycle. This happens before
     * a write can change SR0. */
    uint32_t period_ns = clockgen_period_ns(cpu->timing, UW_LONG_CYCLE_L(uw),
                                           UW_LOAD_BA(uw), relocate);
    uint64_t msyn_ns = cpu->ns_elapsed + clockgen_msyn_ns(cpu->timing, period_ns);
    uint8_t bus_ctl = UW_BUS_CTL(uw);

    /* UBUS C0 is BUF C0 AND BYTE H (K2-1), so a word DATOB becomes DATO. */
    if (bus_ctl == BUS_DATOB && !byte_h) {
        bus_ctl = BUS_DATO;
    }

    uint8_t amux_l = UW_AMUX_L(uw);
    uint16_t amux_out = amux_select(cpu, amux_l, alu_result);

    uint16_t ss_out = ssmux_select(amux_out, e80_swap, e80_sex);

    /* E83 (K1-8) sees UBUS C1 C0 when the transfer starts. Every KT abort
     * is decided and latched here, not when BA was loaded. */
    if (buf_dat_tran && cpu->mmu.page >= 0 && !cpu->mmu_abort) {
        uint32_t pba;
        if (mmu_translate(&cpu->mmu, cpu->vba, cpu->mmu.mode, bus_ctl, 1, PC(cpu), &pba) < 0)
            cpu->mmu_abort = 1;
    }

    /* An odd word address or a KT fault aborts the transfer (K2-1). */
    K2BusAbort bus_abort = k2_1_bus_abort(disable_msyn_plus1_l,
        !cpu->mmu_abort, buf_dat_tran, 1, 0, 0);
    if (buf_dat_tran && bus_abort.abort_h) {
        if (cpu->mmu_abort) {
            cpu->mmu_abort = 0;
            cpu->mmu_kte = 1;
        }
        if (!bus_abort.set_be_l)
            cpu->bus_error = 1;
        nxm_abort = 1;
        buf_dat_tran = 0;
        trace("  BUS CYCLE CANCELLED (E42 INHIBIT MSYN)");
    }

    msyn = buf_dat_tran;
    if (buf_dat_tran) {
        int bus_rc = 0;
        switch (bus_ctl) {
        case BUS_DATI:
        case BUS_DATIP:
            bus_rc = bus_read_at(cpu->bus, cpu->ba, &cpu->unibus_data, msyn_ns);
            trace("  BUS READ @%06o = %06o", cpu->ba, cpu->unibus_data);
            amux_out = amux_select(cpu, amux_l, alu_result);
            ss_out   = ssmux_select(amux_out, e80_swap, e80_sex);
            break;
        case BUS_DATO:
        case BUS_DATOB:
            bus_rc = bus_write_at(cpu->bus, cpu->ba, ss_out, bus_ctl, msyn_ns);
            /* The W bit (E81 and E123, K1-7) is set by a relocated write
             * that is not aborted and does not go to a KT register. */
            if (bus_rc >= 0 && cpu->mmu.page >= 0 && !mmu_kt_write(cpu->ba, bus_ctl & 1))
                cpu->mmu.pdr[cpu->mmu.page] |= PDR_W;
            trace("  BUS WRITE @%06o <- %06o", cpu->ba, ss_out);
            break;
        }
        if (bus_rc < 0) {
            /* Without SSYN, the timeout E14 sets BE and aborts the
             * instruction. */
            timeout = 1;
            bus_abort = k2_1_bus_abort(disable_msyn_plus1_l, 1, 1, 1, 0, 1);
            if (!bus_abort.set_be_l)
                cpu->bus_error = 1;
            nxm_abort = 1;
            trace("  NXM PC=%06o IR=%06o BA=%06o", PC(cpu), cpu->ir, (unsigned)cpu->ba);
        }
        ssyn = !timeout;
    }

    /* LOAD PSW LOW L (E75, K1-10) follows a write to the PSW address. */
    uint8_t load_psw_low = 0;
    if (msyn && bus_ctl >= BUS_DATO && m8265_e54_io_page_enabled(cpu->ba) &&
        m8265_e81_general_enabled(cpu->ba))
        load_psw_low = !M8265_E75_LOAD_PSW_LOW_L(
            m8265_e75_rom[m8265_e74_e75_addr(cpu->ba, (bus_ctl >> 1) & 1, bus_ctl & 1, 0)]);

    /* On a double bus error (DBE, K2-3) the IR is cleared and E53 requests
     * a HALT. */
    if (k2_3_dbe_h(bus_abort.set_be_l, E87_ENAB_DBE_L(e87_d))) {
        dbe = 1;
        cpu->ir = 0;
        cpu->serv_ir = 0;
        uint8_t user_mode_h = !cpu->bus_error && ((cpu->psw >> 15) & 1);
        uint8_t d53 = e53_rom[e53_addr(cpu->ir, user_mode_h)];
        cpu->halt_rqst = !E53_HALT_RQST_L(d53);
        cpu->halted = cpu->halt_rqst;
        trace("  DBE H: IR cleared, HALT RQST L");
    }

    /* OVX sticky (E115 XOR, K1-10): set if BREG[15]^BREG[14], cleared at LOAD_IR */
    if (!E87_ENAB_OVX_L(e87_d)) {
        if (((b_before >> 15) ^ (b_before >> 14)) & 1)
            cpu->ovx = 1;
    }

    /* Misc control. Count saved before CLK_COUNT (E102 reads start-of-cycle). */
    uint8_t count_before = cpu->count;

    uint8_t misc = UW_MISC(uw);
    /* An aborted transfer loads nothing. */
    if (!nxm_abort) switch (misc) {
    case MISC_NOP:
        break;
    case MISC_LOAD_IR: {
        cpu->ir = cpu->unibus_data;
        cpu->serv_ir = 0;
        cpu->ir_valid = 1;
        cpu->ovx = 0;
        /* SR2 = fetch address (PC not yet incremented), frozen while SR0 abort set */
        if (!(cpu->mmu.sr0 & SR0_ABORT_MASK))
            cpu->mmu.sr2 = PC(cpu);

        /* IR CODE is a wired OR of E53, E59, E74 and E70 (K2-6). */
        uint8_t user_mode = (cpu->psw >> 15) & 1;
        uint8_t e53_d;
        if (e53_enabled(cpu->ir)) {
            uint8_t e53_a = e53_addr(cpu->ir, user_mode);
            e53_d = e53_rom[e53_a];
        } else {
            e53_d = 0xF;
        }
        cpu->ir_code = e53_ir_code(e53_d);
        cpu->halt_rqst = !E53_HALT_RQST_L(e53_d);

        if (((cpu->ir >> 12) & 7) == 0) {
            uint8_t e59_d = e59_rom[e59_e60_addr(cpu->ir)];
            cpu->ir_code |= e59_ir_code(e59_d);
        }
        if (!(cpu->ir & 0x8000)) {
            uint8_t e74_d = e74_rom[e74_addr(cpu->ir, 1)];
            if (!E74_IR_CODE_00_L(e74_d))
                cpu->ir_code |= 1;
        }
        {
            uint8_t e70_d = e70_rom[e70_addr(cpu->ir, 1, 1)];
            if (!E70_IR_CODE_00_L(e70_d))
                cpu->ir_code |= 1;
        }

        trace("  IR <- %06o  ir_code=%o halt=%d", cpu->ir, cpu->ir_code, cpu->halt_rqst);
        break;
    }
    case MISC_LOAD_PSW: {
        uint16_t new_psw = cpu->unibus_data;
        if (UW_FORCE_KERNEL(uw)) {
            /* Trap/interrupt: insert old current mode into previous mode */
            new_psw = (new_psw & ~0x3000) | ((cpu->psw & 0xC000) >> 2);
        }
        /* In user mode, RTI and RTT raise DISABLE LOAD PSW H (E54). It blocks
         * LOAD PSW 12-15 L and the priority clock (E122 and E112, K1-2). */
        if (E54_DISABLE_LOAD_PSW(prb_e54))
            new_psw = (new_psw & ~0xF0E0) | (cpu->psw & 0xF0E0);
        cpu->psw = new_psw & 0xF0FF;    /* no bits 11-8 */
        trace("  PSW <- %06o", cpu->psw);
        break;
    }
    case MISC_LOAD_CC: {
        /* Under AUX the PSW MUX (E96, K1-1) takes the CC logic. When the PSW
         * itself is written (E115, E122) it takes the written data. */
        if (!aux_control || load_psw_low) {
            cpu->psw = (cpu->psw & 0xFFF0) | (ss_out & 0xF);
            trace("  LOAD CC (SSMUX): N=%d Z=%d V=%d C=%d  PSW=%06o",
                  (ss_out >> 3) & 1, (ss_out >> 2) & 1,
                  (ss_out >> 1) & 1, ss_out & 1, cpu->psw);
            break;
        }

        /* E68 gives the CC CODE and E107 gives V and C. N and Z come from
         * the ALU (K1-10). */
        uint8_t e68_val = e68_rom[e68_addr(cpu->ir)];
        uint8_t cc_code = E68_CC_CODE(e68_val);
        int is_byte = byte_mux(cpu);

        uint8_t sign_bit = is_byte ? 7 : 15;
        uint16_t z_mask = is_byte ? 0xFF : 0xFFFF;
        uint8_t cc_n = (alu_result >> sign_bit) & 1;
        uint8_t cc_z = ((alu_result & z_mask) == 0) ? 1 : 0;

        uint8_t r_msb = (alu_result >> sign_bit) & 1;
        uint8_t s_msb = (a_leg >> sign_bit) & 1;
        uint8_t d_msb = (b_leg >> sign_bit) & 1;
        uint8_t rot_cbit = rot_cbit_before;
        uint8_t psw_cbit = cpu->psw & 1;

        uint8_t e107_val = e107_rom[e107_addr(r_msb, s_msb, d_msb,
                                              rot_cbit, psw_cbit, cc_code)];
        prb_e107 = e107_val;
        uint8_t cc_v = E107_CC_V(e107_val);
        uint8_t cc_c = E107_CC_C(e107_val);

        cpu->psw = (cpu->psw & 0xFFF0) |
                   (cc_n << 3) | (cc_z << 2) | (cc_v << 1) | cc_c;
        trace("  LOAD CC: N=%d Z=%d V=%d C=%d  PSW=%06o (cc_code=%d%s)",
              cc_n, cc_z, cc_v, cc_c, cpu->psw, cc_code,
              is_byte ? " byte" : "");
        break;
    }
    case MISC_BUT_DEST:                 /* see compute_next_mpc() */
        break;
    case MISC_ENAB_STOV:
        /* E36 (K2-3) sets E29 when R6 is on the SPA lines and ALU 15-8 are
         * zero. E29 is clocked in kernel mode only (E20). */
        if (cpu->stov_en && !((psw_before >> 15) & 1) && 15 - (spa_src & 0xF) == 6 &&
            (alu_result & 0xFF00) == 0)
            cpu->stov = 1;
        break;
    case MISC_LOAD_COUNT:
        cpu->count = ss_out & 0x3F;
        trace("  COUNT <- %02o (from ss_out %06o)", cpu->count, ss_out);
        break;
    case MISC_CLK_COUNT: {
        /* 6-bit counter (E78/E79, 74193): converges toward 0 */
        uint8_t c05 = (cpu->count >> 5) & 1;
        if (c05)
            cpu->count = (cpu->count + 1) & 0x3F;
        else
            cpu->count = (cpu->count - 1) & 0x3F;
        trace("  COUNT %s = %02o", c05 ? "++" : "--", cpu->count);
        break;
    }
    }

    /* Resolve spa_dst after LOAD_BA (write-side uses end-of-cycle BA) */
    uint8_t spa_dst = spa_resolve(cpu, spa_dst_sel, uw);

    /* SP WRITE L (E37 and E91, K2-4) writes in every cycle unless SPA DST
     * SEL is BA. The console path through E86 (SACK RET, K1-10) is not
     * modelled. */
    uint8_t spm_write_en = spa_dst_sel != SPA_RBA;
    uint8_t sp_write = spm_write_en && !nxm_abort;
    if (spm_write_en && !nxm_abort) {
        uint16_t write_val = ss_out;
        /* E80 DISABLE UPPER: byte modify ops only write low byte */
        if (E80_DISABLE_UPPER(e80_d)) {
            write_val = (cpu->sp[spa_dst & 0xF] & 0xFF00) | (write_val & 0xFF);
        }
        trace("  SPM[%o] <- %06o", spa_dst & 0xF, write_val);
        /* Detect SP crash: slot 9 = R6/SP, big jump down (not -2 push) */
        if ((spa_dst & 0xF) == 9 && cpu->sp[9] > 0x0800 &&
            write_val < cpu->sp[9] && (cpu->sp[9] - write_val) > 0x100) {
            trace("=== SP CRASH: %06o -> %06o (delta=%06o) at MPC=%03o PC=%06o IR=%06o ===\n",
                    cpu->sp[9], write_val, cpu->sp[9] - write_val,
                    cpu->mpc, PC(cpu), cpu->ir);
        }
        cpu->sp[spa_dst & 0xF] = write_val;
    }

    /* Observe E52 before compute_next_mpc services and clears the inputs. */
    uint16_t prb_e52_inputs = 0;
    if (cpu->probe) {
        uint8_t ed = e54_rom[e54_addr(kd11ea_ir_pins(cpu), cpu->psw >> 15)];
        prb_e52_inputs = e52_addr(((cpu->psw >> 4) & 1) & E54_ENAB_TBIT(ed),
                                  cpu->stov, cpu->bus_error, 0, cpu->mmu_kte,
                                  0, cpu->ir_code, 1);
    }
    uint16_t next_mpc = compute_next_mpc(cpu, uw, count_before, bx_before, a_leg);
    if (UW_BUT_SERVICE(uw)) {
        prb_e52 = cpu->svc_e52;
        prb_e51 = cpu->svc_e51;
    } else {
        prb_e52 = 0xF;                  /* disabled, pulled up */
        prb_e51 = e51_rom[e51_addr(0xF, 0)];
    }

    if (nxm_abort)
        next_mpc = 0;

    uint8_t  dbg_ir_decoded = 0;
    uint16_t dbg_ir_decode_mpc = 0;
    uint8_t  dbg_service = 0;
    uint8_t  service_load_b = 0;

    /* The service vector goes into B, from the constants for a trap or the
     * D lines for an interrupt. */
    uint8_t intr_this_cycle = 0;
    if (UW_BUT_SERVICE(uw) && cpu->service_trap) {
        cpu->service_trap = 0;
        intr_this_cycle = cpu->service_intr;
        dbg_service = 1;
        service_load_b = 1;
        if (cpu->service_intr)
            cpu->unibus_data = cpu->service_vector;
        amux_out = ss_out = cpu->service_vector;
        trace("  BUT SERVICE override → MPC %03o (B←vec %03o)",
              next_mpc, cpu->service_vector);
        cpu->ir_code = 0;
        cpu->halt_rqst = 0;
        cpu->bus_error = 0;
    }

    /* With IR DECODE (1) the entry is ORed over the NEXT field, which is
     * 000. */
    if (ir_decode_1) {
        uint16_t ir_mpc = ir_decode_entry(cpu->ir, cpu->psw);
        trace("  IR DECODE -> MPC %03o", ir_mpc);
        cpu->ir_valid = 0;
        dbg_ir_decoded = 1;
        dbg_ir_decode_mpc = ir_mpc;
        next_mpc |= ir_mpc;
    }

    /* E84 enables E62 on AUX SETUP H with IR 14-12 at 0. */
    uint8_t aux_setup = UW_AUX_CONTROL(ucode_read(next_mpc));
    if (aux_setup && ((ir_before >> 12) & 7) == 0)
        e62_d = e62_rom[e62_a];

    /* FUNC CODE, from E61 or E83 under AUX SETUP, for the probe only. */
    uint8_t func_code = 0xF;
    if (aux_setup) {
        if (((ir_before >> 12) & 7) == 0) {
            func_code = e61_rom[e61_addr(ir_before, psw_before & 1, (psw_before >> 3) & 1)] & 0xF;
        } else {
            func_code = E83_FUNC_CODE(e83_rom[e83_addr(ir_before)]);
        }
    }

    /* B MODE 01 and 00 L are a wired AND of the open collector outputs of
     * E87 and E62. */
    b_mode &= E62_B_MODE_01_L(e62_d) | (E62_B_MODE_00_L(e62_d) << 1);

    /* The SHIFT MUX (E119, K1-10) gives SHIFT IN B from B15, SERIAL SHIFT,
     * 0 or BX15, and SHIFT IN BX from ALU COUT, OVX, 1 or 0. SHIFT IN 07
     * (E108) is SERIAL SHIFT for a byte and B08 for a word. */
    uint8_t shift_mux = E87_SHIFT_MUX_L(e87_d);
    uint8_t shift_in_b, shift_in_bx;
    switch (shift_mux) {
    default:
    case 0: shift_in_b = (b_before >> 15) & 1; break;
    case 1: shift_in_b = E62_SERIAL_SHIFT(e62_d); break;
    case 2: shift_in_b = 0; break;
    case 3: shift_in_b = (bx_before >> 15) & 1; break;
    }
    switch (shift_mux) {
    default:
    case 0: shift_in_bx = cout; break;
    case 1: shift_in_bx = ovx_before; break;
    case 2: shift_in_bx = 1; break;
    case 3: shift_in_bx = 0; break;
    }
    uint8_t shift_in_07 = byte_mux_ir(ir_before) ? E62_SERIAL_SHIFT(e62_d)
                                                 : (b_before >> 8) & 1;
    breg_update(&cpu->b_reg, b_mode, ss_out, shift_in_b, shift_in_07);

    /* On a right shift BX takes B0. */
    uint8_t bx_shift_in = (bx_mode == BMODE_SHIFT_R) ? (b_before & 1) : shift_in_bx;
    breg_update(&cpu->bx_reg, bx_mode, ss_out, bx_shift_in, (bx_before >> 8) & 1);

    if (service_load_b)
        cpu->b_reg = cpu->service_vector;

    cpu->rot_cbit = E62_ROT_CBIT(e62_d);

    trace("  -> MPC %03o  [PC=%06o R0=%06o B=%06o BX=%06o sp4=%06o]\n",
          next_mpc, PC(cpu), REG(cpu,0), cpu->b_reg, cpu->bx_reg, cpu->sp[4]);

    /* The cycle length comes from the K1-5 clock, TRAN INH (K2-1) and BG
     * INH (K2-2). */
    uint32_t cycle_ns = clockgen_cycle_ns(cpu->timing, period_ns,
                                         buf_dat_tran, bus_ctl,
                                         cpu->bus->last_latency_ns,
                                         UW_BUT_SERVICE(uw), intr_this_cycle);

    cpu->ns_elapsed += cycle_ns;

    /* The RESET instruction asserts BUS INIT while E16 runs, with the clock
     * stopped (K2-2). */
    uint8_t reset_inst = E54_START_RESET(prb_e54);
    uint8_t reset_pulse = reset_inst && !cpu->reset_inst;
    if (reset_pulse) {
        bus_set_init(cpu->bus, 0, cpu->ns_elapsed);
        cpu->ns_elapsed += clock_timing(cpu->timing)->reset_ns;
        cycle_ns += clock_timing(cpu->timing)->reset_ns;
        bus_set_init(cpu->bus, 1, cpu->ns_elapsed);
    }
    cpu->reset_inst = reset_inst;

    dbg_record(&cpu->dbg, &(DbgEntry){
        .mpc          = cpu->mpc,
        .next_mpc     = next_mpc,
        .a_leg        = a_leg,
        .b_leg        = b_leg,
        .alu_out      = alu_result,
        .ba           = cpu->ba,
        .ir           = cpu->ir,
        .psw          = cpu->psw,
        .pc           = PC(cpu),
        .sp           = cpu->sp[9],
        .r0           = REG(cpu, 0),
        .r1           = REG(cpu, 1),
        .b_reg        = cpu->b_reg,
        .bx_reg       = cpu->bx_reg,
        .unibus_data  = cpu->unibus_data,
        .spa_src      = spa_src,
        .spa_src_sel  = UW_SPA_SRC_SEL(uw),
        .alu_s        = alu_s,
        .alu_mode     = alu_mode,
        .alu_cin      = alu_cin,
        .alu_cout     = cout,
        .bus_op       = buf_dat_tran ? ((bus_ctl <= BUS_DATIP) ? 1 : 2) : 0,
        .bus_addr_valid = load_ba,
        .ir_loaded    = (misc == MISC_LOAD_IR),
        .ir_decoded   = dbg_ir_decoded,
        .ir_decode_mpc = dbg_ir_decode_mpc,
        .service_trap = dbg_service,
    });

    if (cpu->probe) {
        DecodePins dp = decode_pins(ir_pins_before, psw_before, ir_decode_1,
                                    misc != MISC_BUT_DEST);
        probe_sample(cpu->probe, &(ProbeSnapshot){
            .ns           = cpu->ns_elapsed,
            .uw           = uw,
            .uw_next      = ucode_read(next_mpc & 0x1FF),
            .count        = count_before,
            .amux_out     = amux_out,
            .ss_out       = ss_out,
            .slice_cout   = alu_74s181_slice_carries(a_leg, b_leg, alu_s, alu_mode, alu_cin),
            .byte_mux     = (uint8_t)byte_mux_ir(ir_before),
            .ovx          = ovx_before,
            .b_mode       = b_mode,
            .shift_in     = (uint8_t)((shift_in_b & 1) | ((shift_in_bx & 1) << 1) |
                            ((shift_in_07 & 1) << 2)),
            .cc_wires     = cc_wires(ir_before, a_leg, b_leg, alu_result, psw_before, rot_cbit_before),
            .func_code    = func_code,
            .rot_cbit     = rot_cbit_before,
            .vba          = cpu->vba,
            .mmu_mode     = mmu_mode_of(uw, psw_before),
            .par_out      = cpu->mmu.par[mmu_index(uw, psw_before, cpu->vba)],
            .pdr_out      = cpu->mmu.pdr[mmu_index(uw, psw_before, cpu->vba)],
            .sr0          = cpu->mmu.sr0,
            .sr2          = cpu->mmu.sr2,
            .mpc          = cpu->mpc,
            .next_mpc     = next_mpc,
            .a_leg        = a_leg,
            .b_leg        = b_leg,
            .alu_out      = alu_result,
            .ba           = cpu->ba,
            .unibus_data  = cpu->unibus_data,
            .ir           = ir_before,
            .psw          = psw_before,
            .b_reg        = b_before,
            .bx_reg       = bx_before,
            .pc           = PC(cpu),
            .sp           = cpu->sp[9],
            .sp_user      = cpu->sp[1],
            .r0           = REG(cpu, 0),
            .r1           = REG(cpu, 1),
            .r2           = REG(cpu, 2),
            .r3           = REG(cpu, 3),
            .r4           = REG(cpu, 4),
            .r5           = REG(cpu, 5),
            .alu_s        = alu_s,
            .alu_mode     = alu_mode,
            .alu_cin      = alu_cin,
            .alu_cout     = cout,
            .bus_op       = buf_dat_tran ? ((bus_ctl <= BUS_DATIP) ? 1 : 2) : 0,
            .long_cycle   = UW_LONG_CYCLE_L(uw) ? 0 : 1,
            .buf_dat_tran = buf_dat_tran,
            .ir_valid     = ir_decode_1,
            .ir_code      = cpu->ir_code,
            .service_trap = dbg_service,
            .bus_error    = cpu->bus_error,
            .halted       = cpu->halted,
            .enab_tbit    = cpu->enab_tbit,
            .kte          = kte_before,
            .dbe          = dbe,
            .br_lines     = br_lines,
            .e29_out      = e29_d,
            .bus_grant    = intr_this_cycle,
            .intr         = intr_this_cycle,
            .bus_init_l   = cpu->bus->init_l,
            .reset_pulse  = reset_pulse,
            .bus_c        = bus_ctl,
            .msyn         = msyn,
            .ssyn         = ssyn,
            .timeout      = timeout,
            .abort_h      = bus_abort.abort_h,
            .set_be       = !bus_abort.set_be_l,
            .dis_msyn_p1  = !disable_msyn_plus1_l,
            .clk_long     = period_ns == clock_timing(cpu->timing)->long_ns,
            .spa          = (uint8_t)(15 - (spa_src & 0xF)),
            .spa_wr       = (uint8_t)(15 - (spa_dst & 0xF)),
            .sp_write     = sp_write,
            .cycle_ns     = cycle_ns,
            .ssyn_ns      = buf_dat_tran ? cpu->bus->last_latency_ns : 0,
            .eot_ns       = buf_dat_tran ? clockgen_eot_ns(bus_ctl) : 0,
            .e52_inputs   = prb_e52_inputs,
            .e53_inputs   = e53_addr(ir_pins_before, psw_before >> 15),
            .e53_selects  = ((ir_pins_before >> 6) & 1) |
                            (!e53_enabled(ir_pins_before & ~0x40u) << 1),
            .e54_inputs   = e54_addr(ir_pins_before, psw_before >> 15),
            .e80_inputs   = e80_a,
            .e51_out      = prb_e51,
            .e52_code     = prb_e52,
            .e53_out      = dp.e53,
            .e54_out      = prb_e54,
            .e59_out      = dp.e59,
            .e60_out      = dp.e60,
            .e62_out      = e62_d,
            .e63_out      = dp.e63,
            .e68_out      = e68_rom[e68_addr(ir_before)],
            .e69_out      = dp.e69,
            .e70_out      = dp.e70,
            .e71_out      = dp.e71,
            .e74_out      = dp.e74,
            .e80_out      = e80_d,
            .e82_out      = e82_d,
            .e87_out      = e87_d,
            .e102_out     = e102_rom[e102_addr(UW_BUT_BITS(uw))],
            .e107_out     = prb_e107,
        });
    }

    /* After a service the IR reads 100000 until the next fetch (K2-3).
     * cpu->ir keeps the last instruction for the debugger. */
    if (UW_BUT_SERVICE(uw) && !cpu->halted)
        cpu->serv_ir = 1;

    cpu->mpc = next_mpc;
    return cycle_ns;
}


/* One instruction, that is micro-cycles until MPC 000. */

uint32_t kd11ea_step(KD11EA *cpu) {
    uint32_t total_ns = 0;
    int cycles = 0;
    int max_cycles = 1000;

    do {
        uint32_t ns = kd11ea_ustep(cpu);
        if (ns == 0)
            return 0;  /* halted */
        total_ns += ns;
        cycles++;
    } while (cpu->mpc != 0 && cycles < max_cycles);

    if (cycles >= max_cycles) {
        trace("WARNING: instruction did not complete in %d micro-cycles\n", max_cycles);
        return 0;
    }
    return total_ns;
}


void kd11ea_reset(KD11EA *cpu) {
    memset(cpu, 0, sizeof(*cpu));

    /* The 85S68 scratchpad holds random values at power-on. They are drawn
     * from the host name, so that each host always gets the same ones. The
     * M9301-YF needs an SP that points into memory. */
    {
        char host[256];
        uint32_t seed = 0x11340034;  /* fallback */
        if (gethostname(host, sizeof(host)) == 0)
            for (const char *p = host; *p; p++)
                seed = seed * 31 + (unsigned char)*p;
        srand(seed);
    }
    for (int i = 0; i < 16; i++)
        cpu->sp[i] = (uint16_t)(rand() & 0x7FFF);

    cpu->mpc = 0;
    cpu->psw = 0x00E0;    /* kernel, priority 7 */
    cpu->sp[1] = 0;
    int_init(&cpu->intc);
    dbg_init(&cpu->dbg);
}
