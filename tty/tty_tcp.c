/*
 * tty/tty_tcp.c -- TCP server TTY backend for ll-34
 *
 * One client at a time, default port 1134. Connect with: nc localhost 1134
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "tty.h"

#define TCP_DEFAULT_PORT 1134

typedef struct {
    TTY base;
    int listen_fd;
    int client_fd;
    uint16_t port;
} TTY_TCP;

static int  tcp_rx_ready(TTY *tty);
static int  tcp_rx_read(TTY *tty);
static void tcp_tx_write(TTY *tty, uint8_t ch);
static void tcp_tick(TTY *tty);
static void tcp_destroy(TTY *tty);

static void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0)
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

TTY *tty_tcp_create(uint16_t port) {
    if (port == 0) port = TCP_DEFAULT_PORT;

    TTY_TCP *t = calloc(1, sizeof(*t));
    if (!t) return NULL;

    t->port = port;
    t->client_fd = -1;

    t->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (t->listen_fd < 0) {
        perror("ll-34: tty_tcp: socket");
        free(t);
        return NULL;
    }

    int optval = 1;
    setsockopt(t->listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);

    if (bind(t->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("ll-34: tty_tcp: bind");
        close(t->listen_fd);
        free(t);
        return NULL;
    }

    if (listen(t->listen_fd, 1) < 0) {
        perror("ll-34: tty_tcp: listen");
        close(t->listen_fd);
        free(t);
        return NULL;
    }

    set_nonblocking(t->listen_fd);

    fprintf(stderr, "ll-34: DL11 serial on tcp://localhost:%u\n", port);

    t->base.rx_ready  = tcp_rx_ready;
    t->base.rx_read   = tcp_rx_read;
    t->base.tx_write  = tcp_tx_write;
    t->base.tick      = tcp_tick;
    t->base.destroy   = tcp_destroy;

    return &t->base;
}

static void tcp_try_accept(TTY_TCP *t) {
    if (t->client_fd >= 0) return;  /* already connected */

    int fd = accept(t->listen_fd, NULL, NULL);
    if (fd < 0) return;  /* no pending connection */

    set_nonblocking(fd);

    int optval = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));

    t->client_fd = fd;
    fprintf(stderr, "ll-34: DL11 client connected\n");
}

static int tcp_rx_ready(TTY *tty) {
    TTY_TCP *t = (TTY_TCP *)tty;
    if (t->client_fd < 0) return 0;

    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(t->client_fd, &fds);
    return select(t->client_fd + 1, &fds, NULL, NULL, &tv) > 0;
}

static int tcp_rx_read(TTY *tty) {
    TTY_TCP *t = (TTY_TCP *)tty;
    if (t->client_fd < 0) return -1;

    uint8_t ch;
    ssize_t n = read(t->client_fd, &ch, 1);
    if (n == 1) {
        if (ch == '\n') ch = '\r';
        return ch;
    }
    if (n == 0) {
        fprintf(stderr, "ll-34: DL11 client disconnected\n");
        close(t->client_fd);
        t->client_fd = -1;
    }
    return -1;
}

static void tcp_tx_write(TTY *tty, uint8_t ch) {
    TTY_TCP *t = (TTY_TCP *)tty;
    if (t->client_fd < 0 || ch == 0) return;

    ssize_t n = write(t->client_fd, &ch, 1);
    if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        fprintf(stderr, "ll-34: DL11 client disconnected\n");
        close(t->client_fd);
        t->client_fd = -1;
    }
}

static void tcp_tick(TTY *tty) {
    tcp_try_accept((TTY_TCP *)tty);
}

static void tcp_destroy(TTY *tty) {
    TTY_TCP *t = (TTY_TCP *)tty;
    if (t->client_fd >= 0) close(t->client_fd);
    if (t->listen_fd >= 0) close(t->listen_fd);
    free(t);
}
