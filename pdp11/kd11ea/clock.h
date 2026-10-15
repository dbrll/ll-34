/*
 * clock.h -- Real-time pacing for ll-34
 *
 * Synchronizes the simulation clock (cpu.ns_elapsed) with wall-clock time
 * so that the gate-simulated KD-11EA runs at real speed (5.5 MHz).
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef struct {
    uint64_t wall_start;        /* wall-clock ns at simulation start */
    uint64_t sim_start;         /* ns_elapsed at simulation start */
    uint64_t next_pace_ns;      /* next sim time to check pacing */
} Clock;

/* Initialize the clock (call just before entering the main loop) */
void clock_init(Clock *clk, uint64_t sim_ns);

/* Pace the simulation: sleep if we are running ahead of real time.
 * Call this after each ustep with the current sim time (cpu.ns_elapsed).
 * Returns 1 if we slept, 0 otherwise. */
int clock_pace(Clock *clk, uint64_t sim_ns);

#endif /* CLOCK_H */
