/* KY11-LB programmer's console. On the real panel an 8008 on the M7859
 * board scans the keypad, drives the display and does the bus cycles of
 * EXAM and DEP (EK-KY1LB-MM-001, chapter 5). This header only holds what
 * can be seen from outside. The rest belongs to the backend, which is
 * console_sim.c for now and could one day be an 8008 emulator. */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

#ifndef UNIBUS_H
typedef struct Bus Bus;
#endif
#ifndef KD11EA_H
typedef struct KD11EA KD11EA;
#endif

/* The 20 keys. HLT/SS, CONT, BOOT, START and INIT need CNTRL, except
 * HLT/SS when the CPU is already halted, which then single steps. */
enum {
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7,
    KEY_HALT_SS,
    KEY_CONT,
    KEY_BOOT,
    KEY_START,
    KEY_DEP,
    KEY_EXAM,
    KEY_INIT,
    KEY_CLR,
    KEY_DIS_AD,
    KEY_LAD,
    KEY_LSR,
    KEY_CNTRL,      /* a toggle here, since a GUI cannot hold two keys */
    KEY_COUNT
};

typedef struct {
    uint32_t display;         /* 6 octal digits, frozen while the CPU runs */
    uint16_t switch_reg;      /* on the M7859, read by the CPU at 777570 */

    uint8_t  led_run;
    uint8_t  led_bus_err;     /* the last console bus cycle got no SSYN */
    uint8_t  led_sr_disp;
    uint8_t  led_maint;
    uint8_t  cntrl_active;

    /* The main loop acts on these and clears them. */
    uint8_t  boot_requested;
    uint8_t  init_requested;

    Bus     *bus;
    KD11EA  *cpu;
    void    *backend;
} Console;

void console_init(Console *con);

/* BUS DC LO clears the address pointer, the indicators, the switch
 * register and the Unibus control, like a master clear (5.3.2). */
void console_reset(Console *con);

/* A key press acts at once, as the firmware does. */
void console_key(Console *con, int key);

/* Follow the CPU state on the LEDs. Call it about 60 times a second. */
void console_tick(Console *con);

uint32_t console_addr_ptr(Console *con);
void console_set_addr(Console *con, uint32_t addr);

#endif
