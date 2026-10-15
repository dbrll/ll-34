/*
 * int.c -- UNIBUS interrupt controller for KD11-EA
 *
 * Simple priority queue (max 8 entries). BR7 is highest priority.
 * When multiple requests share the same BR level, the first one
 * registered wins (approximates UNIBUS daisy-chain grant order).
 */

#include "int.h"
#include "rom_wiring.h"

void int_init(IntController *ic) {
    ic->count = 0;
}

int int_request(IntController *ic, uint16_t vector, uint8_t priority) {
    /* Check if already pending */
    for (int i = 0; i < ic->count; i++) {
        if (ic->queue[i].vector == vector)
            return 0;
    }
    if (ic->count >= INT_MAX_PENDING)
        return -1;
    ic->queue[ic->count].vector   = vector;
    ic->queue[ic->count].priority = priority;
    ic->count++;
    return 0;
}

void int_cancel(IntController *ic, uint16_t vector) {
    for (int i = 0; i < ic->count; i++) {
        if (ic->queue[i].vector == vector) {
            ic->queue[i] = ic->queue[ic->count - 1];
            ic->count--;
            return;
        }
    }
}

int int_pending(IntController *ic, uint8_t cpu_priority) {
    uint8_t br = 0;
    for (int i = 0; i < ic->count; i++) {
        if (ic->queue[i].priority >= 4 && ic->queue[i].priority <= 7)
            br |= 1u << ic->queue[i].priority;
    }

    uint8_t d = e29_rom[e29_addr(cpu_priority, 0,
                                  (br >> 7) & 1, (br >> 6) & 1,
                                  (br >> 5) & 1, (br >> 4) & 1)];
    uint8_t grant = E29_BG7(d) ? 7 : E29_BG6(d) ? 6 :
                    E29_BG5(d) ? 5 : E29_BG4(d) ? 4 : 0;

    if (!grant)
        return -1;
    for (int i = 0; i < ic->count; i++) {
        if (ic->queue[i].priority == grant)
            return i;
    }
    return -1;
}

IntRequest int_ack(IntController *ic, uint8_t cpu_priority) {
    int idx = int_pending(ic, cpu_priority);
    IntRequest req = ic->queue[idx];
    ic->queue[idx] = ic->queue[ic->count - 1];
    ic->count--;
    return req;
}
