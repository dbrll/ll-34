/* MS11-L memory. The Unibus has 18 address bits, and the top 4K words are
 * the I/O page, which leaves 124K words for memory. */

#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include "unibus.h"

#define RAM_MAX_WORDS   126976  /* 248 KB */
#define RAM_BASE        0x0000

typedef struct {
    uint16_t mem[RAM_MAX_WORDS];
    uint32_t size_words;
    uint64_t busy_until_ns, next_refresh_ns;
    uint64_t refresh_count, arbitration_count, wait_ns;
    uint32_t read_ns, write_ns, cycle_ns;
    uint32_t refresh_ns, refresh_interval_ns, arbitration_ns;
} Ram;

void ram_init(Ram *ram, uint32_t size_words);
int ram_register(Ram *ram, Bus *bus);

#endif
