/*
 * loader.c -- Program loader for ll-34
 *
 * Two loaders:
 *   loader_asc()  ASCII format from obj2bin.pl --ascii
 *   loader_lda()  DEC absolute loader binary (.lda/.bin)
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"

int loader_asc(Ram *ram, const char *filename, uint16_t *start_addr) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return -1;
    }

    char line[256];
    unsigned int addr = 0;
    unsigned int last_l = 0;
    int count = 0;

    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;

        if (*p == 'L' || *p == 'l') {
            /* Load address */
            unsigned int a;
            if (sscanf(p + 1, "%o", &a) == 1) {
                addr = a & 0xFFFF;
                last_l = addr;
            }
        } else if (*p == 'D' || *p == 'd') {
            /* Deposit word */
            unsigned int d;
            if (sscanf(p + 1, "%o", &d) == 1) {
                uint16_t word_idx = (addr & ~1) >> 1;
                if (word_idx < ram->size_words) {
                    ram->mem[word_idx] = d & 0xFFFF;
                    count++;
                } else {
                    fprintf(stderr, "loader: address %06o out of RAM range\n", addr);
                }
                addr += 2;
            }
        } else if (*p == 'S' || *p == 's') {
            /* Start -- execution address is the last L */
            break;
        }
        /* else: skip blank/comment lines */
    }

    *start_addr = last_l & 0xFFFF;
    fclose(f);
    return count;
}

/* ----------------------------------------------------------------
 * DEC absolute loader binary format (.lda / .bin)
 *
 * Each block:
 *   byte 0x01        header (skip any leading 0x00 bytes)
 *   byte 0x00        header
 *   uint16_t count   block byte count (includes 6 header bytes)
 *   uint16_t origin  load address for data
 *   byte data[count-6]
 *   byte checksum    (sum of all bytes including header = 0 mod 256)
 *
 * End block: count == 6 (no data), origin = start address.
 * ---------------------------------------------------------------- */
int loader_lda(Ram *ram, const char *filename, uint16_t *start_addr) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror(filename);
        return -1;
    }

    int total = 0;
    *start_addr = 0;

    for (;;) {
        /* Skip leader bytes (0x00) and find 0x01 header */
        int c;
        do {
            c = getc(f);
            if (c == EOF) goto done;
        } while (c == 0);

        if (c != 1) {
            fprintf(stderr, "loader_lda: expected 0x01 header, got 0x%02X\n", c);
            fclose(f);
            return -1;
        }

        /* Second header byte must be 0x00 */
        c = getc(f);
        if (c != 0) {
            fprintf(stderr, "loader_lda: expected 0x00, got 0x%02X\n", c);
            fclose(f);
            return -1;
        }

        uint8_t csum = 1; /* include the 0x01 header byte */
        /* csum += 0 for second header byte */

        /* Read count (little-endian) */
        int cl = getc(f), ch = getc(f);
        if (cl == EOF || ch == EOF) goto done;
        uint16_t count = (uint16_t)(cl | (ch << 8));
        csum += (uint8_t)cl + (uint8_t)ch;

        /* Read origin (little-endian) */
        int ol = getc(f), oh = getc(f);
        if (ol == EOF || oh == EOF) goto done;
        uint16_t origin = (uint16_t)(ol | (oh << 8));
        csum += (uint8_t)ol + (uint8_t)oh;

        /* End block? */
        if (count == 6) {
            /* Consume checksum byte */
            c = getc(f);
            if (c != EOF) csum += (uint8_t)c;
            *start_addr = origin;
            goto done;
        }

        /* Read data bytes */
        uint16_t data_len = count - 6;
        uint16_t addr = origin;
        for (uint16_t i = 0; i < data_len; i++) {
            c = getc(f);
            if (c == EOF) {
                fprintf(stderr, "loader_lda: unexpected EOF in data block\n");
                fclose(f);
                return -1;
            }
            csum += (uint8_t)c;

            /* Deposit byte into RAM */
            uint16_t word_idx = addr >> 1;
            if (word_idx < ram->size_words) {
                if (addr & 1) {
                    /* High byte */
                    ram->mem[word_idx] = (ram->mem[word_idx] & 0x00FF)
                                       | ((uint16_t)c << 8);
                } else {
                    /* Low byte */
                    ram->mem[word_idx] = (ram->mem[word_idx] & 0xFF00)
                                       | (uint16_t)c;
                }
                total++;
            }
            addr++;
        }

        /* Read and verify checksum */
        c = getc(f);
        if (c == EOF) goto done;
        csum += (uint8_t)c;
        if (csum != 0) {
            fprintf(stderr, "loader_lda: checksum error at block origin=%06o "
                    "(sum=0x%02X)\n", origin, csum);
        }
    }

done:
    fclose(f);
    return total;
}
