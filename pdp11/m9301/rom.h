/* M9301-YF bootstrap and diagnostic ROM. Its four PROMs (23-480A9 to
 * 23-483A9) hold 512 words, at 165000-165776 and 173000-173776. A read of
 * 173024 also returns the S1 switches, ORed into the data. The power-up
 * vector points there, so S1 chooses where the CPU starts. */

#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include "../unibus/unibus.h"

#define ROM_LOW_BASE    0x3EA00  /* 765000 */
#define ROM_LOW_END     0x3EBFE
#define ROM_LOW_WORDS   256

#define ROM_HIGH_BASE   0x3F600  /* 773000 */
#define ROM_HIGH_END    0x3F7FE
#define ROM_HIGH_WORDS  256

#define ROM_S1LOC       0x3F614  /* 773024 */

typedef struct {
    uint16_t low[ROM_LOW_WORDS];
    uint16_t high[ROM_HIGH_WORDS];
    uint16_t s1_switches;           /* bits 8-1 */
} Rom;

/* The built-in image is the M9301-YF. */
void rom_init(Rom *rom);
void rom_set_s1(Rom *rom, uint16_t s1);

/* Replace the image with the octal address and data pairs of a .mac file.
 * Returns the number of words loaded, or -1. */
int rom_load_mac(Rom *rom, const char *filename);

int rom_register(Rom *rom, Bus *bus);

#endif
