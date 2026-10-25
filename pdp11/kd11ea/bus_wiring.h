/* KD11-EA bus abort gates (MP00190 K2-1, K2-3, K2-4) */
#ifndef KD11EA_BUS_WIRING_H
#define KD11EA_BUS_WIRING_H
#include <stdint.h>


/* DISABLE MSYN+1 L (K2-4): E95, E20, E112 */
static inline uint8_t k2_4_disable_msyn_plus1_l(uint8_t disab_msyn_l,
                                              uint8_t r6_7_l, uint8_t bdt) {
    uint8_t e38_9 = r6_7_l;
    uint8_t e95_13 = !(e38_9 | bdt);
    uint8_t e20_13 = !(e95_13 | disab_msyn_l);
    return !e20_13;                    /* E112:6 */
}

typedef struct {
    uint8_t inhibit_msyn;  /* E42:3 */
    uint8_t abort_h;       /* E41:8 */
    uint8_t set_be_l;      /* E42:8, presets BE (E33) */
} K2BusAbort;

/* Abort network (K2-1): E42, E40, E23, E41. A KT fault aborts the cycle
 * but does not set BE. */
static inline K2BusAbort k2_1_bus_abort(uint8_t disable_msyn_plus1_l,
                                       uint8_t kt_fault_l, uint8_t clk_msyn_h,
                                       uint8_t enab_abort_h, uint8_t parity_h,
                                       uint8_t to_h) {
    K2BusAbort w;
    w.inhibit_msyn = !(disable_msyn_plus1_l & kt_fault_l);
    uint8_t e40_8 = !(clk_msyn_h & enab_abort_h & w.inhibit_msyn);
    uint8_t e23_1 = !(parity_h | to_h);
    w.abort_h = !(e40_8 & e23_1);
    uint8_t e42_11 = !(disable_msyn_plus1_l & !to_h);
    w.set_be_l = !(w.abort_h & e42_11);
    return w;
}

/* Double bus error (K2-3 E43) */
static inline uint8_t k2_3_dbe_h(uint8_t set_be_l, uint8_t enab_dbe_l) {
    return !(set_be_l | enab_dbe_l);
}
#endif
