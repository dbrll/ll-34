/*
 * unibus.c -- UNIBUS address dispatch for ll-34
 *
 * Linear scan of registered devices, 18-bit addresses.
 */

#include <stdio.h>
#include <string.h>
#include "unibus.h"
#include "../trace.h"

void bus_init(Bus *bus) {
    memset(bus, 0, sizeof(*bus));
    bus->init_l = 1;
}

int bus_register_init(Bus *bus, void *dev, bus_init_fn changed) {
    if (bus->ninit_receivers >= BUS_MAX_DEVICES)
        return -1;
    BusInitReceiver *r = &bus->init_receivers[bus->ninit_receivers++];
    r->dev = dev;
    r->changed = changed;
    changed(dev, bus->init_l);
    return 0;
}

void bus_set_init(Bus *bus, int init_l, uint64_t now_ns) {
    init_l = !!init_l;
    if (bus->init_l == init_l)
        return;
    bus->init_l = (uint8_t)init_l;
    if (init_l)
        bus->init_released_ns = now_ns;
    else {
        bus->init_asserted_ns = now_ns;
        bus->nxm = 0;
    }
    for (int i = 0; i < bus->ninit_receivers; i++)
        bus->init_receivers[i].changed(bus->init_receivers[i].dev, init_l);
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
    d->response = NULL;
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

int bus_read_at(Bus *bus, uint32_t addr, uint16_t *data, uint64_t msyn_ns) {
    bus->now_ns = msyn_ns;
    bus->timed_access = 1;
    return bus_read(bus, addr, data);
}
int bus_write_at(Bus *bus, uint32_t addr, uint16_t data, int type, uint64_t msyn_ns) {
    bus->now_ns = msyn_ns;
    bus->timed_access = 1;
    return bus_write(bus, addr, data, type);
}
int bus_read(Bus *bus, uint32_t addr, uint16_t *data) {
    int timed = bus->timed_access;
    bus->timed_access = 0;
    addr &= 0x3FFFF;
    BusDevice *d = bus_lookup(bus, addr & ~1);
    if (!d) {
        bus->last_latency_ns = 22000;
        bus->nxm = 1;
        *data = 0;
        trace("NXM READ  @%06o\n", addr);
        return -1;
    }
    uint32_t latency = timed && d->response ? d->response(d->dev, addr, BUS_DATI, bus->now_ns) : d->latency_ns;
    int result = d->read(d->dev, addr, data);
    /* A controller's nested DMA must not replace its own SSYN. */
    bus->last_latency_ns = latency;
    return result;
}

int bus_write(Bus *bus, uint32_t addr, uint16_t data, int type) {
    int timed = bus->timed_access;
    bus->timed_access = 0;
    addr &= 0x3FFFF;
    int is_byte = (type == BUS_DATOB);
    BusDevice *d = bus_lookup(bus, addr & ~(is_byte ? 0 : 1));
    if (!d) {
        bus->last_latency_ns = 22000;
        bus->nxm = 1;
        trace("NXM WRITE @%06o <- %06o\n", addr, data);
        return -1;
    }
    uint32_t latency = timed && d->response ? d->response(d->dev, addr, type, bus->now_ns) : d->latency_ns;
    int result = d->write(d->dev, addr, data, is_byte);
    bus->last_latency_ns = latency;
    return result;
}
