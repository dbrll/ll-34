/* Line clock at 177546, with the behaviour of the DL11-W. Bit 7 is set at
 * each edge of the mains clock and cleared by writing 0. With INT ENB (bit
 * 6) set, the next edge requests an interrupt at 100, BR6. Setting INT ENB
 * alone does not interrupt, and clearing it withdraws the request (DL11-W
 * technical manual, 4.3.5.2). */

#ifndef KW11_H
#define KW11_H

#include <stdint.h>
#include "../unibus/unibus.h"

#define KW11_ADDR   0x3FF66  /* 777546 */
#define KW11_VEC    0100
#define KW11_PRI    6

#define KW11_MON    0x0080
#define KW11_IE     0x0040

typedef struct {
    uint8_t init_l;
    uint16_t lks;
    uint64_t next_tick_ns;
    uint64_t tick_interval_ns;
    const uint64_t *clock_ptr;  /* the CPU simulated time */

    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void (*irq_clr)(void *ctx, uint16_t vector);
    void *irq_ctx;
} KW11;

void kw11_init(KW11 *kw, int freq_hz);
void kw11_tick(KW11 *kw, uint64_t now_ns);
int kw11_register(KW11 *kw, Bus *bus);

#endif
