/* Program loaders */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"

/* L sets the address and D deposits a word there, then moves to the next
 * one. S ends the file, and the program starts at the last L address. */
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
            unsigned int a;
            if (sscanf(p + 1, "%o", &a) == 1) {
                addr = a & 0xFFFF;
                last_l = addr;
            }
        } else if (*p == 'D' || *p == 'd') {
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
            break;
        }
    }

    *start_addr = last_l & 0xFFFF;
    fclose(f);
    return count;
}

/* Each block starts with 001 000, then the count, the origin, the data and
 * a checksum. The last block has a count of 6 and holds the start address. */
int loader_lda(Ram *ram, const char *filename, uint16_t *start_addr) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror(filename);
        return -1;
    }

    int total = 0;
    *start_addr = 0;

    for (;;) {
        /* Skip the leader (000 bytes) up to the 001 header */
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

        c = getc(f);
        if (c != 0) {
            fprintf(stderr, "loader_lda: expected 0x00, got 0x%02X\n", c);
            fclose(f);
            return -1;
        }

        uint8_t csum = 1;   /* the 001 header byte */

        int cl = getc(f), ch = getc(f);
        if (cl == EOF || ch == EOF) goto done;
        uint16_t count = (uint16_t)(cl | (ch << 8));
        csum += (uint8_t)cl + (uint8_t)ch;

        int ol = getc(f), oh = getc(f);
        if (ol == EOF || oh == EOF) goto done;
        uint16_t origin = (uint16_t)(ol | (oh << 8));
        csum += (uint8_t)ol + (uint8_t)oh;

        if (count == 6) {
            c = getc(f);
            if (c != EOF) csum += (uint8_t)c;
            *start_addr = origin;
            goto done;
        }

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

            uint16_t word_idx = addr >> 1;
            if (word_idx < ram->size_words) {
                if (addr & 1) {
                    ram->mem[word_idx] = (ram->mem[word_idx] & 0x00FF)
                                       | ((uint16_t)c << 8);
                } else {
                    ram->mem[word_idx] = (ram->mem[word_idx] & 0xFF00)
                                       | (uint16_t)c;
                }
                total++;
            }
            addr++;
        }

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

/* In a MACRO-11 listing the address is in columns 10-15, followed by up to
 * three words. Lines without an address are skipped. The start address is
 * the symbol after .END, which the symbol table at the end resolves. */
int loader_lst(Ram *ram, const char *filename, uint16_t *start_addr,
               int *has_start, uint16_t *lo, uint16_t *hi) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return -1;
    }
    char line[512], end_sym[32] = "";
    int count = 0, symtab = 0;
    unsigned min = 0xFFFF, max = 0;
    *has_start = 0;
    while (fgets(line, sizeof line, f)) {
        if (!strncmp(line, "Symbol table", 12)) { symtab = 1; continue; }
        if (symtab) {                                 /* "NAME  = 001000" */
            if (!end_sym[0]) continue;
            size_t n = strlen(end_sym);
            for (char *p = line; (p = strstr(p, end_sym)) != NULL; p += n) {
                if (p != line && p[-1] != ' ') continue;
                char *q = p + n;
                while (*q == ' ') q++;
                unsigned v;
                if (*q == '=' && sscanf(q + 1, "%o", &v) == 1) {
                    *start_addr = (uint16_t)v; *has_start = 1;
                }
            }
            continue;
        }
        char *e = strstr(line, ".END");
        if (e && (e == line || isspace((unsigned char)e[-1])) && isspace((unsigned char)e[4])) {
            char *q = e + 4;
            while (*q == ' ' || *q == '\t') q++;
            unsigned v;
            if (sscanf(q, "%31[A-Za-z0-9$.]", end_sym) == 1 && isdigit((unsigned char)end_sym[0])) {
                if (sscanf(end_sym, "%o", &v) == 1) { *start_addr = (uint16_t)v; *has_start = 1; }
                end_sym[0] = '\0';
            }
            continue;
        }
        unsigned addr;
        int n;
        if (strlen(line) < 22 || sscanf(line + 9, "%6o%n", &addr, &n) != 1 || n != 6)
            continue;
        if (line[15] == '\'') {
            fprintf(stderr, "%s: relocatable listing (address %06o'), link it or use .ASECT\n",
                    filename, addr);
            fclose(f);
            return -1;
        }
        char *q = line + 15;
        for (int i = 0; i < 3; i++) {
            while (*q == ' ') q++;
            unsigned w;
            int k;
            if (sscanf(q, "%6o%n", &w, &k) != 1 || k != 6 || (q[6] && !isspace((unsigned char)q[6])))
                break;
            if (addr / 2 >= ram->size_words) {
                fprintf(stderr, "%s: address %06o beyond memory\n", filename, addr);
                fclose(f);
                return -1;
            }
            ram->mem[addr >> 1] = (uint16_t)w;
            if (addr < min) min = addr;
            if (addr > max) max = addr;
            addr += 2; q += k; count++;
        }
    }
    fclose(f);
    *lo = (uint16_t)min;
    *hi = (uint16_t)max;
    return count;
}
