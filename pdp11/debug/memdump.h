/*
 * debug/memdump.h -- Memory dump and save for ll-34 debug console
 */

#ifndef MEMDUMP_H
#define MEMDUMP_H

#include <stdio.h>
#include <stdint.h>

/* Dump 'count' words starting at 'addr' in octal (8 words/line).
 * read_word(addr, ctx) returns the 16-bit word or -1 on error.
 * eol is the line terminator ("\r\n" for raw terminal, "\n" for files). */
void mem_dump(uint16_t addr, int count,
              int (*read_word)(uint16_t addr, void *ctx),
              void *ctx, FILE *fp, const char *eol);

/* Save 'nwords' words of raw memory to a binary file (PDP-11 little-endian).
 * Returns 0 on success, -1 on error. */
int mem_save(const char *path, const uint16_t *mem, uint32_t nwords);

#endif /* MEMDUMP_H */
