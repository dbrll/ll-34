/*
 * rom.c -- M9301 boot/diagnostic ROM for ll-34
 *
 * Loads the ROM image from a .mac dump file (octal address/data pairs)
 * and provides read-only bus access to both banks.
 *
 * The M9301 hardware has a special feature: reading location 173024
 * returns the prototype data OR'd with S1 DIP switch bits 8:1.
 * This is the power-on vector PC, so S1 selects the boot entry point.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "rom.h"
#include "m9301_yf_rom.h"

void rom_init(Rom *rom) {
    memset(rom, 0, sizeof(*rom));
    /* Load built-in M9301-YF image */
    memcpy(rom->low, m9301_yf_low, sizeof(rom->low));
    memcpy(rom->high, m9301_yf_high, sizeof(rom->high));
}

void rom_set_s1(Rom *rom, uint16_t s1) {
    /* S1 switches occupy bits 8:1 of the address.
     * S1-3 through S1-10 map to address bits 8 through 1. */
    rom->s1_switches = s1 & 0x01FE;
}

static int rom_read(void *dev, uint32_t addr, uint16_t *data) {
    Rom *rom = dev;
    uint32_t word_addr = addr & ~1u;

    if (word_addr >= ROM_LOW_BASE && word_addr <= ROM_LOW_END) {
        /* Low bank */
        uint32_t idx = (word_addr - ROM_LOW_BASE) >> 1;
        *data = rom->low[idx];
        return 0;
    }

    if (word_addr >= ROM_HIGH_BASE && word_addr <= ROM_HIGH_END) {
        /* High bank */
        uint32_t idx = (word_addr - ROM_HIGH_BASE) >> 1;
        *data = rom->high[idx];

        /* S1LOC magic: OR in the S1 switch configuration */
        if (word_addr == ROM_S1LOC)
            *data |= rom->s1_switches;

        return 0;
    }

    /* Should not happen if registered correctly */
    *data = 0;
    return -1;
}

/* ROM is read-only: writes are silently ignored */
static int rom_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    (void)dev; (void)addr; (void)data; (void)is_byte;
    return 0;
}

/* Load ROM from .mac dump (octal address/data pairs, e.g. "165022 005003") */
int rom_load_mac(Rom *rom, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return -1;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), f)) {
        /* Skip blank lines and pure comment lines */
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == ';')
            continue;

        /* Try to parse two octal numbers at the start */
        unsigned int addr, data;
        if (sscanf(p, "%o %o", &addr, &data) != 2)
            continue;

        /* Convert 16-bit address to 18-bit (I/O page mapping) */
        uint32_t a18 = addr & 0xFFFF;
        if ((a18 & 0xE000) == 0xE000)
            a18 |= 0x30000;
        uint16_t d16 = data & 0xFFFF;

        if (a18 >= ROM_LOW_BASE && a18 <= ROM_LOW_END) {
            uint32_t idx = (a18 - ROM_LOW_BASE) >> 1;
            rom->low[idx] = d16;
            count++;
        } else if (a18 >= ROM_HIGH_BASE && a18 <= ROM_HIGH_END) {
            uint32_t idx = (a18 - ROM_HIGH_BASE) >> 1;
            rom->high[idx] = d16;
            count++;
        }
        /* else: address outside ROM range, skip */
    }

    fclose(f);
    return count;
}

/* Two non-contiguous banks, registered as separate bus devices */
int rom_register(Rom *rom, Bus *bus) {
    int rc;
    rc = bus_register(bus, ROM_LOW_BASE, ROM_LOW_END,
                      rom, rom_read, rom_write, "M9301 low", 150);
    if (rc < 0) return rc;
    rc = bus_register(bus, ROM_HIGH_BASE, ROM_HIGH_END,
                      rom, rom_read, rom_write, "M9301 high", 150);
    return rc;
}
