/*
 * trace.h: Debug trace macro for ll-34
 *
 * trace() calls are kept in the source as documentation of the datapath
 * flow.  They compile to nothing (no-op) and have zero runtime cost.
 *
 * For interactive debugging, use the debug console (Ctrl-E):
 *   s [n]   Single-step n micro-instructions with full state dump
 *   b addr  Set breakpoint at PC address
 */

#ifndef TRACE_H
#define TRACE_H

#define trace(fmt, ...) do { (void)0; } while (0)

#endif /* TRACE_H */
