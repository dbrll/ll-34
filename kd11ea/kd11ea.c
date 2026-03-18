#define _DEFAULT_SOURCE
/*
 * kd11ea.c - KD11-EA (PDP-11/34A) circuit-level emulator
 *
 * Executes the real dumped 512x48-bit microcode ROM, one micro-word
 * per cycle.  All combinatorial ROMs are modeled from their truth
 * tables, reconstituted from the original DEC schematics.
 *
 * Data path reference: KD11-E Maintenance Manual, Chapter 4
 * Flow diagrams: KD11-E Flow Diagrams, sheets 1-26
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
#include "../trace.h"
#include "../probe/probe.h"

/* BYTE H = IR[15] AND NOT(IR[14] AND IR[13]) -- excludes SUB (16xxxx) */
#define IS_BYTE_INSTR(ir) (((ir) & 0x8000) && (((ir) >> 13) & 3) != 3)

/* AMUX: post-ALU mux, feeds SSMUX (E22/E27, 74S157, K1-6/K1-7) */
static uint16_t amux_select(KD11EA *cpu, uint8_t amux_l, uint16_t alu_result) {
    uint8_t sel = ~amux_l & 3;
    switch (sel) {
    case 0: return cpu->unibus_data;  /* UNIBUS data (E16/E21) */
    case 1: return 0;                 /* CONSTANTS (float HIGH when no SERVICE) */
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
    return amux_data;  /* straight through */
}

/* BMUX: ALU B-input select (E38/E39, 74S153, K1-3/K1-4) */
static uint16_t bmux_select(KD11EA *cpu, uint8_t bleg) {
    switch (bleg) {
    case BLEG_BREG:    return cpu->b_reg;
    case BLEG_BXREG:   return cpu->bx_reg;
    case BLEG_PLUS16:  return 14;            /* adjusted -2 for counter timing */
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
        if (!UW_FORCE_RSV1_L(uw))  /* E103 bit 40: active-low, forces RS bit0=1 */
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
 * physical SHIFT_L = logical right. We store MSB=bit15, so we invert. */
static void breg_update(uint16_t *reg, uint8_t mode, uint16_t load_data,
                         uint8_t shift_in) {
    switch (mode) {
    case BMODE_HOLD:    break;
    case BMODE_SHIFT_R: /* physical SHIFT_R = logical left shift (×2) */
        *reg = (*reg << 1) | (shift_in & 1); break;
    case BMODE_SHIFT_L: /* physical SHIFT_L = logical right shift (÷2) */
        *reg = (*reg >> 1) | ((uint16_t)shift_in << 15); break;
    case BMODE_LOAD:    *reg = load_data; break;
    }
}

/* MPC next-address: NEXT field OR'd with branch signals via E102 */
static uint16_t compute_next_mpc(KD11EA *cpu, uint64_t uw, uint8_t count_at_start, uint16_t bx_at_start, uint16_t sp_out) {
    uint16_t next = uw_next_mpc(uw);
    uint8_t misc = UW_MISC(uw);

    uint8_t but_bits = UW_BUT_BITS(uw);
    uint8_t e102_a = e102_addr(but_bits);
    uint8_t e102_d = e102_rom[e102_a];

    /* CC_N/CC_Z are combinatorial from ALU result, not from PSW register */
    if (E102_BUT_ALU_OUT_MPC7(e102_d))
        next |= (cpu->alu_out ? 1 : 0) << 7;  /* TODO: which ALU output bit? */
    if (E102_BUT_CC_N_MPC6(e102_d))
        next |= ((cpu->alu_out >> 15) & 1) << 6;  /* N: ALU result bit 15 */
    if (E102_BUT_BXREG01_MPC5(e102_d))
        next |= ((bx_at_start >> 1) & 1) << 5;
    if (E102_BUT_BXREG00_MPC4(e102_d))
        next |= (bx_at_start & 1) << 4;
    if (E102_BUT_SP15_MPC3(e102_d))
        next |= ((sp_out >> 15) & 1) << 3;  /* Scratchpad output bit 15 (sign) */
    if (E102_BUT_COUNT05_MPC2(e102_d))
        next |= ((count_at_start >> 5) & 1) << 2;
    if (E102_BUT_CC_Z_MPC1(e102_d))
        next |= ((cpu->alu_out == 0) ? 1 : 0) << 1;  /* Z: ALU result == 0 */
    if (E102_BUT_IR09_MPC0(e102_d))
        next |= ((cpu->ir >> 9) & 1);

    /* BUT SERVICE (K2-3): E52 priority-encodes trap conditions,
     * E51 decodes to vector bits. E3 forces MPC bit 0 -> MPC 001. */
    if (UW_BUT_SERVICE(uw)) {
        uint16_t e52_a = e52_addr(
            cpu->tbit_trap,                     /* A0: TBIT AND ENAB_TBIT */
            0,                                  /* A1: STOV (TODO: E24 flip-flop) */
            cpu->bus_error,                      /* A2: BE (bus error / NXM) */
            0,                                  /* A3: PE (TODO: parity error) */
            cpu->mmu_kte,                           /* A4: KTE (MMU abort) */
            0,                                  /* A5: PFAIL (TODO: power fail) */
            cpu->ir_code,                       /* A6-A8: IR CODE[2:0] from E53 */
            0);                                 /* A9: SVC BR PFAIL (backplane) */
        uint8_t e52_d = e52_rom[e52_a];

        if (e52_d != E52_CODE_NONE) {
            /* Trap condition detected. Decode via E51. */
            uint8_t e51_a = e52_to_e51_addr(e52_d);
            uint8_t e51_d = e51_rom[e51_a];

            /* E3 (7400 NAND gate, K2-3): generates MPC 00 L when trap
             * present. This ORs bit 0 into the MPC (wire-OR bus).
             * At MPC 000, NEXT=016, so result = 016|001 = 017. */
            next |= 1;

            /* Store vector and service signals for the trap routine.
             * E51 encodes vectors on 3 bits (C4:C3:C2 → bits [4:2]).
             * KTE (vec 250) exceeds this range - on real hardware, the
             * KT11 option card overrides the vector via additional wiring. */
            if (e52_d == E52_CODE_KTE)
                cpu->service_vector = 0250;     /* KT11 MMU trap vector */
            else
                cpu->service_vector = E51_VECTOR(e51_d);
            cpu->service_trap = 1;

            /* Clear the latched trap conditions that were just serviced.
             * On real hardware, STOV SERV clears the stack overflow FF,
             * and the trap service sequence clears BE, ir_code, etc. */
            cpu->bus_error = 0;
            cpu->tbit_trap = 0;
            cpu->mmu_kte = 0;

            trace("  BUT SERVICE: code=%X vec=%03o%s%s",
                  e52_d, cpu->service_vector,
                  E51_PFAIL_SERV(e51_d) ? " PFAIL_SERV" : "",
                  E51_STOV_SERV(e51_d) ? " STOV_SERV" : "");
        } else if (cpu->halt_rqst) {
            /* HALT detected (E53 HALT RQST, not through E52 priority).
             * On real hardware, drives RUN flip-flop low. */
            cpu->halted = 1;
            trace("  BUT SERVICE: HALT");
        }
        /* else: no trap, ENAB_GRANTS allows bus interrupts (not yet modeled) */
    }

    /* BUT DEST: C switch, not E69/E70 lookup (E69 inactive during BUT_DEST,
     * TM-001 table has errors, gen_e69_e70.py has suspected O0/O1 swap).
     * IR[5:3] -> MPC[2:0], instruction type bits -> MPC[4:3]. */
    if (misc == MISC_BUT_DEST) {
        next |= (cpu->ir >> 3) & 7;  /* Dest mode bits IR[5:3] -> MPC[2:0] */

        /* E88 latch replays E69 outputs (MOV_L, MPC[4:3]) during BUT_DEST */
        uint8_t op = (cpu->ir >> 12) & 0xF;
        if (op >= 1 && op <= 6 && op != 0) {
            /* Word DOP: op 01-06 */
            switch (op) {
            case 0x1:  /* MOV */
                next |= 0010;  /* bit3 */
                break;
            case 0x2:  /* CMP */
            case 0x3:  /* BIT */
                next |= 0020;  /* bit4 (NONMOD) */
                break;
            case 0x4:  /* BIC */
            case 0x5:  /* BIS */
            case 0x6:  /* ADD */
                next |= 0030;  /* bits 4,3 (MOD) */
                break;
            }
        } else if (op >= 0x9 && op <= 0xE) {
            /* Byte DOP: op 11-16 (octal), i.e. 0x9-0xE */
            switch (op) {
            case 0x9:  /* MOVB */
                next |= 0010;  /* bit3 */
                break;
            case 0xA:  /* CMPB */
            case 0xB:  /* BITB */
                next |= 0020;  /* bit4 (NONMOD) */
                break;
            case 0xC:  /* BICB */
            case 0xD:  /* BISB */
                next |= 0030;  /* bits 4,3 (MOD) */
                break;
            case 0xE:  /* SUB */
                next |= 0030;  /* bits 4,3 (MOD+SUB same path) */
                break;
            }
        }
    }

    return next & 0x1FF;
}

/* IR Decode: wire-OR of E59/E60/E63/E69/E70/E71/E74 (K2-5, K2-6).
 * OC bit=0 -> MPC bit=1. Trap dispatch is in compute_next_mpc(). */
static uint16_t ir_decode_entry(uint16_t ir, uint16_t psw) {
    uint16_t mpc = 0;
    uint8_t ir1412 = (ir >> 12) & 7;   /* IR[14:12] */
    uint8_t ir15 = (ir >> 15) & 1;

    /* E69/E70: DOP source fetch. E69->MPC[4:3]+SRC_H, E70->MPC[7:5] */
    {
        uint8_t a69 = e69_addr(ir, /*but_dest_l=*/1, /*ir_decode_h=*/1);
        uint8_t d69 = e69_rom[a69];
        mpc |= (!(d69 & 0x01)) << 4;       /* MPC04 */
        mpc |= (!(d69 & 0x02)) << 3;       /* MPC03 */
        if (!(d69 & 0x08))                  /* SRC H: gate src_mode onto MPC[2:0] */
            mpc |= (ir >> 9) & 7;           /* IR[11:9] → MPC[2:0] via 74H01 */

        uint16_t a70 = e70_addr(ir, /*but_dest_l=*/1, /*ir_decode_h=*/1);
        uint8_t d70 = e70_rom[a70];
        mpc |= (!(d70 & 0x02)) << 7;       /* MPC07 */
        mpc |= (!(d70 & 0x04)) << 6;       /* MPC06 */
        mpc |= (!(d70 & 0x08)) << 5;       /* MPC05 */
    }

    /* E60: SOP/branch/operate MPC[6:3], enabled when IR[14:12]=000 */
    if (ir1412 == 0) {
        uint8_t a60 = e59_e60_addr(ir);
        uint8_t d60 = e60_rom[a60];
        mpc |= (!(d60 & 0x01)) << 6;       /* MPC06 */
        mpc |= (!(d60 & 0x02)) << 5;       /* MPC05 */
        mpc |= (!(d60 & 0x04)) << 4;       /* MPC04 */
        mpc |= (!(d60 & 0x08)) << 3;       /* MPC03 */
    }

    /* E59: DEST gate (O3) -> gates IR[5:3] onto MPC[2:0] */
    if (ir1412 == 0) {
        uint8_t a59 = e59_e60_addr(ir);
        uint8_t d59 = e59_rom[a59];
        if (!(d59 & 0x08))                  /* DEST gate (E59 O3) */
            mpc |= (ir >> 3) & 7;           /* IR[5:3] → MPC[2:0] via 74H01 */
    }

    /* E63: operates MPC[3:0] (HALT, WAIT, RTI, BPT, IOT, RTS, CC, JMP, SWAB) */
    if (ir1412 == 0 && ((ir >> 8) & 0xF) == 0 && !ir15) {
        uint8_t a63 = e63_addr(ir);
        uint8_t d63 = e63_rom[a63];
        mpc |= (!(d63 & 0x01));             /* MPC00 */
        mpc |= (!(d63 & 0x02)) << 1;       /* MPC01 */
        mpc |= (!(d63 & 0x04)) << 2;       /* MPC02 */
        mpc |= (!(d63 & 0x08)) << 3;       /* MPC03 (wire-OR with E60) */
    }

    /* E74: EIS decode (IR15=0 only, active when IR[14:12]=111) */
    if (!ir15) {
        uint8_t a74 = e74_addr(ir, /*ir_decode_h=*/1);
        uint8_t d74 = e74_rom[a74];
        /* O0=IR CODE 00 L, O1-O5=MPC[7:3] L, O6=DEST L */
        mpc |= (!(d74 & 0x02)) << 7;       /* O1: MPC07 */
        mpc |= (!(d74 & 0x04)) << 6;       /* O2: MPC06 */
        mpc |= (!(d74 & 0x08)) << 5;       /* O3: MPC05 */
        mpc |= (!(d74 & 0x10)) << 4;       /* O4: MPC04 */
        mpc |= (!(d74 & 0x20)) << 3;       /* O5: MPC03 */
        /* O6 (bit 6) = DEST gate for EIS (gates dst_mode onto MPC[2:0]) */
        if (!(d74 & 0x40))
            mpc |= (ir >> 3) & 7;           /* IR[5:3] → MPC[2:0] */
    }

    /* E71: branch condition, MPC07 L pulled low when taken */
    if (ir1412 == 0 && !((ir >> 11) & 1)) {
        uint8_t a71 = e71_addr(ir, psw);
        uint8_t d71 = e71_rom[a71];
        mpc |= (!(d71 & 0x01)) << 7;       /* MPC07 (branch taken → 0200) */
    }

    /* JSR: handled by E60+E59. EMT/TRAP/BPT/IOT/illegal/HALT: mpc=0 here,
     * E53 sets IR CODE or HALT RQST, BUT SERVICE overrides to MPC 001. */

    return mpc & 0x1FF;
}


/* Main micro-cycle execution */

uint32_t kd11ea_ustep(KD11EA *cpu) {
    if (cpu->halted)
        return 0;

    /* WAIT: loop until interrupt (flow diagram sheet 17) */
    if (cpu->waiting) {
        uint8_t cpu_pri = (cpu->psw >> 5) & 7;
        if (int_pending(&cpu->intc, cpu_pri) >= 0) {
            cpu->waiting = 0;
            /* Fall through to interrupt handler below */
        } else {
            cpu->ns_elapsed += CYCLE_SHORT_NS;
            return CYCLE_SHORT_NS;
        }
    }

    /* UNIBUS interrupt check at MPC 000 (between instructions) */
    if (cpu->mpc == 0 && !cpu->ir_valid) {
        uint8_t cpu_pri = (cpu->psw >> 5) & 7;
        if (int_pending(&cpu->intc, cpu_pri) >= 0) {
            IntRequest irq = int_ack(&cpu->intc, cpu_pri);
            trace("IRQ: vec=%03o pri=%d PC=%06o PSW=%06o SP=%06o R0=%06o R1=%06o\n",
                    irq.vector, irq.priority, PC(cpu), cpu->psw, cpu->sp[9],
                    REG(cpu,0), REG(cpu,1));
            /* Push PSW then PC via kernel-mode MMU translation */
            uint16_t sp = cpu->sp[9];  /* slot 9 = R6/SP (kernel) */
            sp -= 2;
            {
                uint32_t pba = sp;
                if (cpu->mmu.sr0 & SR0_ENABLE)
                    mmu_translate(&cpu->mmu, sp, 0, 1, PC(cpu), &pba);
                bus_write(cpu->bus, pba, cpu->psw, BUS_DATO);
            }
            sp -= 2;
            {
                uint32_t pba = sp;
                if (cpu->mmu.sr0 & SR0_ENABLE)
                    mmu_translate(&cpu->mmu, sp, 0, 1, PC(cpu), &pba);
                bus_write(cpu->bus, pba, PC(cpu), BUS_DATO);
            }
            cpu->sp[9] = sp;

            /* Load new PC and PSW from vector */
            uint16_t new_pc, new_psw;
            {
                uint32_t pba_pc = irq.vector;
                uint32_t pba_psw = irq.vector + 2;
                if (cpu->mmu.sr0 & SR0_ENABLE) {
                    mmu_translate(&cpu->mmu, irq.vector, 0, 0, PC(cpu), &pba_pc);
                    mmu_translate(&cpu->mmu, (irq.vector + 2) & 0xFFFF, 0, 0, PC(cpu), &pba_psw);
                }
                bus_read(cpu->bus, pba_pc, &new_pc);
                bus_read(cpu->bus, pba_psw, &new_psw);
            }
            trace("  -> ISR PC=%06o PSW=%06o newSP=%06o\n",
                    new_pc, new_psw, sp);
            PC(cpu) = new_pc;
            cpu->psw = new_psw;

            dbg_record(&cpu->dbg, &(DbgEntry){
                .mpc          = 0x1FF,  /* marker: IRQ delivery */
                .next_mpc     = 0,
                .a_leg        = irq.vector,
                .b_leg        = new_pc,
                .alu_out      = new_psw,
                .ba           = cpu->sp[9],
                .ir           = cpu->ir,
                .psw          = cpu->psw,
                .pc           = PC(cpu),
                .sp           = cpu->sp[9],
            });

            /* ~2 micro-cycles for interrupt overhead */
            cpu->ns_elapsed += 480;
            return 480;
        }
    }

    uint64_t uw = ucode_rom[cpu->mpc];
    int nxm_abort = 0;  /* NXM: abort instruction, force MPC->000 */

    trace("MPC=%03o  uw=%012llX", cpu->mpc, (unsigned long long)uw);

    /* Decode expansion ROMs */
    uint8_t alu_field = UW_ALU_FUNC(uw);       /* E99 ROM bits [23:19] */
    uint8_t e82_a = e82_addr(alu_field);
    uint8_t e82_d = e82_rom[e82_a];
    uint8_t alu_s    = E82_ALU_S(e82_d);        /* 74S181 S3-S0 */
    uint8_t alu_mode = E82_MODE(e82_d);         /* 74S181 M */
    uint8_t alu_cin  = !E82_CIN_L(e82_d);       /* 74S181 Cn (active-low) */
    uint8_t bleg     = E82_BLEG(e82_d);         /* BMUX select */

    uint8_t bbx_field = UW_BBXOVXDBE(uw);
    uint8_t e87_a = e87_addr(bbx_field);
    uint8_t e87_d = e87_rom[e87_a];
    uint8_t b_mode  = E87_B_MODE(e87_d);
    uint8_t bx_mode = E87_BX_MODE(e87_d);

    /* Probe: ROM output accumulators (0xFF = not evaluated) */
    uint8_t prb_e53 = 0xFF, prb_e54 = 0xFF;
    uint8_t prb_e59 = 0xFF, prb_e60 = 0xFF;
    uint8_t prb_e61 = 0xFF, prb_e63 = 0xFF;
    uint8_t prb_e68 = 0xFF, prb_e69 = 0xFF, prb_e70 = 0xFF;
    uint8_t prb_e71 = 0xFF, prb_e74 = 0xFF;
    uint8_t prb_e83 = 0xFF, prb_e107 = 0xFF;
    uint8_t prb_e51 = 0xFF, prb_e52 = 0xFF;

    /* Scratchpad read (ALU A-input). DST resolved after LOAD_BA. */
    uint8_t spa_src = spa_resolve(cpu, UW_SPA_SRC_SEL(uw), uw);
    uint8_t spa_dst_sel = UW_SPA_DST_SEL(uw);
    uint8_t aux_control = UW_AUX_CONTROL(uw);

    /* AUX CONTROL: E83 (DOP) or E61/E62 (SOP) override E99 ALU function.
     * Note: TM-001 calls E83 "E81" but E81 is the 7437 buffer on the board. */
    if (aux_control) {
        uint8_t op = (cpu->ir >> 12) & 0xF;

        bleg = BLEG_BREG;  /* B register carries the operand */

        if (op == 0x0 || op == 0x8) {
            /* SOP: E61 provides FUNC CODE, E62 provides shift control */
            uint8_t cbit = cpu->psw & 1;
            uint8_t nbit = (cpu->psw >> 3) & 1;
            uint8_t fc = e61_rom[e61_addr(cpu->ir, cbit, nbit)];

            /* E61 -> E71 latch -> E82. Build alu_field for e82_addr() */
            uint8_t e82_field = (0) |                    /* bit0 = E106 = 0 */
                                (((fc >> 3) & 1) << 1) | /* bit1 = FC03 */
                                (((fc >> 2) & 1) << 2) | /* bit2 = FC02 */
                                (((fc >> 1) & 1) << 3) | /* bit3 = FC01 */
                                ((fc & 1) << 4);         /* bit4 = FC00 */
            uint8_t e82_a = e82_addr(e82_field);
            uint8_t e82_d = e82_rom[e82_a];

            alu_s    = E82_ALU_S(e82_d);
            alu_mode = E82_MODE(e82_d);
            alu_cin  = !E82_CIN_L(e82_d);
            bleg     = E82_BLEG(e82_d);
        } else {
            /* DOP: E83 -> E81 -> ALU (bypasses E82). dop_alu[] from TM-001 Table 4-9.
             * SUB: 74S181 only has A-B, not B-A. Swap A/B legs so A=dest, B=source. */
            if (op == 0xE) {
                /* SUB: A=dest(sp[4]), B=source(sp[5]) → A-B = dest-src */
                spa_src = 4;
            } else {
                spa_src = 5;
            }
            uint8_t dop_d = dop_alu[op];
            alu_s    = DOP_ALU_S(dop_d);
            alu_mode = DOP_ALU_MODE(dop_d);
            alu_cin  = DOP_ALU_CIN(dop_d);
        }
    }

    uint16_t a_leg = cpu->sp[spa_src & 0xF];  /* ALU A-input */
    uint16_t b_leg = bmux_select(cpu, bleg);

    /* SUB: patch b_leg to source (sp[5]) since BMUX can't read scratchpad */
    if (aux_control && ((cpu->ir >> 12) & 0xF) == 0xE) {
        b_leg = cpu->sp[5];  /* source operand (sp[5] = R12 temp) */
    }

    /* DISABLE MSYN+1 L (E37, K1-3): byte instructions force BMUX +1 to 0
     * (CIN still gives +1). Only for PDP-11 regs (SPA 8-15), not internal
     * temps. SP/PC always +2. Deferred modes (3,5) always +2. */
    if (bleg == BLEG_PLUS1 && IS_BYTE_INSTR(cpu->ir)) {
        uint8_t dst_mode = (cpu->ir >> 3) & 7;
        uint8_t src_mode = (cpu->ir >> 9) & 7;
        int is_deferred = 0;
        if (UW_SPA_SRC_SEL(uw) == SPA_RD)
            is_deferred = (dst_mode == 3 || dst_mode == 5);
        else if (UW_SPA_SRC_SEL(uw) == SPA_RS)
            is_deferred = (src_mode == 3 || src_mode == 5);
        if (!is_deferred && (spa_src & 8) && spa_src != 9 && spa_src != 8) {
            b_leg = 0;
        }
    }

    /* ALU: 4x 74S181 + 74S182 CLA */
    uint8_t cout = 0;
    uint16_t alu_result = alu_74s181(a_leg, b_leg, alu_s, alu_mode, alu_cin, &cout);

    cpu->alu_out = alu_result;
    cpu->alu_cout = cout;

    /* Diagnostic: trace INC at passc+020210 (PC=020214 after 2-word fetch) */
    trace("  ALU: A=%06o(%s[%o]) B=%06o S=%X M=%d Cin=%d -> %06o Cout=%d",
          a_leg,
          (const char*[]){"ROM","RS","RD","RBA"}[UW_SPA_SRC_SEL(uw)],
          spa_src,
          b_leg, alu_s, alu_mode, alu_cin, alu_result, cout);

    /* E62 ROT/SHIFT ROM, gated by E84. Substitutes shifted value into alu_result. */
    uint8_t e62_d = 0xF;  /* default: all float (no effect) */
    uint8_t e62_b_mode = BMODE_HOLD;

    if (aux_control && ((cpu->ir >> 12) & 7) == 0) {  /* E84 gate */
        uint8_t sign_bit = (cpu->ir & 0x8000) ? 7 : 15;  /* byte/word */
        uint8_t cc_n = (alu_result >> sign_bit) & 1;
        e62_d = e62_rom[e62_addr(cpu->ir, cpu->b_reg & 1,
                                 cpu->psw & 1, cc_n)];
        e62_b_mode = E62_B_MODE(e62_d);

        /* E62 overrides E87 HOLD via wire-OR */
        b_mode |= e62_b_mode;

        /* Apply same logical shift to alu_result (same inversion as breg_update) */
        if (e62_b_mode == BMODE_SHIFT_R) {
            /* Physical SHIFT_R = logical left */
            uint8_t serial = E62_SERIAL_SHIFT(e62_d);
            alu_result = (alu_result << 1) | (serial & 1);
        } else if (e62_b_mode == BMODE_SHIFT_L) {
            /* Physical SHIFT_L = logical right.
             * Byte: serial -> bit 7, upper byte held (E14/E15 in HOLD). */
            uint8_t serial = E62_SERIAL_SHIFT(e62_d);
            if (sign_bit == 7) {
                /* Byte: shift only low byte, serial → bit 7 */
                alu_result = (alu_result & 0xFF00) |
                             ((alu_result & 0xFF) >> 1) |
                             ((uint16_t)(serial & 1) << 7);
            } else {
                /* Word: shift full 16 bits, serial → bit 15 */
                alu_result = (alu_result >> 1) | ((uint16_t)serial << 15);
            }
        }
    }

    /* VBA register (E49/E40/E47, K1-6): loaded from scratchpad read bus
     * (not ALU output), so it captures the pre-increment value. */
    uint8_t load_ba = UW_LOAD_BA(uw);
    if (load_ba) {
        uint16_t vba = a_leg & 0xFFFF;  /* Scratchpad read → VBA latch (K1-6) */

        /* KT11 MMU translation (3x 74S283 adders, K1-6/K1-7) */
        int mmu_mode = (cpu->psw >> 14) & 3;  /* current mode from PSW[15:14] */
        if (UW_FORCE_KERNEL(uw))
            mmu_mode = 0;                      /* force kernel (trap vectors etc.) */
        else if (!UW_PREV_MODE_L(uw))
            mmu_mode = (cpu->psw >> 12) & 3;   /* previous mode (MFPI/MTPI) */

        uint32_t pba;
        if (mmu_translate(&cpu->mmu, vba, mmu_mode,
                          (UW_BUS_CTL(uw) >= BUS_DATO) ? 1 : 0,
                          PC(cpu), &pba) < 0) {
            cpu->mmu_abort = 1;
            cpu->ba = 0;
            trace("  BA <- %06o (VBA=%06o MMU ABORT)", cpu->ba, vba);
        } else {
            cpu->ba = pba;
            trace("  BA <- %06o (VBA=%06o)", cpu->ba, vba);
        }
    }

    /* Bus operations (before AMUX so UDATA is fresh) */
    uint8_t buf_dat_tran = UW_BUF_DAT_TRAN(uw);
    uint8_t bus_ctl = UW_BUS_CTL(uw);

    /* DBE gating (K2-8): DATOB downgraded to DATO for word instructions */
    if (bus_ctl == BUS_DATOB && !IS_BYTE_INSTR(cpu->ir)) {
        bus_ctl = BUS_DATO;
    }

    /* E80 Byte/Write Control ROM (K2-8): decodes SS + context -> SWAP, SEX */
    uint8_t amux_l = UW_AMUX_L(uw);
    uint16_t amux_out = amux_select(cpu, amux_l, alu_result);  /* E22/E27 (74S157) */
    uint8_t ssmux_ctl = UW_SSMUX(uw);
    uint8_t ss01 = (ssmux_ctl >> 1) & 1;
    uint8_t ss00 = ssmux_ctl & 1;
    uint8_t byte_h = IS_BYTE_INSTR(cpu->ir) ? 1 : 0;
    uint8_t vba00_bdt = (cpu->ba & 1) & buf_dat_tran;
    /* MOV L from E69 latch: 0 = MOV/MOVB, 1 = other */
    uint8_t mov_l = (((cpu->ir >> 12) & 7) == 1) ? 0 : 1;
    uint8_t e80_a = e80_addr(ss01, ss00, buf_dat_tran, aux_control,
                              byte_h, vba00_bdt, mov_l);
    uint8_t e80_d = e80_rom[e80_a];
    uint8_t e80_swap = E80_SWAP(e80_d);
    uint8_t e80_sex  = E80_SEX(e80_d);

    uint16_t ss_out = ssmux_select(amux_out, e80_swap, e80_sex);

    if (buf_dat_tran && cpu->mmu_abort) {
        cpu->mmu_abort = 0;
        cpu->mmu_kte = 1;
        nxm_abort = 1;  /* terminate instruction, like NXM */
        buf_dat_tran = 0;
        trace("  BUS CYCLE CANCELLED (MMU ABORT)");
    }

    if (buf_dat_tran) {
        int bus_rc = 0;
        switch (bus_ctl) {
        case BUS_DATI:
        case BUS_DATIP:
            bus_rc = bus_read(cpu->bus, cpu->ba, &cpu->unibus_data);
            trace("  BUS READ @%06o = %06o", cpu->ba, cpu->unibus_data);
            /* Recompute AMUX/SSMUX now that unibus_data is updated */
            amux_out = amux_select(cpu, amux_l, alu_result);
            ss_out   = ssmux_select(amux_out, e80_swap, e80_sex);
            break;
        case BUS_DATO:
        case BUS_DATOB:
            bus_rc = bus_write(cpu->bus, cpu->ba, ss_out, bus_ctl);
            trace("  BUS WRITE @%06o <- %06o", cpu->ba, ss_out);
            break;
        }
        if (bus_rc < 0) {
            cpu->bus_error = 1;
            /* Abort instruction on NXM (Handbook S2.6), force MPC->000.
             * Without this, MFPI pushes garbage on NXM, corrupts V6 nofault. */
            nxm_abort = 1;
            trace("  NXM PC=%06o IR=%06o BA=%06o", PC(cpu), cpu->ir, (unsigned)cpu->ba);
        }
    }

    /* B/BX register update */
    /* SHIFT MUX (E119, 74153, K1-10):
     *  F0 (SHIFT IN B):   C0=BREG[15], C1=SERIAL SHIFT, C2=GND, C3=BX[15]
     *  F1 (SHIFT IN BX):  C0=ALU COUT, C1=OVX,          C2=+3V, C3=GND
     *  BX DSR pin = B[0] (direct wire, not through E119). */
    uint8_t shift_mux_l = E87_SHIFT_MUX_L(e87_d);
    uint8_t shift_mux = shift_mux_l;  /* direct to E119 (no inversion) */
    uint8_t shift_in_b, shift_in_bx;

    /* Save before breg_update. BUT reads BX at start of cycle. */
    uint8_t b_bit0 = cpu->b_reg & 1;
    uint16_t bx_before = cpu->bx_reg;

    if (e62_b_mode != BMODE_HOLD) {
        /* E62 active: SERIAL SHIFT overrides E119 for B */
        shift_in_b = E62_SERIAL_SHIFT(e62_d);
        shift_in_bx = shift_in_b;  /* SOP shifts don't use BX */
    } else {
        /* E119 F0 (B): */
        switch (shift_mux) {
        default:
        case 0: shift_in_b = (cpu->b_reg >> 15) & 1; break;  /* BREG[15] */
        case 1: shift_in_b = E62_SERIAL_SHIFT(e62_d); break;
        case 2: shift_in_b = 0; break;
        case 3: shift_in_b = (cpu->bx_reg >> 15) & 1; break;
        }
        /* E119 F1 (BX): */
        switch (shift_mux) {
        default:
        case 0: shift_in_bx = cout; break;          /* ALU COUT */
        case 1: shift_in_bx = cpu->ovx; break;   /* OVX */
        case 2: shift_in_bx = 1; break;           /* +3V */
        case 3: shift_in_bx = 0; break;           /* GND */
        }
    }
    breg_update(&cpu->b_reg, b_mode, ss_out, shift_in_b);

    /* BX shift: SHIFT_L -> DSR = B[0], SHIFT_R -> DSL = E119 F1 */
    uint8_t bx_shift_in;
    if (bx_mode == BMODE_SHIFT_L)
        bx_shift_in = b_bit0;
    else
        bx_shift_in = shift_in_bx;
    breg_update(&cpu->bx_reg, bx_mode, ss_out, bx_shift_in);

    /* OVX sticky (E115 XOR, K1-10): set if BREG[15]^BREG[14], cleared at LOAD_IR */
    if (!E87_ENAB_OVX_L(e87_d)) {
        if (((cpu->b_reg >> 15) ^ (cpu->b_reg >> 14)) & 1)
            cpu->ovx = 1;
    }

    /* Misc control. Count saved before CLK_COUNT (E102 reads start-of-cycle). */
    uint8_t count_before = cpu->count;

    uint8_t misc = UW_MISC(uw);
    switch (misc) {
    case MISC_NOP:
        break;
    case MISC_LOAD_IR: {
        cpu->ir = cpu->unibus_data;
        cpu->ir_valid = 1;
        cpu->ovx = 0;  /* E100: LOAD_IR clears OVX */
        /* SR2 = fetch address (PC not yet incremented), frozen while SR0 abort set */
        if (!(cpu->mmu.sr0 & SR0_ABORT_MASK))
            cpu->mmu.sr2 = PC(cpu);

        /* E53 Trap Decode (K2-6): IR -> IR CODE + HALT RQST */
        uint8_t user_mode = (cpu->psw >> 15) & 1;
        uint8_t e53_d;
        if (e53_enabled(cpu->ir)) {
            uint8_t e53_a = e53_addr(cpu->ir, user_mode);
            e53_d = e53_rom[e53_a];
        } else {
            e53_d = 0xF;  /* OC disabled: float HIGH */
        }
        prb_e53 = e53_d;
        cpu->ir_code = e53_ir_code(e53_d);
        cpu->halt_rqst = !E53_HALT_RQST_L(e53_d);

        /* Wire-OR: E59 and E74 also contribute to IR CODE bus */
        if (((cpu->ir >> 12) & 7) == 0) {
            uint8_t e59_d = e59_rom[e59_e60_addr(cpu->ir)];
            prb_e59 = e59_d;
            cpu->ir_code |= e59_ir_code(e59_d);
        }
        if (!(cpu->ir & 0x8000)) {
            uint8_t e74_d = e74_rom[e74_addr(cpu->ir, 1)];
            prb_e74 = e74_d;
            if (!E74_IR_CODE_00_L(e74_d))
                cpu->ir_code |= 1;
        }

        /* JMP/JSR mode 0: illegal (E53 can't catch these, IR[6]=1 disables it).
         * Inject ir_code=1 -> vector 010. RT-11 relies on this. */
        {
            uint16_t ir = cpu->ir;
            uint8_t dst_mode = (ir >> 3) & 7;
            if (dst_mode == 0) {
                /* JMP: opcode 0001DD, i.e. IR[15:6] = 00_0000_01xx */
                uint8_t is_jmp = ((ir >> 6) & 0x3FF) == 0x001;  /* IR[15:6]=0000000001 */
                /* JSR: opcode 004RDD, i.e. IR[15:9] = 0000_100 */
                uint8_t is_jsr = ((ir >> 9) & 0x7F) == 0x04;
                if (is_jmp || is_jsr)
                    cpu->ir_code = 1;  /* ILLEGAL → E52 → vector 010 */
            }
        }

        /* E54 Reset/Trap Control (K2-6): ENAB TBIT (RTT suppresses) */
        uint8_t e54_a = e54_addr(cpu->ir, user_mode);
        uint8_t e54_d = e54_rom[e54_a];
        prb_e54 = e54_d;
        cpu->enab_tbit = E54_ENAB_TBIT(e54_d);

        /* E50 (7408): TBIT AND ENAB_TBIT -> E52 A0 */
        uint8_t tbit = (cpu->psw >> 4) & 1;
        cpu->tbit_trap = tbit & cpu->enab_tbit;

        trace("  IR <- %06o  ir_code=%o halt=%d enab_tbit=%d tbit_trap=%d",
              cpu->ir, cpu->ir_code, cpu->halt_rqst, cpu->enab_tbit, cpu->tbit_trap);
        break;
    }
    case MISC_LOAD_PSW: {
        uint16_t new_psw = cpu->unibus_data;
        if (UW_FORCE_KERNEL(uw)) {
            /* Trap/interrupt: insert old current mode into previous mode */
            new_psw = (new_psw & ~0x3000) | ((cpu->psw & 0xC000) >> 2);
        }
        cpu->psw = new_psw;
        trace("  PSW <- %06o", cpu->psw);
        break;
    }
    case MISC_LOAD_CC: {
        /* PSW MUX (E96, K1-1): AUX=0 -> SSMUX[3:0], AUX=1 -> CC logic */

        if (!aux_control) {
            /* PSW MUX selects SSMUX → PSW[3:0] = ss_out[3:0] */
            cpu->psw = (cpu->psw & 0xFFF0) | (ss_out & 0xF);
            trace("  LOAD CC (SSMUX): N=%d Z=%d V=%d C=%d  PSW=%06o",
                  (ss_out >> 3) & 1, (ss_out >> 2) & 1,
                  (ss_out >> 1) & 1, ss_out & 1, cpu->psw);
            break;
        }

        /* AUX=1: E68 -> CC_CODE, E108 byte mux, E107 -> V,C. N,Z from wiring. */

        uint8_t e68_val = e68_rom[e68_addr(cpu->ir)];  /* E68 (23-163A2, K2-6) */
        prb_e68 = e68_val;
        uint8_t cc_code = E68_CC_CODE(e68_val);
        /* BYTE MUX (E108, K1-10): byte_cc = BYTE_H OR (!E68_BYTE_L AND !IR[15]).
         * Handles SWAB (byte CC despite IR[15]=0) and SUB (word CC despite E68). */
        int is_byte = IS_BYTE_INSTR(cpu->ir) ||
                      (!E68_BYTE_L(e68_val) && !(cpu->ir & 0x8000));

        uint8_t sign_bit = is_byte ? 7 : 15;
        uint16_t z_mask = is_byte ? 0xFF : 0xFFFF;
        uint8_t cc_n = (alu_result >> sign_bit) & 1;
        uint8_t cc_z = ((alu_result & z_mask) == 0) ? 1 : 0;

        uint8_t r_msb = (alu_result >> sign_bit) & 1;
        uint8_t s_msb = (a_leg >> sign_bit) & 1;
        uint8_t d_msb = (b_leg >> sign_bit) & 1;
        uint8_t rot_cbit = E62_ROT_CBIT(e62_d);  /* ROT CBIT from E62 (→ E55 latch) */
        uint8_t psw_cbit = cpu->psw & 1;

        uint8_t e107_val = e107_rom[e107_addr(r_msb, s_msb, d_msb,  /* E107 (23-164A2, K1-10) */
                                              rot_cbit, psw_cbit, cc_code)];
        prb_e107 = e107_val;
        uint8_t cc_v = e107_val & 1;
        uint8_t cc_c = (e107_val >> 1) & 1;

        cpu->psw = (cpu->psw & 0xFFF0) |
                   (cc_n << 3) | (cc_z << 2) | (cc_v << 1) | cc_c;
        trace("  LOAD CC: N=%d Z=%d V=%d C=%d  PSW=%06o (cc_code=%d%s)",
              cc_n, cc_z, cc_v, cc_c, cpu->psw, cc_code,
              is_byte ? " byte" : "");
        break;
    }
    case MISC_BUT_DEST:
        /* Handled in next-MPC computation */
        break;
    case MISC_ENAB_STOV:
        /* TODO: compare SP against stack limit register */
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

    /* Scratchpad write, gated by ENAB GR L (E86 ROM, K1-10).
     * E86 suppresses write when BA >= 0x10 (outside register space). */
    uint8_t amux_sel = (~amux_l) & 3;
    uint8_t suppress_load_ir_psw = (misc == MISC_LOAD_IR || misc == MISC_LOAD_PSW);
    uint8_t is_dato = buf_dat_tran && (bus_ctl == BUS_DATO || bus_ctl == BUS_DATOB);
    uint8_t rba_out_of_range = (spa_dst_sel == SPA_RBA) && (cpu->ba >= 0x10);
    uint8_t rba_dato_suppress = (spa_dst_sel == SPA_RBA) && is_dato;
    uint8_t spm_write_en = !suppress_load_ir_psw &&
                           !rba_out_of_range &&
                           !rba_dato_suppress &&
                           ((amux_sel != 0) ||  /* ALU/PSW/const result */
                            (!is_dato && buf_dat_tran) || /* bus read data (not during DATO) */
                            load_ba);            /* address computation */
    if (spm_write_en && !nxm_abort) {
        uint16_t write_val = ss_out;
        /* E80 DISABLE UPPER: byte modify ops only write low byte */
        if (E80_DISABLE_UPPER(e80_d)) {
            /* Byte modify op: preserve high byte from register, write low only */
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

    /* --- Next MPC --- */
    uint16_t next_mpc = compute_next_mpc(cpu, uw, count_before, bx_before, a_leg);

    /* Diagnostic: trace MTPI (R1) MPC chain */

    /* NXM abort: force MPC 000 for BUT SERVICE */
    if (nxm_abort)
        next_mpc = 0;

    /* IR Decode and BUT SERVICE at MPC 000: service_trap overrides ir_decode */
    uint8_t  dbg_ir_decoded = 0;
    uint16_t dbg_ir_decode_mpc = 0;
    uint8_t  dbg_service = 0;

    /* BUT SERVICE: fires at MPC 000 regardless of ir_valid */
    if (cpu->mpc == 0 && cpu->service_trap) {
        cpu->service_trap = 0;
        dbg_service = 1;
        /* AMUX S0 H forces vector onto bus, B captures it (TM-001 S4.11.2) */
        cpu->b_reg = cpu->service_vector;
        trace("  BUT SERVICE override → MPC %03o (B←vec %03o)",
              next_mpc, cpu->service_vector);
        cpu->ir_valid = 0;
        cpu->ir_code = 0;
        cpu->halt_rqst = 0;
        cpu->bus_error = 0;
    } else if (cpu->mpc == 0 && cpu->ir_valid) {
        uint16_t ir_mpc = ir_decode_entry(cpu->ir, cpu->psw);
        trace("  IR DECODE -> MPC %03o", ir_mpc);
        cpu->ir_valid = 0;
        dbg_ir_decoded = 1;
        dbg_ir_decode_mpc = ir_mpc;

        if (cpu->ir == 0x0001) {
            /* WAIT: stay at MPC 000 until interrupt */
            cpu->waiting = 1;
            next_mpc = 0;
            trace("  WAIT: entering wait state (PC=%06o PSW=%06o)",
                  PC(cpu), cpu->psw);
        } else {
            next_mpc = ir_mpc;
        }

    }

    trace("  -> MPC %03o  [PC=%06o R0=%06o B=%06o BX=%06o sp4=%06o]\n",
          next_mpc, PC(cpu), REG(cpu,0), cpu->b_reg, cpu->bx_reg, cpu->sp[4]);

    /* Cycle duration (E106 delay line model) */
    uint32_t cycle_ns = clockgen_cycle_ns(buf_dat_tran, bus_ctl,
                                          UW_LONG_CYCLE_L(uw),
                                          cpu->bus->last_latency_ns);

    cpu->ns_elapsed += cycle_ns;

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

    /* Probe sampling */
    if (cpu->probe) {
        probe_sample(cpu->probe, &(ProbeSnapshot){
            .ns           = cpu->ns_elapsed,
            .mpc          = cpu->mpc,
            .next_mpc     = next_mpc,
            .a_leg        = a_leg,
            .b_leg        = b_leg,
            .alu_out      = alu_result,
            .ba           = cpu->ba,
            .unibus_data  = cpu->unibus_data,
            .ir           = cpu->ir,
            .psw          = cpu->psw,
            .b_reg        = cpu->b_reg,
            .bx_reg       = cpu->bx_reg,
            .pc           = PC(cpu),
            .sp           = cpu->sp[9],
            .r0           = REG(cpu, 0),
            .r1           = REG(cpu, 1),
            .alu_s        = alu_s,
            .alu_mode     = alu_mode,
            .alu_cin      = alu_cin,
            .alu_cout     = cout,
            .bus_op       = buf_dat_tran ? ((bus_ctl <= BUS_DATIP) ? 1 : 2) : 0,
            .long_cycle   = UW_LONG_CYCLE_L(uw) ? 0 : 1,
            .buf_dat_tran = buf_dat_tran,
            .ir_valid     = cpu->ir_valid,
            .ir_code      = cpu->ir_code,
            .service_trap = dbg_service,
            .bus_error    = cpu->bus_error,
            .halted       = cpu->halted,
            .enab_tbit    = cpu->enab_tbit,
            .cycle_ns     = cycle_ns,
            .ssyn_ns      = buf_dat_tran ? cpu->bus->last_latency_ns : 0,
            .eot_ns       = buf_dat_tran ? clockgen_eot_ns(bus_ctl) : 0,
            /* ROM raw outputs */
            .e51_out      = prb_e51,
            .e52_code     = prb_e52,
            .e53_out      = prb_e53,
            .e54_out      = prb_e54,
            .e59_out      = prb_e59,
            .e60_out      = prb_e60,
            .e61_out      = prb_e61,
            .e62_out      = e62_d,
            .e63_out      = prb_e63,
            .e68_out      = prb_e68,
            .e69_out      = prb_e69,
            .e70_out      = prb_e70,
            .e71_out      = prb_e71,
            .e74_out      = prb_e74,
            .e80_out      = e80_d,
            .e82_out      = e82_d,
            .e83_out      = prb_e83,
            .e87_out      = e87_d,
            .e102_out     = e102_rom[e102_addr(UW_BUT_BITS(uw))],
            .e107_out     = prb_e107,
        });
    }

    cpu->mpc = next_mpc;
    return cycle_ns;
}


/* Execute one full instruction (micro-cycles until MPC 000) */

uint32_t kd11ea_step(KD11EA *cpu) {
    uint32_t total_ns = 0;
    int cycles = 0;
    int max_cycles = 1000;  /* safety limit */

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

    /* Scratchpad (85S68): indeterminate at power-on.
     * Seed from hostname for deterministic per-machine values.
     * M9301-YF ODT needs SP pointing to valid RAM. */
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

    cpu->mpc = 0;          /* SERVICE entry */
    cpu->psw = 0x00E0;    /* priority 7, kernel mode */
    cpu->sp[1] = 0;       /* USP slot */
    int_init(&cpu->intc);
    dbg_init(&cpu->dbg);
}
