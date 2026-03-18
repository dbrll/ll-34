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

typedef int (*bus_read_fn)(void *dev, uint32_t addr, uint16_t *data);
typedef int (*bus_write_fn)(void *dev, uint32_t addr, uint16_t data, int is_byte);

typedef struct {
    uint32_t     base;      /* Base address (word-aligned), 18-bit */
    uint32_t     end;       /* Last address (inclusive), 18-bit */
    void        *dev;       /* Device-specific state */
    bus_read_fn  read;      /* DATI/DATIP handler */
    bus_write_fn write;     /* DATO/DATOB handler */
    const char  *name;      /* Device name for diagnostics */
    uint32_t     latency_ns; /* Bus response latency in nanoseconds */
} BusDevice;

typedef struct Bus {
    BusDevice devices[BUS_MAX_DEVICES];
    int       ndevices;
    uint32_t  last_latency_ns;  /* Latency of most recent bus operation */
    uint8_t   nxm;              /* NXM flag: set on bus timeout (no SSYN) */
} Bus;

void bus_init(Bus *bus);
int bus_register(Bus *bus, uint32_t base, uint32_t end,
                 void *dev, bus_read_fn read, bus_write_fn write,
                 const char *name, uint32_t latency_ns);
int bus_read(Bus *bus, uint32_t addr, uint16_t *data);
int bus_write(Bus *bus, uint32_t addr, uint16_t data, int type);

#endif /* UNIBUS_H */
