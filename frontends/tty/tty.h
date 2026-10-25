/* Host side of the console terminal. The DL11 talks to one of these
 * backends: stdio (the default), a TCP server (port 1134 by default), or a
 * pseudo-terminal. */

#ifndef TTY_H
#define TTY_H

#include <stdint.h>

typedef struct TTY {
    int  (*rx_ready)(struct TTY *tty);
    int  (*rx_read)(struct TTY *tty);               /* -1 if none */
    void (*tx_write)(struct TTY *tty, uint8_t ch);
    void (*tick)(struct TTY *tty);                  /* accept a connection... */
    void (*destroy)(struct TTY *tty);
} TTY;

/* Each returns NULL on error. */
TTY *tty_stdio_create(void);
TTY *tty_tcp_create(uint16_t port);     /* 0 for 1134 */
TTY *tty_pty_create(void);

#ifdef __EMSCRIPTEN__
TTY *tty_wasm_create(void);
#endif

#endif
