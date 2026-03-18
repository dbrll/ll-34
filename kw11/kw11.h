/*
 * kw11.h -- KW11-L Line Clock for ll-34
 *
 * The KW11-L generates a 50 Hz or 60 Hz interrupt from the AC power line.
 * It has a single UNIBUS register:
 *
 *   177546  LKS  Line Clock Status
 *              [7] CLOCK MONITOR -- set on each tick, cleared by writing 0
 *              [6] INT ENB       -- interrupt enable (vector 100, BR6)
 *
 * The interrupt fires at vector 100 priority BR6 on each tick when
 * INT ENB is set.
 */

#ifndef KW11_H
#define KW11_H

#include <stdint.h>
#include "../unibus/unibus.h"

#define KW11_ADDR   0x3FF66  /* 777546 octal (18-bit) */
#define KW11_VEC    0100    /* vector 100 */
#define KW11_PRI    6       /* BR6 */

#define KW11_MON    0x0080  /* bit 7: clock monitor */
#define KW11_IE     0x0040  /* bit 6: interrupt enable */

typedef struct {
    uint16_t lks;               /* LKS register */
    uint64_t next_tick_ns;      /* sim time of next clock tick */
    uint64_t tick_interval_ns;  /* ns between ticks (16666667 for 60Hz) */
    const uint64_t *clock_ptr;  /* points to cpu.ns_elapsed */

    /* Interrupt callbacks */
    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void *irq_ctx;
} KW11;

void kw11_init(KW11 *kw, int freq_hz);

/* Advance clock state: check if a tick has occurred. */
void kw11_tick(KW11 *kw, uint64_t now_ns);

/* Register KW11-L on the bus */
int kw11_register(KW11 *kw, Bus *bus);

#endif /* KW11_H */
