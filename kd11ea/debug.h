/*
 * debug.h: Ring buffer trace for post-mortem analysis
 *
 * Records the last N micro-steps in a circular buffer.
 * Zero overhead during normal execution (just a struct copy per ustep).
 * Call dbg_dump() on HALT to see what led to the crash.
 */

#ifndef KD11EA_DEBUG_H
#define KD11EA_DEBUG_H

#include <stdint.h>
#include <stdio.h>

#define DBG_RING_SIZE  4096  /* must be power of 2 */

/* One micro-step snapshot */
typedef struct {
    uint16_t mpc;           /* MPC at entry */
    uint16_t next_mpc;      /* MPC at exit */
    uint16_t a_leg;         /* ALU A input */
    uint16_t b_leg;         /* ALU B input */
    uint16_t alu_out;       /* ALU result */
    uint16_t ba;            /* Bus Address register */
    uint16_t ir;            /* Instruction Register */
    uint16_t psw;           /* PSW */
    uint16_t pc;            /* R7/PC */
    uint16_t sp;            /* R6/SP */
    uint16_t r0;            /* R0 */
    uint16_t r1;            /* R1 */
    uint16_t b_reg;         /* B register */
    uint16_t bx_reg;        /* BX register */
    uint16_t unibus_data;   /* last bus data */
    uint8_t  spa_src;       /* scratchpad source address */
    uint8_t  spa_src_sel;   /* SPA_SRC_SEL field (0=ROM,1=RS,2=RD,3=RBA) */
    uint8_t  alu_s;         /* ALU function S3:S0 */
    uint8_t  alu_mode;      /* ALU mode (0=arith, 1=logic) */
    uint8_t  alu_cin;       /* ALU carry in */
    uint8_t  alu_cout;      /* ALU carry out */
    uint8_t  bus_op;        /* 0=none, 1=read, 2=write */
    uint8_t  bus_addr_valid;/* BA was loaded this cycle */
    uint8_t  ir_loaded;     /* IR was loaded this cycle */
    uint8_t  ir_decoded;    /* IR decode happened (dispatch) */
    uint16_t ir_decode_mpc; /* MPC from IR decode (if ir_decoded) */
    uint8_t  service_trap;  /* BUT SERVICE override fired */
} DbgEntry;

typedef struct {
    DbgEntry ring[DBG_RING_SIZE];
    uint32_t head;          /* next write position */
    uint32_t count;         /* total entries written (for "how full" check) */
} DbgRing;

/* Initialize the ring buffer */
void dbg_init(DbgRing *dbg);

/* Record one micro-step (called from kd11ea_ustep) */
void dbg_record(DbgRing *dbg, const DbgEntry *entry);

/* Dump the ring buffer contents to a file (stderr, or a FILE*).
 * Dumps the oldest-to-newest entries still in the buffer. */
void dbg_dump(const DbgRing *dbg, FILE *out);

#endif /* KD11EA_DEBUG_H */
