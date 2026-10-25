/*
 * unibus.h -- UNIBUS address dispatch for ll-34
 *
 * 18-bit address bus (256KB). Devices register address ranges and callbacks.
 */

#ifndef UNIBUS_H
#define UNIBUS_H

#include <stdint.h>

#define BUS_DATI   0   /* Data In (read word) */
#define BUS_DATIP  1   /* Data In, Pause (read-modify-write, read phase) */
#define BUS_DATO   2   /* Data Out (write word) */
#define BUS_DATOB  3   /* Data Out, Byte (write byte) */

#define BUS_MAX_DEVICES 16

/* Electrical INIT L, independent of address/data transfers.
 * Masters must stop issuing transactions while INIT L is asserted. */
typedef void (*bus_init_fn)(void *dev, int init_l);
typedef struct {
    void *dev;
    bus_init_fn changed;
} BusInitReceiver;

typedef int (*bus_read_fn)(void *dev, uint32_t addr, uint16_t *data);
typedef int (*bus_write_fn)(void *dev, uint32_t addr, uint16_t data, int is_byte);

/* Optional slave response at MSYN, separate from the data callback. */
typedef uint32_t (*bus_response_fn)(void *dev, uint32_t addr, int type,
                                    uint64_t msyn_ns);
typedef struct {
    uint32_t     base;      /* Base address (word-aligned), 18-bit */
    uint32_t     end;       /* Last address (inclusive), 18-bit */
    void        *dev;       /* Device-specific state */
    bus_read_fn  read;      /* DATI/DATIP handler */
    bus_write_fn write;     /* DATO/DATOB handler */
    const char  *name;      /* Device name for diagnostics */
    uint32_t     latency_ns; /* Bus response latency in nanoseconds */
    bus_response_fn response;
} BusDevice;

typedef struct Bus {
    BusDevice devices[BUS_MAX_DEVICES];
    int       ndevices;
    uint64_t  now_ns;           /* MSYN instant supplied by the master */
    uint8_t timed_access;       /* consumed before callbacks: nested DMA stays untimed */
    uint32_t  last_latency_ns;  /* Latency of most recent bus operation */
    uint8_t   nxm;              /* NXM flag: set on bus timeout (no SSYN) */
    uint8_t   init_l;           /* BUS INIT L: idle=1, asserted=0 */
    uint64_t  init_asserted_ns, init_released_ns;
    BusInitReceiver init_receivers[BUS_MAX_DEVICES];
    int ninit_receivers;
} Bus;

void bus_init(Bus *bus);
int bus_register_init(Bus *bus, void *dev, bus_init_fn changed);
void bus_set_init(Bus *bus, int init_l, uint64_t now_ns);
int bus_register(Bus *bus, uint32_t base, uint32_t end,
                 void *dev, bus_read_fn read, bus_write_fn write,
                 const char *name, uint32_t latency_ns);
int bus_read_at(Bus *bus, uint32_t addr, uint16_t *data, uint64_t msyn_ns);
int bus_write_at(Bus *bus, uint32_t addr, uint16_t data, int type, uint64_t msyn_ns);
/* Untimed administrative and existing bulk-DMA interfaces. */
int bus_read(Bus *bus, uint32_t addr, uint16_t *data);
int bus_write(Bus *bus, uint32_t addr, uint16_t data, int type);

#endif /* UNIBUS_H */
