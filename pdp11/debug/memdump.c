/*
 * debug/memdump.c -- Memory dump and save for ll-34 debug console
 */

#include <stdlib.h>
#include "memdump.h"

/* Map byte to printable ASCII, or '.' if non-printable */
static char printable(uint8_t b)
{
    return (b >= 0x20 && b <= 0x7E) ? (char)b : '.';
}

void mem_dump(uint16_t addr, int count,
              int (*read_word)(uint16_t addr, void *ctx),
              void *ctx, FILE *fp, const char *eol)
{
    uint16_t words[8];
    int valid[8];

    for (int i = 0; i < count; i++) {
        int col = i & 7;
        if (col == 0) {
            if (i > 0) {
                /* ASCII column for previous line */
                fprintf(fp, "  |");
                for (int j = 0; j < 8 && (i - 8 + j) < count; j++) {
                    if (valid[j]) {
                        fprintf(fp, "%c%c",
                                printable(words[j] & 0xFF),
                                printable((words[j] >> 8) & 0xFF));
                    } else {
                        fprintf(fp, "??");
                    }
                }
                fprintf(fp, "|");
                fprintf(fp, "%s", eol);
            }
            fprintf(fp, "  %06o:", (unsigned)(uint16_t)(addr + i * 2));
        }
        int w = read_word((uint16_t)(addr + i * 2), ctx);
        words[col] = (uint16_t)w;
        valid[col] = (w >= 0);
        if (w < 0)
            fprintf(fp, " ??????");
        else
            fprintf(fp, " %06o", (unsigned)(uint16_t)w);
    }

    /* ASCII column for last line */
    int last_col = ((count - 1) & 7) + 1;
    /* Pad remaining columns */
    for (int j = last_col; j < 8; j++)
        fprintf(fp, "       ");
    fprintf(fp, "  |");
    for (int j = 0; j < last_col; j++) {
        if (valid[j]) {
            fprintf(fp, "%c%c",
                    printable(words[j] & 0xFF),
                    printable((words[j] >> 8) & 0xFF));
        } else {
            fprintf(fp, "??");
        }
    }
    fprintf(fp, "|");
    fprintf(fp, "%s", eol);
}

int mem_save(const char *path, const uint16_t *mem, uint32_t nwords)
{
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;

    /* PDP-11 is little-endian: write each word as 2 bytes (low, high) */
    for (uint32_t i = 0; i < nwords; i++) {
        uint8_t lo = mem[i] & 0xFF;
        uint8_t hi = (mem[i] >> 8) & 0xFF;
        if (fwrite(&lo, 1, 1, fp) != 1 || fwrite(&hi, 1, 1, fp) != 1) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}
