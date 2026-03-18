#define _DEFAULT_SOURCE   /* clock_gettime, nanosleep on musl/glibc */
/*
 * clock.c -- Real-time pacing for ll-34
 *
 * Uses CLOCK_MONOTONIC to compare simulated time with wall-clock time.
 * When the simulation is running ahead (which it will, since a modern
 * CPU can execute thousands of PDP-11 micro-cycles in the time a real
 * KD11-EA takes for one), we sleep to let real time catch up.
 *
 * Pacing is checked every ~1 ms of simulated time to avoid excessive
 * clock_gettime() syscalls. The 100 µs threshold prevents micro-sleeps
 * that would be absorbed by OS scheduling granularity anyway.
 */

#include <time.h>
#include "clock.h"

#define PACE_INTERVAL_NS  1000000   /* check every 1 ms of sim time */
#define PACE_THRESHOLD_NS  100000   /* sleep only if >100 µs ahead */

static uint64_t wallclock_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

void clock_init(Clock *clk, uint64_t sim_ns) {
    clk->wall_start   = wallclock_ns();
    clk->sim_start    = sim_ns;
    clk->next_pace_ns = sim_ns + PACE_INTERVAL_NS;
}

int clock_pace(Clock *clk, uint64_t sim_ns) {
    if (sim_ns < clk->next_pace_ns)
        return 0;

    clk->next_pace_ns = sim_ns + PACE_INTERVAL_NS;

    uint64_t sim_elapsed  = sim_ns - clk->sim_start;
    uint64_t wall_elapsed = wallclock_ns() - clk->wall_start;

    if (sim_elapsed > wall_elapsed + PACE_THRESHOLD_NS) {
        uint64_t sleep_ns = sim_elapsed - wall_elapsed;
        struct timespec ts = {
            .tv_sec  = (time_t)(sleep_ns / 1000000000ULL),
            .tv_nsec = (long)(sleep_ns % 1000000000ULL)
        };
        nanosleep(&ts, NULL);
        return 1;
    }
    return 0;
}
