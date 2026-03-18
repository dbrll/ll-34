/*
 * pc11.h -- PC11 paper tape reader/punch
 *
 * High-speed paper tape reader (300 chars/sec)
 * and punch (50 chars/sec). Register pairs:
 *
 *   177550  PRS  Reader Status        (read/write)
 *   177552  PRB  Reader Buffer        (read-only)
 *   177554  PPS  Punch Status         (read/write)
 *   177556  PPB  Punch Buffer         (write-only)
 *
 * Interrupts: vector 070 (reader), vector 074 (punch), both BR4.
 */

#ifndef PC11_H
#define PC11_H

#include <stdio.h>
#include <stdint.h>
#include "../unibus/unibus.h"

/* Register addresses (18-bit) */
#define PC11_BASE   0x3FF68  /* 777550 octal */
#define PC11_END    0x3FF6F  /* 777557 octal */

/* PRS (Reader Status) bits */
#define PRS_GO      0x0001  /* bit 0: start read (write-only) */
#define PRS_IE      0x0040  /* bit 6: interrupt enable */
#define PRS_DONE    0x0080  /* bit 7: data ready */
#define PRS_BUSY    0x0800  /* bit 11: read in progress */
#define PRS_ERR     0x8000  /* bit 15: error (no tape, EOF) */

/* PPS (Punch Status) bits */
#define PPS_IE      0x0040  /* bit 6: interrupt enable */
#define PPS_DONE    0x0080  /* bit 7: ready for next byte */
#define PPS_ERR     0x8000  /* bit 15: error (no punch file) */

/* Interrupt vectors */
#define PC11_PTR_VEC    0070  /* reader vector (octal) */
#define PC11_PTP_VEC    0074  /* punch vector (octal) */
#define PC11_PRI        4     /* BR4 */

typedef struct {
    /* Registers */
    uint16_t prs;
    uint16_t prb;
    uint16_t pps;
    uint16_t ppb;

    /* Host file handles */
    FILE    *reader;        /* paper tape input file */
    FILE    *punch;         /* paper tape output file */

    /* Timing */
    uint64_t reader_done_ns;    /* when current read completes */
    uint64_t punch_done_ns;     /* when current punch completes */
    uint64_t reader_char_ns;    /* reader character time (default 3.33 ms) */
    uint64_t punch_char_ns;     /* punch character time (default 20 ms) */
    uint64_t *clock_ptr;        /* pointer to cpu.ns_elapsed */

    /* Interrupt callbacks */
    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void (*irq_clr)(void *ctx, uint16_t vector);
    void *irq_ctx;
} PC11;

/* Initialize the PC11 */
void pc11_init(PC11 *pc);

/* Attach a paper tape image for reading (binary file) */
int pc11_attach_reader(PC11 *pc, const char *filename);

/* Attach an output file for the punch */
int pc11_attach_punch(PC11 *pc, const char *filename);

/* Periodic update (call from tick loop) */
void pc11_tick(PC11 *pc, uint64_t now_ns);

/* Register PC11 on the bus */
int pc11_register(PC11 *pc, Bus *bus);

#endif /* PC11_H */
