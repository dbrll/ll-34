/* DL11 serial line, the console terminal. RCSR 177560, RBUF 177562, XCSR
 * 177564 and XBUF 177566. The receiver interrupts at 060, the transmitter
 * at 064. Writing XBUF clears XMIT RDY for one character time, ten bits at
 * the baud rate. */

#ifndef DL11_H
#define DL11_H

#include <stdint.h>
#include "../unibus/unibus.h"

#define DL11_BASE   0x3FF70  /* 777560 */
#define DL11_END    0x3FF77
#define DL11_RCSR   0x3FF70
#define DL11_RBUF   0x3FF72
#define DL11_XCSR   0x3FF74
#define DL11_XBUF   0x3FF76

#define DL11_RCSR_DONE  0x0080
#define DL11_RCSR_IE    0x0040
#define DL11_XCSR_RDY   0x0080
#define DL11_XCSR_IE    0x0040

typedef struct {
    uint8_t init_l;
    uint16_t rcsr;
    uint16_t rbuf;
    uint16_t xcsr;

    uint64_t tx_done_at_ns;     /* end of the character being sent, 0 idle */
    uint64_t char_time_ns;
    const uint64_t *clock_ptr;  /* the CPU simulated time */

    /* The host terminal */
    int  (*rx_ready)(void *ctx);
    int  (*rx_read)(void *ctx);
    void (*tx_write)(void *ctx, uint8_t ch);
    void *io_ctx;

    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void (*irq_clr)(void *ctx, uint16_t vector);
    void *irq_ctx;
} DL11;

void dl11_init(DL11 *dl, uint32_t baud_rate);

/* Finish a transmission that is due and take a received character, if
 * the host has one. now_ns is the simulated time. */
void dl11_tick(DL11 *dl, uint64_t now_ns);

int dl11_register(DL11 *dl, Bus *bus);

#endif
