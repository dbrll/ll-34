/*
 * unibus.c -- UNIBUS address dispatch for ll-34
 *
 * Linear scan of registered devices, 18-bit addresses.
 */

#include <stdio.h>
#include <string.h>
#include "unibus.h"
#include "trace.h"

void bus_init(Bus *bus) {
    memset(bus, 0, sizeof(*bus));
}

int bus_register(Bus *bus, uint32_t base, uint32_t end,
                 void *dev, bus_read_fn read, bus_write_fn write,
                 const char *name, uint32_t latency_ns)
{
    if (bus->ndevices >= BUS_MAX_DEVICES) {
        fprintf(stderr, "bus: too many devices (max %d)\n", BUS_MAX_DEVICES);
        return -1;
    }
    BusDevice *d = &bus->devices[bus->ndevices++];
    d->base       = base & 0x3FFFF;  /* mask to 18 bits */
    d->end        = end & 0x3FFFF;
    d->dev        = dev;
    d->read       = read;
    d->write      = write;
    d->name       = name;
    d->latency_ns = latency_ns;
    return 0;
}

static BusDevice *bus_lookup(Bus *bus, uint32_t addr) {
    for (int i = 0; i < bus->ndevices; i++) {
        BusDevice *d = &bus->devices[i];
        if (addr >= d->base && addr <= d->end)
            return d;
    }
    return NULL;
}

int bus_read(Bus *bus, uint32_t addr, uint16_t *data) {
    addr &= 0x3FFFF;
    BusDevice *d = bus_lookup(bus, addr & ~1);
    if (!d) {
        bus->last_latency_ns = 22000;
        bus->nxm = 1;
        *data = 0;
        trace("NXM READ  @%06o\n", addr);
        return -1;
    }
    bus->last_latency_ns = d->latency_ns;
    return d->read(d->dev, addr, data);
}

int bus_write(Bus *bus, uint32_t addr, uint16_t data, int type) {
    addr &= 0x3FFFF;
    int is_byte = (type == BUS_DATOB);
    BusDevice *d = bus_lookup(bus, addr & ~(is_byte ? 0 : 1));
    if (!d) {
        bus->last_latency_ns = 22000;
        bus->nxm = 1;
        trace("NXM WRITE @%06o <- %06o\n", addr, data);
        return -1;
    }
    bus->last_latency_ns = d->latency_ns;
    return d->write(d->dev, addr, data, is_byte);
}
