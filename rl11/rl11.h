/*
 * rl11.h -- RL11 controller (RL01/RL02 disk) for ll-34
 *
 * Up to 4 drives (RL01=5MB, RL02=10MB). Registers at 174400-174406.
 * Vector 160, BR5. Instantaneous DMA.
 */

#ifndef RL11_H
#define RL11_H

#include <stdio.h>
#include <stdint.h>
#include "../unibus/unibus.h"

#define RL11_BASE   0x3F900  /* 774400 (18-bit), 174400 (16-bit) */
#define RL11_END    0x3F909  /* 774411 */

#define RLCS_DRDY   0x0001  /* drive ready (read-only) */
#define RLCS_FUNC   0x000E  /* function [3:1] */
#define RLCS_MEX    0x0030  /* memory extension [5:4] */
#define RLCS_IE     0x0040  /* interrupt enable */
#define RLCS_CRDY   0x0080  /* controller ready (done) */
#define RLCS_DRIVE  0x0300  /* drive select [9:8] */
#define RLCS_INCMP  0x0400  /* incomplete */
#define RLCS_CRC    0x0800  /* CRC error */
#define RLCS_DLT    0x1000  /* data late */
#define RLCS_NXM    0x2000  /* non-existent memory */
#define RLCS_DRE    0x4000  /* drive error */
#define RLCS_ERR    0x8000  /* error summary (read-only) */

/* No GO bit: writing RLCS with CRDY=0 starts the command */

#define RLCS_ALLERR (RLCS_ERR | RLCS_DRE | RLCS_NXM | RLCS_DLT | \
                     RLCS_CRC | RLCS_INCMP)

#define RLCS_F_NOP      0
#define RLCS_F_WCHK     1
#define RLCS_F_GSTA     2
#define RLCS_F_SEEK     3
#define RLCS_F_RHDR     4
#define RLCS_F_WRITE    5
#define RLCS_F_READ     6
#define RLCS_F_RNOHDR   7

#define RLDA_SK_DIR     0x0004  /* seek direction: 1=out (higher cyl), 0=in (lower cyl) */
#define RLDA_SK_HD      0x0010  /* head select */
#define RLDA_V_CYL      7       /* cylinder offset in seek word */
#define RLDA_M_CYL      0x01FF  /* cylinder mask (9 bits) */

#define RLDA_V_SECT     0
#define RLDA_M_SECT     0x003F  /* sector [5:0] */
#define RLDA_V_HD       6       /* head [6] */
#define RLDA_V_CYL_RW   7       /* cylinder [15:7] */
#define RLDA_M_CYL_RW   0x01FF

#define RLDA_GS         0x0002  /* get status marker */
#define RLDA_GS_CLR     0x0008  /* clear errors */

#define RLDS_LOCK       0x0005  /* state: lock on (heads loaded) */
#define RLDS_BHO        0x0008  /* brushes home */
#define RLDS_HDO        0x0010  /* heads out */
#define RLDS_HD         0x0040  /* current head */
#define RLDS_RL02       0x0080  /* RL02 type (vs RL01) */
#define RLDS_DSE        0x0100  /* drive select error */
#define RLDS_VCK        0x0200  /* volume check */
#define RLDS_WGE        0x0400  /* write gate error */
#define RLDS_SPE        0x0800  /* spin error */
#define RLDS_STO        0x1000  /* seek timeout */
#define RLDS_WLK        0x2000  /* write locked */
#define RLDS_HCE        0x4000  /* head current error */
#define RLDS_WDE        0x8000  /* write data error */
#define RLDS_ERR        (RLDS_WDE | RLDS_HCE | RLDS_STO | RLDS_SPE | \
                         RLDS_WGE | RLDS_VCK | RLDS_DSE)

#define RL_NUMWD        128     /* words per sector */
#define RL_NUMSC        40      /* sectors per surface */
#define RL_NUMSF        2       /* surfaces per cylinder */
#define RL01_NUMCY      256     /* cylinders per drive (RL01) */
#define RL02_NUMCY      512     /* cylinders per drive (RL02) */
#define RL_NUMDR        4       /* max drives */

typedef struct {
    uint16_t rlcs, rlba, rlda, rlmp;
    FILE    *disk[RL_NUMDR];
    int      read_only[RL_NUMDR];
    int      is_rl02[RL_NUMDR];
    uint16_t cyl[RL_NUMDR];
    uint16_t hd[RL_NUMDR];
    uint16_t stat[RL_NUMDR];
    Bus     *bus;
    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void (*irq_clr)(void *ctx, uint16_t vector);
    void *irq_ctx;
} RL11;

void rl11_init(RL11 *rl);
int rl11_attach(RL11 *rl, int drive, const char *filename, int read_only);
int rl11_register(RL11 *rl, Bus *bus);

#endif /* RL11_H */
