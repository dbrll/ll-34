/* PC11 paper tape reader (300 characters a second) and punch (50). PRS
 * 177550, PRB 177552, PPS 177554 and PPB 177556. The reader interrupts at
 * 070 and the punch at 074, both at BR4. */

#ifndef PC11_H
#define PC11_H

#include <stdio.h>
#include <stdint.h>
#include "../unibus/unibus.h"

#define PC11_BASE   0x3FF68  /* 777550 */
#define PC11_END    0x3FF6F

#define PRS_GO      0x0001
#define PRS_IE      0x0040
#define PRS_DONE    0x0080
#define PRS_BUSY    0x0800
#define PRS_ERR     0x8000  /* no tape, or its end */

#define PPS_IE      0x0040
#define PPS_DONE    0x0080
#define PPS_ERR     0x8000  /* no punch file */

#define PC11_PTR_VEC    0070
#define PC11_PTP_VEC    0074
#define PC11_PRI        4

typedef struct {
    uint8_t init_l;
    uint16_t prs;
    uint16_t prb;
    uint16_t pps;
    uint16_t ppb;

    FILE    *reader;
    FILE    *punch;

    uint64_t reader_done_ns;
    uint64_t punch_done_ns;
    uint64_t reader_char_ns;
    uint64_t punch_char_ns;
    uint64_t *clock_ptr;        /* the CPU simulated time */

    void (*irq_set)(void *ctx, uint16_t vector, uint8_t priority);
    void (*irq_clr)(void *ctx, uint16_t vector);
    void *irq_ctx;
} PC11;

void pc11_init(PC11 *pc);
int pc11_attach_reader(PC11 *pc, const char *filename);
int pc11_attach_punch(PC11 *pc, const char *filename);
void pc11_tick(PC11 *pc, uint64_t now_ns);
int pc11_register(PC11 *pc, Bus *bus);

#endif
