/* KD11-EA clock and bus timing */
#ifndef CLOCKGEN_H
#define CLOCKGEN_H
#include <stdint.h>
#define CYCLE_SHORT_NS 180 /* nominal, for frontend pacing */
typedef enum { TIMING_NOMINAL=0, TIMING_AGED=1 } TimingKind;
typedef struct {
    const char *name;
    uint32_t short_ns, long_ns;         /* K1-5 cycle periods */
    uint32_t msyn_rc_ns, msyn_logic_ns; /* K2-1 MSYN delay */
    uint32_t read_eot_ns, write_ack_ns; /* end of transfer after SSYN */
    uint32_t grant_rc_ns, grant_logic_ns, restart_ns; /* K2-2 BUT SERVICE */
    uint32_t sack_rc_ns, device_intr_ns;  /* interrupt grant to SACK */
    uint32_t reset_ns;                  /* RESET instruction, E16 (K2-2) */
} ClockTiming;
extern const ClockTiming clock_nominal, clock_aged;
/* Low/high tolerance cases, for tests */
extern const ClockTiming clock_aged_low, clock_aged_high;
static inline const ClockTiming *clock_timing(const ClockTiming *p) {
    return p ? p : &clock_nominal;
}
uint32_t clockgen_period_ns(const ClockTiming *p,int long_cycle_l,int load_ba,int relocate);
uint32_t clockgen_msyn_ns(const ClockTiming *p,uint32_t period_ns);
uint32_t clockgen_cycle_ns(const ClockTiming *p,uint32_t period_ns,int transfer,
                         int bus_ctl,uint32_t ssyn_ns,int but_service,int intr);
static inline uint32_t clockgen_eot_ns(int bus_ctl) { return bus_ctl<=1?100u:28u; }
#endif
