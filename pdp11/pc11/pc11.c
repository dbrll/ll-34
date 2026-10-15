/*
 * pc11.c -- PC11 paper tape reader/punch for ll-34
 *
 * Reader: 300 chars/sec, GO starts read, DONE when byte ready.
 * Punch: 50 chars/sec, write PPB starts punch, DONE when complete.
 */

#include <stdio.h>
#include <string.h>
#include "pc11.h"
#include "../trace.h"

void pc11_init(PC11 *pc) {
    memset(pc, 0, sizeof(*pc));
    pc->pps = PPS_DONE;  /* punch starts ready */
    pc->reader_char_ns = 3333333ULL;   /* 300 chars/sec */
    pc->punch_char_ns = 20000000ULL;  /* 50 chars/sec */
}

int pc11_attach_reader(PC11 *pc, const char *filename) {
    pc->reader = fopen(filename, "rb");
    if (!pc->reader) {
        fprintf(stderr, "pc11: cannot open reader file %s\n", filename);
        pc->prs |= PRS_ERR;
        return -1;
    }
    pc->prs &= ~PRS_ERR;
    return 0;
}

int pc11_attach_punch(PC11 *pc, const char *filename) {
    pc->punch = fopen(filename, "wb");
    if (!pc->punch) {
        fprintf(stderr, "pc11: cannot open punch file %s\n", filename);
        pc->pps |= PPS_ERR;
        return -1;
    }
    pc->pps &= ~PPS_ERR;
    return 0;
}

static void pc11_reader_service(PC11 *pc) {
    pc->prs &= ~PRS_BUSY;

    if (!pc->reader) {
        pc->prs = (pc->prs | PRS_ERR | PRS_DONE) & ~PRS_BUSY;
        pc->reader_done_ns = 0;
        if ((pc->prs & PRS_IE) && pc->irq_set)
            pc->irq_set(pc->irq_ctx, PC11_PTR_VEC, PC11_PRI);
        return;
    }

    int ch = getc(pc->reader);
    if (ch == EOF) {
        pc->prs = (pc->prs | PRS_ERR | PRS_DONE) & ~PRS_BUSY;
        pc->reader_done_ns = 0;
        trace("PC11: reader EOF\n");
    } else {
        pc->prb = (uint16_t)(ch & 0xFF);
        pc->prs = (pc->prs | PRS_DONE) & ~(PRS_BUSY | PRS_ERR);
        pc->reader_done_ns = 0;
    }

    if ((pc->prs & PRS_IE) && pc->irq_set)
        pc->irq_set(pc->irq_ctx, PC11_PTR_VEC, PC11_PRI);
}

static void pc11_punch_service(PC11 *pc) {
    if (!pc->punch) {
        pc->pps |= PPS_ERR | PPS_DONE;
        pc->punch_done_ns = 0;
        if ((pc->pps & PPS_IE) && pc->irq_set)
            pc->irq_set(pc->irq_ctx, PC11_PTP_VEC, PC11_PRI);
        return;
    }

    if (putc(pc->ppb & 0xFF, pc->punch) == EOF) {
        pc->pps |= PPS_ERR | PPS_DONE;
        trace("PC11: punch I/O error\n");
    } else {
        fflush(pc->punch);
        pc->pps = (pc->pps | PPS_DONE) & ~PPS_ERR;
    }
    pc->punch_done_ns = 0;

    if ((pc->pps & PPS_IE) && pc->irq_set)
        pc->irq_set(pc->irq_ctx, PC11_PTP_VEC, PC11_PRI);
}

void pc11_tick(PC11 *pc, uint64_t now_ns) {
    if ((pc->prs & PRS_BUSY) && pc->reader_done_ns > 0
        && now_ns >= pc->reader_done_ns) {
        pc11_reader_service(pc);
    }

    if (!(pc->pps & PPS_DONE) && pc->punch_done_ns > 0
        && now_ns >= pc->punch_done_ns) {
        pc11_punch_service(pc);
    }
}

static int pc11_read(void *dev, uint32_t addr, uint16_t *data) {
    PC11 *pc = (PC11 *)dev;
    uint32_t reg = addr & ~1u;

    switch (reg) {
    case 0x3FF68:  /* PRS */
        /* Inline completion for accurate busy-wait */
        if ((pc->prs & PRS_BUSY) && pc->reader_done_ns > 0
            && pc->clock_ptr && *pc->clock_ptr >= pc->reader_done_ns) {
            pc11_reader_service(pc);
        }
        *data = pc->prs;
        return 0;

    case 0x3FF6A:  /* PRB */
        *data = pc->prb & 0xFF;
        pc->prs &= ~PRS_DONE;
        if (pc->irq_clr)
            pc->irq_clr(pc->irq_ctx, PC11_PTR_VEC);
        return 0;

    case 0x3FF6C:  /* PPS */
        if (!(pc->pps & PPS_DONE) && pc->punch_done_ns > 0
            && pc->clock_ptr && *pc->clock_ptr >= pc->punch_done_ns) {
            pc11_punch_service(pc);
        }
        *data = pc->pps;
        return 0;

    case 0x3FF6E:  /* PPB */
        *data = 0;
        return 0;
    }
    *data = 0;
    return -1;
}

static int pc11_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    PC11 *pc = (PC11 *)dev;
    uint32_t reg = addr & ~1u;
    (void)is_byte;

    switch (reg) {
    case 0x3FF68: {  /* PRS */
        uint16_t old_ie = pc->prs & PRS_IE;
        pc->prs = (pc->prs & ~PRS_IE) | (data & PRS_IE);

        if (!(old_ie) && (data & PRS_IE)
            && (pc->prs & (PRS_DONE | PRS_ERR)) && pc->irq_set)
            pc->irq_set(pc->irq_ctx, PC11_PTR_VEC, PC11_PRI);

        if (old_ie && !(data & PRS_IE) && pc->irq_clr)
            pc->irq_clr(pc->irq_ctx, PC11_PTR_VEC);

        if ((data & PRS_GO) && !(pc->prs & PRS_BUSY)) {
            pc->prs = (pc->prs & ~(PRS_DONE | PRS_ERR)) | PRS_BUSY;
            if (pc->irq_clr)
                pc->irq_clr(pc->irq_ctx, PC11_PTR_VEC);
            if (pc->reader && pc->clock_ptr) {
                pc->reader_done_ns = *pc->clock_ptr + pc->reader_char_ns;
            } else {
                pc->prs = (pc->prs | PRS_ERR | PRS_DONE) & ~PRS_BUSY;
                if ((pc->prs & PRS_IE) && pc->irq_set)
                    pc->irq_set(pc->irq_ctx, PC11_PTR_VEC, PC11_PRI);
            }
        }
        return 0;
    }

    case 0x3FF6A:  /* PRB */
        return 0;

    case 0x3FF6C: {  /* PPS */
        uint16_t old_ie = pc->pps & PPS_IE;
        pc->pps = (pc->pps & ~PPS_IE) | (data & PPS_IE);

        if (!(old_ie) && (data & PPS_IE)
            && (pc->pps & (PPS_DONE | PPS_ERR)) && pc->irq_set)
            pc->irq_set(pc->irq_ctx, PC11_PTP_VEC, PC11_PRI);

        if (old_ie && !(data & PPS_IE) && pc->irq_clr)
            pc->irq_clr(pc->irq_ctx, PC11_PTP_VEC);
        return 0;
    }

    case 0x3FF6E:  /* PPB */
        pc->ppb = data & 0xFF;
        pc->pps &= ~PPS_DONE;
        if (pc->irq_clr)
            pc->irq_clr(pc->irq_ctx, PC11_PTP_VEC);
        if (pc->punch && pc->clock_ptr) {
            pc->punch_done_ns = *pc->clock_ptr + pc->punch_char_ns;
        } else {
            pc->pps |= PPS_ERR | PPS_DONE;
            if ((pc->pps & PPS_IE) && pc->irq_set)
                pc->irq_set(pc->irq_ctx, PC11_PTP_VEC, PC11_PRI);
        }
        return 0;
    }
    return -1;
}

int pc11_register(PC11 *pc, Bus *bus) {
    return bus_register(bus, PC11_BASE, PC11_END,
                        pc, pc11_read, pc11_write, "PC11", 150);
}
