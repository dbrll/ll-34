/*
 * kw11.c -- line clock for ll-34, as on the DL11-W (M7856) of the 11/34A
 *
 * 50/60 Hz clock: sets CLOCK MONITOR on each tick and, if INT ENB is set,
 * requests an interrupt (vector 100, BR6) on that tick. INT ENB low clears
 * the request flip-flop. Setting it does not interrupt by itself, the next
 * line clock edge does (EK-DL11W-TM-002 4.3.5.2).
 */

#include <stdio.h>
#include <string.h>
#include "kw11.h"
#include "../trace.h"

void kw11_init(KW11 *kw, int freq_hz) {
    memset(kw, 0, sizeof(*kw));
    kw->init_l = 1;
    if (freq_hz <= 0) freq_hz = 60;
    kw->tick_interval_ns = 1000000000ULL / (unsigned)freq_hz;
}

void kw11_tick(KW11 *kw, uint64_t now_ns) {
    if (!kw->init_l) return;
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
    if (!kw->init_l) return 0;
    kw->lks = data & (KW11_MON | KW11_IE);
    /* INT ENB low direct-clears the interrupt request flip-flop */
    if (!(kw->lks & KW11_IE) && kw->irq_clr)
        kw->irq_clr(kw->irq_ctx, KW11_VEC);
    return 0;
}

/* BUS INIT: clear IE, set MON (DEC-11-HKWB-D 3.4) */
static void kw11_bus_init(void *dev, int init_l) {
    KW11 *kw = dev;
    kw->init_l = (uint8_t)init_l;
    if (init_l) return;
    kw->lks = KW11_MON;
    if (kw->irq_clr) kw->irq_clr(kw->irq_ctx, KW11_VEC);
}

int kw11_register(KW11 *kw, Bus *bus) {
    if (bus_register_init(bus, kw, kw11_bus_init) < 0) return -1;
    return bus_register(bus, KW11_ADDR, KW11_ADDR + 1,
                        kw, kw11_read, kw11_write, "KW11-L", 100);
}
