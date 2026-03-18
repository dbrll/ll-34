/*
 * tty/tty.h -- Abstract TTY interface for ll-34
 *
 * On a real PDP-11/34, the DL11 serial port is a physical RS-232 connector,
 * separate from the operator console panel.  This interface decouples the
 * DL11 serial I/O from the host implementation, allowing multiple backends:
 *
 *   stdio  -- host terminal (stdin/stdout), the default
 *   tcp    -- TCP server (default port 1134, a nod to the PDP-11/34)
 *   pty    -- pseudo-terminal (/dev/ttysN)
 *
 * Each backend extends the TTY struct with its own private data and provides
 * the five method pointers below.
 */

#ifndef TTY_H
#define TTY_H

#include <stdint.h>

typedef struct TTY {
    int  (*rx_ready)(struct TTY *tty);              /* non-zero if char available */
    int  (*rx_read)(struct TTY *tty);               /* read one char, -1 if none */
    void (*tx_write)(struct TTY *tty, uint8_t ch);  /* write one char */
    void (*tick)(struct TTY *tty);                   /* periodic poll (accept, etc.) */
    void (*destroy)(struct TTY *tty);                /* cleanup and free */
} TTY;

/* Backend constructors.  Each returns a heap-allocated TTY (or NULL on error). */
TTY *tty_stdio_create(void);
TTY *tty_tcp_create(uint16_t port);     /* 0 = default (1134) */
TTY *tty_pty_create(void);

#ifdef __EMSCRIPTEN__
TTY *tty_wasm_create(void);
#endif

#endif /* TTY_H */
