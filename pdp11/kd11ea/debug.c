/*
 * debug.c: Ring buffer trace for post-mortem analysis
 *
 * Keeps the last DBG_RING_SIZE micro-step snapshots in a circular buffer.
 * On HALT, dbg_dump() prints them oldest-first so what the CPU was doing
 * in the ~500 usteps leading to the crash can be seen.
 */

#include "debug.h"
#include <string.h>

void dbg_init(DbgRing *dbg)
{
    memset(dbg, 0, sizeof(*dbg));
}

void dbg_record(DbgRing *dbg, const DbgEntry *entry)
{
    dbg->ring[dbg->head & (DBG_RING_SIZE - 1)] = *entry;
    dbg->head++;
    if (dbg->count < DBG_RING_SIZE)
        dbg->count++;
}

static const char *spa_sel_name[] = { "ROM", "RS", "RD", "RBA" };

void dbg_dump(const DbgRing *dbg, FILE *out)
{
    if (dbg->count == 0) {
        fprintf(out, "[debug] ring buffer empty\n");
        return;
    }

    uint32_t n = (dbg->count < DBG_RING_SIZE) ? dbg->count : DBG_RING_SIZE;
    uint32_t start = dbg->head - n;  /* wraps via unsigned arithmetic */

    fprintf(out, "\n=== Post-mortem trace: last %u micro-steps ===\n", n);

    for (uint32_t i = 0; i < n; i++) {
        const DbgEntry *e = &dbg->ring[(start + i) & (DBG_RING_SIZE - 1)];

        /* Main line: MPC, ALU operation, result */
        fprintf(out, "MPC=%03o  A=%06o(%s[%02o]) B=%06o S=%X M=%d Cin=%d -> %06o Cout=%d",
                e->mpc,
                e->a_leg, spa_sel_name[e->spa_src_sel & 3], e->spa_src,
                e->b_leg, e->alu_s, e->alu_mode, e->alu_cin,
                e->alu_out, e->alu_cout);

        /* Bus activity */
        if (e->bus_addr_valid)
            fprintf(out, "  BA<-%06o", e->ba);
        if (e->bus_op == 1)
            fprintf(out, "  RD@%06o=%06o", e->ba, e->unibus_data);
        else if (e->bus_op == 2)
            fprintf(out, "  WR@%06o", e->ba);

        /* IR load */
        if (e->ir_loaded)
            fprintf(out, "  IR<-%06o", e->ir);

        /* Dispatch */
        if (e->service_trap)
            fprintf(out, "  SERVICE");
        if (e->ir_decoded)
            fprintf(out, "  DECODE->%03o", e->ir_decode_mpc);

        /* State at end of ustep */
        fprintf(out, "  ->%03o  [PC=%06o SP=%06o R0=%06o R1=%06o PSW=%06o B=%06o BX=%06o]\n",
                e->next_mpc, e->pc, e->sp, e->r0, e->r1,
                e->psw, e->b_reg, e->bx_reg);
    }

    fprintf(out, "=== End trace ===\n\n");
}
