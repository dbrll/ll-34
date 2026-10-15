/*
 * rk11.c -- RK11-D controller (RK05 disk) for ll-34
 *
 * Instantaneous DMA, no seek/rotational delay.
 * Reference: EK-RK11D-MM, SimH pdp11_rk.c
 */

#include <stdio.h>
#include <string.h>
#include "rk11.h"
#include "../trace.h"

#define DA_SECT(da)   ((da) & RKDA_SECT)
#define DA_SURF(da)   (((da) >> 4) & 1)
#define DA_CYL(da)    (((da) >> 5) & 0xFF)
#define DA_DRIVE(da)  (((da) >> 13) & 7)

static uint32_t da_to_sector(uint16_t da) {
    return ((uint32_t)DA_CYL(da) * RK_NUMSF + DA_SURF(da)) * RK_NUMSC + DA_SECT(da);
}

static void rk05_set_done(RK05 *rk) {
    rk->rkcs |= RKCS_DONE;
    if ((rk->rkcs & RKCS_IE) && rk->irq_set)
        rk->irq_set(rk->irq_ctx, 0220, 5);  /* vector 220, BR5 */
}

static void rk05_update_err(RK05 *rk) {
    if (rk->rker)
        rk->rkcs |= RKCS_ERR;
    if (rk->rker & (RKER_NXD | RKER_NXC | RKER_NXS | RKER_NXM))
        rk->rkcs |= RKCS_HERR;
}

static void rk05_go(RK05 *rk) {
    Bus *bus = rk->bus;
    int func = (rk->rkcs >> 1) & 7;
    uint16_t da = rk->rkda;
    int drive = DA_DRIVE(da);

    rk->rkcs &= ~(RKCS_DONE | RKCS_SCP | RKCS_ERR | RKCS_HERR);
    rk->rker = 0;

    if (func == RKCS_CTLRESET) {
        rk->rkcs = RKCS_DONE;
        rk->rker = 0;
        rk->rkda = 0;
        rk->rkba = 0;
        rk->rkwc = 0;
        return;
    }

    if (!rk->disk[drive]) {
        rk->rker |= RKER_NXD;
        rk05_update_err(rk);
        rk05_set_done(rk);
        return;
    }

    if (rk->read_only[drive] && (func == RKCS_WRITE || func == RKCS_WLK)) {
        rk->rker |= RKER_WLO;
        rk05_update_err(rk);
        rk05_set_done(rk);
        return;
    }

    if (DA_SECT(da) >= RK_NUMSC) {
        rk->rker |= RKER_NXS;
        rk05_update_err(rk);
        rk05_set_done(rk);
        return;
    }
    if (DA_CYL(da) >= RK_NUMCY) {
        rk->rker |= RKER_NXC;
        rk05_update_err(rk);
        rk05_set_done(rk);
        return;
    }

    /* Word count (2's complement) */
    uint32_t wc = (0x10000 - rk->rkwc) & 0xFFFF;
    if (wc == 0) wc = 0x10000;

    uint32_t ma = ((uint32_t)(rk->rkcs & RKCS_MEX) << 12) | rk->rkba;

    uint32_t sector = da_to_sector(da);
    long file_offset = (long)sector * RK_NUMWD * 2;

    switch (func) {
    case RKCS_READ:
    case RKCS_RCHK: {
        if (fseek(rk->disk[drive], file_offset, SEEK_SET) != 0) {
            rk->rker |= RKER_CSE;
            break;
        }
        uint8_t buf[512];
        uint32_t done_words = 0;
        while (done_words < wc) {
            size_t to_read = (wc - done_words > RK_NUMWD) ? RK_NUMWD : (wc - done_words);
            size_t got = fread(buf, 2, to_read, rk->disk[drive]);
            if (got == 0) {
                rk->rker |= RKER_OVR;
                break;
            }
            if (func == RKCS_READ) {
                for (size_t i = 0; i < got; i++) {
                    uint16_t word = (uint16_t)buf[i*2] | ((uint16_t)buf[i*2+1] << 8);
                    uint32_t addr = ma & 0x3FFFF;
                    if (bus_write(bus, addr, word, BUS_DATO) < 0) {
                        rk->rker |= RKER_NXM;
                        goto xfer_done;
                    }
                    ma += 2;
                }
            } else {
                ma += (uint32_t)got * 2;
            }
            done_words += (uint32_t)got;
        }
        break;
    }

    case RKCS_WRITE:
    case RKCS_WCHK: {
        if (fseek(rk->disk[drive], file_offset, SEEK_SET) != 0) {
            rk->rker |= RKER_CSE;
            break;
        }
        uint8_t buf[512];
        uint32_t done_words = 0;
        while (done_words < wc) {
            size_t to_write = (wc - done_words > RK_NUMWD) ? RK_NUMWD : (wc - done_words);

            if (func == RKCS_WRITE) {
                for (size_t i = 0; i < to_write; i++) {
                    uint16_t word;
                    uint32_t addr = ma & 0x3FFFF;
                    if (bus_read(bus, addr, &word) < 0) {
                        rk->rker |= RKER_NXM;
                        goto xfer_done;
                    }
                    buf[i*2]     = (uint8_t)(word & 0xFF);
                    buf[i*2 + 1] = (uint8_t)(word >> 8);
                    ma += 2;
                }
                fwrite(buf, 2, to_write, rk->disk[drive]);
            } else {
                size_t got = fread(buf, 2, to_write, rk->disk[drive]);
                for (size_t i = 0; i < got; i++) {
                    uint16_t word;
                    uint32_t addr = ma & 0x3FFFF;
                    if (bus_read(bus, addr, &word) < 0) {
                        rk->rker |= RKER_NXM;
                        goto xfer_done;
                    }
                    uint16_t disk_word = (uint16_t)buf[i*2] | ((uint16_t)buf[i*2+1] << 8);
                    if (word != disk_word)
                        rk->rker |= RKER_WCE;
                    ma += 2;
                }
                done_words += (uint32_t)got;
                continue;
            }
            done_words += (uint32_t)to_write;
        }
        break;
    }

    case RKCS_SEEK:
    case RKCS_DRVRESET:
        rk->rkcs |= RKCS_SCP;
        break;

    case RKCS_WLK:
        break;

    default:
        rk->rker |= RKER_PGE;
        break;
    }

xfer_done:
    rk->rkwc = (uint16_t)((rk->rkwc + (wc & 0xFFFF)) & 0xFFFF);
    rk->rkba = (uint16_t)(ma & 0xFFFF);
    rk->rkcs = (rk->rkcs & ~RKCS_MEX) | (uint16_t)((ma >> 12) & RKCS_MEX);

    uint32_t new_sector = sector + (wc + RK_NUMWD - 1) / RK_NUMWD;
    uint32_t cyl  = new_sector / (RK_NUMSF * RK_NUMSC);
    uint32_t surf = (new_sector / RK_NUMSC) % RK_NUMSF;
    uint32_t sect = new_sector % RK_NUMSC;
    rk->rkda = (rk->rkda & RKDA_DRIVE) |
               (uint16_t)(sect | (surf << 4) | (cyl << 5));

    rk05_update_err(rk);
    rk05_set_done(rk);
}

static int rk05_read(void *dev, uint32_t addr, uint16_t *data) {
    RK05 *rk = (RK05 *)dev;
    int reg = (addr >> 1) & 7;

    switch (reg) {
    case 0: {
        int drv = DA_DRIVE(rk->rkda);
        rk->rkds = RKDS_RK05 | RKDS_SC_OK | (uint16_t)(drv << 13);
        if (rk->disk[drv]) {
            rk->rkds |= RKDS_RDY | RKDS_RWS;
            if (rk->read_only[drv])
                rk->rkds |= RKDS_WLK;
        }
        *data = rk->rkds;
        break;
    }
    case 1:
        *data = rk->rker;
        break;
    case 2: rk05_update_err(rk); *data = rk->rkcs; break;
    case 3: *data = rk->rkwc; break;
    case 4: *data = rk->rkba; break;
    case 5: *data = rk->rkda;
        break;
    default:
        *data = 0;
        break;
    }
    return 0;
}

static int rk05_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    RK05 *rk = (RK05 *)dev;
    int reg = (addr >> 1) & 7;
    (void)is_byte;

    switch (reg) {
    case 2:
        rk->rkcs = (rk->rkcs & (RKCS_DONE | RKCS_SCP | RKCS_HERR | RKCS_ERR)) |
                   (data & ~(RKCS_DONE | RKCS_SCP | RKCS_HERR | RKCS_ERR | RKCS_GO));
        if ((data & RKCS_GO) && (rk->rkcs & RKCS_DONE)) {
            rk->rkcs &= ~RKCS_DONE;
            rk05_go(rk);
        }
        break;
    case 3: rk->rkwc = data; break;
    case 4: rk->rkba = data; break;
    case 5: rk->rkda = data; break;
    default:
        break;
    }
    return 0;
}

void rk05_init(RK05 *rk) {
    memset(rk, 0, sizeof(*rk));
    rk->rkcs = RKCS_DONE;
}

int rk05_attach(RK05 *rk, int drive, const char *filename, int read_only) {
    if (drive < 0 || drive >= RK_NUMDR)
        return -1;
    if (!read_only) {
        rk->disk[drive] = fopen(filename, "r+b");
        if (rk->disk[drive]) {
            rk->read_only[drive] = 0;
            return 0;
        }
    }
    rk->disk[drive] = fopen(filename, "rb");
    if (!rk->disk[drive]) {
        fprintf(stderr, "rk05: cannot open %s\n", filename);
        return -1;
    }
    rk->read_only[drive] = 1;
    return 0;
}

int rk05_register(RK05 *rk, Bus *bus) {
    rk->bus = bus;
    return bus_register(bus, RK05_BASE, RK05_END,
                        rk, rk05_read, rk05_write, "RK11-D", 150);
}
