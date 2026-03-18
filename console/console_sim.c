/*
 * console_sim.c -- KY11-LB Simulated Backend
 *
 * High-level simulation of the M7859 firmware behavior.
 * Register-transfer sequences from EK-KY1LB-MM-001 S5.2.1.
 */

#include "console.h"
#include "../unibus/unibus.h"
#include "../kd11ea/kd11ea.h"

/* Backend private state (8008 scratchpad RAM equivalent) */
typedef struct {
    uint32_t temporary;     /* 18-bit Temporary Data Buffer */
    uint32_t addr_ptr;      /* 18-bit Unibus Address Pointer */
    uint8_t  flag_exam;     /* pre-increment on next EXAM */
    uint8_t  flag_dep;      /* pre-increment on next DEP */
    uint8_t  manual_clk_en; /* manual clock enable (§5.3.12 E64 D2) */
} ConsoleSim;

static ConsoleSim sim;      /* single instance (matches single Console) */

/* Console register addresses: 777700-777707 (R0-R7, internal decode) */
#define ADDR_REG_BASE   0x3FFC0     /* 777700 */
#define ADDR_REG_END    0x3FFC7     /* 777707 */
#define ADDR_R7         0x3FFC7     /* 777707 = R7 (PC) */

/* --- Helpers --- */

static void clear_indicators(Console *con)
{
    con->led_bus_err = 0;
    con->led_sr_disp = 0;
}

static uint16_t bus_master_read(Console *con, uint32_t addr)
{
    if (addr >= ADDR_REG_BASE && addr <= ADDR_REG_END) {
        con->led_bus_err = 0;
        return REG(con->cpu, addr & 7);
    }
    uint16_t data = 0;
    int rc = bus_read(con->bus, addr & 0x3FFFE, &data);
    con->led_bus_err = (rc < 0) ? 1 : 0;
    return data;
}

static void bus_master_write(Console *con, uint32_t addr, uint16_t data)
{
    if (addr >= ADDR_REG_BASE && addr <= ADDR_REG_END) {
        con->led_bus_err = 0;
        REG(con->cpu, addr & 7) = data;
        return;
    }
    int rc = bus_write(con->bus, addr & 0x3FFFE, data, BUS_DATO);
    con->led_bus_err = (rc < 0) ? 1 : 0;
}

static void display_switch_reg(Console *con)
{
    con->display = con->switch_reg;
    con->led_sr_disp = 1;
}

/* Read R7 via Unibus and display it (S5.2.1 steps 2-6) */
static void read_and_display_pc(Console *con, ConsoleSim *s)
{
    uint16_t data = bus_master_read(con, ADDR_R7);
    s->temporary = data;
    con->display = s->temporary;
}

/* Console mode key functions (S5.2.1) */

static void key_numeric(Console *con, ConsoleSim *s, uint8_t digit)
{
    s->temporary = ((s->temporary << 3) | (digit & 7)) & 0x3FFFF;
    con->display = s->temporary;
    clear_indicators(con);
}

static void key_clr(Console *con, ConsoleSim *s)
{
    s->temporary = 0;
    con->display = 0;
    clear_indicators(con);
    s->flag_exam = 0;
    s->flag_dep = 0;
    con->led_maint = 0;
}

static void key_lad(Console *con, ConsoleSim *s)
{
    s->addr_ptr = s->temporary;
    s->temporary = 0;
    con->display = 0;
    clear_indicators(con);
}

static void key_lsr(Console *con, ConsoleSim *s)
{
    con->switch_reg = s->temporary & 0xFFFF;
    display_switch_reg(con);
}

static void key_dis_ad(Console *con, ConsoleSim *s)
{
    con->display = s->addr_ptr;
    s->flag_exam = 0;
    s->flag_dep = 0;
}

static void key_exam(Console *con, ConsoleSim *s)
{
    if (!con->cpu->halted) return;
    if (s->flag_exam)
        s->addr_ptr = (s->addr_ptr + 2) & 0x3FFFF;

    uint16_t data = bus_master_read(con, s->addr_ptr);
    s->temporary = data;
    con->display = s->temporary;
    s->flag_exam = 1;
    s->flag_dep = 0;
}

static void key_dep(Console *con, ConsoleSim *s)
{
    if (!con->cpu->halted) return;
    if (s->flag_dep)
        s->addr_ptr = (s->addr_ptr + 2) & 0x3FFFF;

    bus_master_write(con, s->addr_ptr, s->temporary & 0xFFFF);
    s->flag_dep = 1;
    s->flag_exam = 0;
}

static void key_halt_ss(Console *con, ConsoleSim *s)
{
    if (!con->cpu->halted) return;

    con->cpu->halted = 0;
    int steps = 0;
    do {
        kd11ea_ustep(con->cpu);
        steps++;
    } while (con->cpu->mpc != 0 && steps < 1000);
    con->cpu->halted = 1;

    read_and_display_pc(con, s);
}

/* CNTRL + key functions (S5.2.1) */

static void cntrl_halt_ss(Console *con, ConsoleSim *s)
{
    if (con->cpu->halted) {
        key_halt_ss(con, s);    /* single-step + display new PC */
        return;
    }
    con->cpu->halted = 1;
    read_and_display_pc(con, s);
}

static void cntrl_cont(Console *con, ConsoleSim *s)
{
    (void)s;
    con->cpu->halted = 0;
    display_switch_reg(con);
}

static void cntrl_start(Console *con, ConsoleSim *s)
{
    if (!con->cpu->halted) return;
    bus_master_write(con, ADDR_R7, s->addr_ptr & 0xFFFF);
    con->init_requested = 1;    /* main loop asserts BUS INIT */
    con->cpu->halted = 0;
    display_switch_reg(con);
}

static void cntrl_boot(Console *con, ConsoleSim *s)
{
    (void)s;
    if (!con->cpu->halted) return;
    con->boot_requested = 1;    /* main loop handles reset + power-on */
    display_switch_reg(con);
}

static void cntrl_init(Console *con, ConsoleSim *s)
{
    (void)s;
    if (!con->cpu->halted) return;
    con->init_requested = 1;
}

static void cntrl_7(Console *con, ConsoleSim *s)
{
    s->temporary = (s->addr_ptr + s->temporary + 2) & 0x3FFFF;
    con->display = s->temporary;
}

static void cntrl_6(Console *con, ConsoleSim *s)
{
    s->temporary = (s->temporary + con->switch_reg) & 0x3FFFF;
    con->display = s->temporary;
}

static void cntrl_1(Console *con, ConsoleSim *s)
{
    (void)s;
    /* Enter maintenance mode (S5.2.1) */
    con->led_maint = 1;
    con->cpu->halted = 1;
    con->display = con->cpu->mpc;
}

/* Maintenance mode (S5.2.2) */

static void dispatch_maint(Console *con, ConsoleSim *s, int key)
{
    switch (key) {
    case KEY_CLR:
        s->manual_clk_en = 0;
        con->led_maint = 0;
        con->led_bus_err = 0;
        con->led_sr_disp = 0;
        con->cpu->halted = 1;
        read_and_display_pc(con, s);
        break;

    case KEY_DIS_AD:
        con->display = con->cpu->ba;
        break;

    case KEY_EXAM:
        con->display = con->cpu->unibus_data;
        break;

    case KEY_HALT_SS:
        s->manual_clk_en = 1;
        con->cpu->halted = 1;
        con->display = con->cpu->mpc;
        break;

    case KEY_CONT:  /* one microstep */
        s->manual_clk_en = 1;
        con->cpu->halted = 0;
        kd11ea_ustep(con->cpu);
        con->cpu->halted = 1;
        con->display = con->cpu->mpc;
        break;

    case KEY_BOOT:
        con->boot_requested = 1;
        break;

    case KEY_START:
        s->manual_clk_en = 0;
        con->display = con->cpu->mpc;
        break;

    case KEY_5:  /* TAKE BUS, not implemented */
        break;

    default:
        break;
    }
}

static void dispatch_cntrl(Console *con, ConsoleSim *s, int key)
{
    switch (key) {
    case KEY_HALT_SS: cntrl_halt_ss(con, s); break;
    case KEY_CONT:    cntrl_cont(con, s);    break;
    case KEY_START:   cntrl_start(con, s);   break;
    case KEY_BOOT:    cntrl_boot(con, s);    break;
    case KEY_INIT:    cntrl_init(con, s);    break;
    case KEY_7:       cntrl_7(con, s);       break;
    case KEY_6:       cntrl_6(con, s);       break;
    case KEY_1:       cntrl_1(con, s);       break;
    default:
        /* Other keys with CNTRL: process as normal key */
        con->cntrl_active = 0;
        console_key(con, key);
        return;
    }
    con->cntrl_active = 0;
}

void console_init(Console *con)
{
    con->display = 0;
    con->switch_reg = 0;
    con->led_run = 0;
    con->led_bus_err = 0;
    con->led_sr_disp = 0;
    con->led_maint = 0;
    con->cntrl_active = 0;
    con->boot_requested = 0;
    con->init_requested = 0;
    con->bus = 0;
    con->cpu = 0;

    sim.temporary = 0;
    sim.addr_ptr = 0;
    sim.flag_exam = 0;
    sim.flag_dep = 0;
    sim.manual_clk_en = 0;

    con->backend = &sim;
}

void console_reset(Console *con)
{
    /* KY11-LB power-up clear (S5.3.2), preserves bus/cpu pointers */
    con->display = 0;
    con->led_run = 0;
    con->led_bus_err = 0;
    con->led_sr_disp = 0;
    con->led_maint = 0;
    con->cntrl_active = 0;
    con->boot_requested = 0;
    con->init_requested = 0;

    ConsoleSim *s = (ConsoleSim *)con->backend;
    s->temporary = 0;
    s->addr_ptr = 0;
    s->flag_exam = 0;
    s->flag_dep = 0;
    s->manual_clk_en = 0;
}

void console_key(Console *con, int key)
{
    if (!con->bus || !con->cpu)
        return;

    ConsoleSim *s = (ConsoleSim *)con->backend;

    if (key == KEY_CNTRL) {
        con->cntrl_active = !con->cntrl_active;
        return;
    }

    if (con->cntrl_active) {
        dispatch_cntrl(con, s, key);
        return;
    }

    if (con->led_maint) {
        dispatch_maint(con, s, key);
        return;
    }

    if (key >= KEY_0 && key <= KEY_7) {
        key_numeric(con, s, (uint8_t)key);
        return;
    }

    switch (key) {
    case KEY_CLR:     key_clr(con, s);     break;
    case KEY_LAD:     key_lad(con, s);     break;
    case KEY_LSR:     key_lsr(con, s);     break;
    case KEY_DIS_AD:  key_dis_ad(con, s);  break;
    case KEY_EXAM:    key_exam(con, s);    break;
    case KEY_DEP:     key_dep(con, s);     break;
    case KEY_HALT_SS: key_halt_ss(con, s); break;

    /* Right-side keys: no-op without CNTRL */
    case KEY_CONT:
    case KEY_BOOT:
    case KEY_START:
    case KEY_INIT:
        break;
    }
}

uint32_t console_addr_ptr(Console *con)
{
    ConsoleSim *s = (ConsoleSim *)con->backend;
    return s->addr_ptr;
}

void console_set_addr(Console *con, uint32_t addr)
{
    ConsoleSim *s = (ConsoleSim *)con->backend;
    s->addr_ptr = addr & 0x3FFFF;
}

void console_tick(Console *con)
{
    if (!con->cpu)
        return;

    uint8_t was_running = con->led_run;
    con->led_run = con->cpu->halted ? 0 : 1;

    /* Running -> halted: read and display PC (S5.2.1) */
    if (was_running && !con->led_run) {
        ConsoleSim *s = (ConsoleSim *)con->backend;
        read_and_display_pc(con, s);
        con->led_sr_disp = 0;
    }
}
