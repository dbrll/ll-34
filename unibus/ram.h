/*
 * ram.h -- Main memory (MS11-E MOS) for ll-34
 *
 * The MS11-E supports up to 128K words (256KB).
 * With 18-bit UNIBUS addressing (256KB total), the top 4K words
 * are reserved for the I/O page, leaving 124K words (248KB) for RAM.
 *
 * The CPU generates 18-bit addresses: 16-bit VBA with I/O page
 * mapping (VBA[15:13]=111 → PBA[17:16]=11). DMA devices (RK11)
 * also use 18-bit addresses directly on the UNIBUS.
 */

#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include "unibus.h"

#define RAM_MAX_WORDS   126976  /* 248KB = 126976 words (256KB minus 8KB I/O page) */
#define RAM_BASE        0x0000

typedef struct {
    uint16_t mem[RAM_MAX_WORDS];
    uint32_t size_words;        /* actual configured size */
} Ram;

void ram_init(Ram *ram, uint32_t size_words);

/* Register RAM on the bus */
int ram_register(Ram *ram, Bus *bus);

#endif /* RAM_H */
