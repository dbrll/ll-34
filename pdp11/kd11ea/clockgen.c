#include "clockgen.h"
/* KD11-EA clock and bus timing.
 * nominal: DEC nominal values. aged: the cycle times, bus transfers, BUT SERVICE
 * and the RESET pulse measured on the actual PDP-11/34A. */
const ClockTiming clock_nominal={"nominal-11/34A+MS11-L",180,240,170,38,100,28,200,84,51,100,150,100000000};
const ClockTiming clock_aged={"aged-11/34A+MS11-L",189,251,170,38,115,35,134,84,51,100,150,74500000};
const ClockTiming clock_aged_low={"aged-low-sensitivity",190,240,170,38,100,28,180,0,30,90,0,74500000};
const ClockTiming clock_aged_high={"aged-high-sensitivity",190,240,170,38,100,28,220,143,65,110,300,74500000};
uint32_t clockgen_period_ns(const ClockTiming *p,int long_cycle_l,int load_ba,int relocate) {
    p=clock_timing(p);
    return (!long_cycle_l||(load_ba&&relocate))?p->long_ns:p->short_ns;
}
uint32_t clockgen_msyn_ns(const ClockTiming *p,uint32_t period_ns) {
    p=clock_timing(p);return period_ns/2+p->msyn_rc_ns+p->msyn_logic_ns;
}
uint32_t clockgen_cycle_ns(const ClockTiming *p,uint32_t period_ns,int transfer,
                         int bus_ctl,uint32_t ssyn_ns,int but_service,int intr) {
    p=clock_timing(p);
    uint32_t duration=period_ns;
    if(but_service) {
        uint32_t end=p->grant_rc_ns+p->grant_logic_ns+p->restart_ns;
        if(intr)end+=p->device_intr_ns+p->sack_rc_ns; /* BG INH until SACK */
        if(end>duration)duration=end;
    }
    if(transfer) {
        uint32_t end=clockgen_msyn_ns(p,period_ns)+ssyn_ns+
                     (bus_ctl<=1?p->read_eot_ns:p->write_ack_ns)+p->restart_ns;
        if(end>duration)duration=end;
    }
    return duration;
}
