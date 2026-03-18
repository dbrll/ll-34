/*
 * kw11.c -- KW11-L Line Clock for ll-34
 *
 * Simple 50/60 Hz clock: sets CLOCK MONITOR on each tick and raises
 * an interrupt on the UNIBUS (vector 100, BR6) if INT ENB is set.
 */

#include <stdio.h>
#include <string.h>
#include "kw11.h"
#include "../trace.h"

void kw11_init(KW11 *kw, int freq_hz) {
    memset(kw, 0, sizeof(*kw));
    if (freq_hz <= 0) freq_hz = 60;
    kw->tick_interval_ns = 1000000000ULL / (unsigned)freq_hz;
}

void kw11_tick(KW11 *kw, uint64_t now_ns) {
    if (kw->next_tick_ns == 0)
        kw->next_tick_ns = now_ns + kw->tick_interval_ns;

    if (now_ns >= kw->next_tick_ns) {
        kw->lks |= KW11_MON;           /* set CLOCK MONITOR */
        kw->next_tick_ns += kw->tick_interval_ns;
        /* Avoid falling behind if simulation was paused */
        if (kw->next_tick_ns < now_ns)
            kw->next_tick_ns = now_ns + kw->tick_interval_ns;

        if ((kw->lks & KW11_IE) && kw->irq_set) {
            trace("KW11: tick now=%llu next=%llu IE=%d\n",
                    (unsigned long long)now_ns,
                    (unsigned long long)kw->next_tick_ns,
                    !!(kw->lks & KW11_IE));
            kw->irq_set(kw->irq_ctx, KW11_VEC, KW11_PRI);
        }
    }
}

static int kw11_read(void *dev, uint32_t addr, uint16_t *data) {
    KW11 *kw = dev;
    (void)addr;
    *data = kw->lks;
    return 0;
}

static int kw11_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    KW11 *kw = dev;
    (void)addr; (void)is_byte;
    kw->lks = data & (KW11_MON | KW11_IE);
    /* If IE just got set and MON is already set, raise interrupt */
    if ((kw->lks & KW11_IE) && (kw->lks & KW11_MON) && kw->irq_set)
        kw->irq_set(kw->irq_ctx, KW11_VEC, KW11_PRI);
    return 0;
}

int kw11_register(KW11 *kw, Bus *bus) {
    return bus_register(bus, KW11_ADDR, KW11_ADDR + 1,
                        kw, kw11_read, kw11_write, "KW11-L", 100);
}
