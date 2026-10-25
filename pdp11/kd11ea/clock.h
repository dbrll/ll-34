/* Real-time pacing. It keeps the simulated time in step with the wall
 * clock, so that the KD11-EA runs at its real speed. */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef struct {
    uint64_t wall_start;
    uint64_t sim_start;
    uint64_t next_pace_ns;
} Clock;

/* Call it just before the main loop, and again after a pause. */
void clock_init(Clock *clk, uint64_t sim_ns);

/* Sleep if the simulation is ahead of real time. Returns 1 if it slept. */
int clock_pace(Clock *clk, uint64_t sim_ns);

#endif
