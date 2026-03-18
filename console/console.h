/*
 * console.h -- KY11-LB Programmer's Console for ll-34
 *
 * Observable state of the PDP-11/34 front panel (KY11-LB).
 *
 * The real hardware is an Intel 8008 microprocessor on the M7859
 * interface board, running 1024 bytes of firmware that scans the
 * 20-key keypad, drives the 6-digit 7-segment display, and performs
 * bus master operations (EXAM/DEP) on behalf of the operator.
 *
 * This header defines:
 *   - The KEY_* enum for the 20 keypad keys
 *   - The Console struct: observable state only (display, LEDs,
 *     switch register, requests). Internal state lives in the
 *     backend (console_sim.c today, maybe 8008 emulator tomorrow).
 *   - The public API: init, key, tick
 *
 * No stdio dependencies, suitable for native CLI, WebAssembly,
 * and a future 8008-emulated backend.
 *
 * Reference: EK-KY1LB-MM-001, Chapter 5
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

/* Forward declarations: avoid pulling in full headers.
 * Guard against re-definition if unibus.h / kd11ea.h already included. */
#ifndef UNIBUS_H
typedef struct Bus Bus;
#endif
#ifndef KD11EA_H
typedef struct KD11EA KD11EA;
#endif

/* --- Console keypad (KY11-LB, 20 keys) --- */
enum {
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7,
    KEY_HALT_SS,    /* HLT/SS: halt (with CNTRL) or single-step */
    KEY_CONT,       /* CONT: continue from halt (with CNTRL) */
    KEY_BOOT,       /* BOOT: bootstrap (with CNTRL) */
    KEY_START,      /* START: set PC, run (with CNTRL) */
    KEY_DEP,        /* DEP: deposit data at address */
    KEY_EXAM,       /* EXAM: examine address, show data */
    KEY_INIT,       /* INIT: bus reset (with CNTRL) */
    KEY_CLR,        /* CLR: clear entry / indicators */
    KEY_DIS_AD,     /* DIS AD: display address pointer */
    KEY_LAD,        /* LAD: load address from temporary */
    KEY_LSR,        /* LSR: load switch register from temporary */
    KEY_CNTRL,      /* CNTRL: modifier (sticky toggle for GUI) */
    KEY_COUNT
};

/* --- Console observable state --- */
typedef struct {
    /* Display: single 18-bit value shown on 7-segment (6 octal digits).
     * Updated by key operations; does NOT change while CPU runs. */
    uint32_t display;

    /* Switch Register Image (16-bit).
     * Owned by the console (real HW: on the M7859 board).
     * Readable from Unibus as address 777570. */
    uint16_t switch_reg;

    /* Indicators (directly readable by frontend) */
    uint8_t  led_run;         /* RUN: CPU not halted */
    uint8_t  led_bus_err;     /* BUS ERR: NXM on last console bus op */
    uint8_t  led_sr_disp;     /* SR DISP: switch register displayed */
    uint8_t  led_maint;       /* MAINT: maintenance mode active */
    uint8_t  cntrl_active;    /* CNTRL key latched (for GUI highlight) */

    /* Requests for main loop (polled and cleared by caller) */
    uint8_t  boot_requested;  /* 1 = CNTRL-BOOT pressed, needs power_on */
    uint8_t  init_requested;  /* 1 = CNTRL-INIT pressed, needs bus reset */

    /* Connections (set by caller after init) */
    Bus     *bus;
    KD11EA  *cpu;

    /* Opaque backend state, allocated and managed by the backend.
     * For console_sim.c: points to a ConsoleSim struct.
     * For a future 8008 backend: points to 8008 registers + RAM. */
    void    *backend;
} Console;

/* Initialize console (zeroes observable state, allocates backend).
 * Set bus/cpu pointers after calling this. */
void console_init(Console *con);

/* Power-up reset: clear display, indicators, and backend state.
 * Preserves bus/cpu pointers.  Per KY11-LB §5.3.2, BUS DC LO
 * clears the bus address register, indicator register, switch
 * register, and Unibus control, equivalent to a master clear. */
void console_reset(Console *con);

/* Process a key press. Executes the corresponding action immediately
 * (bus read/write, halt/run, etc.) per KY11-LB firmware behavior. */
void console_key(Console *con, int key);

/* Update LED state from CPU. Call periodically (~60 Hz). */
void console_tick(Console *con);

/* Return the current 18-bit Unibus Address Pointer (for CLI display). */
uint32_t console_addr_ptr(Console *con);

/* Set the Unibus Address Pointer directly (CLI convenience). */
void console_set_addr(Console *con, uint32_t addr);

#endif /* CONSOLE_H */
