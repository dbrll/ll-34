/*
 * debug/disasm.h -- PDP-11 instruction disassembler for ll-34
 *
 * Written from scratch for the ll-34 project (MIT license).
 * Covers the full PDP-11/34 instruction set including EIS (MUL/DIV/ASH/ASHC).
 */

#ifndef DISASM_H
#define DISASM_H

#include <stdint.h>

/* Disassemble one PDP-11 instruction at the given virtual address.
 *
 * read_word(addr, ctx) must return the 16-bit word at addr, or -1 on error.
 * The disassembler calls it for the instruction word and any additional
 * operand words (immediate, index, absolute address).
 *
 * buf/buflen: output buffer for the disassembled text.
 * Returns the number of bytes consumed (2, 4, or 6). */
int pdp11_disasm(uint16_t addr,
                 int (*read_word)(uint16_t addr, void *ctx),
                 void *ctx,
                 char *buf, int buflen);

#endif /* DISASM_H */
