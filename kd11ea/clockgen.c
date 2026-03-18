/*
 * clockgen.c: KD11-EA clock generator model (E106 delay line)
 *
 * Models the delay line oscillator on schematic K1-5.
 * Given the microword control signals, computes the duration
 * of each micro-cycle in nanoseconds.
 */

#include "clockgen.h"

uint32_t clockgen_cycle_ns(int buf_dat_tran, int bus_ctl,
                           int long_cycle_l, uint32_t ssyn_ns)
{
    if (buf_dat_tran) {
        /* Bus transfer: clock stops at TAP 30 (TRAN INH), resumes
         * after slave returns SSYN.  Total =
         *   MSYN_SETUP (180) + Tssyn (device) + EOT + CLK_RESTART */
        return MSYN_SETUP_NS
             + ssyn_ns
             + clockgen_eot_ns(bus_ctl)
             + CLK_RESTART_NS;
    }

    /* No bus transfer: short (180 ns) or long (240 ns) cycle.
     * LONG_CYCLE_L is active-low: 1 = short, 0 = long. */
    return long_cycle_l ? CYCLE_SHORT_NS : CYCLE_LONG_NS;
}
