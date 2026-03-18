/*
 * tty/tty_stdio.c -- Stdio TTY backend for ll-34
 *
 * Raw mode terminal, restores settings on exit.
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/select.h>
#include "tty.h"

typedef struct {
    TTY base;
    struct termios orig;
    int raw_active;
} TTY_Stdio;

static int  stdio_rx_ready(TTY *tty);
static int  stdio_rx_read(TTY *tty);
static void stdio_tx_write(TTY *tty, uint8_t ch);
static void stdio_tick(TTY *tty);
static void stdio_destroy(TTY *tty);

static TTY_Stdio *g_stdio;

static void cleanup(void) {
    if (g_stdio && g_stdio->raw_active) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_stdio->orig);
        g_stdio->raw_active = 0;
    }
}

static void sighandler(int sig) {
    (void)sig;
    cleanup();
    _exit(0);
}

TTY *tty_stdio_create(void) {
    if (!isatty(STDIN_FILENO))
        return NULL;

    TTY_Stdio *s = calloc(1, sizeof(*s));
    if (!s) return NULL;

    if (tcgetattr(STDIN_FILENO, &s->orig) < 0) {
        free(s);
        return NULL;
    }

    struct termios raw = s->orig;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= CS8;
    /* ISIG off: Ctrl-C/Z/\ passed to PDP-11 as normal characters */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        free(s);
        return NULL;
    }

    s->raw_active = 1;
    g_stdio = s;
    atexit(cleanup);
    signal(SIGTERM, sighandler);

    s->base.rx_ready  = stdio_rx_ready;
    s->base.rx_read   = stdio_rx_read;
    s->base.tx_write  = stdio_tx_write;
    s->base.tick      = stdio_tick;
    s->base.destroy   = stdio_destroy;

    return &s->base;
}

static int stdio_rx_ready(TTY *tty) {
    (void)tty;
    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

static int stdio_rx_read(TTY *tty) {
    (void)tty;
    uint8_t ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\n') ch = '\r';
        return ch;
    }
    return -1;
}

static void stdio_tx_write(TTY *tty, uint8_t ch) {
    (void)tty;
    if (ch == 0) return;
    (void)!write(STDOUT_FILENO, &ch, 1);
}

static void stdio_tick(TTY *tty) { (void)tty; }

static void stdio_destroy(TTY *tty) {
    TTY_Stdio *s = (TTY_Stdio *)tty;
    if (s->raw_active) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &s->orig);
        s->raw_active = 0;
    }
    g_stdio = NULL;
    free(s);
}
