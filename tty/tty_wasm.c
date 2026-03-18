/*
 * tty/tty_wasm.c -- WebAssembly TTY backend for ll-34
 *
 * RX/TX circular queues between JS and the DL11 emulation.
 */

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <stdlib.h>
#include <stdint.h>
#include "tty.h"

#define WASM_BUF  4096
#define WASM_MASK (WASM_BUF - 1)

static uint8_t rx_buf[WASM_BUF];
static uint8_t tx_buf[WASM_BUF];
static int rx_head, rx_tail;   /* JS pushes to head, C reads from tail */
static int tx_head, tx_tail;   /* C pushes to head, JS reads from tail */


EMSCRIPTEN_KEEPALIVE
void wasm_rx_push(uint8_t ch) {
    int next = (rx_head + 1) & WASM_MASK;
    if (next != rx_tail) {
        rx_buf[rx_head] = ch;
        rx_head = next;
    }
}

EMSCRIPTEN_KEEPALIVE
int wasm_tx_poll(void) {
    if (tx_tail == tx_head) return -1;
    uint8_t ch = tx_buf[tx_tail];
    tx_tail = (tx_tail + 1) & WASM_MASK;
    return ch;
}

static int wasm_rx_ready(TTY *tty) {
    (void)tty;
    return rx_tail != rx_head;
}

static int wasm_rx_read(TTY *tty) {
    (void)tty;
    if (rx_tail == rx_head) return -1;
    uint8_t ch = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) & WASM_MASK;
    return ch;
}

static void wasm_tx_write(TTY *tty, uint8_t ch) {
    (void)tty;
    if (ch == 0) return;
    int next = (tx_head + 1) & WASM_MASK;
    if (next != tx_tail) {
        tx_buf[tx_head] = ch;
        tx_head = next;
    }
}

static void wasm_tick_fn(TTY *tty) { (void)tty; }

static void wasm_destroy(TTY *tty) { free(tty); }

TTY *tty_wasm_create(void) {
    TTY *t = calloc(1, sizeof(TTY));
    if (!t) return NULL;
    t->rx_ready = wasm_rx_ready;
    t->rx_read  = wasm_rx_read;
    t->tx_write = wasm_tx_write;
    t->tick     = wasm_tick_fn;
    t->destroy  = wasm_destroy;
    return t;
}
