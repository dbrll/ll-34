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

/* MS11-L response time and refresh (EK-MS11L-UG-001, table 1-2) */
static uint32_t ram_response(void *dev, uint32_t addr, int type, uint64_t now) {
    Ram *r = dev;
    (void)addr;
    uint64_t start = now > r->busy_until_ns ? now : r->busy_until_ns;
    while (r->refresh_interval_ns && r->next_refresh_ns <= start) {
        uint64_t ref = r->next_refresh_ns;
        if (ref < r->busy_until_ns) ref = r->busy_until_ns;
        r->busy_until_ns = ref + r->refresh_ns;
        r->next_refresh_ns += r->refresh_interval_ns;
        r->refresh_count++;
        if (start < r->busy_until_ns) start = r->busy_until_ns;
    }
    if (r->refresh_interval_ns && r->next_refresh_ns-start <= r->arbitration_ns) {
        start += r->arbitration_ns;
        r->arbitration_count++;
    }
    r->busy_until_ns = start + r->cycle_ns;
    r->wait_ns += start-now;
    return (uint32_t)(start-now) + (type <= BUS_DATIP ? r->read_ns : r->write_ns);
}
void ram_init(Ram *ram, uint32_t size_words) {
    memset(ram, 0, sizeof(*ram));
    ram->read_ns = 385; ram->write_ns = 125; ram->cycle_ns = 510;
    ram->refresh_ns = 570; ram->refresh_interval_ns = 14500;
    ram->arbitration_ns = 60;
    ram->next_refresh_ns = ram->refresh_interval_ns; /* unknown power-up phase: zero reference */
    if (size_words == 0 || size_words > RAM_MAX_WORDS)
        size_words = RAM_MAX_WORDS;
    ram->size_words = size_words;
}

int ram_register(Ram *ram, Bus *bus) {
    uint32_t end = (ram->size_words * 2) - 2;
    int rc = bus_register(bus, RAM_BASE, end,
                          ram, ram_read, ram_write, "MS11-L", ram->read_ns);
    if (rc == 0) bus->devices[bus->ndevices-1].response = ram_response;
    return rc;
}
