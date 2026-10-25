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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <libgen.h>
#include <sys/select.h>
#include "pdp11/unibus/unibus.h"
#include "pdp11/unibus/ram.h"
#include "frontends/tty/tty.h"
#include "pdp11/m9301/rom.h"
#include "pdp11/machine.h"
#include "pdp11/dl11/dl11.h"
#include "pdp11/rk11/rk11.h"
#include "pdp11/rl11/rl11.h"
#include "pdp11/kd11ea/kd11ea.h"
#include "pdp11/kd11ea/ucode_labels.h"
#include "pdp11/kd11ea/clock.h"
#include "pdp11/kw11/kw11.h"
#include "pdp11/pc11/pc11.h"
#include "pdp11/ky11/console.h"
#include "pdp11/probe/probe.h"
#include "pdp11/version.h"
#include "pdp11/debug/disasm.h"
#include "pdp11/debug/memdump.h"
#include "pdp11/unibus/loader.h"
#include "pdp11/trace.h"

/* PSW at 177776, accessible as a UNIBUS register */

static Machine m;
static TTY *tty;
static volatile sig_atomic_t got_sigterm;
static int console_shown_help;     /* show help banner on first halt */

/* --- Breakpoints --- */
static void sigterm_handler(int sig) { (void)sig; got_sigterm = 1; }

/* Operator keys (intercepted before reaching the OS) */
#define CONSOLE_HALT_CHAR 0x10  /* Ctrl-P - programmer console (halt) */
#define DEBUG_CHAR        0x05  /* Ctrl-E - debug console */
#define PROBE_CHAR        0x0C  /* Ctrl-L - logic analyzer */

#define CLI_SLICE_NS      1000000  /* run slice between host checks (1 ms) */
static int halt_requested;
static int unpaced;            /* --unpaced: no real-time pacing */
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

static int stdin_eof;   /* stdin is a file or a pipe, and it has ended */

static int stdin_rx_ready(void) {
    if (stdin_eof) return 0;
    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

static int stdin_rx_read(void) {
    uint8_t ch;
    ssize_t n = read(STDIN_FILENO, &ch, 1);
    if (n == 1) return ch;
    if (n == 0) stdin_eof = 1;
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

/* The emulated terminal in green, ll-34 itself (consoles, messages) in the
 * default colour, unless NO_COLOR is set */
static int colour_ok = -1, colour_now;
static void term_colour(int emulation) {
    if (colour_ok < 0) colour_ok = getenv("NO_COLOR") == NULL && isatty(STDOUT_FILENO);
    if (!colour_ok || colour_now == emulation) return;
    colour_now = emulation;
    fputs(emulation ? "\033[32m" : "\033[0m", stdout);
    fflush(stdout);
}

static void console_prompt(void) {
    term_colour(0);
    /* KY11-LB address pointer and 6-digit display */
    fprintf(stderr, "\rconsole |ADDR=%06o DISP=%06o|> ", console_addr_ptr(&m.con), m.con.display);
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
        "\r\n"
        "  (Ctrl-E debugger, Ctrl-L logic analyzer)\r\n"
        "\r\n");
}

static void enter_console_mode(const char *reason) {
    term_colour(0);
    if (reason)
        fprintf(stderr, "\r\nll-34: %s\r\n", reason);
    else
        fprintf(stderr, "\r\n");
    if (!console_shown_help) {
        console_shown_help = 1;
        fprintf(stderr, "Programmer console (? for help)\r\n");
    }
    console_set_addr(&m.con, PC(&m.cpu));
    console_key(&m.con, KEY_EXAM);
    console_prompt();
}

static void probe_cli(void);  /* forward declaration */
static int run_reported;      /* CONT, START or BOOT already announced */
static void open_consoles(int mode, int resume);

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
                term_colour(0);
                return -1;
            }
            fprintf(stderr, "n\r\n");
            console_prompt();
            return 0;
        }
    }

    /* '?' = help */
    if (ch == '?' || ch == 'h') {
        console_help();
        console_prompt();
        return 0;
    }

    if (ch == PROBE_CHAR || ch == DEBUG_CHAR) {   /* other consoles */
        open_consoles(ch, 0);
        if (m.cpu.halted)
            enter_console_mode(NULL);
        return 0;
    }

    int key = map_console_key(ch);
    if (key < 0)
        return 0;

    /* Auto-prefix CNTRL (no physical modifier in CLI) */
    if (key == KEY_CONT || key == KEY_START ||
        key == KEY_BOOT || key == KEY_INIT) {
        console_key(&m.con, KEY_CNTRL);
    }
    /* HLT/SS: CNTRL only when running (halted = single-step) */
    if (key == KEY_HALT_SS && !m.cpu.halted) {
        console_key(&m.con, KEY_CNTRL);
    }

    console_key(&m.con, key);
    if (!m.cpu.halted) {                           /* CONT, START, BOOT */
        term_colour(0);
        if (key == KEY_START)
            fprintf(stderr, "\r\nll-34: START at %06o\r\n", PC(&m.cpu));
        else
            fprintf(stderr, "\r\nll-34: %s\r\n", key == KEY_BOOT ? "BOOT" : "CONT");
        run_reported = 1;
        return 0;
    }

    /* Auto-EXAM after address-changing actions */
    if (key == KEY_HALT_SS && m.cpu.halted) {
        console_set_addr(&m.con, PC(&m.cpu));
        console_key(&m.con, KEY_EXAM);
    } else if (key == KEY_LAD && m.cpu.halted) {
        console_key(&m.con, KEY_EXAM);
    } else if (key == KEY_DEP && m.cpu.halted) {
        console_key(&m.con, KEY_EXAM);
    }

    console_prompt();
    return 0;
}

static void power_on(void) {
    machine_power_on(&m);
    fprintf(stderr, "ll-34: power-on vector PC=%06o PS=%06o\n",
            PC(&m.cpu), m.cpu.psw);
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
    "list", "find", "add", "rm", "clear", "trigger", "depth", "pos",
    "arm", "disarm", "status", "show", "export", NULL
};

/* Console to open next (Ctrl-P, Ctrl-E, Ctrl-L typed in a console), 0 none */
static int next_mode;

/* Console using the line editor: its prompt and help */
static const char *cli_prompt = "probe> ";
static void (*cli_help)(void);
static const char *cli_immediate = "q";   /* commands taken without Enter */
static int cli_self;                       /* key of the console in use */

/* Sheet of a signal: "K2-8", "BUS", or board and chip "K2:E102" */
static void probe_sheet_of(const ProbeDef *d, char *out, int max) {
    const char *n = d->name, *e = n;
    if (!n) { out[0] = '\0'; return; }
    if (n[0] == 'K' && n[2] == ':') {                 /* "K2:E102:1" */
        e = strchr(n + 3, ':');
        if (!e) e = n + strlen(n);
    } else {
        while (*e && *e != ' ') e++;
    }
    int l = (int)(e - n) < max - 1 ? (int)(e - n) : max - 1;
    memcpy(out, n, l);
    out[l] = '\0';
}

/* Print strings in columns, at most 60 */
static void probe_columns(const char **v, int nv) {
    int w = 0;
    for (int i = 0; i < nv; i++) { int l = (int)strlen(v[i]); if (l > w) w = l; }
    w += 2;
    int cols = 78 / w; if (cols < 1) cols = 1;
    int shown = nv > 60 ? 60 : nv;
    for (int i = 0; i < shown; i++)
        fprintf(stderr, "%s%-*s%s", i % cols ? "" : "  ", w, v[i],
                (i % cols == cols - 1 || i == shown - 1) ? "\r\n" : "");
    if (nv > shown)
        fprintf(stderr, "  ... %d more: type more letters, or list <sheet|chip>\r\n", nv - shown);
}

/* Tab: the first word completes as a command. After add, rm, trigger,
 * the item being typed (after the command or the last comma, spaces
 * included) completes as a signal: DEC name, alias, chip:pin or
 * board:chip:pin. After list: sheets and chips. */
static int probe_complete(char *buf, int n, int max) {
    buf[n] = '\0';
    int cmd_end = 0;
    while (buf[cmd_end] == ' ') cmd_end++;
    int cmd_start = cmd_end;
    while (buf[cmd_end] && buf[cmd_end] != ' ') cmd_end++;
    static const char *cand[2048];
    int nc = 0, item;

    if (cmd_end == n) {                             /* command */
        item = cmd_start;
        for (int i = 0; probe_commands[i]; i++)
            if (!strncasecmp(probe_commands[i], buf + item, n - item))
                cand[nc++] = probe_commands[i];
    } else {
        char cmd[16];
        int cl = cmd_end - cmd_start < 15 ? cmd_end - cmd_start : 15;
        memcpy(cmd, buf + cmd_start, cl); cmd[cl] = '\0';
        item = cmd_end;
        for (int i = cmd_end; i < n; i++) if (buf[i] == ',') item = i + 1;
        while (item < n && buf[item] == ' ') item++;
        const char *pre = buf + item;
        int pl = n - item;
        int count = probe_def_count();
        if (!strcasecmp(cmd, "list")) {             /* sheets and chips */
            static char names[512][24];
            int nn = 0;
            for (int i = 0; i < count && nn < 512; i++) {
                const ProbeDef *d = probe_def_get(i);
                char sh[24]; probe_sheet_of(d, sh, sizeof sh);
                int dup = 0;
                for (int k = 0; k < nn; k++) if (!strcasecmp(names[k], sh)) dup = 1;
                if (!dup && sh[0]) strcpy(names[nn++], sh);
            }
            if (!strncasecmp("all", pre, pl)) cand[nc++] = "all";
            for (int k = 0; k < nn && nc < 2048; k++)
                if (!strncasecmp(names[k], pre, pl)) cand[nc++] = names[k];
        } else if (!strcasecmp(cmd, "add") || !strcasecmp(cmd, "rm") || !strcasecmp(cmd, "trigger")) {
            for (int i = 0; i < count && nc < 2040; i++) {
                const ProbeDef *d = probe_def_get(i);
                const char *q = probe_def_qloc(i);
                if (d->name && !strncasecmp(d->name, pre, pl)) cand[nc++] = d->name;
                if (d->alias && !strncasecmp(d->alias, pre, pl)) cand[nc++] = d->alias;
                if (q && pl >= 2 && !strncasecmp(q, pre, pl)) cand[nc++] = q;
            }
        }
    }
    if (nc == 0) return n;
    int common = (int)strlen(cand[0]);
    for (int i = 1; i < nc; i++) {
        int k = 0;
        while (k < common && cand[i][k] && toupper((unsigned char)cand[i][k]) == toupper((unsigned char)cand[0][k])) k++;
        common = k;
    }
    if (common > n - item && item + common < max - 1) {   /* extend */
        for (int i = item; i < n; i++) fprintf(stderr, "\b \b");
        memcpy(buf + item, cand[0], common);
        n = item + common;
        buf[n] = '\0';
        fwrite(buf + item, 1, common, stderr);
        return n;
    }
    if (nc > 1) {
        fprintf(stderr, "\r\n");
        probe_columns(cand, nc);
        fprintf(stderr, "%s%s", cli_prompt, buf);
    }
    return n;
}

static void probe_help(void) {
    fprintf(stderr,
        "  list [all|sheet|chip]   sheets, all signals, or those of a sheet (K2-8) or chip (E38)\r\n"
        "  find <text>             search signal by name, chip or description\r\n"
        "  add <sig>[, <sig>...]   add signals: DEC name, alias, E38:9 or K2:E107:3\r\n"
        "  rm <sig>[, <sig>...]    remove signals\r\n"
        "  clear                   remove all signals\r\n"
        "  trigger <sig> <val> [mask]  trigger on a value (octal)\r\n"
        "  depth <n>               buffer depth (power of 2)\r\n"
        "  rate [n]                sample divider (1 = every micro-cycle)\r\n"
        "  pos <pct>               trigger position (0 start, 50 centre, 100 end)\r\n"
        "  arm | disarm            arm or disarm the capture\r\n"
        "  status                  current state\r\n"
        "  show [N|all]            captured data (default 20 lines)\r\n"
        "  export <file> [MHz]     export capture to VCD or sigrok, by file extension\r\n"
        "  q                       back to the program or the console\r\n"
        "\r\n"
        "  Tab completes commands and signal names.\r\n"
        "\r\n"
        "  (Ctrl-E debugger, Ctrl-L logic analyzer)\r\n"
        "\r\n");
}

/* Readline with echo, history, and tab completion. Returns -1 on Ctrl-C. */
static int probe_readline(char *buf, int max) {
    int n = 0;
    int hist_pos = hist_count;  /* browsing position: hist_count = "new line" */

    while (n < max - 1) {
        int ch = term_getc();

        if (ch == 0x03) return -1;       /* Ctrl-C → quit probe CLI */
        if (ch == cli_self)              /* already in that console */
            continue;
        if (ch == CONSOLE_HALT_CHAR || ch == DEBUG_CHAR || ch == PROBE_CHAR) {
            next_mode = ch;              /* switch console */
            fprintf(stderr, "\r\n");
            return -1;
        }

        if (n == 0 && (ch == '?' || ch == 'h') && cli_help) {   /* help at once */
            fprintf(stderr, "%c\r\n", ch);
            cli_help();
            fprintf(stderr, "%s", cli_prompt);
            continue;
        }

        if (n == 0 && ch > ' ' && strchr(cli_immediate, ch)) {   /* no argument */
            fprintf(stderr, "%c\r\n", ch);
            buf[0] = (char)ch; buf[1] = '\0';
            return 1;
        }

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

/* Sort key: K1-1..K1-10, K2-1..K2-10, then chips by number, then BUS */
static int probe_sheet_key(const char *n) {
    if ((n[0] == 'K') && n[2] == '-') return (n[1] - '0') * 100 + atoi(n + 3);
    if ((n[0] == 'K') && n[2] == ':' && n[3] == 'E') return 1000 + atoi(n + 4);
    return 5000;
}

static void probe_print_def(int i) {
    const ProbeDef *d = probe_def_get(i);
    const char *q = probe_def_qloc(i);
    fprintf(stderr, "  %-30s %-12s %s\r\n", probe_display_name(d),
            q ? q : (d->loc ? d->loc : ""), d->desc ? d->desc : "");
}

/* list shows the sheets, list all every signal, list K2-8 a sheet,
 * list E38 or K2:E107 a chip */
static void probe_cmd_list(const char *rest) {
    int count = probe_def_count();
    char sh[24], arg[64];
    snprintf(arg, sizeof arg, "%s", rest ? rest : "");
    for (char *e = arg + strlen(arg); e > arg && e[-1] == ' '; ) *--e = '\0';
    if (!strcasecmp(arg, "all")) {
        for (int i = 0; i < count; i++) probe_print_def(i);
        return;
    }
    if (!*arg) {
        static char names[512][24];
        static int num[512];
        int nn = 0;
        memset(num, 0, sizeof num);
        for (int i = 0; i < count; i++) {
            probe_sheet_of(probe_def_get(i), sh, sizeof sh);
            if (!sh[0]) continue;
            int k = 0;
            while (k < nn && strcasecmp(names[k], sh)) k++;
            if (k == nn && nn < 512) strcpy(names[nn++], sh);
            if (k < 512) num[k]++;
        }
        fprintf(stderr, "  %-9s %3d  %s\r\n", "all", count, "All signals");
        int ord[512];                                 /* schematic order */
        for (int k = 0; k < nn; k++) ord[k] = k;
        for (int i = 1; i < nn; i++)
            for (int j = i; j > 0 && probe_sheet_key(names[ord[j]]) < probe_sheet_key(names[ord[j - 1]]); j--) {
                int t = ord[j]; ord[j] = ord[j - 1]; ord[j - 1] = t;
            }
        for (int i = 0; i < nn; i++) {
            int k = ord[i];
            const char *t = probe_chip_name(names[k]);
            fprintf(stderr, "  %-9s %3d  %s\r\n", names[k], num[k], t ? t : "");
        }
        fprintf(stderr, "  list all, list <sheet> or list <chip>, Tab completes\r\n");
        return;
    }
    int shown = 0;
    for (int i = 0; i < count; i++) {                 /* a sheet */
        probe_sheet_of(probe_def_get(i), sh, sizeof sh);
        if (!strcasecmp(sh, arg)) { probe_print_def(i); shown++; }
    }
    if (shown) return;
    const char *chip = arg, *board = NULL;            /* a chip */
    char bb[3] = "";
    if ((arg[0] == 'K' || arg[0] == 'k') && arg[1] && arg[2] == ':') {
        bb[0] = 'K'; bb[1] = arg[1]; bb[2] = '\0'; board = bb; chip = arg + 3;
    }
    size_t cl = strlen(chip);
    for (int i = 0; i < count; i++) {
        const ProbeDef *d = probe_def_get(i);
        const char *db = probe_def_board(d);
        if (d->loc && !strncasecmp(d->loc, chip, cl) && (d->loc[cl] == ':' || !d->loc[cl]) &&
            (!board || (db && !strcasecmp(db, board)))) { probe_print_def(i); shown++; }
    }
    if (!shown) fprintf(stderr, "  no sheet or chip %s\r\n", arg);
}

static void probe_cmd_find(const char *text) {
    int count = probe_def_count(), shown = 0;
    size_t tl = strlen(text);
    for (int i = 0; i < count; i++) {
        const ProbeDef *d = probe_def_get(i);
        const char *f[4] = { d->name, d->alias, d->loc, d->desc };
        int hit = 0;
        for (int k = 0; k < 4 && !hit; k++)
            for (const char *c = f[k]; c && *c && !hit; c++)
                if (!strncasecmp(c, text, tl)) hit = 1;
        if (hit) { probe_print_def(i); shown++; }
    }
    if (!shown) fprintf(stderr, "  nothing matches %s\r\n", text);
}

/* add and rm: items separated by commas, a name may contain spaces.
 * Old forms are still accepted: space separated, K2:E60:1-5 ranges. */
static int probe_add_rm_one(const char *it, int add) {
    if (add) {
        const char *dash = strchr(it, '-'), *colon = strrchr(it, ':');
        if (dash && colon && dash > colon) {          /* K2:E60:1-5 */
            int a = atoi(colon + 1), b = atoi(dash + 1), ok = 0;
            for (int pin = a; pin <= b; pin++) {
                char name[80];
                snprintf(name, sizeof name, "%.*s%d", (int)(colon - it + 1), it, pin);
                if (probe_add_signal(&probe, name) == 0) ok++;
            }
            return ok;
        }
        return probe_add_signal(&probe, it) == 0;
    }
    return probe_rm_signal(&probe, it) == 0;
}

static void probe_cmd_add_rm(char *rest, int add) {
    int done = 0, before = probe.nsignals;
    char *item = rest;
    for (;;) {
        char *comma = strchr(item, ',');
        if (comma) *comma = '\0';
        while (*item == ' ') item++;
        char *e = item + strlen(item);
        while (e > item && e[-1] == ' ') *--e = '\0';
        if (*item) {
            if (probe_find_signal(item) == -2) {
                fprintf(stderr, "  %s is on both boards: K1:%s or K2:%s\r\n", item, item, item);
            } else if (probe_add_rm_one(item, add)) {
                done++;
            } else if (strchr(item, ' ')) {           /* old form: one per word */
                char tmp[256], *w, *sv;
                snprintf(tmp, sizeof tmp, "%s", item);
                for (w = strtok_r(tmp, " ", &sv); w; w = strtok_r(NULL, " ", &sv)) {
                    if (probe_add_rm_one(w, add)) done++;
                    else fprintf(stderr, "  unknown signal: %s\r\n", w);
                }
            } else {
                fprintf(stderr, "  unknown signal: %s\r\n", item);
            }
        }
        if (!comma) break;
        item = comma + 1;
    }
    (void)done;
    int delta = add ? probe.nsignals - before : before - probe.nsignals;
    fprintf(stderr, "  %d signal%s %s (%d total)\r\n", delta, delta == 1 ? "" : "s",
            add ? "added" : "removed", probe.nsignals);
}

/* trigger <signal> <value> [mask]: the signal may contain spaces */
static void probe_cmd_trigger(char *rest) {
    char *w[24];
    int nw = tokenize(rest, w, 24);
    for (int nnum = 1; nnum <= 2 && nnum < nw; nnum++) {
        char name[200] = "";
        for (int i = 0; i < nw - nnum; i++) {
            if (i) strncat(name, " ", sizeof name - strlen(name) - 1);
            strncat(name, w[i], sizeof name - strlen(name) - 1);
        }
        if (probe_find_signal(name) < 0) continue;
        uint32_t val = parse_octal(w[nw - nnum]);
        uint32_t mask = nnum == 2 ? parse_octal(w[nw - 1]) : 0xFFFFFFFF;
        if (probe_set_trigger(&probe, name, val, mask) == 0) {
            if (nnum == 2)
                fprintf(stderr, "  trigger: %s == %o (mask %o)\r\n", name, val, mask);
            else
                fprintf(stderr, "  trigger: %s == %o\r\n", name, val);
            return;
        }
    }
    fprintf(stderr, "  usage: trigger <signal> <value> [mask] (signal unknown or ambiguous)\r\n");
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

    uint32_t eff_ns = clock_timing(m.cpu.timing)->short_ns * probe.divider;
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
    if (m.cpu.mmu.sr0 & SR0_ENABLE) {
        int mode = (m.cpu.psw >> 14) & 3;
        if (mmu_translate(&m.cpu.mmu, vba, mode, 0, m.cpu.mmu.sr0 & SR0_ENABLE, PC(&m.cpu), &pba) < 0) {
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
    if (bus_read(&m.bus, pba, &data) < 0)
        return -1;
    return data;
}

static void debug_show_mmu(void) {
    MMU *mmu = &m.cpu.mmu;
    int enabled = (mmu->sr0 & SR0_ENABLE) != 0;
    int mode = (m.cpu.psw >> 14) & 3;

    fprintf(stderr, "  SR0=%06o  SR2=%06o  %s  mode=%s\r\n",
            mmu->sr0, mmu->sr2,
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
            uint16_t par = mmu->par[base + p];
            uint16_t pdr = mmu->pdr[base + p];
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

/* One disassembled instruction: address, words, text. Returns its size. */
static int debug_print_insn(uint16_t a) {
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
    return nbytes;
}

static void debug_help(void) {
    fprintf(stderr,
        "  n [n]                   next n instructions (default 1)\r\n"
        "  s [n]                   single-step n micro-instructions (default 1)\r\n"
        "  g                       go (resume execution)\r\n"
        "  b <addr>                set a breakpoint (octal)\r\n"
        "  bl                      list breakpoints\r\n"
        "  bd <n>                  delete breakpoint n\r\n"
        "  u [addr] [n]            unassemble (default PC, 16 instructions)\r\n"
        "  c [addr] [n] [@file]    core dump (default PC, 32 words)\r\n"
        "  r                       registers\r\n"
        "  mmu                     MMU state (SR0, PAR/PDR)\r\n"
        "  save <file>             save RAM to a binary file\r\n"
        "  q                       back to the program or the console\r\n"
        "\r\n"
        "  Enter repeats n and s.\r\n"
        "\r\n"
        "  (Ctrl-E debugger, Ctrl-L logic analyzer)\r\n"
        "\r\n");
}

static void debug_cli(void) {
    fprintf(stderr, "\r\nDebug console (? for help)\r\n");

    char line[256];
    char *tok[16];
    for (;;) {
        static char prompt[32];
        snprintf(prompt, sizeof prompt, "debug |PC=%06o|> ", PC(&m.cpu));
        cli_prompt = prompt;
        cli_help = debug_help;
        cli_immediate = "qgr";
        cli_self = DEBUG_CHAR;
        fprintf(stderr, "%s", prompt);
        int len = probe_readline(line, sizeof(line));
        if (len < 0) return;  /* Ctrl-C */
        static char last[256];                /* Enter repeats n or s */
        if (len == 0 && last[0]) snprintf(line, sizeof line, "%s", last);
        else if (len == 0) continue;
        else last[0] = '\0';
        if ((line[0] == 'n' || line[0] == 's') && (line[1] == '\0' || line[1] == ' '))
            snprintf(last, sizeof last, "%s", line);

        int ntok = tokenize(line, tok, 16);
        if (ntok == 0) continue;

        if (strcmp(tok[0], "q") == 0 || strcmp(tok[0], "quit") == 0) {
            return;
        }
        else if (strcmp(tok[0], "g") == 0 || strcmp(tok[0], "go") == 0) {
            m.cpu.halted = 0;
            return;
        }
        else if (strcmp(tok[0], "?") == 0 || strcmp(tok[0], "h") == 0 || strcmp(tok[0], "help") == 0) {
            debug_help();
        }
        else if (strcmp(tok[0], "n") == 0) {
            int count = ntok >= 2 ? atoi(tok[1]) : 1;
            if (count < 1) count = 1;
            if (count > 4096) count = 4096;
            for (int i = 0; i < count; i++) {
                debug_print_insn(PC(&m.cpu));
                machine_step(&m, 1);                  /* devices run too */
                MachineStop st = machine_run(&m, 100000000ULL);
                if (st == MACHINE_STOP_HALT) {
                    fprintf(stderr, "  CPU halted\r\n");
                    break;
                }
                if (st != MACHINE_STOP_STEP && st != MACHINE_STOP_BREAKPOINT) {
                    machine_halt(&m);
                    fprintf(stderr, "  instruction not finished after 100 ms (WAIT?)\r\n");
                    break;
                }
            }
            m.cpu.halted = 1;
            fprintf(stderr, "  R0=%06o R1=%06o R2=%06o R3=%06o R4=%06o R5=%06o SP=%06o PSW=%06o\r\n",
                    REG(&m.cpu, 0), REG(&m.cpu, 1), REG(&m.cpu, 2), REG(&m.cpu, 3),
                    REG(&m.cpu, 4), REG(&m.cpu, 5), REG(&m.cpu, 6), m.cpu.psw);
        }
        else if (strcmp(tok[0], "s") == 0) {
            int count = 1;
            if (ntok >= 2) count = atoi(tok[1]);
            if (count < 1) count = 1;
            if (count > 4096) count = 4096;

            m.cpu.halted = 0;
            for (int i = 0; i < count; i++) {
                uint32_t ns = kd11ea_ustep(&m.cpu);
                if (m.cpu.dbg.count > 0) {
                    const DbgEntry *e = &m.cpu.dbg.ring[(m.cpu.dbg.head - 1) & (DBG_RING_SIZE - 1)];
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
                if (m.cpu.halted || ns == 0) {
                    fprintf(stderr, "  CPU halted\r\n");
                    break;
                }
            }
            m.cpu.halted = 1;
        }
        else if (strcmp(tok[0], "b") == 0) {
            uint32_t addr = ntok >= 2 ? parse_octal(tok[1]) : (uint32_t)-1;
            if (addr > 0xFFFF || (addr & 1)) {
                fprintf(stderr, "  usage: b <address> (octal, even)\r\n");
                continue;
            }
            int slot = machine_bp_set(&m, (uint16_t)addr);
            if (slot < 0)
                fprintf(stderr, "  max breakpoints reached (%d)\r\n", MACHINE_MAX_BP);
            else
                fprintf(stderr, "  breakpoint #%d at %06o\r\n", slot, addr & 0xFFFF);
        }
        else if (strcmp(tok[0], "bl") == 0) {
            int found = 0;
            for (int i = 0; i < MACHINE_MAX_BP; i++) {
                if (m.bp_used[i]) {
                    fprintf(stderr, "  #%d  %06o\r\n", i, m.bp_addr[i]);
                    found = 1;
                }
            }
            if (!found) fprintf(stderr, "  no breakpoints set\r\n");
        }
        else if (strcmp(tok[0], "bd") == 0) {
            if (ntok < 2) {
                fprintf(stderr, "  usage: bd <n> (bl lists the breakpoints)\r\n");
                continue;
            }
            int idx = atoi(tok[1]);
            if (machine_bp_del(&m, idx) == 0) {
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
                addr = PC(&m.cpu);
            if (ntok >= 3)
                count = atoi(tok[2]);
            if (count < 1) count = 1;
            if (count > 256) count = 256;

            uint16_t a = (uint16_t)addr;
            for (int i = 0; i < count; i++)
                a = (uint16_t)(a + debug_print_insn(a));
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
                addr = PC(&m.cpu);
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
                REG(&m.cpu, 0), REG(&m.cpu, 1), REG(&m.cpu, 2), REG(&m.cpu, 3),
                REG(&m.cpu, 4), REG(&m.cpu, 5), REG(&m.cpu, 6), PC(&m.cpu),
                m.cpu.psw,
                (m.cpu.psw & 0x10) ? 'T' : '-',
                (m.cpu.psw & 0x08) ? 'N' : '-',
                (m.cpu.psw & 0x04) ? 'Z' : '-',
                (m.cpu.psw & 0x02) ? 'V' : '-',
                (m.cpu.psw & 0x01) ? 'C' : '-',
                (m.cpu.psw >> 5) & 7,
                ((m.cpu.psw >> 14) & 3) == 0 ? "kernel" : "user");
        }
        else if (strcmp(tok[0], "mmu") == 0) {
            debug_show_mmu();
        }
        else if (strcmp(tok[0], "save") == 0) {
            if (ntok < 2) {
                fprintf(stderr, "  usage: save <filename>\r\n");
            } else {
                if (mem_save(tok[1], m.ram.mem, m.ram.size_words) < 0)
                    fprintf(stderr, "  error writing %s\r\n", tok[1]);
                else
                    fprintf(stderr, "  saved %u words (%uKB) to %s\r\n",
                            m.ram.size_words, m.ram.size_words / 512, tok[1]);
            }
        }
        else {
            fprintf(stderr, "  unknown command: %s (? for help)\r\n", tok[0]);
        }
    }
}

/* One logic analyzer command. Returns 1 for q. */
static int probe_exec(const char *line_in) {
    char line[256];
    char *tok[16];
    snprintf(line, sizeof line, "%s", line_in);
    char raw[256], *rest;
    snprintf(raw, sizeof raw, "%s", line);
    rest = raw;
    while (*rest == ' ') rest++;
    while (*rest && *rest != ' ') rest++;          /* after the command */
    while (*rest == ' ') rest++;
    int ntok = tokenize(line, tok, 16);
    if (ntok == 0) return 0;

    if (strcmp(tok[0], "q") == 0 || strcmp(tok[0], "quit") == 0) {
        return 1;
    }
    else if (strcmp(tok[0], "?") == 0 || strcmp(tok[0], "h") == 0 || strcmp(tok[0], "help") == 0) {
        probe_help();
    }
    else if (strcmp(tok[0], "list") == 0) {
        probe_cmd_list(rest);
    }
    else if (strcmp(tok[0], "find") == 0) {
        if (*rest) probe_cmd_find(rest);
        else fprintf(stderr, "  usage: find <text>\r\n");
    }
    else if (strcmp(tok[0], "add") == 0) {
        if (*rest) probe_cmd_add_rm(rest, 1);
        else fprintf(stderr, "  usage: add <signal>[, <signal>...] (list or find to see them)\r\n");
    }
    else if (strcmp(tok[0], "rm") == 0) {
        if (*rest) probe_cmd_add_rm(rest, 0);
        else fprintf(stderr, "  usage: rm <signal>[, <signal>...]\r\n");
    }
    else if (strcmp(tok[0], "clear") == 0) {
        probe_clear_signals(&probe);
        fprintf(stderr, "  signals cleared\r\n");
    }
    else if (strcmp(tok[0], "trigger") == 0) {
        probe_cmd_trigger(rest);
    }
    else if (strcmp(tok[0], "depth") == 0) {
        if (ntok >= 2) {
            char *end;
            unsigned long requested = strtoul(tok[1], &end, 10);
            if (tok[1][0] == '-' || *end || requested == 0) {
                fprintf(stderr, "  depth: positive sample count required\r\n");
                return 0;
            }
            if (probe.state == PROBE_ARMED || probe.state == PROBE_CAPTURING) {
                fprintf(stderr, "  depth: disarm the capture first\r\n");
                return 0;
            }
            if (requested > PROBE_MAX_DEPTH) requested = PROBE_MAX_DEPTH;
            uint32_t d = 1;
            while (d * 2 <= requested) d *= 2;
            if (d != probe.capacity) {
                ProbeSnapshot *buf = realloc(probe_buf, (size_t)d * sizeof(*buf));
                if (!buf) {
                    fprintf(stderr, "  depth: cannot allocate %u samples\r\n", d);
                    return 0;
                }
                probe_buf = buf;
                probe.buf = buf;
                probe.capacity = d;
            }
            probe_set_depth(&probe, d);
        }
        fprintf(stderr, "  depth: %u samples\r\n", probe.depth);
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
        uint32_t sr = 1000000000u / (clock_timing(m.cpu.timing)->short_ns * probe.divider);
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
            m.cpu.probe = &probe;
            fprintf(stderr, "  armed (%u samples, trigger %s)\r\n",
                    probe.depth,
                    probe.trigger_signal >= 0 ? "set" : "free-run");
            fprintf(stderr, "  the capture runs with the CPU (q or g resumes it)\r\n");
        }
    }
    else if (strcmp(tok[0], "disarm") == 0) {
        probe_disarm(&probe);
        m.cpu.probe = NULL;
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
        uint32_t sr = 1000000000u / (clock_timing(m.cpu.timing)->short_ns * probe.divider);
        if (probe.divider == 1)
            fprintf(stderr, "  sample rate: %s Hz (realtime CPU clock)\r\n", fmt_comma(sr));
        else
            fprintf(stderr, "  sample rate: %s Hz (1:%u divider)\r\n", fmt_comma(sr), probe.divider);
    }
    else if (strcmp(tok[0], "show") == 0) {
        probe_cmd_show(&probe, 20, ntok, tok);
    }
    else if (strcmp(tok[0], "export") == 0) {
        size_t fl = ntok >= 2 ? strlen(tok[1]) : 0;
        int sr = fl > 3 && !strcasecmp(tok[1] + fl - 3, ".sr");
        int vcd = fl > 4 && !strcasecmp(tok[1] + fl - 4, ".vcd");
        char err[400];
        if (!sr && !vcd) {
            fprintf(stderr, "  usage: export <file.vcd> or export <file.sr> [MHz]\r\n");
        } else if (probe.count == 0) {
            fprintf(stderr, "  no data captured\r\n");
        } else if (sr) {
            uint32_t mhz = ntok >= 3 ? (uint32_t)atoi(tok[2]) : 50;
            int n = -1;                           /* 40 ns clock pulse */
            if (mhz < 25 || mhz > 1000)
                snprintf(err, sizeof err, "sample rate 25 to 1000 MHz, 50 by default");
            else
                n = probe_export_sr(&probe, tok[1], mhz * 1000000u, err, sizeof err);
            if (n >= 0)
                fprintf(stderr, "  wrote %d samples at %u MHz to %s\r\n", n, mhz, tok[1]);
            else
                fprintf(stderr, "  %s\r\n", err);
        } else {
            int n = probe_export_vcd(&probe, tok[1], err, sizeof err);
            if (n >= 0)
                fprintf(stderr, "  wrote %d micro-cycles to %s\r\n", n, tok[1]);
            else
                fprintf(stderr, "  %s\r\n", err);
        }
    }
    else {
        fprintf(stderr, "  unknown command: %s (? for help)\r\n", tok[0]);
    }
    return 0;
}

static void probe_cli(void) {
    fprintf(stderr, "\r\nLogic analyzer (? for help)\r\n");

    char line[256];
    for (;;) {
        static const char *states[] = {"IDLE", "ARMED", "CAPTURING", "DONE"};
        static char prompt[64];
        snprintf(prompt, sizeof prompt, "probe |%s, %d signal%s|> ", states[probe.state],
                 probe.nsignals, probe.nsignals == 1 ? "" : "s");
        cli_prompt = prompt;
        cli_help = probe_help;
        cli_immediate = "q";
        cli_self = PROBE_CHAR;
        fprintf(stderr, "%s", prompt);
        int len = probe_readline(line, sizeof(line));
        if (len < 0) return;  /* Ctrl-C */
        if (len == 0) continue;

        if (probe_exec(line))
            return;
    }
}

/* Probe script (--probe): analyzer commands, one per line, # comments.
 * A line waits while a capture is armed or running, so what follows arm
 * runs once the capture is done. exit leaves ll-34. */
static char probe_script[256][256];
static int probe_script_n, probe_script_pos, probe_script_exit;

static int probe_script_load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return -1; }
    char line[256];
    while (fgets(line, sizeof line, f) && probe_script_n < 256) {
        char *c = strchr(line, '#');
        if (c) *c = '\0';
        char *e = line + strlen(line);
        while (e > line && isspace((unsigned char)e[-1])) *--e = '\0';
        char *b = line;
        while (isspace((unsigned char)*b)) b++;
        if (*b) snprintf(probe_script[probe_script_n++], sizeof probe_script[0], "%s", b);
    }
    fclose(f);
    return 0;
}

static void probe_script_run(void) {
    while (probe_script_pos < probe_script_n) {
        if (probe.state == PROBE_ARMED || probe.state == PROBE_CAPTURING)
            return;
        const char *line = probe_script[probe_script_pos++];
        term_colour(0);
        fprintf(stderr, "probe> %s\r\n", line);
        if (!strcmp(line, "exit")) { probe_script_exit = 1; return; }
        probe_exec(line);
    }
}

/* Host I/O, called by machine_run() before each device tick */
static int cli_host_tick(void *ctx) {
    (void)ctx;
    tty->tick(tty);
    console_tick(&m.con);

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
    return halt_requested || debug_requested || probe_requested;
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options]\n", prog);
    fprintf(stderr, "Load options:\n");
    fprintf(stderr, "  --load FILE   Load with ASCII deposit commands (obj2bin --ascii format)\n");
    fprintf(stderr, "  --lda FILE    Load an absolute binary image (DEC absolute loader .lda/.bin)\n");
    fprintf(stderr, "  --lst FILE    Load program from its MACRO-11 listing, with CPU halted at .END\n");
    fprintf(stderr, "  --start ADDR  Start the CPU at ADDR (octal), as the front panel START\n");
    fprintf(stderr, "  --ptr FILE    Paper tape reader input (binary)\n");
    fprintf(stderr, "  --ptp FILE    Paper tape punch output\n");
    fprintf(stderr, "  --rk FILE     Attach RK05 disk image (up to 8 drives)\n");
    fprintf(stderr, "  --rl FILE     Attach RL01/RL02 disk image (up to 4 drives)\n");
    fprintf(stderr, "  --rw          Write to original disk images instead of /tmp snapshots (dangerous)\n");
    fprintf(stderr, "Boot options:\n");
    fprintf(stderr, "  --rom FILE    Override boot ROM (default: M9301-YF)\n");
    fprintf(stderr, "  --s1 OCTAL    M9301 S1 switch setting (default: 0, boot to console emulator)\n");
    fprintf(stderr, "Machine configuration:\n");
    fprintf(stderr, "  --mem KB      Memory size in KB (default: 248, max: 248)\n");
    fprintf(stderr, "  --timing NAME nominal (DEC factory values, default) or aged (reference hardware from 1978)\n");
    fprintf(stderr, "  --rtc HZ      Line clock frequency, 50 or 60 (default: 60)\n");
    fprintf(stderr, "  --unpaced     Run as fast as the host allows, not at the real speed\n");
    fprintf(stderr, "  --tty MODE    DL11 serial backend: stdio (default), tcp[:port=1134], pty\n");
    fprintf(stderr, "  --probe FILE  Run logic analyzer commands from FILE\n");
}

/* Run the debugger or the logic analyzer, then whichever console the user
 * asks for with Ctrl-E or Ctrl-L. Ctrl-P ends in the programmer console.
 * q goes back to where the user was: the running program if resume is set,
 * otherwise the programmer console */
static void open_consoles(int mode, int resume) {
    term_colour(0);
    for (;;) {
        next_mode = 0;
        if (mode == DEBUG_CHAR)
            debug_cli();
        else if (mode == PROBE_CHAR)
            probe_cli();
        else
            break;
        if (!next_mode && resume && m.cpu.halted) {
            m.cpu.halted = 0;
            fprintf(stderr, "ll-34: resumed\r\n");
        }
        if (!next_mode || !m.cpu.halted)
            break;
        mode = next_mode;
    }
}

int main(int argc, char **argv) {
    const char *rom_file = NULL;
    const char *load_file = NULL;
    const char *lst_file = NULL;
    const char *probe_file = NULL;
    uint32_t start_opt = (uint32_t)-1;
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
    TimingKind timing = TIMING_NOMINAL;
    unsigned rtc_hz = 60;

    /* Parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--rom") == 0 && i + 1 < argc) {
            rom_file = argv[++i];
        } else if (strcmp(argv[i], "--load") == 0 && i + 1 < argc) {
            load_file = argv[++i];
        } else if (strcmp(argv[i], "--lst") == 0 && i + 1 < argc) {
            lst_file = argv[++i];
        } else if (strcmp(argv[i], "--probe") == 0 && i + 1 < argc) {
            probe_file = argv[++i];
        } else if (strcmp(argv[i], "--start") == 0 && i + 1 < argc) {
            start_opt = parse_octal(argv[++i]);
            if (start_opt > 0xFFFF || (start_opt & 1)) {
                fprintf(stderr, "--start: even octal address\n");
                return 1;
            }
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
        } else if (strcmp(argv[i], "--timing") == 0 && i + 1 < argc) {
            const char *name = argv[++i];
            if (strcmp(name, "nominal") == 0) timing = TIMING_NOMINAL;
            else if (strcmp(name, "aged") == 0) timing = TIMING_AGED;
            else { fprintf(stderr, "Unknown timing profile: %s (nominal or aged)\n", name); return 1; }
        } else if (strcmp(argv[i], "--rtc") == 0 && i + 1 < argc) {
            rtc_hz = (unsigned)atoi(argv[++i]);
            if (rtc_hz != 50 && rtc_hz != 60) { fprintf(stderr, "--rtc: 50 or 60\n"); return 1; }
        } else if (strcmp(argv[i], "--unpaced") == 0) {
            unpaced = 1;
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

    MachineConfig cfg = { .mem_kb = mem_kb, .timing = timing, .rtc_hz = rtc_hz };
    if (machine_init(&m, &cfg) < 0)
        return 1;
    fprintf(stderr, "ll-34 %s, PDP-11/34A\n", LL34_VERSION);
    fprintf(stderr, "ll-34: %uKB RAM (%u words)\n", mem_kb, m.ram.size_words);
    fprintf(stderr, "ll-34: timing %s\n", m.cpu.timing->name);

    if (rom_file) {
        int nwords = rom_load_mac(&m.rom, rom_file);
        if (nwords < 0)
            return 1;
        fprintf(stderr, "ll-34: loaded %d words from %s\n", nwords, rom_file);
    }

    static char rk_tmp_paths[RK_NUMDR][256];
    for (int d = 0; d < rk_ndisks; d++) {
        if (rk_raw_rw) {
            if (rk05_attach(&m.rk, d, rk_files[d], 0) < 0)
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
            if (rk05_attach(&m.rk, d, rk_tmp_paths[d], 0) < 0)
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
            if (rl11_attach(&m.rl, d, rl_files[d], 0) < 0)
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
            if (rl11_attach(&m.rl, d, rl_tmp_paths[d], 0) < 0)
                return 1;
            fprintf(stderr, "ll-34: RL dl%d snapshot %s\n",
                    d, rl_tmp_paths[d]);
        }
    }

    if (ptr_file) {
        if (pc11_attach_reader(&m.pc, ptr_file) < 0)
            return 1;
        fprintf(stderr, "ll-34: PC11 reader attached %s\n", ptr_file);
        if (s1_val == 0)
            s1_val = 0706;  /* M9301-YF: boot PR, skip diagnostics */
    }
    if (ptp_file) {
        if (pc11_attach_punch(&m.pc, ptp_file) < 0)
            return 1;
        fprintf(stderr, "ll-34: PC11 punch attached %s\n", ptp_file);
    }

    machine_set_s1(&m, s1_val);
    /* Make sure the host is fast enough to run ll-34 */
    {
        double ns_per = machine_benchmark(&m, s1_val);
        double ratio = (double)clock_timing(m.cpu.timing)->short_ns / ns_per;
        if (ratio >= 1.0)
            fprintf(stderr, "ll-34: host can emulate %.1fx real KD11-EA: speed OK\n", ratio);
        else
            fprintf(stderr, "ll-34: WARNING: host too slow (%.1fx real KD11-EA speed)"
                    "cycle accuracy not guaranteed\n", ratio);
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

    m.dl.rx_ready = dl11_tty_rx_ready;
    m.dl.rx_read  = dl11_tty_rx_read;
    m.dl.tx_write = dl11_tty_tx_write;
    m.dl.io_ctx   = tty;

    uint16_t load_start = 0;
    if (load_file) {
        int nwords = loader_asc(&m.ram, load_file, &load_start);
        if (nwords < 0)
            return 1;
        fprintf(stderr, "ll-34: loaded %d words from %s, start=%06o\n",
                nwords, load_file, load_start);
    }
    if (lda_file) {
        int nbytes = loader_lda(&m.ram, lda_file, &load_start);
        if (nbytes < 0)
            return 1;
        fprintf(stderr, "ll-34: loaded %d bytes from %s, start=%06o\n",
                nbytes, lda_file, load_start);
    }

    int lst_has_start = 0;
    uint16_t lst_start = 0;
    if (lst_file) {
        uint16_t lo, hi;
        int nwords = loader_lst(&m.ram, lst_file, &lst_start, &lst_has_start, &lo, &hi);
        if (nwords < 0)
            return 1;
        fprintf(stderr, "ll-34: loaded %d words from %s (%06o-%06o)", nwords, lst_file, lo, hi);
        if (lst_has_start) fprintf(stderr, ", .END %06o", lst_start);
        fprintf(stderr, "\n");
        if (!lst_has_start) lst_start = lo;
    }
    if (probe_file && probe_script_load(probe_file) < 0)
        return 1;

    probe_buf = calloc(PROBE_DEFAULT_DEPTH, sizeof(ProbeSnapshot));
    if (!probe_buf) {
        fprintf(stderr, "ll-34: cannot allocate probe buffer\n");
        return 1;
    }
    probe_init(&probe, probe_buf, PROBE_DEFAULT_DEPTH);

    int halted_at_start = 0;
    if (start_opt <= 0xFFFF) {                    /* LAD, CNTRL-START */
        m.cpu.halted = 1;
        console_set_addr(&m.con, (uint16_t)start_opt);
        console_key(&m.con, KEY_CNTRL);
        console_key(&m.con, KEY_START);
        m.con.init_requested = 0;
        machine_bus_init(&m);
        fprintf(stderr, "ll-34: START at %06o\n", (unsigned)start_opt);
    } else if (lst_file) {                        /* loaded, the operator starts */
        PC(&m.cpu) = lst_start;
        m.cpu.halted = 1;
        halted_at_start = 1;
    } else if (load_file || lda_file) {
        PC(&m.cpu) = load_start;
        m.cpu.psw = 0;
        m.cpu.mpc = 0;
        m.cpu.halted = 0;
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
            m.ram.mem[(02000 >> 1) + i] = rl_boot_rom[i];
        PC(&m.cpu) = 02002;  /* entry past "LD" magic */
        m.cpu.psw = 0340;
        m.cpu.mpc = 0;
        m.cpu.halted = 0;
        fprintf(stderr, "ll-34: RL bootstrap injected at 002000, boot PC=002002\n");
    } else {
        power_on();
    }

    /* Non-stdio backends still need raw stdin for operator keys */
    if (strcmp(tty_arg, "stdio") != 0) {
        if (stdin_raw_enter() < 0)
            fprintf(stderr, "ll-34: warning: cannot enter raw stdin mode\n");
    }

    fprintf(stderr, "ll-34: Ctrl-P = console, Ctrl-E = debugger, Ctrl-L = logic analyzer\n");

    signal(SIGTERM, sigterm_handler);

    Clock clk;
    clock_init(&clk, m.cpu.ns_elapsed);

    m.host_tick = cli_host_tick;

    probe_script_run();
    if (halted_at_start)
        enter_console_mode("CPU halted");

    int quit_requested = 0;
    while (!got_sigterm && !quit_requested && !probe_script_exit) {
        if (probe_script_pos < probe_script_n)
            probe_script_run();
        if (!m.cpu.halted) {
            term_colour(1);                        /* emulated terminal */
            MachineStop stop = machine_run(&m, CLI_SLICE_NS);
            if (stop != MACHINE_STOP_BUDGET || halt_requested || debug_requested ||
                probe_requested || (probe.state == PROBE_DONE && !probe_notified))
                term_colour(0);                    /* ll-34 speaks */

            if (stop == MACHINE_STOP_HALT || stop == MACHINE_STOP_STEP) {
                enter_console_mode("CPU halted");
                continue;
            }

            if (stop == MACHINE_STOP_BREAKPOINT) {
                fprintf(stderr, "\r\nll-34: breakpoint #%d at PC=%06o\r\n",
                        m.bp_hit, PC(&m.cpu));
                open_consoles(DEBUG_CHAR, 0);
                if (!m.cpu.halted)
                    clock_init(&clk, m.cpu.ns_elapsed);
                else
                    enter_console_mode(NULL);
                continue;
            }

            if (halt_requested && !m.host_stop) {       /* at the instruction end */
                halt_requested = 0;
                m.cpu.halted = 1;
                enter_console_mode("CPU halted");
            }

            if (debug_requested && !m.host_stop) {       /* at the instruction end */
                debug_requested = 0;
                m.cpu.halted = 1;
                fprintf(stderr, "\r\nll-34: suspended at PC=%06o\r\n", PC(&m.cpu));
                open_consoles(DEBUG_CHAR, 1);
                if (!m.cpu.halted)
                    clock_init(&clk, m.cpu.ns_elapsed);
                else
                    enter_console_mode(NULL);
            }

            if (probe_requested && !m.host_stop) {       /* at the instruction end */
                probe_requested = 0;
                m.cpu.halted = 1;
                fprintf(stderr, "\r\nll-34: suspended at PC=%06o\r\n", PC(&m.cpu));
                open_consoles(PROBE_CHAR, 1);
                if (!m.cpu.halted)
                    clock_init(&clk, m.cpu.ns_elapsed);
                else
                    enter_console_mode(NULL);
            }

            if (probe.state == PROBE_DONE && !probe_notified) {
                probe_notified = 1;
                fprintf(stderr, "\r\nll-34: probe capture complete (%u samples)\r\n",
                        probe.count < probe.depth ? probe.count : probe.depth);
            }

            if (!unpaced)
                clock_pace(&clk, m.cpu.ns_elapsed);
        } else {
            console_tick(&m.con);
            if (console_poll() < 0) {
                quit_requested = 1;
                break;
            }
            /* Halted, and stdin is a file that has ended: nothing can
             * happen any more. */
            if (stdin_eof && probe_script_pos >= probe_script_n) {
                quit_requested = 1;
                break;
            }

            if (m.con.boot_requested) {
                m.con.boot_requested = 0;
                fprintf(stderr, "\r\nll-34: BOOT\r\n");
                machine_bus_init(&m);
                machine_cpu_reset(&m);
                power_on();
                clock_init(&clk, m.cpu.ns_elapsed);
                continue;
            }

            if (m.con.init_requested) {
                m.con.init_requested = 0;
                machine_bus_init(&m);
                m.con.led_bus_err = 0;
                if (m.cpu.halted) {                /* INIT key, not START */
                    fprintf(stderr, "\r\nll-34: BUS INIT\r\n");
                    console_prompt();
                }
            }

            usleep(10000);

            if (!m.cpu.halted) {                   /* g in the debugger */
                if (!run_reported)
                    fprintf(stderr, "\r\nll-34: CONT\r\n");
                run_reported = 0;
                clock_init(&clk, m.cpu.ns_elapsed);
            }
        }
    }

    term_colour(0);
    tty->destroy(tty);
    stdin_raw_leave();
    fprintf(stderr, "ll-34: exit at PC=%06o PSW=%06o\n",
            PC(&m.cpu), m.cpu.psw);

    free(probe_buf);
    return 0;
}
