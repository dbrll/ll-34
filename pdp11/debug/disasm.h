/* Disassembler for the PDP-11/34 instruction set. */
#ifndef DISASM_H
#define DISASM_H

#include <stdint.h>

/* Disassemble the instruction at addr into buf. read_word returns the word
 * at an address, or -1 if it cannot be read. The return value is the size
 * of the instruction in bytes. */
int pdp11_disasm(uint16_t addr,
                 int (*read_word)(uint16_t addr, void *ctx),
                 void *ctx,
                 char *buf, int buflen);

#endif
