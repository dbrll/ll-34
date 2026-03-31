/*
 * ll-34 -- Circuit-Level PDP-11/34A Emulator
 *
 * System integration: wires the KD11-EA CPU, UNIBUS, RAM, M9301 boot ROM,
 * DL11 serial console, RK05 disk, and KW11 line clock into a complete
 * PDP-11/34A system.  Also hosts the programmer console (Ctrl-P),
 * debug console (Ctrl-E), and logic analyzer (Ctrl-L).
 *
 * Damien Boureille, 2026
 * MIT Licence
 */

#define _DEFAULT_SOURCE   /* usleep, strncasecmp on musl/glibc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <libgen.h>
#include <sys/select.h>
#include "unibus/unibus.h"
#include "unibus/ram.h"
#include "tty/tty.h"
#include "m9301/rom.h"
#include "dl11/dl11.h"
#include "rk11/rk11.h"
#include "rl11/rl11.h"
#include "kd11ea/kd11ea.h"
#include "kd11ea/ucode_labels.h"
#include "kd11ea/clock.h"
#include "kw11/kw11.h"
#include "pc11/pc11.h"
#include "console/console.h"
#include "probe/probe.h"
#include "debug/disasm.h"
#include "debug/memdump.h"
#include "unibus/loader.h"
#include "trace.h"

/* PSW at 177776, accessible as a UNIBUS register */
#define PSW_ADDR  0x3FFFE  /* 777776 octal (18-bit) */
#define SWREG_ADDR 0x3FF78  /* 777570 octal (18-bit) */

static KD11EA cpu;
static Console con;
static TTY *tty;
static Bus *g_bus;   /* set in main(), used by debug_cli() */
static Ram *g_ram;   /* set in main(), used by debug_cli() */
static volatile sig_atomic_t got_sigterm;
static int console_shown_help;     /* show help banner on first halt */

/* --- Breakpoints --- */
#define MAX_BREAKPOINTS 16
static struct { uint16_t addr; int active; } breakpoints[MAX_BREAKPOINTS];
static int num_breakpoints;
static void sigterm_handler(int sig) { (void)sig; got_sigterm = 1; }

/* Operator keys (intercepted before reaching the OS) */
#define CONSOLE_HALT_CHAR 0x10  /* Ctrl-P - programmer console (halt) */
#define DEBUG_CHAR        0x05  /* Ctrl-E - debug console */
#define PROBE_CHAR        0x0C  /* Ctrl-L - logic analyzer */
static int halt_requested;
static int debug_requested;
static int probe_requested;

/* Pushback: non-operator chars read during polling are fed back to DL11 */
static int stdin_pushback = -1;
static int tty_uses_stdin;

/* Operator key polling via raw stdin */
static struct termios stdin_orig_termios;
static int stdin_raw_active;

static void stdin_raw_leave(void) {
    if (stdin_raw_active) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &stdin_orig_termios);
        stdin_raw_active = 0;
    }
}

static int stdin_raw_enter(void) {
    if (!isatty(STDIN_FILENO)) return -1;
    if (tcgetattr(STDIN_FILENO, &stdin_orig_termios) < 0) return -1;
    struct termios raw = stdin_orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) return -1;
    stdin_raw_active = 1;
    atexit(stdin_raw_leave);
    return 0;
}

static int stdin_rx_ready(void) {
    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

static int stdin_rx_read(void) {
    uint8_t ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) return ch;
    return -1;
}

/* DL11 callback adapters (bridge TTY vtable, intercept operator keys) */

static int dl11_tty_rx_ready(void *ctx) {
    if (stdin_pushback >= 0) return 1;
    return ((TTY *)ctx)->rx_ready((TTY *)ctx);
}

static int dl11_tty_rx_read(void *ctx) {
    int ch;
    if (stdin_pushback >= 0) {
        ch = stdin_pushback;
        stdin_pushback = -1;
    } else {
        TTY *t = (TTY *)ctx;
        ch = t->rx_read(t);
    }
    if (ch == CONSOLE_HALT_CHAR) {
        halt_requested = 1;
        return -1;
    }
    if (ch == DEBUG_CHAR) {
        debug_requested = 1;
        return -1;
    }
    if (ch == PROBE_CHAR) {
        probe_requested = 1;
        return -1;
    }
    return ch;
}

static void dl11_tty_tx_write(void *ctx, uint8_t ch) {
    TTY *t = (TTY *)ctx;
    t->tx_write(t, ch);
}

static int map_console_key(int ch) {
    if (ch >= '0' && ch <= '7') return KEY_0 + (ch - '0');
    switch (ch) {
    case 'e':              return KEY_EXAM;
    case 'd':              return KEY_DEP;
    case 'l':              return KEY_LAD;
    case 'S':              return KEY_START;
    case 'c':  case 'C':  return KEY_CONT;
    case 's':              return KEY_HALT_SS;
    case 'b':  case 'B':  return KEY_BOOT;
    case 'i':  case 'I':  return KEY_INIT;
    case 'a':              return KEY_DIS_AD;
    case 0x1B:             return KEY_CLR;  /* Escape */
    }
    return -1;
}

static void console_prompt(void) {
    fprintf(stderr, "\r%06o %06o > ", console_addr_ptr(&con), con.display);
}

static void console_help(void) {
    fprintf(stderr,
        "\r\n"
        "  0-7     octal digit       l   load address\r\n"
        "  e       examine next      d   deposit\r\n"
        "  S       start at addr     c   continue\r\n"
        "  s       single step       b   boot\r\n"
        "  i       bus init          Esc clear\r\n"
        "  a       display address   q   quit ll-34\r\n"
        "\r\n");
}

static void enter_console_mode(const char *reason) {
    fprintf(stderr, "\r\nll-34: %s at PC=%06o PSW=%06o\r\n",
            reason, PC(&cpu), cpu.psw);
    if (!console_shown_help) {
        console_shown_help = 1;
        fprintf(stderr, "Programmer console (? for help)\r\n");
    }
    console_set_addr(&con, PC(&cpu));
    console_key(&con, KEY_EXAM);
    console_prompt();
}

static void probe_cli(void);  /* forward declaration */

/* Poll stdin for console keys. Returns -1 on quit. */
static int console_poll(void) {
    if (!stdin_rx_ready())
        return 0;
    int ch = stdin_rx_read();
    if (ch < 0)
        return 0;

    /* Quit with confirmation */
    if (ch == 'q' || ch == 'Q' || ch == 0x03) {
        fprintf(stderr, "\r\nTurn off CPU and leave ll-34? (y/n) ");
        for (;;) {
            if (!stdin_rx_ready()) { usleep(10000); continue; }
            int c2 = stdin_rx_read();
            if (c2 == 'y' || c2 == 'Y') {
                fprintf(stderr, "y\r\n");
                return -1;
            }
            fprintf(stderr, "n\r\n");
            console_prompt();
            return 0;
        }
    }

    /* '?' = help */
    if (ch == '?') {
        console_help();
        console_prompt();
        return 0;
    }

    if (ch == PROBE_CHAR) {
        probe_cli();
        console_prompt();
        return 0;
    }

    int key = map_console_key(ch);
    if (key < 0)
        return 0;

    /* Auto-prefix CNTRL (no physical modifier in CLI) */
    if (key == KEY_CONT || key == KEY_START ||
        key == KEY_BOOT || key == KEY_INIT) {
        console_key(&con, KEY_CNTRL);
    }
    /* HLT/SS: CNTRL only when running (halted = single-step) */
    if (key == KEY_HALT_SS && !cpu.halted) {
        console_key(&con, KEY_CNTRL);
    }

    console_key(&con, key);

    /* Auto-EXAM after address-changing actions */
    if (key == KEY_HALT_SS && cpu.halted) {
        console_set_addr(&con, PC(&cpu));
        console_key(&con, KEY_EXAM);
    } else if (key == KEY_LAD && cpu.halted) {
        console_key(&con, KEY_EXAM);
    } else if (key == KEY_DEP && cpu.halted) {
        console_key(&con, KEY_EXAM);
    }

    console_prompt();
    return 0;
}

/* BUS INIT: reset device registers (BINIT L) */
static void do_bus_reset(DL11 *dl, RK05 *rk, KW11 *kw, Bus *bus) {
    dl->rcsr = 0;
    dl->rbuf = 0;
    dl->xcsr = DL11_XCSR_RDY;
    dl->tx_done_at_ns = 0;

    /* RK05: clear errors, controller done, preserve disk state */
    rk->rker = 0;
    rk->rkcs = RKCS_DONE;
    rk->rkwc = 0;
    rk->rkba = 0;
    rk->rkda = 0;

    /* KW11-L: clear status */
    kw->lks = 0;

    /* Bus: clear NXM */
    bus->nxm = 0;

    int_init(&cpu.intc);
}

static int psw_read(void *dev, uint32_t addr, uint16_t *data) {
    (void)dev; (void)addr;
    *data = cpu.psw;
    return 0;
}

static int psw_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    (void)dev; (void)addr;
    if (is_byte) {
        /* DATOB: low byte only */
        cpu.psw = (cpu.psw & 0xFF00) | (data & 0xFF);
    } else {
        cpu.psw = data;
    }
    return 0;
}

static int swreg_read(void *dev, uint32_t addr, uint16_t *data) {
    (void)dev; (void)addr;
    *data = con.switch_reg;
    return 0;
}

static int swreg_write(void *dev, uint32_t addr, uint16_t data, int is_byte) {
    (void)dev; (void)addr; (void)is_byte;
    con.switch_reg = data;
    return 0;
}

static void cpu_irq_set(void *ctx, uint16_t vector, uint8_t priority) {
    (void)ctx;
    int_request(&cpu.intc, vector, priority);
}

static void cpu_irq_clr(void *ctx, uint16_t vector) {
    (void)ctx;
    int_cancel(&cpu.intc, vector);
}

/* Power-on bootstrap: microcode copies PC to R5, then loads vector */
static void power_on(KD11EA *cpu, Bus *bus) {
    uint16_t pc, ps;
    REG(cpu, 5) = 0;  /* flow 26-N: PC cleared before copy to R5 */

    bus_read(bus, 0x3F614, &pc);   /* 773024: prototype PC (OR'd with S1, 18-bit) */
    bus_read(bus, 0x3F616, &ps);   /* 773026: prototype PS (18-bit) */
    PC(cpu) = pc;
    cpu->psw = ps;
    cpu->mpc = 0;
    cpu->halted = 0;
    fprintf(stderr, "ll-34: power-on vector PC=%06o PS=%06o\n", pc, ps);
}

/* Logic analyzer CLI (backend in probe/probe.c) */

#define PROBE_DEFAULT_DEPTH  4096

static Probe probe;
static ProbeSnapshot *probe_buf;
static int probe_notified;  /* 1 = "capture complete" already printed */

/* Parse octal string, returns (uint32_t)-1 on error */
static uint32_t parse_octal(const char *s) {
    uint32_t val = 0;
    if (!s || !*s) return (uint32_t)-1;
    while (*s) {
        if (*s < '0' || *s > '7') return (uint32_t)-1;
        val = (val << 3) | (*s - '0');
        s++;
    }
    return val;
}

/* Command history */
#define HIST_MAX 16
#define HIST_LINE 256
static char hist_buf[HIST_MAX][HIST_LINE];
static int hist_count;  /* total entries added */

static void hist_add(const char *line) {
    if (!line[0]) return;  /* skip empty */
    /* Skip duplicates */
    if (hist_count > 0) {
        int prev = (hist_count - 1) % HIST_MAX;
        if (strcmp(hist_buf[prev], line) == 0) return;
    }
    int slot = hist_count % HIST_MAX;
    strncpy(hist_buf[slot], line, HIST_LINE - 1);
    hist_buf[slot][HIST_LINE - 1] = '\0';
    hist_count++;
}

/* Blocking terminal read */
static int term_getc(void) {
    for (;;) {
        if (stdin_rx_ready())
            return stdin_rx_read();
        usleep(10000);
    }
}

static void line_replace(int old_len, const char *new_str, int new_len) {
    for (int i = 0; i < old_len; i++) fprintf(stderr, "\b \b");
    fwrite(new_str, 1, new_len, stderr);
}

/* Tab completion for probe/debug CLI */
static const char *probe_commands[] = {
    "list", "add", "rm", "clear", "trigger", "depth", "pos",
    "arm", "disarm", "status", "show", "dump", "quit", "help", NULL
};

static int probe_complete(char *buf, int n, int max) {
    int word_start = n;
    while (word_start > 0 && buf[word_start - 1] != ' ' && buf[word_start - 1] != '\t')
        word_start--;
    int prefix_len = n - word_start;
    if (prefix_len == 0) return n;

    char prefix[80];
    if (prefix_len >= (int)sizeof(prefix)) return n;
    memcpy(prefix, buf + word_start, prefix_len);
    prefix[prefix_len] = '\0';

    /* First word = command, subsequent = signal */
    int is_first_word = 1;
    for (int i = 0; i < word_start; i++) {
        if (buf[i] != ' ' && buf[i] != '\t') { is_first_word = 0; break; }
    }

    const char *matches[128];
    int nmatch = 0;

    if (is_first_word) {
        /* Complete commands */
        for (int i = 0; probe_commands[i]; i++) {
            if (strncasecmp(probe_commands[i], prefix, prefix_len) == 0
                && nmatch < 128)
                matches[nmatch++] = probe_commands[i];
        }
    } else {
        /* Complete signal names (chip_pin and alias) */
        int count = probe_def_count();
        for (int i = 0; i < count && nmatch < 128; i++) {
            const ProbeDef *d = probe_def_get(i);
            if (d->chip_pin && strncasecmp(d->chip_pin, prefix, prefix_len) == 0)
                matches[nmatch++] = d->chip_pin;
            if (d->alias && strncasecmp(d->alias, prefix, prefix_len) == 0
                && nmatch < 128)
                matches[nmatch++] = d->alias;
        }
    }

    if (nmatch == 0) return n;

    if (nmatch == 1) {
        const char *m = matches[0];
        int mlen = (int)strlen(m);
        int add = mlen - prefix_len;
        if (word_start + mlen + 1 < max) {
                fwrite(m + prefix_len, 1, add, stderr);
            fprintf(stderr, " ");
            memcpy(buf + word_start, m, mlen);
            buf[word_start + mlen] = ' ';
            return word_start + mlen + 1;
        }
        return n;
    }

    /* Multiple matches: extend to common prefix */
    int common = prefix_len;
    for (;; common++) {
        char c = matches[0][common];
        if (!c) break;
        int all_same = 1;
        for (int i = 1; i < nmatch; i++) {
            if (!matches[i][common] || matches[i][common] != c) {
                all_same = 0;
                break;
            }
        }
        if (!all_same) break;
    }

    if (common > prefix_len) {
        int add = common - prefix_len;
        if (word_start + common < max) {
            fwrite(matches[0] + prefix_len, 1, add, stderr);
            memcpy(buf + word_start, matches[0], common);
            return word_start + common;
        }
    }

    fprintf(stderr, "\r\n");
    for (int i = 0; i < nmatch; i++)
        fprintf(stderr, "  %s\r\n", matches[i]);
    fprintf(stderr, "probe> ");
    buf[n] = '\0';
    fwrite(buf, 1, n, stderr);
    return n;
}

/* Readline with echo, history, and tab completion. Returns -1 on Ctrl-C. */
static int probe_readline(char *buf, int max) {
    int n = 0;
    int hist_pos = hist_count;  /* browsing position: hist_count = "new line" */

    while (n < max - 1) {
        int ch = term_getc();

        if (ch == 0x03) return -1;       /* Ctrl-C → quit probe CLI */

        if (ch == '\r' || ch == '\n') {
            fprintf(stderr, "\r\n");
            buf[n] = '\0';
            if (n > 0) hist_add(buf);
            return n;
        }

        if (ch == 0x7F || ch == 0x08) {  /* backspace/delete */
            if (n > 0) {
                n--;
                fprintf(stderr, "\b \b");
            }
            continue;
        }

        if (ch == 0x09) {  /* Tab → autocomplete */
            buf[n] = '\0';
            n = probe_complete(buf, n, max);
            continue;
        }

        /* ESC sequence: arrow keys → ESC [ A/B */
        if (ch == 0x1B) {
            int c2 = term_getc();
            if (c2 != '[') continue;
            int c3 = term_getc();
            if (c3 == 'A') {
                /* Up arrow - older history */
                int avail = hist_count < HIST_MAX ? hist_count : HIST_MAX;
                if (avail == 0) continue;
                if (hist_pos > hist_count - avail)
                    hist_pos--;
                if (hist_pos < 0) hist_pos = 0;
                int slot = hist_pos % HIST_MAX;
                int hlen = (int)strlen(hist_buf[slot]);
                line_replace(n, hist_buf[slot], hlen);
                memcpy(buf, hist_buf[slot], hlen);
                n = hlen;
            } else if (c3 == 'B') {
                /* Down arrow - newer history */
                if (hist_pos < hist_count) {
                    hist_pos++;
                    if (hist_pos >= hist_count) {
                        /* Back to empty "new" line */
                        line_replace(n, "", 0);
                        n = 0;
                    } else {
                        int slot = hist_pos % HIST_MAX;
                        int hlen = (int)strlen(hist_buf[slot]);
                        line_replace(n, hist_buf[slot], hlen);
                        memcpy(buf, hist_buf[slot], hlen);
                        n = hlen;
                    }
                }
            }
            /* Ignore other escape sequences (left/right/etc.) */
            continue;
        }

        if (ch >= 0x20 && ch < 0x7F) {
            buf[n++] = ch;
            fprintf(stderr, "%c", ch);
        }
    }
    buf[n] = '\0';
    return n;
}

static int tokenize(char *line, char **tokens, int max_tokens) {
    int n = 0;
    char *p = line;
    while (*p && n < max_tokens) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        tokens[n++] = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) *p++ = '\0';
    }
    return n;
}

static int probe_dump_csv(Probe *p, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;

    fprintf(f, "ns");
    for (int i = 0; i < p->nsignals; i++) {
        const ProbeDef *def = probe_def_get(p->signal_idx[i]);
        fprintf(f, ",%s", probe_display_name(def));
    }
    fprintf(f, "\n");

    uint32_t start;
    uint32_t total;
    if (p->count >= p->depth) {
        start = p->head;   /* oldest entry (just wrapped) */
        total = p->depth;
    } else {
        start = 0;
        total = p->count;
    }

    for (uint32_t i = 0; i < total; i++) {
        uint32_t idx = (start + i) & p->mask;
        const ProbeSnapshot *snap = &p->buf[idx];

        fprintf(f, "%llu", (unsigned long long)snap->ns);

        for (int s = 0; s < p->nsignals; s++) {
            const ProbeDef *def = probe_def_get(p->signal_idx[s]);
            uint32_t val = probe_read_value(snap, def);
            if (def->bit != PROBE_BIT_WHOLE)
                fprintf(f, ",%u", val);
            else
                fprintf(f, ",%o", val);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return (int)total;
}

static const char *fmt_comma(uint32_t val) {
    static char buf[20];
    char tmp[20];
    int len = snprintf(tmp, sizeof(tmp), "%u", val);
    int commas = (len - 1) / 3;
    int pos = 0;
    int first = len % 3;
    if (first == 0) first = 3;
    for (int i = 0; i < len; ) {
        int chunk = (i == 0) ? first : 3;
        if (i > 0) buf[pos++] = ',';
        for (int j = 0; j < chunk; j++)
            buf[pos++] = tmp[i++];
    }
    buf[pos] = '\0';
    (void)commas;
    return buf;
}

static void probe_cmd_list(int ntok, char **tok) {
    int count = probe_def_count();
    if (ntok >= 2) {
        const char *prefix = tok[1];
        int plen = strlen(prefix);
        for (int i = 0; i < count; i++) {
            const ProbeDef *d = probe_def_get(i);
            if (d->chip_pin && strncasecmp(d->chip_pin, prefix, plen) == 0) {
                fprintf(stderr, "  %-16s %-12s %s  [%s]\r\n",
                        d->chip_pin,
                        d->alias ? d->alias : "",
                        d->desc,
                        d->level == 1 ? "direct" : "derived");
            }
        }
        return;
    }

    char prev_prefix[32] = "";
    for (int i = 0; i < count; i++) {
        const ProbeDef *d = probe_def_get(i);
        if (!d->chip_pin) continue;
        const char *last_colon = strrchr(d->chip_pin, ':');
        if (!last_colon) continue;
        int prefix_len = (int)(last_colon - d->chip_pin);
        char prefix[32];
        if (prefix_len >= (int)sizeof(prefix)) prefix_len = sizeof(prefix) - 1;
        memcpy(prefix, d->chip_pin, prefix_len);
        prefix[prefix_len] = '\0';
        const char *pin_str = last_colon + 1;  /* "1", "5", "*" */

        if (strcmp(prev_prefix, prefix) != 0) {
            if (prev_prefix[0]) fprintf(stderr, "\r\n");
            fprintf(stderr, "  %s  [%s]\r\n", prefix,
                    d->level == 1 ? "direct" : "derived");
            memcpy(prev_prefix, prefix, prefix_len + 1);
        }
        fprintf(stderr, "    :%s\t%-12s %s\r\n",
                pin_str, d->alias ? d->alias : "", d->desc);
    }
    fprintf(stderr, "\r\n");
}

static void probe_cmd_show(Probe *p, int show_lines, int ntok, char **tok) {
    uint32_t total = p->count < p->depth ? p->count : p->depth;
    if (total == 0) {
        fprintf(stderr, "  no data captured\r\n");
        return;
    }

    int want = show_lines;
    if (ntok >= 2) {
        if (strcmp(tok[1], "all") == 0)
            want = (int)total;
        else
            want = atoi(tok[1]);
        if (want <= 0) want = show_lines;
    }
    if ((uint32_t)want > total) want = (int)total;

    uint32_t buf_start = (p->count >= p->depth) ? p->head : 0;
    uint32_t win_start_idx;  /* index into capture (0 = oldest) */

    if (p->trigger_signal >= 0 && p->trigger_sample != 0) {
        uint32_t trig_abs = p->trigger_sample;  /* ring index of trigger */
        uint32_t trig_rel;
        if (p->count >= p->depth) {
            trig_rel = (trig_abs - buf_start) & p->mask;
        } else {
            trig_rel = trig_abs;
        }
        int half = want / 2;
        if ((int)trig_rel < half)
            win_start_idx = 0;
        else if (trig_rel + (want - half) > total)
            win_start_idx = total - want;
        else
            win_start_idx = trig_rel - half;
    } else {
        win_start_idx = (total > (uint32_t)want) ? total - want : 0;
    }

    int col_w[PROBE_MAX_SIGNALS];
    for (int s = 0; s < p->nsignals; s++) {
        const ProbeDef *def = probe_def_get(p->signal_idx[s]);
        const char *name = def->alias ? def->alias : probe_display_name(def);
        int nlen = (int)strlen(name);
        col_w[s] = nlen > 6 ? nlen : 6;
    }

    uint32_t ref_ring_idx = (buf_start + win_start_idx) & p->mask;
    uint64_t ref_ns = p->buf[ref_ring_idx].ns;

    uint32_t trig_rel = 0;
    int has_trigger = 0;
    if (p->trigger_signal >= 0) {
        if (p->count >= p->depth)
            trig_rel = (p->trigger_sample - buf_start) & p->mask;
        else
            trig_rel = p->trigger_sample;
        has_trigger = 1;
    }

    uint32_t eff_ns = CYCLE_SHORT_NS * probe.divider;
    uint32_t sr = 1000000000u / eff_ns;
    if (probe.divider == 1)
        fprintf(stderr, "  SR: %s Hz (realtime CPU clock), resolution: %u ns/sample\r\n",
                fmt_comma(sr), eff_ns);
    else
        fprintf(stderr, "  SR: %s Hz (1:%u divider), resolution: %u ns/sample\r\n",
                fmt_comma(sr), probe.divider, eff_ns);

    fprintf(stderr, "   # delta-ns ");
    for (int s = 0; s < p->nsignals; s++) {
        const ProbeDef *def = probe_def_get(p->signal_idx[s]);
        const char *name = def->alias ? def->alias : probe_display_name(def);
        fprintf(stderr, " %*s", col_w[s], name);
    }
    fprintf(stderr, "\r\n");

    fprintf(stderr, "  -- -------- ");
    for (int s = 0; s < p->nsignals; s++)
        fprintf(stderr, " %.*s", col_w[s], "----------------");
    fprintf(stderr, "\r\n");

    for (int row = 0; row < want; row++) {
        uint32_t idx = win_start_idx + row;
        uint32_t ring_idx = (buf_start + idx) & p->mask;
        const ProbeSnapshot *snap = &p->buf[ring_idx];

        char marker = (has_trigger && idx == trig_rel) ? '>' : ' ';
        int64_t delta = (int64_t)(snap->ns - ref_ns);

        fprintf(stderr, "%c%3u %8lld ", marker, idx, (long long)delta);
        for (int s = 0; s < p->nsignals; s++) {
            const ProbeDef *def = probe_def_get(p->signal_idx[s]);
            uint32_t val = probe_read_value(snap, def);
            if (def->bit != PROBE_BIT_WHOLE)
                fprintf(stderr, " %*u", col_w[s], val);
            else
                fprintf(stderr, " %*o", col_w[s], val);
        }
        fprintf(stderr, "\r\n");
    }

    fprintf(stderr, "  (%u samples, showing %u-%u", total,
            win_start_idx, win_start_idx + want - 1);
    if (has_trigger)
        fprintf(stderr, ", trigger at #%u", trig_rel);
    fprintf(stderr, ")\r\n");
}

/* Debug console: read via bus with MMU translation */
static int debug_read_word(uint16_t vba, void *ctx) {
    (void)ctx;
    uint32_t pba;
    if (cpu.mmu.sr0 & SR0_ENABLE) {
        int mode = (cpu.psw >> 14) & 3;
        if (mmu_translate(&cpu.mmu, vba, mode, 0, PC(&cpu), &pba) < 0) {
            /* MMU abort - direct mapping fallback */
            pba = vba;
            if ((vba & 0xE000) == 0xE000)
                pba = 0x30000 | vba;
        }
    } else {
        pba = vba;
        if ((vba & 0xE000) == 0xE000)
            pba = 0x30000 | vba;
    }
    uint16_t data;
    if (bus_read(g_bus, pba, &data) < 0)
        return -1;
    return data;
}

static void debug_show_mmu(void) {
    MMU *m = &cpu.mmu;
    int enabled = (m->sr0 & SR0_ENABLE) != 0;
    int mode = (cpu.psw >> 14) & 3;

    fprintf(stderr, "  SR0=%06o  SR2=%06o  %s  mode=%s\r\n",
            m->sr0, m->sr2,
            enabled ? "ENABLED" : "disabled",
            mode == 0 ? "kernel" : mode == 3 ? "user" : "???");

    if (!enabled) return;

    static const char *acf_str[4] = { "NR", "RO", "--", "RW" };
    static const struct { int base; const char *name; } modes[2] = {
        { 0, "Kernel" }, { 8, "User" }
    };
    for (int mi = 0; mi < 2; mi++) {
        int base = modes[mi].base;
        fprintf(stderr, "  %s pages%s:\r\n", modes[mi].name,
                ((mi == 0 && mode == 0) || (mi == 1 && mode == 3)) ? " (current)" : "");
        fprintf(stderr, "  Page  PAR    PDR    ACF  ED  PLF   W   VBA range     PBA base\r\n");
        for (int p = 0; p < 8; p++) {
            uint16_t par = m->par[base + p];
            uint16_t pdr = m->pdr[base + p];
            int acf = (pdr >> 1) & 3;
            int ed  = (pdr >> 3) & 1;
            int plf = (pdr >> 8) & 0x7F;
            int w   = (pdr >> 6) & 1;

            fprintf(stderr, "  %d     %06o %06o %s   %s  %03o   %d   %06o-%06o  %06o\r\n",
                    p, par, pdr,
                    acf_str[acf],
                    ed ? "D" : "U",
                    plf, w,
                    (unsigned)(p << 13),
                    (unsigned)((p << 13) | 017777),
                    (unsigned)((par & 0xFFF) << 6));
        }
    }
}

static void debug_cli(void) {
    fprintf(stderr, "\r\nDebug console (? for help)\r\n");

    char line[256];
    char *tok[16];
    for (;;) {
        fprintf(stderr, "debug> ");
        int len = probe_readline(line, sizeof(line));
        if (len < 0) return;  /* Ctrl-C */
        if (len == 0) continue;

        int ntok = tokenize(line, tok, 16);
        if (ntok == 0) continue;

        if (strcmp(tok[0], "q") == 0 || strcmp(tok[0], "quit") == 0) {
            return;
        }
        else if (strcmp(tok[0], "g") == 0 || strcmp(tok[0], "go") == 0) {
            cpu.halted = 0;
            return;
        }
        else if (strcmp(tok[0], "?") == 0 || strcmp(tok[0], "help") == 0) {
            fprintf(stderr,
                "  s [n]            Single-step n micro-instructions (default: 1)\r\n"
                "  g                Go (resume execution)\r\n"
                "  b addr           Set breakpoint at address (octal)\r\n"
                "  bl               List breakpoints\r\n"
                "  bd n             Delete breakpoint #n\r\n"
                "  u [addr] [n]     Unassemble (default: PC, 16 insns)\r\n"
                "  c [addr] [n] [@file]  Core dump (default: PC, 32 words)\r\n"
                "  r                Show registers\r\n"
                "  mmu              Show MMU state (SR0, PAR/PDR)\r\n"
                "  save <file>      Save RAM to binary file\r\n"
                "  q                Return to programmer console\r\n");
        }
        else if (strcmp(tok[0], "s") == 0) {
            int count = 1;
            if (ntok >= 2) count = atoi(tok[1]);
            if (count < 1) count = 1;
            if (count > 4096) count = 4096;

            cpu.halted = 0;
            for (int i = 0; i < count; i++) {
                uint32_t ns = kd11ea_ustep(&cpu);
                if (cpu.dbg.count > 0) {
                    const DbgEntry *e = &cpu.dbg.ring[(cpu.dbg.head - 1) & (DBG_RING_SIZE - 1)];
                    fprintf(stderr, "  MPC=%03o -> %03o  A=%06o B=%06o ALU=%06o",
                            e->mpc, e->next_mpc, e->a_leg, e->b_leg, e->alu_out);
                    if (e->bus_op == 1)
                        fprintf(stderr, "  RD@%06o=%06o", e->ba, e->unibus_data);
                    else if (e->bus_op == 2)
                        fprintf(stderr, "  WR@%06o=%06o", e->ba, e->unibus_data);
                    if (e->ir_loaded)
                        fprintf(stderr, "  IR<-%06o", e->ir);
                    if (e->service_trap)
                        fprintf(stderr, "  SERVICE");
                    if (e->ir_decoded)
                        fprintf(stderr, "  DECODE->%03o", e->ir_decode_mpc);
                    fprintf(stderr, "  [PC=%06o PSW=%06o SP=%06o]\r\n",
                            e->pc, e->psw, e->sp);
                    const UcodeLabel *lbl = &ucode_labels[e->mpc];
                    fprintf(stderr, "  OP= %s", lbl->op ? lbl->op : "?");
                    if (lbl->desc)
                        fprintf(stderr, " [DESC= %s]", lbl->desc);
                    fprintf(stderr, "\r\n");
                }
                if (cpu.halted || ns == 0) {
                    fprintf(stderr, "  CPU halted\r\n");
                    break;
                }
            }
            cpu.halted = 1;
        }
        else if (strcmp(tok[0], "b") == 0 && ntok >= 2) {
            unsigned int addr;
            sscanf(tok[1], "%o", &addr);
            if (num_breakpoints >= MAX_BREAKPOINTS) {
                fprintf(stderr, "  max breakpoints reached (%d)\r\n", MAX_BREAKPOINTS);
            } else {
                breakpoints[num_breakpoints].addr = (uint16_t)addr;
                breakpoints[num_breakpoints].active = 1;
                fprintf(stderr, "  breakpoint #%d at %06o\r\n", num_breakpoints, addr & 0xFFFF);
                num_breakpoints++;
            }
        }
        else if (strcmp(tok[0], "bl") == 0) {
            int found = 0;
            for (int i = 0; i < num_breakpoints; i++) {
                if (breakpoints[i].active) {
                    fprintf(stderr, "  #%d  %06o\r\n", i, breakpoints[i].addr);
                    found = 1;
                }
            }
            if (!found) fprintf(stderr, "  no breakpoints set\r\n");
        }
        else if (strcmp(tok[0], "bd") == 0 && ntok >= 2) {
            int idx = atoi(tok[1]);
            if (idx >= 0 && idx < num_breakpoints && breakpoints[idx].active) {
                breakpoints[idx].active = 0;
                fprintf(stderr, "  breakpoint #%d deleted\r\n", idx);
            } else {
                fprintf(stderr, "  invalid breakpoint #%d\r\n", idx);
            }
        }
        else if (strcmp(tok[0], "u") == 0) {
            unsigned int addr;
            int count = 16;
            if (ntok >= 2)
                sscanf(tok[1], "%o", &addr);
            else
                addr = PC(&cpu);
            if (ntok >= 3)
                count = atoi(tok[2]);
            if (count < 1) count = 1;
            if (count > 256) count = 256;

            uint16_t a = (uint16_t)addr;
            for (int i = 0; i < count; i++) {
                char disasm_buf[80];
                int nbytes = pdp11_disasm(a, debug_read_word, NULL,
                                          disasm_buf, sizeof(disasm_buf));
                int w0 = debug_read_word(a, NULL);
                char opcodes[40];
                if (nbytes == 2)
                    snprintf(opcodes, sizeof(opcodes), "%06o",
                             w0 < 0 ? 0 : w0);
                else if (nbytes == 4) {
                    int w1 = debug_read_word((uint16_t)(a + 2), NULL);
                    snprintf(opcodes, sizeof(opcodes), "%06o %06o",
                             w0 < 0 ? 0 : w0, w1 < 0 ? 0 : w1);
                } else {
                    int w1 = debug_read_word((uint16_t)(a + 2), NULL);
                    int w2 = debug_read_word((uint16_t)(a + 4), NULL);
                    snprintf(opcodes, sizeof(opcodes), "%06o %06o %06o",
                             w0 < 0 ? 0 : w0, w1 < 0 ? 0 : w1,
                             w2 < 0 ? 0 : w2);
                }
                fprintf(stderr, "  %06o: %-20s %s\r\n", a, opcodes, disasm_buf);
                a = (uint16_t)(a + nbytes);
            }
        }
        else if (strcmp(tok[0], "c") == 0) {
            unsigned int addr;
            int count = 32;
            const char *outfile = NULL;

            for (int i = 1; i < ntok; i++) {
                if (tok[i][0] == '@') {
                    outfile = tok[i] + 1;
                } else if (i == 1) {
                    sscanf(tok[1], "%o", &addr);
                } else if (i == 2) {
                    count = atoi(tok[2]);
                }
            }
            if (ntok < 2 || (ntok == 2 && outfile))
                addr = PC(&cpu);
            if (count < 1) count = 1;
            if (count > 65536) count = 65536;

            if (outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    fprintf(stderr, "  cannot open %s\r\n", outfile);
                } else {
                    mem_dump((uint16_t)addr, count, debug_read_word, NULL, f, "\n");
                    fclose(f);
                    fprintf(stderr, "  dumped %d words to %s\r\n", count, outfile);
                }
            } else {
                mem_dump((uint16_t)addr, count, debug_read_word, NULL, stderr, "\r\n");
            }
        }
        else if (strcmp(tok[0], "r") == 0) {
            fprintf(stderr,
                "  R0=%06o  R1=%06o  R2=%06o  R3=%06o\r\n"
                "  R4=%06o  R5=%06o  SP=%06o  PC=%06o\r\n"
                "  PSW=%06o [%c%c%c%c%c pri=%d %s]\r\n",
                REG(&cpu, 0), REG(&cpu, 1), REG(&cpu, 2), REG(&cpu, 3),
                REG(&cpu, 4), REG(&cpu, 5), REG(&cpu, 6), PC(&cpu),
                cpu.psw,
                (cpu.psw & 0x10) ? 'T' : '-',
                (cpu.psw & 0x08) ? 'N' : '-',
                (cpu.psw & 0x04) ? 'Z' : '-',
                (cpu.psw & 0x02) ? 'V' : '-',
                (cpu.psw & 0x01) ? 'C' : '-',
                (cpu.psw >> 5) & 7,
                ((cpu.psw >> 14) & 3) == 0 ? "kernel" : "user");
        }
        else if (strcmp(tok[0], "mmu") == 0) {
            debug_show_mmu();
        }
        else if (strcmp(tok[0], "save") == 0) {
            if (ntok < 2) {
                fprintf(stderr, "  usage: save <filename>\r\n");
            } else {
                if (mem_save(tok[1], g_ram->mem, g_ram->size_words) < 0)
                    fprintf(stderr, "  error writing %s\r\n", tok[1]);
                else
                    fprintf(stderr, "  saved %u words (%uKB) to %s\r\n",
                            g_ram->size_words, g_ram->size_words / 512, tok[1]);
            }
        }
        else {
            fprintf(stderr, "  unknown command: %s (? for help)\r\n", tok[0]);
        }
    }
}

static void probe_cli(void) {
    fprintf(stderr, "\r\nLogic analyzer (? for help)\r\n");

    char line[256];
    char *tok[16];
    for (;;) {
        fprintf(stderr, "probe> ");
        int len = probe_readline(line, sizeof(line));
        if (len < 0) return;  /* Ctrl-C */
        if (len == 0) continue;

        int ntok = tokenize(line, tok, 16);
        if (ntok == 0) continue;

        if (strcmp(tok[0], "q") == 0 || strcmp(tok[0], "quit") == 0) {
            return;
        }
        else if (strcmp(tok[0], "?") == 0 || strcmp(tok[0], "help") == 0) {
            fprintf(stderr,
                "  list [chip]          List available signals\r\n"
                "  add <sig> [sig...]   Add signals (chip-pin or alias)\r\n"
                "  rm <sig> [sig...]    Remove signals\r\n"
                "  clear                Remove all signals\r\n"
                "  trigger <sig> <val> [mask]  Set trigger (octal values)\r\n"
                "  depth <n>            Buffer depth (power of 2, max 65536)\r\n"
                "  rate [n]             Sample divider (1=every ustep, 10=1:10)\r\n"
                "  pos <pct>            Trigger position (0=start, 50=center, 100=end)\r\n"
                "  arm                  Arm capture\r\n"
                "  disarm               Disarm\r\n"
                "  status               Show current state\r\n"
                "  show [N|all]         Show captured data (default: 20 lines)\r\n"
                "  dump <file>          Export to CSV\r\n"
                "  q                    Return to console\r\n");
        }
        else if (strcmp(tok[0], "list") == 0) {
            probe_cmd_list(ntok, tok);
        }
        else if (strcmp(tok[0], "add") == 0) {
            int added = 0;
            for (int i = 1; i < ntok; i++) {
                /* Handle ranges: KD1:E60:1-5 */
                char *dash = strchr(tok[i], '-');
                char *last_colon = strrchr(tok[i], ':');
                if (dash && last_colon && dash > last_colon) {
                    /* Pin range: extract prefix and range */
                    int pin_start = atoi(last_colon + 1);
                    int pin_end = atoi(dash + 1);
                    char prefix[64];
                    int plen = (int)(last_colon - tok[i]) + 1;
                    if (plen < (int)sizeof(prefix)) {
                        memcpy(prefix, tok[i], plen);
                        for (int pin = pin_start; pin <= pin_end; pin++) {
                            char name[80];
                            snprintf(name, sizeof(name), "%.*s%d", plen, prefix, pin);
                            if (probe_add_signal(&probe, name) == 0)
                                added++;
                        }
                    }
                } else {
                    if (probe_add_signal(&probe, tok[i]) == 0)
                        added++;
                    else
                        fprintf(stderr, "  unknown signal: %s\r\n", tok[i]);
                }
            }
            if (added > 0)
                fprintf(stderr, "  %d signal%s added (%d total)\r\n",
                        added, added > 1 ? "s" : "", probe.nsignals);
        }
        else if (strcmp(tok[0], "rm") == 0) {
            for (int i = 1; i < ntok; i++)
                probe_rm_signal(&probe, tok[i]);
            fprintf(stderr, "  %d signals remaining\r\n", probe.nsignals);
        }
        else if (strcmp(tok[0], "clear") == 0) {
            probe_clear_signals(&probe);
            fprintf(stderr, "  signals cleared\r\n");
        }
        else if (strcmp(tok[0], "trigger") == 0) {
            if (ntok < 3) {
                fprintf(stderr, "  usage: trigger <signal> <value> [mask]\r\n");
            } else {
                uint32_t val = parse_octal(tok[2]);
                uint32_t mask = (ntok >= 4) ? parse_octal(tok[3]) : 0xFFFFFFFF;
                if (probe_set_trigger(&probe, tok[1], val, mask) == 0)
                    fprintf(stderr, "  trigger: %s == %o (mask %o)\r\n",
                            tok[1], val, mask);
                else
                    fprintf(stderr, "  unknown signal: %s\r\n", tok[1]);
            }
        }
        else if (strcmp(tok[0], "depth") == 0) {
            if (ntok >= 2) {
                uint32_t d = (uint32_t)atoi(tok[1]);
                probe_set_depth(&probe, d);
                fprintf(stderr, "  depth: %u samples\r\n", probe.depth);
            } else {
                fprintf(stderr, "  depth: %u samples\r\n", probe.depth);
            }
        }
        else if (strcmp(tok[0], "pos") == 0) {
            if (ntok >= 2) {
                int pct = atoi(tok[1]);
                probe_set_trigger_pos(&probe, pct);
                fprintf(stderr, "  trigger position: %d%%\r\n", probe.trigger_pos_pct);
            } else {
                fprintf(stderr, "  trigger position: %d%%\r\n", probe.trigger_pos_pct);
            }
        }
        else if (strcmp(tok[0], "rate") == 0) {
            if (ntok >= 2) {
                uint32_t d = (uint32_t)atoi(tok[1]);
                if (d < 1) d = 1;
                probe.divider = d;
            }
            uint32_t sr = 1000000000u / (CYCLE_SHORT_NS * probe.divider);
            if (probe.divider == 1)
                fprintf(stderr, "  sample rate: %s Hz (realtime CPU clock)\r\n", fmt_comma(sr));
            else
                fprintf(stderr, "  sample rate: %s Hz (1:%u divider)\r\n", fmt_comma(sr), probe.divider);
        }
        else if (strcmp(tok[0], "arm") == 0) {
            if (probe.nsignals == 0) {
                fprintf(stderr, "  no signals selected (use 'add' first)\r\n");
            } else {
                probe_arm(&probe);
                probe_notified = 0;
                cpu.probe = &probe;
                fprintf(stderr, "  armed (%u samples, trigger %s)\r\n",
                        probe.depth,
                        probe.trigger_signal >= 0 ? "set" : "free-run");
                fprintf(stderr, "  resume CPU (c/s from console) to start capture\r\n");
            }
        }
        else if (strcmp(tok[0], "disarm") == 0) {
            probe_disarm(&probe);
            cpu.probe = NULL;
            fprintf(stderr, "  disarmed\r\n");
        }
        else if (strcmp(tok[0], "status") == 0) {
            const char *states[] = {"IDLE", "ARMED", "CAPTURING", "DONE"};
            fprintf(stderr, "  state: %s\r\n", states[probe.state]);
            fprintf(stderr, "  signals: %d, depth: %u, samples: %u\r\n",
                    probe.nsignals, probe.depth, probe.count);
            if (probe.trigger_signal >= 0) {
                const ProbeDef *td = probe_def_get(probe.trigger_signal);
                fprintf(stderr, "  trigger: %s == %o (mask %o) pos %d%%\r\n",
                        probe_display_name(td),
                        probe.trigger_value, probe.trigger_mask,
                        probe.trigger_pos_pct);
            }
            if (probe.nsignals > 0) {
                fprintf(stderr, "  channels:");
                for (int i = 0; i < probe.nsignals; i++) {
                    const ProbeDef *d = probe_def_get(probe.signal_idx[i]);
                    fprintf(stderr, " %s", probe_display_name(d));
                }
                fprintf(stderr, "\r\n");
            }
            uint32_t sr = 1000000000u / (CYCLE_SHORT_NS * probe.divider);
            if (probe.divider == 1)
                fprintf(stderr, "  sample rate: %s Hz (realtime CPU clock)\r\n", fmt_comma(sr));
            else
                fprintf(stderr, "  sample rate: %s Hz (1:%u divider)\r\n", fmt_comma(sr), probe.divider);
        }
        else if (strcmp(tok[0], "show") == 0) {
            probe_cmd_show(&probe, 20, ntok, tok);
        }
        else if (strcmp(tok[0], "dump") == 0) {
            if (ntok < 2) {
                fprintf(stderr, "  usage: dump <filename.csv>\r\n");
            } else if (probe.count == 0) {
                fprintf(stderr, "  no data captured\r\n");
            } else {
                int n = probe_dump_csv(&probe, tok[1]);
                if (n >= 0)
                    fprintf(stderr, "  wrote %d samples to %s\r\n", n, tok[1]);
                else
                    fprintf(stderr, "  error writing %s\r\n", tok[1]);
            }
        }
        else {
            fprintf(stderr, "  unknown command: %s (? for help)\r\n", tok[0]);
        }
    }
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options]\n", prog);
    fprintf(stderr, "  --load FILE   Load program (obj2bin --ascii format)\n");
    fprintf(stderr, "  --lda FILE    Load program (DEC absolute loader .lda/.bin)\n");
    fprintf(stderr, "  --rom FILE    Override built-in M9301-YF ROM\n");
    fprintf(stderr, "  --rk FILE     Attach RK05 disk image (repeatable, up to 8 drives)\n");
    fprintf(stderr, "  --rl FILE     Attach RL01/RL02 disk image (repeatable, up to 4 drives)\n");
    fprintf(stderr, "  --s1 OCTAL    S1 switch setting (default: 0, boot to ODT)\n");
    fprintf(stderr, "  --mem KB      Memory size in KB (default: 248, max: 248)\n");
    fprintf(stderr, "  --ptr FILE    Paper tape reader input (binary)\n");
    fprintf(stderr, "  --ptp FILE    Paper tape punch output\n");
    fprintf(stderr, "  --rw          Write to original disk images instead of /tmp snapshots (dangerous)\n");
    fprintf(stderr, "  --tty MODE    DL11 serial backend: stdio (default), tcp[:port=1134], pty\n");
    fprintf(stderr, "\nCtrl-P = console, Ctrl-E = debugger, Ctrl-L = logic analyzer\n");
}

int main(int argc, char **argv) {
    const char *rom_file = NULL;
    const char *load_file = NULL;
    const char *lda_file = NULL;
    const char *rk_files[RK_NUMDR] = {0};
    int rk_ndisks = 0;
    const char *rl_files[RL_NUMDR] = {0};
    int rl_ndisks = 0;
    const char *ptr_file = NULL;
    const char *ptp_file = NULL;
    const char *tty_arg = "stdio";
    unsigned int s1_val = 0;
    unsigned int mem_kb = 248;  /* default: 248KB (256K - 8K I/O page), native 18-bit */
    int rk_raw_rw = 0;

    /* Parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--rom") == 0 && i + 1 < argc) {
            rom_file = argv[++i];
        } else if (strcmp(argv[i], "--load") == 0 && i + 1 < argc) {
            load_file = argv[++i];
        } else if (strcmp(argv[i], "--lda") == 0 && i + 1 < argc) {
            lda_file = argv[++i];
        } else if (strcmp(argv[i], "--rk") == 0 && i + 1 < argc) {
            if (rk_ndisks >= RK_NUMDR) {
                fprintf(stderr, "Too many --rk options (max %d)\n", RK_NUMDR);
                return 1;
            }
            rk_files[rk_ndisks++] = argv[++i];
        } else if (strcmp(argv[i], "--rl") == 0 && i + 1 < argc) {
            if (rl_ndisks >= RL_NUMDR) {
                fprintf(stderr, "Too many --rl options (max %d)\n", RL_NUMDR);
                return 1;
            }
            rl_files[rl_ndisks++] = argv[++i];
        } else if (strcmp(argv[i], "--ptr") == 0 && i + 1 < argc) {
            ptr_file = argv[++i];
        } else if (strcmp(argv[i], "--ptp") == 0 && i + 1 < argc) {
            ptp_file = argv[++i];
        } else if (strcmp(argv[i], "--s1") == 0 && i + 1 < argc) {
            sscanf(argv[++i], "%o", &s1_val);
        } else if (strcmp(argv[i], "--mem") == 0 && i + 1 < argc) {
            mem_kb = (unsigned int)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--rw") == 0) {
            rk_raw_rw = 1;
        } else if (strcmp(argv[i], "--tty") == 0 && i + 1 < argc) {
            tty_arg = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    Bus bus;
    Ram ram;
    Rom rom;
    DL11 dl;
    RK05 rk;
    RL11 rl;
    KW11 kw;
    PC11 pc;

    bus_init(&bus);
    g_bus = &bus;
    ram_init(&ram, (uint32_t)mem_kb * 512);
    g_ram = &ram;
    fprintf(stderr, "ll-34: %uKB RAM (%u words)\n", mem_kb, ram.size_words);
    rom_init(&rom);
    dl11_init(&dl, 9600);
    rk05_init(&rk);
    rl11_init(&rl);
    kw11_init(&kw, 60);
    pc11_init(&pc);

    if (rom_file) {
        int nwords = rom_load_mac(&rom, rom_file);
        if (nwords < 0)
            return 1;
        fprintf(stderr, "ll-34: loaded %d words from %s\n", nwords, rom_file);
    }

    static char rk_tmp_paths[RK_NUMDR][256];
    for (int d = 0; d < rk_ndisks; d++) {
        if (rk_raw_rw) {
            if (rk05_attach(&rk, d, rk_files[d], 0) < 0)
                return 1;
            fprintf(stderr, "ll-34: RK05 dk%d attached %s (WARNING: direct r/w)\n",
                    d, rk_files[d]);
        } else {
            char *base = basename((char *)rk_files[d]);
            snprintf(rk_tmp_paths[d], sizeof(rk_tmp_paths[d]),
                     "/tmp/ll34_dk%d_%s", d, base);
            FILE *src = fopen(rk_files[d], "rb");
            if (!src) {
                fprintf(stderr, "ll-34: cannot open %s\n", rk_files[d]);
                return 1;
            }
            FILE *dst = fopen(rk_tmp_paths[d], "wb");
            if (!dst) {
                fprintf(stderr, "ll-34: cannot create %s\n", rk_tmp_paths[d]);
                fclose(src);
                return 1;
            }
            char cpbuf[65536];
            size_t n;
            while ((n = fread(cpbuf, 1, sizeof(cpbuf), src)) > 0)
                fwrite(cpbuf, 1, n, dst);
            fclose(src);
            fclose(dst);
            if (rk05_attach(&rk, d, rk_tmp_paths[d], 0) < 0)
                return 1;
            fprintf(stderr, "ll-34: RK05 dk%d snapshot %s\n",
                    d, rk_tmp_paths[d]);
        }
    }
    if (rk_ndisks > 0) {
        if (s1_val == 0)
            s1_val = 0146;  /* M9301-YF: boot DK, skip diagnostics */
    }

    static char rl_tmp_paths[RL_NUMDR][256];
    for (int d = 0; d < rl_ndisks; d++) {
        if (rk_raw_rw) {
            if (rl11_attach(&rl, d, rl_files[d], 0) < 0)
                return 1;
            fprintf(stderr, "ll-34: RL dl%d attached %s (WARNING: direct r/w)\n",
                    d, rl_files[d]);
        } else {
            char *base = basename((char *)rl_files[d]);
            snprintf(rl_tmp_paths[d], sizeof(rl_tmp_paths[d]),
                     "/tmp/ll34_dl%d_%s", d, base);
            FILE *src = fopen(rl_files[d], "rb");
            if (!src) {
                fprintf(stderr, "ll-34: cannot open %s\n", rl_files[d]);
                return 1;
            }
            FILE *dst = fopen(rl_tmp_paths[d], "wb");
            if (!dst) {
                fprintf(stderr, "ll-34: cannot create %s\n", rl_tmp_paths[d]);
                fclose(src);
                return 1;
            }
            char cpbuf[65536];
            size_t n;
            while ((n = fread(cpbuf, 1, sizeof(cpbuf), src)) > 0)
                fwrite(cpbuf, 1, n, dst);
            fclose(src);
            fclose(dst);
            if (rl11_attach(&rl, d, rl_tmp_paths[d], 0) < 0)
                return 1;
            fprintf(stderr, "ll-34: RL dl%d snapshot %s\n",
                    d, rl_tmp_paths[d]);
        }
    }

    if (ptr_file) {
        if (pc11_attach_reader(&pc, ptr_file) < 0)
            return 1;
        fprintf(stderr, "ll-34: PC11 reader attached %s\n", ptr_file);
        if (s1_val == 0)
            s1_val = 0706;  /* M9301-YF: boot PR, skip diagnostics */
    }
    if (ptp_file) {
        if (pc11_attach_punch(&pc, ptp_file) < 0)
            return 1;
        fprintf(stderr, "ll-34: PC11 punch attached %s\n", ptp_file);
    }

    rom_set_s1(&rom, s1_val);
    con.switch_reg = s1_val;

    ram_register(&ram, &bus);
    rom_register(&rom, &bus);
    dl11_register(&dl, &bus);
    rk05_register(&rk, &bus);
    rl11_register(&rl, &bus);
    kw11_register(&kw, &bus);
    pc11_register(&pc, &bus);
    bus_register(&bus, PSW_ADDR, PSW_ADDR + 1,
                 NULL, psw_read, psw_write, "PSW", 100);
    bus_register(&bus, SWREG_ADDR, SWREG_ADDR + 1,
                 NULL, swreg_read, swreg_write, "SWITCH", 100);

    /* MMU registers */
    mmu_init(&cpu.mmu);
    bus_register(&bus, 0x3F480, 0x3F5FE,   /* Kernel+Super 772200-772776 */
                 &cpu.mmu, mmu_read, mmu_write, "KS PAR/PDR", 100);
    bus_register(&bus, 0x3FF7A, 0x3FF7E,   /* SR0-SR2 777572-777576 */
                 &cpu.mmu, mmu_read, mmu_write, "MMU SR", 100);
    bus_register(&bus, 0x3FF80, 0x3FFBE,   /* User 777600-777676 */
                 &cpu.mmu, mmu_read, mmu_write, "U PAR/PDR", 100);

    /* Make sure the host is fast enough to run ll-34:
     * benchmark 1M usteps in ODT, then reset for normal boot
     */
    {
        kd11ea_reset(&cpu);
        cpu.bus = &bus;
        rom_set_s1(&rom, 0);
        power_on(&cpu, &bus);

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (long i = 0; i < 1000000L; i++)
            kd11ea_ustep(&cpu);
        clock_gettime(CLOCK_MONOTONIC, &t1);

        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) * 1e-9;
        double ns_per = elapsed / 1e6 * 1e9;
        double ratio = (double)CYCLE_SHORT_NS / ns_per;
        if (ratio >= 1.0)
            fprintf(stderr, "ll-34: host can emulate %.1fx real KD11-EA: speed OK\n", ratio);
        else
            fprintf(stderr, "ll-34: WARNING: host too slow (%.1fx real KD11-EA speed)"
                    "cycle accuracy not guaranteed\n", ratio);

        /* Reset CPU and peripherals for normal boot */
        kd11ea_reset(&cpu);
        cpu.bus = &bus;
        rom_set_s1(&rom, s1_val);
        dl11_init(&dl, 9600);
        kw11_init(&kw, 60);
    }

    if (strcmp(tty_arg, "stdio") == 0) {
        tty = tty_stdio_create();
        tty_uses_stdin = 1;
    } else if (strncmp(tty_arg, "tcp", 3) == 0) {
        uint16_t port = 0;
        if (tty_arg[3] == ':')
            port = (uint16_t)atoi(tty_arg + 4);
        tty = tty_tcp_create(port);
    } else if (strcmp(tty_arg, "pty") == 0) {
        tty = tty_pty_create();
    } else {
        fprintf(stderr, "ll-34: unknown tty mode: %s\n", tty_arg);
        return 1;
    }
    if (!tty) {
        fprintf(stderr, "ll-34: cannot create tty backend '%s'\n", tty_arg);
        return 1;
    }

    dl.rx_ready  = dl11_tty_rx_ready;
    dl.rx_read   = dl11_tty_rx_read;
    dl.tx_write  = dl11_tty_tx_write;
    dl.io_ctx    = tty;
    dl.clock_ptr = &cpu.ns_elapsed;
    dl.irq_set   = cpu_irq_set;
    dl.irq_ctx   = &cpu;

    rk.irq_set = cpu_irq_set;
    rk.irq_clr = cpu_irq_clr;
    rk.irq_ctx = &cpu;

    rl.irq_set = cpu_irq_set;
    rl.irq_clr = cpu_irq_clr;
    rl.irq_ctx = &cpu;

    kw.clock_ptr = &cpu.ns_elapsed;
    kw.irq_set = cpu_irq_set;
    kw.irq_ctx = &cpu;

    pc.clock_ptr = &cpu.ns_elapsed;
    pc.irq_set = cpu_irq_set;
    pc.irq_clr = cpu_irq_clr;
    pc.irq_ctx = &cpu;

    uint16_t load_start = 0;
    if (load_file) {
        int nwords = loader_asc(&ram, load_file, &load_start);
        if (nwords < 0)
            return 1;
        fprintf(stderr, "ll-34: loaded %d words from %s, start=%06o\n",
                nwords, load_file, load_start);
    }
    if (lda_file) {
        int nbytes = loader_lda(&ram, lda_file, &load_start);
        if (nbytes < 0)
            return 1;
        fprintf(stderr, "ll-34: loaded %d bytes from %s, start=%06o\n",
                nbytes, lda_file, load_start);
    }

    probe_buf = calloc(PROBE_DEFAULT_DEPTH, sizeof(ProbeSnapshot));
    if (!probe_buf) {
        fprintf(stderr, "ll-34: cannot allocate probe buffer\n");
        return 1;
    }
    probe_init(&probe, probe_buf, PROBE_DEFAULT_DEPTH);

    kd11ea_reset(&cpu);
    cpu.bus = &bus;

    console_init(&con);
    con.bus = &bus;
    con.cpu = &cpu;

    if (load_file || lda_file) {
        PC(&cpu) = load_start;
        cpu.psw = 0;
        cpu.mpc = 0;
        cpu.halted = 0;
        fprintf(stderr, "ll-34: direct boot PC=%06o\n", load_start);
    } else if (rl_ndisks > 0 && rk_ndisks == 0 && !ptr_file) {
        /* RL11 bootstrap: M9301-YF has no RL boot, inject DEC bootstrap */
        static const uint16_t rl_boot_rom[] = {
            0042114,                        /* "LD" */
            0012706, 0002000,               /* MOV #2000, SP */
            0012700, 0000000,               /* MOV #unit, R0 */
            0010003,                        /* MOV R0, R3 */
            0000303,                        /* SWAB R3 */
            0012701, 0174400,               /* MOV #RLCS, R1 */
            0012761, 0000013, 0000004,      /* MOV #13, 4(R1)  ; clr err */
            0052703, 0000004,               /* BIS #4, R3       ; unit+gstat */
            0010311,                        /* MOV R3, (R1)     ; issue cmd */
            0105711,                        /* TSTB (R1)        ; wait */
            0100376,                        /* BPL .-2 */
            0105003,                        /* CLRB R3 */
            0052703, 0000010,               /* BIS #10, R3      ; unit+rdhdr */
            0010311,                        /* MOV R3, (R1)     ; issue cmd */
            0105711,                        /* TSTB (R1)        ; wait */
            0100376,                        /* BPL .-2 */
            0016102, 0000006,               /* MOV 6(R1), R2    ; get hdr */
            0042702, 0000077,               /* BIC #77, R2      ; clr sector */
            0005202,                        /* INC R2           ; magic bit */
            0010261, 0000004,               /* MOV R2, 4(R1)    ; seek to 0 */
            0105003,                        /* CLRB R3 */
            0052703, 0000006,               /* BIS #6, R3       ; unit+seek */
            0010311,                        /* MOV R3, (R1)     ; issue cmd */
            0105711,                        /* TSTB (R1)        ; wait */
            0100376,                        /* BPL .-2 */
            0005061, 0000002,               /* CLR 2(R1)        ; clr ba */
            0005061, 0000004,               /* CLR 4(R1)        ; clr da */
            0012761, 0177000, 0000006,      /* MOV #-512., 6(R1); set wc */
            0105003,                        /* CLRB R3 */
            0052703, 0000014,               /* BIS #14, R3      ; unit+read */
            0010311,                        /* MOV R3, (R1)     ; issue cmd */
            0105711,                        /* TSTB (R1)        ; wait */
            0100376,                        /* BPL .-2 */
            0042711, 0000377,               /* BIC #377, (R1) */
            0005002,                        /* CLR R2 */
            0005003,                        /* CLR R3 */
            0012704, 0002020,               /* MOV #2020, R4 */
            0005005,                        /* CLR R5 */
            0005007                         /* CLR PC */
        };
        for (size_t i = 0; i < sizeof(rl_boot_rom)/sizeof(rl_boot_rom[0]); i++)
            ram.mem[(02000 >> 1) + i] = rl_boot_rom[i];
        PC(&cpu) = 02002;  /* entry past "LD" magic */
        cpu.psw = 0340;
        cpu.mpc = 0;
        cpu.halted = 0;
        fprintf(stderr, "ll-34: RL bootstrap injected at 002000, boot PC=002002\n");
    } else {
        power_on(&cpu, &bus);
    }

    /* Non-stdio backends still need raw stdin for operator keys */
    if (strcmp(tty_arg, "stdio") != 0) {
        if (stdin_raw_enter() < 0)
            fprintf(stderr, "ll-34: warning: cannot enter raw stdin mode\n");
    }

    fprintf(stderr, "ll-34: Ctrl-P = console, Ctrl-E = debugger, Ctrl-L = logic analyzer\n");

    signal(SIGTERM, sigterm_handler);

    Clock clk;
    clock_init(&clk, cpu.ns_elapsed);

    int poll_counter = 0;
    int quit_requested = 0;
    while (!got_sigterm && !quit_requested) {
        if (!cpu.halted) {
            uint32_t ns = kd11ea_ustep(&cpu);
            if (cpu.halted) {
                enter_console_mode("CPU halted");
                continue;
            }
            if (ns == 0) {
                enter_console_mode("CPU halted");
                continue;
            }

            /* Breakpoint check at MPC=0 (PC holds next instruction) */
            if (cpu.mpc == 0 && num_breakpoints > 0) {
                uint16_t pc = PC(&cpu);
                for (int i = 0; i < num_breakpoints; i++) {
                    if (breakpoints[i].active && breakpoints[i].addr == pc) {
                        cpu.halted = 1;
                        fprintf(stderr, "\r\nll-34: breakpoint #%d at PC=%06o\r\n", i, pc);
                        debug_cli();
                        if (!cpu.halted) {
                            clock_init(&clk, cpu.ns_elapsed);
                            poll_counter = 0;
                        } else {
                            enter_console_mode("debug console closed");
                        }
                        break;
                    }
                }
                if (cpu.halted) continue;
            }

            if (++poll_counter >= 256) {
                poll_counter = 0;
                tty->tick(tty);
                dl11_tick(&dl, cpu.ns_elapsed);
                kw11_tick(&kw, cpu.ns_elapsed);
                pc11_tick(&pc, cpu.ns_elapsed);
                console_tick(&con);

                /* Operator key poll (independent of DL11) */
                if (!halt_requested && !debug_requested && !probe_requested
                    && stdin_pushback < 0 && stdin_rx_ready()) {
                    int ch = stdin_rx_read();
                    if (ch == CONSOLE_HALT_CHAR)
                        halt_requested = 1;
                    else if (ch == DEBUG_CHAR)
                        debug_requested = 1;
                    else if (ch == PROBE_CHAR)
                        probe_requested = 1;
                    else if (tty_uses_stdin)
                        stdin_pushback = ch;
                }

                if (halt_requested) {
                    halt_requested = 0;
                    cpu.halted = 1;
                    enter_console_mode("halted by operator");
                }

                if (debug_requested) {
                    debug_requested = 0;
                    cpu.halted = 1;
                    fprintf(stderr, "\r\nll-34: halted at PC=%06o PSW=%06o\r\n",
                            PC(&cpu), cpu.psw);
                    debug_cli();
                    if (!cpu.halted) {
                        clock_init(&clk, cpu.ns_elapsed);
                        poll_counter = 0;
                    } else {
                        enter_console_mode("debug console closed");
                    }
                }

                if (probe_requested) {
                    probe_requested = 0;
                    cpu.halted = 1;
                    fprintf(stderr, "\r\nll-34: halted at PC=%06o PSW=%06o\r\n",
                            PC(&cpu), cpu.psw);
                    probe_cli();
                    enter_console_mode("logic analyzer closed");
                }

                if (probe.state == PROBE_DONE && !probe_notified) {
                    probe_notified = 1;
                    fprintf(stderr, "\r\nll-34: probe capture complete (%u samples)\r\n",
                            probe.count < probe.depth ? probe.count : probe.depth);
                }
            }

            clock_pace(&clk, cpu.ns_elapsed);
        } else {
            console_tick(&con);
            if (console_poll() < 0) {
                quit_requested = 1;
                break;
            }

            if (con.boot_requested) {
                con.boot_requested = 0;
                fprintf(stderr, "\r\nll-34: BOOT\r\n");
                do_bus_reset(&dl, &rk, &kw, &bus);
                kd11ea_reset(&cpu);
                cpu.bus = &bus;
                power_on(&cpu, &bus);
                clock_init(&clk, cpu.ns_elapsed);
                poll_counter = 0;
                continue;
            }

            if (con.init_requested) {
                con.init_requested = 0;
                do_bus_reset(&dl, &rk, &kw, &bus);
                con.led_bus_err = 0;
                fprintf(stderr, "\r\nll-34: BUS INIT\r\n");
                console_prompt();
            }

            usleep(10000);

            if (!cpu.halted) {
                fprintf(stderr, "\r\nll-34: running\r\n");
                clock_init(&clk, cpu.ns_elapsed);
                poll_counter = 0;
            }
        }
    }

    tty->destroy(tty);
    stdin_raw_leave();
    fprintf(stderr, "ll-34: exit at PC=%06o PSW=%06o\n",
            PC(&cpu), cpu.psw);

    free(probe_buf);
    return 0;
}
