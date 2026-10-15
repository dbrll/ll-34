/*
 * int.h -- UNIBUS interrupt controller for KD11-EA
 *
 * Models the BR4-BR7 priority arbitration on the UNIBUS.
 * Devices call int_request() to assert an interrupt; the CPU checks
 * int_pending() between instructions and calls int_ack() to service
 * the highest-priority request whose BR level exceeds the CPU priority.
 */

#ifndef KD11EA_INT_H
#define KD11EA_INT_H

#include <stdint.h>

#define INT_MAX_PENDING 8

typedef struct {
    uint16_t vector;
    uint8_t  priority;   /* BR level: 4-7 */
} IntRequest;

typedef struct {
    IntRequest queue[INT_MAX_PENDING];
    int count;
} IntController;

/* Initialize the interrupt controller (clear all pending requests) */
void int_init(IntController *ic);

/* Assert an interrupt request. If the same vector is already pending,
 * this is a no-op. Returns 0 on success, -1 if queue is full. */
int int_request(IntController *ic, uint16_t vector, uint8_t priority);

/* Cancel a pending interrupt request by vector number. */
void int_cancel(IntController *ic, uint16_t vector);

/* Check if any pending request has priority > cpu_priority.
 * Returns the index of the highest-priority request, or -1. */
int int_pending(IntController *ic, uint8_t cpu_priority);

/* Acknowledge the highest-priority interrupt: removes it from the
 * queue and returns it. Only call after int_pending() returned >= 0. */
IntRequest int_ack(IntController *ic, uint8_t cpu_priority);

#endif /* KD11EA_INT_H */
