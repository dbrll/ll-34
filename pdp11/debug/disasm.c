/*
 * debug/disasm.c -- PDP-11 instruction disassembler for ll-34
 *
 * Written from scratch for the ll-34 project (MIT license).
 * Covers the PDP-11/34 instruction set: basic, EIS (MUL/DIV/ASH/ASHC),
 * and SOB/XOR/MARK/SPL/MFPI/MTPI/SXT/SWAB.
 * Does NOT cover FPP (FIS/FP11) or CIS.
 *
 * All output uses PDP-11 octal convention.
 */

#include <stdio.h>
#include <string.h>
#include "disasm.h"

/* Register names */
static const char *rname[8] = {
    "R0", "R1", "R2", "R3", "R4", "R5", "SP", "PC"
};

/* Format an operand (6-bit: mode[5:3] + reg[2:0]).
 * pc_next: address of the next word after the instruction/operand so far.
 * Returns bytes consumed by this operand (0 or 2). */
static int fmt_operand(uint16_t op6, uint16_t pc_next,
                       int (*read_word)(uint16_t, void *), void *ctx,
                       char *out, int maxlen)
{
    int mode = (op6 >> 3) & 7;
    int reg  = op6 & 7;
    int extra = 0;

    switch (mode) {
    case 0:  /* Register: Rn */
        snprintf(out, maxlen, "%s", rname[reg]);
        break;
    case 1:  /* Register deferred: (Rn) */
        snprintf(out, maxlen, "(%s)", rname[reg]);
        break;
    case 2:  /* Autoincrement: (Rn)+ or #imm if PC */
        if (reg == 7) {
            int w = read_word(pc_next, ctx);
            if (w < 0) { snprintf(out, maxlen, "#ERR"); return 2; }
            snprintf(out, maxlen, "#%o", (unsigned)w);
            extra = 2;
        } else {
            snprintf(out, maxlen, "(%s)+", rname[reg]);
        }
        break;
    case 3:  /* Autoincrement deferred: @(Rn)+ or @#abs if PC */
        if (reg == 7) {
            int w = read_word(pc_next, ctx);
            if (w < 0) { snprintf(out, maxlen, "@#ERR"); return 2; }
            snprintf(out, maxlen, "@#%o", (unsigned)w);
            extra = 2;
        } else {
            snprintf(out, maxlen, "@(%s)+", rname[reg]);
        }
        break;
    case 4:  /* Autodecrement: -(Rn) */
        snprintf(out, maxlen, "-(%s)", rname[reg]);
        break;
    case 5:  /* Autodecrement deferred: @-(Rn) */
        snprintf(out, maxlen, "@-(%s)", rname[reg]);
        break;
    case 6:  /* Index: X(Rn) or addr if PC */
        {
            int w = read_word(pc_next, ctx);
            if (w < 0) { snprintf(out, maxlen, "ERR(ERR)"); return 2; }
            if (reg == 7) {
                uint16_t target = (uint16_t)(pc_next + 2 + (uint16_t)w);
                snprintf(out, maxlen, "%o", (unsigned)target);
            } else {
                snprintf(out, maxlen, "%o(%s)", (unsigned)(uint16_t)w, rname[reg]);
            }
            extra = 2;
        }
        break;
    case 7:  /* Index deferred: @X(Rn) or @addr if PC */
        {
            int w = read_word(pc_next, ctx);
            if (w < 0) { snprintf(out, maxlen, "@ERR(ERR)"); return 2; }
            if (reg == 7) {
                uint16_t target = (uint16_t)(pc_next + 2 + (uint16_t)w);
                snprintf(out, maxlen, "@%o", (unsigned)target);
            } else {
                snprintf(out, maxlen, "@%o(%s)", (unsigned)(uint16_t)w, rname[reg]);
            }
            extra = 2;
        }
        break;
    }
    return extra;
}

/* Branch target from 8-bit offset */
static uint16_t branch_target(uint16_t pc, uint16_t ir)
{
    int8_t off = (int8_t)(ir & 0xFF);
    return (uint16_t)(pc + 2 + off * 2);
}

/* Double-operand instruction */
static int do_dop(const char *mnem, uint16_t ir, uint16_t addr,
                  int (*read_word)(uint16_t, void *), void *ctx,
                  char *buf, int buflen)
{
    uint16_t pc = addr + 2;
    char src[32], dst[32];
    int src6 = (ir >> 6) & 077;
    int dst6 = ir & 077;

    int n1 = fmt_operand(src6, pc, read_word, ctx, src, sizeof(src));
    int n2 = fmt_operand(dst6, (uint16_t)(pc + n1), read_word, ctx, dst, sizeof(dst));

    snprintf(buf, buflen, "%-6s %s, %s", mnem, src, dst);
    return 2 + n1 + n2;
}

/* Single-operand instruction */
static int do_sop(const char *mnem, uint16_t ir, uint16_t addr,
                  int (*read_word)(uint16_t, void *), void *ctx,
                  char *buf, int buflen)
{
    uint16_t pc = addr + 2;
    char dst[32];
    int dst6 = ir & 077;

    int n = fmt_operand(dst6, pc, read_word, ctx, dst, sizeof(dst));
    snprintf(buf, buflen, "%-6s %s", mnem, dst);
    return 2 + n;
}

/* Condition code ops (NOP, CLC, CLV, CLZ, CLN, CCC, SEC, SEV, SEZ, SEN, SCC) */
static void do_cc(uint16_t ir, char *buf, int buflen)
{
    if ((ir & 0x1F) == 0) {
        snprintf(buf, buflen, "NOP");
        return;
    }
    int set = (ir >> 4) & 1;
    char tmp[32] = "";
    if (ir & 1) strcat(tmp, "C");
    if (ir & 2) strcat(tmp, "V");
    if (ir & 4) strcat(tmp, "Z");
    if (ir & 8) strcat(tmp, "N");
    if ((ir & 0xF) == 0xF)
        snprintf(buf, buflen, "%s", set ? "SCC" : "CCC");
    else
        snprintf(buf, buflen, "%s%s", set ? "SE" : "CL", tmp);
}

int pdp11_disasm(uint16_t addr,
                 int (*read_word)(uint16_t addr, void *ctx),
                 void *ctx,
                 char *buf, int buflen)
{
    int w = read_word(addr, ctx);
    if (w < 0) {
        snprintf(buf, buflen, "ERR");
        return 2;
    }
    uint16_t ir = (uint16_t)w;

    /* Double-operand: bits [15:12] */
    switch ((ir >> 12) & 0xF) {
    case 001: return do_dop("MOV",  ir, addr, read_word, ctx, buf, buflen);
    case 002: return do_dop("CMP",  ir, addr, read_word, ctx, buf, buflen);
    case 003: return do_dop("BIT",  ir, addr, read_word, ctx, buf, buflen);
    case 004: return do_dop("BIC",  ir, addr, read_word, ctx, buf, buflen);
    case 005: return do_dop("BIS",  ir, addr, read_word, ctx, buf, buflen);
    case 006: return do_dop("ADD",  ir, addr, read_word, ctx, buf, buflen);
    case 011: return do_dop("MOVB", ir, addr, read_word, ctx, buf, buflen);
    case 012: return do_dop("CMPB", ir, addr, read_word, ctx, buf, buflen);
    case 013: return do_dop("BITB", ir, addr, read_word, ctx, buf, buflen);
    case 014: return do_dop("BICB", ir, addr, read_word, ctx, buf, buflen);
    case 015: return do_dop("BISB", ir, addr, read_word, ctx, buf, buflen);
    case 016: return do_dop("SUB",  ir, addr, read_word, ctx, buf, buflen);
    }

    /* Branches: bits [15:8] */
    {
        uint16_t target = branch_target(addr, ir);
        switch ((ir >> 8) & 0xFF) {
        case 0001: snprintf(buf, buflen, "BR     %o", target); return 2;
        case 0002: snprintf(buf, buflen, "BNE    %o", target); return 2;
        case 0003: snprintf(buf, buflen, "BEQ    %o", target); return 2;
        case 0004: snprintf(buf, buflen, "BGE    %o", target); return 2;
        case 0005: snprintf(buf, buflen, "BLT    %o", target); return 2;
        case 0006: snprintf(buf, buflen, "BGT    %o", target); return 2;
        case 0007: snprintf(buf, buflen, "BLE    %o", target); return 2;
        case 0200: snprintf(buf, buflen, "BPL    %o", target); return 2;
        case 0201: snprintf(buf, buflen, "BMI    %o", target); return 2;
        case 0202: snprintf(buf, buflen, "BHI    %o", target); return 2;
        case 0203: snprintf(buf, buflen, "BLOS   %o", target); return 2;
        case 0204: snprintf(buf, buflen, "BVC    %o", target); return 2;
        case 0205: snprintf(buf, buflen, "BVS    %o", target); return 2;
        case 0206: snprintf(buf, buflen, "BCC    %o", target); return 2;
        case 0207: snprintf(buf, buflen, "BCS    %o", target); return 2;
        }
    }

    /* EMT / TRAP */
    if ((ir & 0xFF00) == 0x8800) {  /* 104000-104377: EMT */
        snprintf(buf, buflen, "EMT    %o", ir & 0xFF);
        return 2;
    }
    if ((ir & 0xFF00) == 0x8C00) {  /* 104400-104777: TRAP */
        snprintf(buf, buflen, "TRAP   %o", ir & 0xFF);
        return 2;
    }

    /* JSR: 004RDD */
    if ((ir & 0xFE00) == 0x0800) {
        int reg = (ir >> 6) & 7;
        char dst[32];
        int n = fmt_operand(ir & 077, (uint16_t)(addr + 2), read_word, ctx, dst, sizeof(dst));
        snprintf(buf, buflen, "JSR    %s, %s", rname[reg], dst);
        return 2 + n;
    }

    /* RTS: 00020R */
    if ((ir & 0xFFF8) == 0x0080) {
        snprintf(buf, buflen, "RTS    %s", rname[ir & 7]);
        return 2;
    }

    /* SOB: 077RNN */
    if ((ir & 0xFE00) == 0x7E00) {
        int reg = (ir >> 6) & 7;
        uint16_t target = (uint16_t)(addr + 2 - 2 * (ir & 077));
        snprintf(buf, buflen, "SOB    %s, %o", rname[reg], target);
        return 2;
    }

    /* XOR: 074RDD */
    if ((ir & 0xFE00) == 0x7A00) {
        int reg = (ir >> 6) & 7;
        char dst[32];
        int n = fmt_operand(ir & 077, (uint16_t)(addr + 2), read_word, ctx, dst, sizeof(dst));
        snprintf(buf, buflen, "XOR    %s, %s", rname[reg], dst);
        return 2 + n;
    }

    /* EIS: MUL/DIV/ASH/ASHC: 07RRDD (070-073) */
    if ((ir & 0xF000) == 0x7000) {
        static const char *eis_mnem[4] = { "MUL", "DIV", "ASH", "ASHC" };
        int sub = (ir >> 9) & 3;
        int reg = (ir >> 6) & 7;
        char src[32];
        int n = fmt_operand(ir & 077, (uint16_t)(addr + 2), read_word, ctx, src, sizeof(src));
        snprintf(buf, buflen, "%-6s %s, %s", eis_mnem[sub], src, rname[reg]);
        return 2 + n;
    }

    /* JMP: 0001DD */
    if ((ir & 0xFFC0) == 0x0040) {
        return do_sop("JMP", ir, addr, read_word, ctx, buf, buflen);
    }

    /* SWAB: 0003DD */
    if ((ir & 0xFFC0) == 0x00C0) {
        return do_sop("SWAB", ir, addr, read_word, ctx, buf, buflen);
    }

    /* Single-operand group: bits [15:6] */
    switch ((ir >> 6) & 0x3FF) {
    /* Word */
    case 0050: return do_sop("CLR",  ir, addr, read_word, ctx, buf, buflen);
    case 0051: return do_sop("COM",  ir, addr, read_word, ctx, buf, buflen);
    case 0052: return do_sop("INC",  ir, addr, read_word, ctx, buf, buflen);
    case 0053: return do_sop("DEC",  ir, addr, read_word, ctx, buf, buflen);
    case 0054: return do_sop("NEG",  ir, addr, read_word, ctx, buf, buflen);
    case 0055: return do_sop("ADC",  ir, addr, read_word, ctx, buf, buflen);
    case 0056: return do_sop("SBC",  ir, addr, read_word, ctx, buf, buflen);
    case 0057: return do_sop("TST",  ir, addr, read_word, ctx, buf, buflen);
    case 0060: return do_sop("ROR",  ir, addr, read_word, ctx, buf, buflen);
    case 0061: return do_sop("ROL",  ir, addr, read_word, ctx, buf, buflen);
    case 0062: return do_sop("ASR",  ir, addr, read_word, ctx, buf, buflen);
    case 0063: return do_sop("ASL",  ir, addr, read_word, ctx, buf, buflen);
    case 0064: /* MARK */
        snprintf(buf, buflen, "MARK   %o", ir & 077);
        return 2;
    case 0065: return do_sop("MFPI", ir, addr, read_word, ctx, buf, buflen);
    case 0066: return do_sop("MTPI", ir, addr, read_word, ctx, buf, buflen);
    case 0067: return do_sop("SXT",  ir, addr, read_word, ctx, buf, buflen);
    /* Byte */
    case 0450: return do_sop("CLRB", ir, addr, read_word, ctx, buf, buflen);
    case 0451: return do_sop("COMB", ir, addr, read_word, ctx, buf, buflen);
    case 0452: return do_sop("INCB", ir, addr, read_word, ctx, buf, buflen);
    case 0453: return do_sop("DECB", ir, addr, read_word, ctx, buf, buflen);
    case 0454: return do_sop("NEGB", ir, addr, read_word, ctx, buf, buflen);
    case 0455: return do_sop("ADCB", ir, addr, read_word, ctx, buf, buflen);
    case 0456: return do_sop("SBCB", ir, addr, read_word, ctx, buf, buflen);
    case 0457: return do_sop("TSTB", ir, addr, read_word, ctx, buf, buflen);
    case 0460: return do_sop("RORB", ir, addr, read_word, ctx, buf, buflen);
    case 0461: return do_sop("ROLB", ir, addr, read_word, ctx, buf, buflen);
    case 0462: return do_sop("ASRB", ir, addr, read_word, ctx, buf, buflen);
    case 0463: return do_sop("ASLB", ir, addr, read_word, ctx, buf, buflen);
    case 0465: return do_sop("MFPD", ir, addr, read_word, ctx, buf, buflen);
    case 0466: return do_sop("MTPD", ir, addr, read_word, ctx, buf, buflen);
    }

    /* SPL: 000230-000237 */
    if ((ir & 0xFFF8) == 0x00B8) {
        snprintf(buf, buflen, "SPL    %o", ir & 7);
        return 2;
    }

    /* Misc fixed opcodes */
    switch (ir) {
    case 0000000: snprintf(buf, buflen, "HALT");  return 2;
    case 0000001: snprintf(buf, buflen, "WAIT");  return 2;
    case 0000002: snprintf(buf, buflen, "RTI");   return 2;
    case 0000003: snprintf(buf, buflen, "BPT");   return 2;
    case 0000004: snprintf(buf, buflen, "IOT");   return 2;
    case 0000005: snprintf(buf, buflen, "RESET"); return 2;
    case 0000006: snprintf(buf, buflen, "RTT");   return 2;
    }

    /* Condition code ops: 000240-000277 */
    if ((ir & 0xFFE0) == 0x00A0) {
        do_cc(ir, buf, buflen);
        return 2;
    }

    /* Unknown */
    snprintf(buf, buflen, ".WORD  %06o", ir);
    return 2;
}
