/* Memory dump and save, for the debugger. */

#ifndef MEMDUMP_H
#define MEMDUMP_H

#include <stdio.h>
#include <stdint.h>

/* Print count words from addr in octal, eight to a line. read_word returns
 * the word at an address, or -1. eol is "\r\n" on a raw terminal. */
void mem_dump(uint16_t addr, int count,
              int (*read_word)(uint16_t addr, void *ctx),
              void *ctx, FILE *fp, const char *eol);

/* Write nwords words of memory to a file, low byte first as on the
 * PDP-11. Returns 0, or -1 on error. */
int mem_save(const char *path, const uint16_t *mem, uint32_t nwords);

#endif
