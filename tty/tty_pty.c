#define _XOPEN_SOURCE 600
#define _DEFAULT_SOURCE
/*
 * tty/tty_pty.c -- Pseudo-terminal TTY backend for ll-34
 *
 * Slave side appears as a tty device: screen /dev/ttysNNN
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "tty.h"

typedef struct {
    TTY base;
    int master_fd;
    char slave_name[128];
} TTY_PTY;

static int  pty_rx_ready(TTY *tty);
static int  pty_rx_read(TTY *tty);
static void pty_tx_write(TTY *tty, uint8_t ch);
static void pty_tick(TTY *tty);
static void pty_destroy(TTY *tty);

TTY *tty_pty_create(void) {
    TTY_PTY *p = calloc(1, sizeof(*p));
    if (!p) return NULL;

    p->master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (p->master_fd < 0) {
        perror("ll-34: tty_pty: posix_openpt");
        free(p);
        return NULL;
    }

    if (grantpt(p->master_fd) < 0) {
        perror("ll-34: tty_pty: grantpt");
        close(p->master_fd);
        free(p);
        return NULL;
    }

    if (unlockpt(p->master_fd) < 0) {
        perror("ll-34: tty_pty: unlockpt");
        close(p->master_fd);
        free(p);
        return NULL;
    }

    const char *name = ptsname(p->master_fd);
    if (!name) {
        perror("ll-34: tty_pty: ptsname");
        close(p->master_fd);
        free(p);
        return NULL;
    }
    strncpy(p->slave_name, name, sizeof(p->slave_name) - 1);

    int flags = fcntl(p->master_fd, F_GETFL, 0);
    if (flags >= 0)
        fcntl(p->master_fd, F_SETFL, flags | O_NONBLOCK);

    fprintf(stderr, "ll-34: DL11 serial on %s\n", p->slave_name);

    p->base.rx_ready  = pty_rx_ready;
    p->base.rx_read   = pty_rx_read;
    p->base.tx_write  = pty_tx_write;
    p->base.tick      = pty_tick;
    p->base.destroy   = pty_destroy;

    return &p->base;
}

static int pty_rx_ready(TTY *tty) {
    TTY_PTY *p = (TTY_PTY *)tty;
    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(p->master_fd, &fds);
    return select(p->master_fd + 1, &fds, NULL, NULL, &tv) > 0;
}

static int pty_rx_read(TTY *tty) {
    TTY_PTY *p = (TTY_PTY *)tty;
    uint8_t ch;
    if (read(p->master_fd, &ch, 1) == 1) {
        return ch;
    }
    return -1;
}

static void pty_tx_write(TTY *tty, uint8_t ch) {
    TTY_PTY *p = (TTY_PTY *)tty;
    if (ch == 0) return;
    (void)!write(p->master_fd, &ch, 1);
}

static void pty_tick(TTY *tty) { (void)tty; }

static void pty_destroy(TTY *tty) {
    TTY_PTY *p = (TTY_PTY *)tty;
    if (p->master_fd >= 0)
        close(p->master_fd);
    free(p);
}
