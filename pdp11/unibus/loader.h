/*
 * loader.h -- Program loader for ll-34
 *
 * Two formats supported:
 *
 * 1. ASCII (obj2bin.pl --ascii, M9312-style console load):
 *    L <address_octal>   -- set load address
 *    D <word_octal>      -- deposit word, advance address by 2
 *    S                   -- start execution at last L address
 *
 * 2. Binary absolute loader (.lda/.bin, DEC standard):
 *    Blocks: 0x01 0x00 count(2) origin(2) data... checksum
 *    End block: count=6, origin=start address
 */

#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include "ram.h"

/* Load a program from an ASCII dump file into RAM.
 * Sets *start_addr to the execution start address (from the final L before S).
 * Returns the number of words loaded, or -1 on error. */
int loader_asc(Ram *ram, const char *filename, uint16_t *start_addr);

/* Load a program from a DEC absolute loader binary (.lda/.bin) into RAM.
 * Sets *start_addr to the execution start address (from the end block).
 * Returns the number of bytes loaded, or -1 on error. */
int loader_lda(Ram *ram, const char *filename, uint16_t *start_addr);

#endif /* LOADER_H */
