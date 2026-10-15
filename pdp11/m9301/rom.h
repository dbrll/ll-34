/*
 * rom.h -- M9301 boot/diagnostic ROM for ll-34
 *
 * The M9301-YF contains 4 PROMs (23-480A9 through 23-483A9) providing
 * 1024 words in two non-contiguous banks:
 *   Low bank:  165000-165776  (256 words) -- diagnostics, console emulator
 *   High bank: 173000-173776  (256 words) -- entry points, boot stubs
 *
 * Special location 173024: when read, the M9301 hardware OR's the S1
 * DIP switch configuration into the data bus. This is used as the
 * power-on vector PC, so S1 selects which entry point the CPU starts at.
 */

#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include "../unibus/unibus.h"

/* Low bank: 765000-765776 (0x3EA00-0x3EBFE, 18-bit) -- 256 words */
#define ROM_LOW_BASE    0x3EA00
#define ROM_LOW_END     0x3EBFE
#define ROM_LOW_WORDS   256

/* High bank: 773000-773776 (0x3F600-0x3F7FE, 18-bit) -- 256 words */
#define ROM_HIGH_BASE   0x3F600
#define ROM_HIGH_END    0x3F7FE
#define ROM_HIGH_WORDS  256

/* S1LOC: magic address where S1 switches are OR'd into read data */
#define ROM_S1LOC       0x3F614  /* 773024 octal (18-bit) */

typedef struct {
    uint16_t low[ROM_LOW_WORDS];    /* 165000-165776 */
    uint16_t high[ROM_HIGH_WORDS];  /* 173000-173776 */
    uint16_t s1_switches;           /* S1 DIP switch value (bits 8:1) */
} Rom;

/* Initialize ROM with built-in M9301-YF image */
void rom_init(Rom *rom);

/* Set S1 switch configuration */
void rom_set_s1(Rom *rom, uint16_t s1);

/* Load ROM contents from a .mac file (address/data pairs in octal),
 * replacing the built-in image. Returns number of words loaded, or -1 on error. */
int rom_load_mac(Rom *rom, const char *filename);

/* Register both ROM banks on the bus */
int rom_register(Rom *rom, Bus *bus);

#endif /* ROM_H */
