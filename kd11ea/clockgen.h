/*
 * clockgen.h: KD11-EA clock generator model (E106 delay line)
 *
 * The KD11-EA processor clock is a delay line (E106) with feedback taps.
 * Schematic reference: KD11-E Maintenance Manual, K1-5.
 *
 * Short cycle: TAP 90 feedback → 180 ns total.
 * Long cycle:  TAP 120 feedback → 240 ns total.
 *
 * During bus transfers (BUF_DAT_TRAN=1), TAP 30 asserts TRAN INH
 * which stops the clock until SSYN returns from the slave device.
 * The total bus cycle includes:
 *   - MSYN setup: 30 ns (TAP 30) + 150 ns (RC delay) = 180 ns
 *   - Slave SSYN response time (device-dependent)
 *   - EOT synchronization: 100 ns for DATI/DATIP, 0 for DATO/DATOB
 *   - Clock restart: ~30 ns
 *
 * (EK-KD11E-TM-001 §4.7.2.1, §4.7.2.8, §4.9)
 */

#ifndef CLOCKGEN_H
#define CLOCKGEN_H

#include <stdint.h>

/* Delay line tap positions (nanoseconds) */
#define TAP_30_NS          30   /* scratchpad latch, VBA load, bus start */
#define TAP_90_NS          90   /* short cycle feedback point */
#define TAP_120_NS        120   /* long cycle feedback point, LOAD BA */

/* Micro-cycle durations */
#define CYCLE_SHORT_NS    180   /* short cycle: 2 × TAP 90 */
#define CYCLE_LONG_NS     240   /* long cycle: 2 × TAP 120 */

/* Bus transfer timing */
#define MSYN_SETUP_NS     180   /* TAP30(30) + RC delay(150) → MSYN assert */
#define EOT_DATI_NS       100   /* EOT delay after SSYN for DATI/DATIP */
#define EOT_DATO_NS         0   /* DATO/DATOB: immediate TRAN INH release */
#define CLK_RESTART_NS     30   /* clock restart after TRAN INH release */

/* Compute the duration of one micro-cycle in nanoseconds.
 *
 *   buf_dat_tran  - 1 if a bus transfer is active (BUF_DAT_TRAN H)
 *   bus_ctl       - bus cycle type (BUS_DATI..BUS_DATOB)
 *   long_cycle_l  - LONG_CYCLE_L microword bit (1=short, 0=long)
 *   ssyn_ns       - slave SSYN response time (from bus->last_latency_ns)
 */
uint32_t clockgen_cycle_ns(int buf_dat_tran, int bus_ctl,
                           int long_cycle_l, uint32_t ssyn_ns);

/* Return the EOT delay for a given bus cycle type.
 * 100 ns for DATI/DATIP (read), 0 for DATO/DATOB (write). */
static inline uint32_t clockgen_eot_ns(int bus_ctl) {
    return (bus_ctl <= 1) ? EOT_DATI_NS : EOT_DATO_NS;  /* BUS_DATIP=1 */
}

#endif /* CLOCKGEN_H */
