/*
 * dl11.h -- DL11 serial line interface for ll-34
 *
 * The DL11 provides a single asynchronous serial line (console terminal).
 * It has 4 UNIBUS registers:
 *
 *   177560  RCSR  Receiver Control/Status
 *              [7] RCVR DONE  -- character available in RBUF
 *              [6] RCVR IE    -- interrupt enable (vector 060)
 *   177562  RBUF  Receiver Data Buffer (read-only)
 *              [7:0] received character
 *   177564  XCSR  Transmitter Control/Status
 *              [7] XMIT RDY  -- transmitter ready for next character
 *              [6] XMIT IE   -- interrupt enable (vector 064)
 *   177566  XBUF  Transmitter Data Buffer (write-only)
 *              [7:0] character to transmit
 *
 * UART timing: writing to XBUF clears XMIT RDY and starts a transmission
 * at the configured baud rate. XMIT RDY goes high again after one character
 * time (10 bits at baud rate for 8N1: start + 8 data + stop).
 */

#ifndef DL11_H
#define DL11_H

#include <stdint.h>
#include "../unibus/unibus.h"

/* DL11 register addresses */
#define DL11_BASE   0x3FF70  /* 777560 octal (18-bit) */
#define DL11_END    0x3FF77  /* 777567 octal (18-bit) */
#define DL11_RCSR   0x3FF70  /* 777560 */
#define DL11_RBUF   0x3FF72  /* 777562 */
#define DL11_XCSR   0x3FF74  /* 777564 */
#define DL11_XBUF   0x3FF76  /* 777566 */

/* CSR bits */
#define DL11_RCSR_DONE  0x0080  /* bit 7: character available */
#define DL11_RCSR_IE    0x0040  /* bit 6: receiver interrupt enable */
#define DL11_XCSR_RDY   0x0080  /* bit 7: transmitter ready */
#define DL11_XCSR_IE    0x0040  /* bit 6: transmitter interrupt enable */

typedef struct {
    uint16_t rcsr;      /* Receiver CSR */
    uint16_t rbuf;      /* Receiver data */
    uint16_t xcsr;      /* Transmitter CSR */

    /* UART transmit timing.
     * When a character is written to XBUF, XCSR[7] (XMIT RDY) goes LOW.
     * After char_time_ns nanoseconds, XMIT RDY goes HIGH again.
     * The simulation clock is read via clock_ptr (points to cpu.ns_elapsed). */
    uint64_t tx_done_at_ns;     /* ns_elapsed when TX completes (0 = idle) */
    uint64_t char_time_ns;      /* nanoseconds per character at baud rate */
    const uint64_t *clock_ptr;  /* points to cpu.ns_elapsed */

    /* Host terminal I/O callbacks */
    int  (*rx_ready)(void *ctx);                    /* non-zero if char available */
    int  (*rx_read)(void *ctx);                     /* read one char (0-255) */
    void (*tx_write)(void *ctx, uint8_t ch);        /* write one char */
    void *io_ctx;                                   /* context for callbacks */

    /* Interrupt callbacks (connected to CPU interrupt controller) */
    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void *irq_ctx;
} DL11;

void dl11_init(DL11 *dl, uint32_t baud_rate);

/* Advance DL11 state: check TX completion and poll for RX.
 * now_ns is the CPU's ns_elapsed value. */
void dl11_tick(DL11 *dl, uint64_t now_ns);

/* Register DL11 on the bus */
int dl11_register(DL11 *dl, Bus *bus);

#endif /* DL11_H */
