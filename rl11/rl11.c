/*
 * rl11.c -- RL11 controller (RL01/RL02 disk) for ll-34
 *
 * Instantaneous DMA, relative seek. Reference: EK-RL012-UG, SimH pdp11_rl.c
 */

#include <stdio.h>
#include <string.h>
#include "rl11.h"
#include "../trace.h"

#define GET_FUNC(cs)    (((cs) >> 1) & 7)
#define GET_DRIVE(cs)   (((cs) >> 8) & 3)
#define GET_MEX(cs)     (((cs) >> 4) & 3)

#define DA_SECT(da)     ((da) & RLDA_M_SECT)
#define DA_HD(da)       (((da) >> RLDA_V_HD) & 1)
#define DA_CYL_RW(da)   (((da) >> RLDA_V_CYL_RW) & RLDA_M_CYL_RW)

#define MAX_CYL(rl, d)  ((rl)->is_rl02[d] ? RL02_NUMCY : RL01_NUMCY)

#define RL01_SIZE       ((long)RL01_NUMCY * RL_NUMSF * RL_NUMSC * RL_NUMWD * 2)
#define RL02_SIZE       ((long)RL02_NUMCY * RL_NUMSF * RL_NUMSC * RL_NUMWD * 2)

static long chs_to_offset(int cyl, int hd, int sect) {
    return ((long)cyl * RL_NUMSF * RL_NUMSC +
            (long)hd * RL_NUMSC +
            (long)sect) * RL_NUMWD * 2;
}

static void rl11_set_done(RL11 *rl) {
    rl->rlcs |= RLCS_CRDY;
    if ((rl->rlcs & RLCS_IE) && rl->irq_set)
        rl->irq_set(rl->irq_ctx, 0160, 5);  /* vector 160, BR5 */
}

static void rl11_set_err(RL11 *rl, uint16_t bits) {
    rl->rlcs |= bits | RLCS_ERR;
}

static void rl11_go(RL11 *rl) {
    Bus *bus = rl->bus;
    int func = GET_FUNC(rl->rlcs);
    int drive = GET_DRIVE(rl->rlcs);
    /* Clear CRDY and errors */
    rl->rlcs &= ~(RLCS_CRDY | RLCS_ALLERR);

    /* NOP */
    if (func == RLCS_F_NOP) {
        rl11_set_done(rl);
        return;
    }

    /* Check drive is attached */
    if (!rl->disk[drive]) {
        rl11_set_err(rl, RLCS_INCMP | RLCS_DRE);
        rl->stat[drive] |= RLDS_DSE;
        rl11_set_done(rl);
        return;
    }

    switch (func) {
    case RLCS_F_GSTA: {
        if (rl->rlda & RLDA_GS_CLR)
            rl->stat[drive] &= ~RLDS_ERR;
        uint16_t st = RLDS_LOCK | RLDS_BHO | RLDS_HDO;
        if (rl->is_rl02[drive])
            st |= RLDS_RL02;
        if (rl->read_only[drive])
            st |= RLDS_WLK;
        if (rl->hd[drive])
            st |= RLDS_HD;
        st |= rl->stat[drive] & RLDS_ERR;
        rl->rlmp = st;

        rl->rlcs |= RLCS_DRDY;
        break;
    }

    case RLCS_F_SEEK: {
        int diff = (rl->rlda >> RLDA_V_CYL) & RLDA_M_CYL;
        int new_cyl;
        int maxcyl = MAX_CYL(rl, drive);

        if (rl->rlda & RLDA_SK_DIR)
            new_cyl = (int)rl->cyl[drive] + diff;   /* out = higher */
        else
            new_cyl = (int)rl->cyl[drive] - diff;   /* in = lower */

        if (new_cyl < 0) new_cyl = 0;
        if (new_cyl >= maxcyl) new_cyl = maxcyl - 1;
        rl->cyl[drive] = (uint16_t)new_cyl;
        rl->hd[drive] = (rl->rlda & RLDA_SK_HD) ? 1 : 0;
        break;
    }

    case RLCS_F_RHDR: {
        rl->rlmp = (uint16_t)((rl->cyl[drive] << 7) |
                              (rl->hd[drive] << 6));
        break;
    }

    case RLCS_F_READ:
    case RLCS_F_RNOHDR:
    case RLCS_F_WCHK: {
        /* Transfer clamped to end of track */
        int cyl  = DA_CYL_RW(rl->rlda);
        int hd   = DA_HD(rl->rlda);
        int sect = DA_SECT(rl->rlda);

        if (sect >= RL_NUMSC || cyl >= MAX_CYL(rl, drive)) {
            rl11_set_err(rl, RLCS_INCMP | RLCS_DRE);
            rl11_set_done(rl);
            return;
        }

        uint32_t wc = (0x10000 - rl->rlmp) & 0xFFFF;
        if (wc == 0) wc = 0x10000;

        uint32_t maxwc = (uint32_t)(RL_NUMSC - sect) * RL_NUMWD;
        if (wc > maxwc)
            wc = maxwc;

        uint32_t ma = ((uint32_t)GET_MEX(rl->rlcs) << 16) | rl->rlba;

        long file_offset = chs_to_offset(cyl, hd, sect);
        if (fseek(rl->disk[drive], file_offset, SEEK_SET) != 0) {
            rl11_set_err(rl, RLCS_INCMP);
            break;
        }

        uint8_t buf[256];
        uint32_t done_words = 0;
        while (done_words < wc) {
            size_t to_read = (wc - done_words > RL_NUMWD) ? RL_NUMWD : (wc - done_words);
            size_t got = fread(buf, 2, to_read, rl->disk[drive]);
            if (got == 0)
                break;
            if (func != RLCS_F_WCHK) {
                for (size_t i = 0; i < got; i++) {
                    uint16_t word = (uint16_t)buf[i*2] | ((uint16_t)buf[i*2+1] << 8);
                    uint32_t addr = ma & 0x3FFFF;
                    if (bus_write(bus, addr, word, BUS_DATO) < 0) {
                        rl11_set_err(rl, RLCS_NXM);
                        goto rd_xfer_done;
                    }
                    ma += 2;
                }
            } else {
                for (size_t i = 0; i < got; i++) {
                    uint16_t disk_word = (uint16_t)buf[i*2] | ((uint16_t)buf[i*2+1] << 8);
                    uint16_t mem_word;
                    uint32_t addr = ma & 0x3FFFF;
                    if (bus_read(bus, addr, &mem_word) < 0) {
                        rl11_set_err(rl, RLCS_NXM);
                        goto rd_xfer_done;
                    }
                    if (disk_word != mem_word) {
                        rl11_set_err(rl, RLCS_CRC);
                        goto rd_xfer_done;
                    }
                    ma += 2;
                }
            }
            done_words += (uint32_t)got;
        }

    rd_xfer_done:
        rl->rlmp = (uint16_t)((rl->rlmp + (done_words & 0xFFFF)) & 0xFFFF);
        rl->rlba = (uint16_t)(ma & 0xFFFF);
        rl->rlcs = (rl->rlcs & ~RLCS_MEX) |
                   (uint16_t)(((ma >> 16) & 3) << 4);

        sect += (int)(done_words / RL_NUMWD);
        rl->rlda = (uint16_t)((cyl << RLDA_V_CYL_RW) |
                              (hd << RLDA_V_HD) |
                              (sect & RLDA_M_SECT));

        if (rl->rlmp != 0)
            rl11_set_err(rl, RLCS_INCMP | RLCS_DLT);  /* HDE = DLT+INCMP */

        rl->cyl[drive] = (uint16_t)cyl;
        rl->hd[drive] = (uint16_t)hd;
        break;
    }

    case RLCS_F_WRITE: {
        int cyl  = DA_CYL_RW(rl->rlda);
        int hd   = DA_HD(rl->rlda);
        int sect = DA_SECT(rl->rlda);

        if (sect >= RL_NUMSC || cyl >= MAX_CYL(rl, drive)) {
            rl11_set_err(rl, RLCS_INCMP | RLCS_DRE);
            rl11_set_done(rl);
            return;
        }

        if (rl->read_only[drive]) {
            rl->stat[drive] |= RLDS_WGE;
            rl11_set_err(rl, RLCS_DRE);
            rl11_set_done(rl);
            return;
        }

        uint32_t wc = (0x10000 - rl->rlmp) & 0xFFFF;
        if (wc == 0) wc = 0x10000;

        uint32_t maxwc = (uint32_t)(RL_NUMSC - sect) * RL_NUMWD;
        if (wc > maxwc)
            wc = maxwc;

        uint32_t ma = ((uint32_t)GET_MEX(rl->rlcs) << 16) | rl->rlba;

        long file_offset = chs_to_offset(cyl, hd, sect);
        if (fseek(rl->disk[drive], file_offset, SEEK_SET) != 0) {
            rl11_set_err(rl, RLCS_INCMP);
            break;
        }

        uint8_t buf[256];
        uint32_t done_words = 0;
        while (done_words < wc) {
            size_t to_write = (wc - done_words > RL_NUMWD) ? RL_NUMWD : (wc - done_words);

            for (size_t i = 0; i < to_write; i++) {
                uint16_t word;
                uint32_t addr = ma & 0x3FFFF;
                if (bus_read(bus, addr, &word) < 0) {
                    rl11_set_err(rl, RLCS_NXM);
                    goto wr_xfer_done;
                }
                buf[i*2]     = (uint8_t)(word & 0xFF);
                buf[i*2 + 1] = (uint8_t)(word >> 8);
                ma += 2;
            }
            fwrite(buf, 2, to_write, rl->disk[drive]);
            done_words += (uint32_t)to_write;
        }

    wr_xfer_done:
        rl->rlmp = (uint16_t)((rl->rlmp + (done_words & 0xFFFF)) & 0xFFFF);
        rl->rlba = (uint16_t)(ma & 0xFFFF);
        rl->rlcs = (rl->rlcs & ~RLCS_MEX) |
                   (uint16_t)(((ma >> 16) & 3) << 4);

        sect += (int)(done_words / RL_NUMWD);
        rl->rlda = (uint16_t)((cyl << RLDA_V_CYL_RW) |
                              (hd << RLDA_V_HD) |
                              (sect & RLDA_M_SECT));

        if (rl->rlmp != 0)
            rl11_set_err(rl, RLCS_INCMP | RLCS_DLT);

        rl->cyl[drive] = (uint16_t)cyl;
        rl->hd[drive] = (uint16_t)hd;
        break;
    }

    default:
        rl11_set_err(rl, RLCS_INCMP);
        break;
    }

    rl11_set_done(rl);
}

static int rl11_read(void *dev, uint32_t addr, uint16_t *data) {
    RL11 *rl = (RL11 *)dev;
    int reg = (addr >> 1) & 3;

    switch (reg) {
    case 0: {
        int drive = GET_DRIVE(rl->rlcs);
        if (rl->disk[drive])
            rl->rlcs |= RLCS_DRDY;
        else
            rl->rlcs &= ~RLCS_DRDY;
        if (rl->rlcs & (RLCS_DRE | RLCS_NXM | RLCS_DLT | RLCS_CRC | RLCS_INCMP))
            rl->rlcs |= RLCS_ERR;
        else
            rl->rlcs &= ~RLCS_ERR;
        *data = rl->rlcs;
        break;
    }
    case 1: *data = rl->rlba; break;
    case 2: *data = rl->rlda; break;
    case 3: *data = rl->rlmp; break;
    }
    return 0;
}

static int rl11_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    RL11 *rl = (RL11 *)dev;
    int reg = (addr >> 1) & 3;

    if (is_byte) {
        uint16_t cur = 0;
        switch (reg) {
        case 0: cur = rl->rlcs; break;
        case 1: cur = rl->rlba; break;
        case 2: cur = rl->rlda; break;
        case 3: cur = rl->rlmp; break;
        }
        if (addr & 1)
            data = (cur & 0x00FF) | ((data & 0xFF) << 8);
        else
            data = (cur & 0xFF00) | (data & 0xFF);
    }

    switch (reg) {
    case 0: {
        uint16_t ro_mask = RLCS_CRDY | RLCS_DRDY | RLCS_ALLERR;
        rl->rlcs = (rl->rlcs & ro_mask) |
                   (data & (RLCS_FUNC | RLCS_MEX | RLCS_IE | RLCS_DRIVE));
        /* CRDY=1 in data: no command, just update IE */
        if (data & RLCS_CRDY) {
            break;
        }
        if (rl->rlcs & RLCS_CRDY) {
            rl->rlcs &= ~(RLCS_CRDY | RLCS_ALLERR);
            rl11_go(rl);
        }
        break;
    }
    case 1: rl->rlba = data & 0xFFFE; break;
    case 2: rl->rlda = data; break;
    case 3: rl->rlmp = data; break;
    }
    return 0;
}

void rl11_init(RL11 *rl) {
    memset(rl, 0, sizeof(*rl));
    rl->rlcs = RLCS_CRDY;
}

int rl11_attach(RL11 *rl, int drive, const char *filename, int read_only) {
    if (drive < 0 || drive >= RL_NUMDR)
        return -1;

    if (!read_only) {
        rl->disk[drive] = fopen(filename, "r+b");
        if (rl->disk[drive]) {
            rl->read_only[drive] = 0;
            goto detect;
        }
    }
    rl->disk[drive] = fopen(filename, "rb");
    if (!rl->disk[drive]) {
        fprintf(stderr, "rl11: cannot open %s\n", filename);
        return -1;
    }
    rl->read_only[drive] = 1;

detect:
    fseek(rl->disk[drive], 0, SEEK_END);
    long size = ftell(rl->disk[drive]);
    fseek(rl->disk[drive], 0, SEEK_SET);
    rl->is_rl02[drive] = (size > RL01_SIZE) ? 1 : 0;

    /* VCK set on attach, bootstrap clears it via GSTAT+GS_CLR */
    rl->stat[drive] = RLDS_VCK;

    fprintf(stderr, "rl11: dl%d attached %s (%s%s)\n",
            drive, filename,
            rl->is_rl02[drive] ? "RL02" : "RL01",
            rl->read_only[drive] ? ", read-only" : "");
    return 0;
}

int rl11_register(RL11 *rl, Bus *bus) {
    rl->bus = bus;
    return bus_register(bus, RL11_BASE, RL11_END,
                        rl, rl11_read, rl11_write, "RL11", 150);
}
