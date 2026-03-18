/*
 * rk11.h -- RK11-D controller (RK05 disk) for ll-34
 *
 * Up to 8 drives, 2.5 MB each. Registers at 177400-177416.
 * Vector 220, BR5. Instantaneous DMA.
 */

#ifndef RK05_H
#define RK05_H

#include <stdio.h>
#include <stdint.h>
#include "../unibus/unibus.h"

#define RK05_BASE   0x3FF00  /* 777400 octal (18-bit) */
#define RK05_END    0x3FF0F  /* 777417 octal (18-bit) */

#define RKDS_SC     0x000F  /* sector counter [3:0] */
#define RKDS_ON_SC  0x0010  /* on sector */
#define RKDS_WLK    0x0020  /* write lock */
#define RKDS_RWS    0x0040  /* read/write/seek ready */
#define RKDS_RDY    0x0080  /* drive ready */
#define RKDS_SC_OK  0x0100  /* sector counter OK */
#define RKDS_INH    0x0200  /* seek incomplete */
#define RKDS_UNSAFE 0x0400  /* unsafe */
#define RKDS_RK05   0x0800  /* RK05 type */
#define RKDS_PWR    0x1000  /* power low */
#define RKDS_ID     0xE000  /* drive ID [15:13] */

#define RKER_WCE    0x0001  /* write check error */
#define RKER_CSE    0x0002  /* checksum error */
#define RKER_NXS    0x0020  /* non-existent sector */
#define RKER_NXC    0x0040  /* non-existent cylinder */
#define RKER_NXD    0x0080  /* non-existent disk */
#define RKER_TE     0x0100  /* timing error */
#define RKER_DLT    0x0200  /* data late */
#define RKER_NXM    0x0400  /* non-existent memory */
#define RKER_PGE    0x0800  /* programming error */
#define RKER_SKE    0x1000  /* seek error */
#define RKER_WLO    0x2000  /* write lockout */
#define RKER_OVR    0x4000  /* overrun */
#define RKER_DRE    0x8000  /* drive error */

#define RKCS_GO     0x0001  /* go */
#define RKCS_FUNC   0x000E  /* function [3:1] */
#define RKCS_MEX    0x0030  /* memory extension [5:4] */
#define RKCS_IE     0x0040  /* interrupt enable */
#define RKCS_DONE   0x0080  /* done */
#define RKCS_SSE    0x0100  /* stop on soft error */
#define RKCS_FMT    0x0400  /* format */
#define RKCS_INH    0x0800  /* inhibit incrementing BA */
#define RKCS_SCP    0x2000  /* search complete */
#define RKCS_HERR   0x4000  /* hard error */
#define RKCS_ERR    0x8000  /* error (composite) */

#define RKCS_CTLRESET  0
#define RKCS_WRITE     1
#define RKCS_READ      2
#define RKCS_WCHK      3
#define RKCS_SEEK      4
#define RKCS_RCHK      5
#define RKCS_DRVRESET  6
#define RKCS_WLK       7

#define RKDA_SECT   0x000F  /* sector [3:0] */
#define RKDA_SURF   0x0010  /* surface [4] */
#define RKDA_CYL    0x1FE0  /* cylinder [12:5] */
#define RKDA_DRIVE  0xE000  /* drive [15:13] */

#define RK_NUMWD    256     /* words per sector */
#define RK_NUMSC    12      /* sectors per surface */
#define RK_NUMSF    2       /* surfaces per cylinder */
#define RK_NUMCY    203     /* cylinders per drive */
#define RK_NUMDR    8       /* max drives */
#define RK_SECT_PER_DRIVE  (RK_NUMCY * RK_NUMSF * RK_NUMSC)

typedef struct {
    uint16_t rkds, rker, rkcs, rkwc, rkba, rkda;
    FILE    *disk[RK_NUMDR];
    int      read_only[RK_NUMDR];
    Bus     *bus;
    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void (*irq_clr)(void *ctx, uint16_t vector);
    void *irq_ctx;
} RK05;

void rk05_init(RK05 *rk);
int rk05_attach(RK05 *rk, int drive, const char *filename, int read_only);
int rk05_register(RK05 *rk, Bus *bus);

#endif /* RK05_H */
