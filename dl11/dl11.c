/*
 * dl11.c -- DL11 serial line interface for ll-34
 *
 * XBUF write clears XMIT RDY for one character time at the configured
 * baud rate (~1 ms at 9600) to match real hardware busy-wait timing.
 */

#include <stdio.h>
#include <string.h>
#include "dl11.h"
#include "../trace.h"

void dl11_init(DL11 *dl, uint32_t baud_rate) {
    memset(dl, 0, sizeof(*dl));
    dl->xcsr = DL11_XCSR_RDY;  /* Transmitter starts ready */

    /* Character time: 10 bits per char (8N1 = start + 8 data + stop).
     * char_time_ns = 10 * 1e9 / baud_rate */
    if (baud_rate == 0) baud_rate = 9600;
    dl->char_time_ns = (uint64_t)10 * 1000000000ULL / baud_rate;
}

void dl11_tick(DL11 *dl, uint64_t now_ns) {
    /* TX completion */
    if (!(dl->xcsr & DL11_XCSR_RDY) && dl->tx_done_at_ns > 0) {
        if (now_ns >= dl->tx_done_at_ns) {
            dl->xcsr |= DL11_XCSR_RDY;
            dl->tx_done_at_ns = 0;
            /* IRQ on RDY 0->1 edge */
            if ((dl->xcsr & DL11_XCSR_IE) && dl->irq_set)
                dl->irq_set(dl->irq_ctx, 0064, 4);
        }
    }

    /* RX poll */
    if (!(dl->rcsr & DL11_RCSR_DONE)) {
        if (dl->rx_ready && dl->rx_ready(dl->io_ctx)) {
            int ch = dl->rx_read(dl->io_ctx);
            if (ch >= 0) {
                dl->rbuf = ch & 0xFF;
                dl->rcsr |= DL11_RCSR_DONE;
                /* IRQ on DONE 0->1 edge */
                if ((dl->rcsr & DL11_RCSR_IE) && dl->irq_set)
                    dl->irq_set(dl->irq_ctx, 0060, 4);
            }
        }
    }
}

static int dl11_read(void *dev, uint32_t addr, uint16_t *data) {
    DL11 *dl = dev;
    uint32_t reg = addr & ~1u;

    switch (reg) {
    case DL11_RCSR:
        /* Inline RX poll for accurate busy-wait (tick is every 256 usteps) */
        if (!(dl->rcsr & DL11_RCSR_DONE)) {
            if (dl->rx_ready && dl->rx_ready(dl->io_ctx)) {
                int ch = dl->rx_read(dl->io_ctx);
                if (ch >= 0) {
                    dl->rbuf = ch & 0xFF;
                    dl->rcsr |= DL11_RCSR_DONE;
                }
            }
        }
        *data = dl->rcsr;
        return 0;
    case DL11_RBUF:
        *data = dl->rbuf;
        dl->rcsr &= ~DL11_RCSR_DONE;  /* Reading RBUF clears DONE */
        return 0;
    case DL11_XCSR: {
        /* Inline TX completion check */
        if (!(dl->xcsr & DL11_XCSR_RDY) && dl->tx_done_at_ns > 0) {
            if (dl->clock_ptr && *dl->clock_ptr >= dl->tx_done_at_ns) {
                dl->xcsr |= DL11_XCSR_RDY;
                dl->tx_done_at_ns = 0;
                if ((dl->xcsr & DL11_XCSR_IE) && dl->irq_set)
                    dl->irq_set(dl->irq_ctx, 0064, 4);
            }
        }
        *data = dl->xcsr;
        return 0;
    }
    case DL11_XBUF:
        *data = 0;
        return 0;
    }
    *data = 0;
    return -1;
}

static int dl11_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    DL11 *dl = dev;
    uint32_t reg = addr & ~1u;

    switch (reg) {
    case DL11_RCSR: {
        uint16_t old_ie = dl->rcsr & DL11_RCSR_IE;
        dl->rcsr = (dl->rcsr & ~DL11_RCSR_IE) | (data & DL11_RCSR_IE);
        uint16_t new_ie = dl->rcsr & DL11_RCSR_IE;
        if (old_ie != new_ie)
            trace("DL11: RCSR IE %s (DONE=%d)\n",
                    new_ie ? "SET" : "CLR",
                    !!(dl->rcsr & DL11_RCSR_DONE));
        if ((dl->rcsr & DL11_RCSR_IE) && (dl->rcsr & DL11_RCSR_DONE) && dl->irq_set)
            dl->irq_set(dl->irq_ctx, 0060, 4);
        return 0;
    }
    case DL11_RBUF:
        return 0;
    case DL11_XCSR: {
        uint16_t old_ie = dl->xcsr & DL11_XCSR_IE;
        dl->xcsr = (dl->xcsr & ~DL11_XCSR_IE) | (data & DL11_XCSR_IE);
        uint16_t new_ie = dl->xcsr & DL11_XCSR_IE;
        if (old_ie != new_ie)
            trace("DL11: XCSR IE %s (RDY=%d)\n",
                    new_ie ? "SET" : "CLR",
                    !!(dl->xcsr & DL11_XCSR_RDY));
        if ((dl->xcsr & DL11_XCSR_IE) && (dl->xcsr & DL11_XCSR_RDY) && dl->irq_set)
            dl->irq_set(dl->irq_ctx, 0064, 4);
        return 0;
    }
    case DL11_XBUF: {
        uint8_t ch = data & 0x7F;
        if (dl->tx_write)
            dl->tx_write(dl->io_ctx, ch);
        dl->xcsr &= ~DL11_XCSR_RDY;
        if (dl->clock_ptr)
            dl->tx_done_at_ns = *dl->clock_ptr + dl->char_time_ns;
        return 0;
    }
    }
    (void)is_byte;
    return -1;
}

int dl11_register(DL11 *dl, Bus *bus) {
    return bus_register(bus, DL11_BASE, DL11_END,
                        dl, dl11_read, dl11_write, "DL11", 150);
}
