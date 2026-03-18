/*
 * ram.c -- Main memory for ll-34
 */

#include <stdio.h>
#include <string.h>
#include "ram.h"

static int ram_read(void *dev, uint32_t addr, uint16_t *data) {
    Ram *ram = dev;
    uint32_t word_addr = (addr & ~1u) >> 1;
    if (word_addr >= ram->size_words) {
        *data = 0;
        return -1;
    }
    *data = ram->mem[word_addr];
    return 0;
}

static int ram_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    Ram *ram = dev;
    if (is_byte) {
        if (addr >= ram->size_words * 2)
            return -1;
        uint8_t *p = (uint8_t *)ram->mem;
        /* UNIBUS DATOB: the KD11-EA SSMUX (via E80 SWAP) places the byte
         * on the correct DATA lines: DATA[7:0] for even, DATA[15:8] for
         * odd addresses.  The memory slave selects the byte using A00. */
        p[addr] = (addr & 1) ? ((data >> 8) & 0xFF) : (data & 0xFF);
    } else {
        uint32_t word_addr = (addr & ~1u) >> 1;
        if (word_addr >= ram->size_words)
            return -1;
        ram->mem[word_addr] = data;
    }
    return 0;
}

void ram_init(Ram *ram, uint32_t size_words) {
    memset(ram->mem, 0, sizeof(ram->mem));
    if (size_words == 0 || size_words > RAM_MAX_WORDS)
        size_words = RAM_MAX_WORDS;
    ram->size_words = size_words;
}

int ram_register(Ram *ram, Bus *bus) {
    uint32_t end = (ram->size_words * 2) - 2;
    return bus_register(bus, RAM_BASE, end,
                        ram, ram_read, ram_write, "RAM", 300);
}
