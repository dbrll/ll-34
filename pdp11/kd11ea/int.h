/* Interrupt requests on BR7-BR4. A device asserts its request with
 * int_request() and withdraws it with int_cancel(). The CPU takes the
 * highest request above its priority. */

#ifndef KD11EA_INT_H
#define KD11EA_INT_H

#include <stdint.h>

#define INT_MAX_PENDING 8

typedef struct {
    uint16_t vector;
    uint8_t  priority;   /* 4 to 7 */
} IntRequest;

typedef struct {
    IntRequest queue[INT_MAX_PENDING];
    int count;
} IntController;

void int_init(IntController *ic);

/* A vector already pending is not added twice. Returns -1 if the queue
 * is full. */
int int_request(IntController *ic, uint16_t vector, uint8_t priority);
void int_cancel(IntController *ic, uint16_t vector);

/* Index of the highest request above cpu_priority, or -1. */
int int_pending(IntController *ic, uint8_t cpu_priority);

/* The BR7-BR4 lines, as bits 7-4. */
uint8_t int_br_lines(const IntController *ic);

/* Remove the highest request and return it. Call it only after
 * int_pending() found one. */
IntRequest int_ack(IntController *ic, uint8_t cpu_priority);

#endif
