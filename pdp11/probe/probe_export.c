/* Export a probe capture as VCD or as a sigrok session (.sr).
 * Each micro-cycle lasts from the previous snapshot to its own. PROC CLK L
 * is drawn low for the last 40 ns of a cycle, as measured on MPC.sr, and
 * the PSW clock high for the last 40 ns of LOAD CC and LOAD PSW. */
#include "probe.h"
#include "../version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SR_CHUNK     (4u << 20)     /* bytes per logic file, as sigrok does */
#define SR_MAX_CH    64             /* PulseView keeps a sample in 64 bits */
#define SR_CLK_LOW_NS 40

static uint32_t crc_table[256];

static uint32_t crc32_update(uint32_t crc, const uint8_t *p, size_t n) {
    if (!crc_table[1])
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int k = 0; k < 8; k++) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            crc_table[i] = c;
        }
    crc = ~crc;
    while (n--) crc = crc_table[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    return ~crc;
}

typedef struct {
    FILE *f;
    int n;
    char name[64][16];
    uint32_t crc[64], size[64], offset[64];
} Zip;

static void put16(FILE *f, unsigned v) { fputc(v & 0xFF, f); fputc((v >> 8) & 0xFF, f); }
static void put32(FILE *f, uint32_t v) { put16(f, v & 0xFFFF); put16(f, v >> 16); }

/* One stored entry: local header, then the data */
static int zip_add(Zip *z, const char *name, const void *data, uint32_t size) {
    if (z->n >= 64) return -1;
    int i = z->n++;
    snprintf(z->name[i], sizeof z->name[i], "%s", name);
    z->crc[i] = crc32_update(0, data, size);
    z->size[i] = size;
    z->offset[i] = (uint32_t)ftell(z->f);
    put32(z->f, 0x04034b50); put16(z->f, 10); put16(z->f, 0); put16(z->f, 0);
    put16(z->f, 0); put16(z->f, 0x21);               /* time 0, date 1980-01-01 */
    put32(z->f, z->crc[i]); put32(z->f, size); put32(z->f, size);
    put16(z->f, (unsigned)strlen(name)); put16(z->f, 0);
    fputs(name, z->f);
    return fwrite(data, 1, size, z->f) == size ? 0 : -1;
}

static int zip_close(Zip *z) {
    uint32_t start = (uint32_t)ftell(z->f);
    for (int i = 0; i < z->n; i++) {
        put32(z->f, 0x02014b50); put16(z->f, 10); put16(z->f, 10); put16(z->f, 0);
        put16(z->f, 0); put16(z->f, 0); put16(z->f, 0x21);
        put32(z->f, z->crc[i]); put32(z->f, z->size[i]); put32(z->f, z->size[i]);
        put16(z->f, (unsigned)strlen(z->name[i])); put16(z->f, 0); put16(z->f, 0);
        put16(z->f, 0); put16(z->f, 0); put32(z->f, 0); put32(z->f, z->offset[i]);
        fputs(z->name[i], z->f);
    }
    uint32_t end = (uint32_t)ftell(z->f);
    put32(z->f, 0x06054b50); put16(z->f, 0); put16(z->f, 0);
    put16(z->f, (unsigned)z->n); put16(z->f, (unsigned)z->n);
    put32(z->f, end - start); put32(z->f, start); put16(z->f, 0);
    return fclose(z->f);
}

/* 1 is PROC CLK L, 2 is the PSW clock */
static int is_clock(const ProbeDef *d) {
    if (!d->name) return 0;
    if (!strcmp(d->name, "K1-5 PROC CLK L")) return 1;
    if (!strcmp(d->name, "K1-1 PSW CLK H")) return 2;
    return 0;
}

/* Bits used by a signal in the capture, at least 1 */
static int signal_width(const Probe *p, const ProbeDef *d, uint32_t total) {
    int w = 1;
    if (probe_is_1bit(d) || is_clock(d)) return 1;
    uint32_t any = 0;
    for (uint32_t i = 0; i < total; i++) any |= probe_read_value(probe_get(p, i), d);
    while (w < 32 && (any >> w)) w++;
    return w;
}

typedef struct {
    const ProbeDef *def;
    int bit;            /* value bit, -1 for PROC CLK L, -2 for PSW clock */
} Channel;

int probe_export_sr(const Probe *p, const char *path, uint32_t rate_hz, char *err, int errlen) {
    uint32_t total = probe_valid_count(p);
    if (total < 2) { snprintf(err, errlen, "need at least 2 samples"); return -1; }
    if (!rate_hz) rate_hz = 50000000;

    /* Channels */
    Channel ch[SR_MAX_CH];
    int nch = 0, need = 0, width[PROBE_MAX_SIGNALS];
    for (int s = 0; s < p->nsignals; s++) {
        const ProbeDef *d = probe_def_get(p->signal_idx[s]);
        width[s] = signal_width(p, d, total);
        need += width[s];
    }
    if (need > SR_MAX_CH) {                           /* name the wide signals */
        int n = snprintf(err, errlen, "%d channels needed, PulseView reads %d at most:", need, SR_MAX_CH);
        for (int s = 0; s < p->nsignals && n < errlen; s++)
            if (width[s] > 1)
                n += snprintf(err + n, errlen - n, " %s %d,",
                              probe_display_name(probe_def_get(p->signal_idx[s])), width[s]);
        if (n < errlen && err[n - 1] == ',') err[n - 1] = '\0';
        return -1;
    }
    for (int s = 0; s < p->nsignals; s++) {
        const ProbeDef *d = probe_def_get(p->signal_idx[s]);
        int clk = is_clock(d);
        for (int b = 0; b < width[s]; b++) {
            ch[nch].def = d;
            ch[nch].bit = clk ? -clk : (width[s] > 1 ? b : 0);
            nch++;
        }
    }
    if (!nch) { snprintf(err, errlen, "no signals"); return -1; }
    int unit = nch <= 8 ? 1 : nch <= 16 ? 2 : nch <= 32 ? 4 : 8;

    /* Metadata */
    char meta[8192];
    int m = snprintf(meta, sizeof meta,
                     "[global]\nsigrok version=ll-34 " LL34_VERSION " emulator\n\n[device 1]\ncapturefile=logic-1\n"
                     "total probes=%d\nsamplerate=%u Hz\ntotal analog=0\n", nch, rate_hz);
    for (int c = 0; c < nch && m < (int)sizeof meta - 200; c++) {
        const ProbeDef *d = ch[c].def;
        char loc[24] = "";
        if (d->loc && strchr(d->loc, ':')) {
            snprintf(loc, sizeof loc, "%s", d->loc);
            *strchr(loc, ':') = '-';
        }
        int multi = c + 1 < nch && ch[c + 1].def == d && ch[c].bit >= 0;
        multi |= c > 0 && ch[c - 1].def == d && ch[c].bit >= 0;
        m += snprintf(meta + m, sizeof meta - m, "probe%d=%s%s%s", c + 1,
                      loc, loc[0] ? " | " : "", probe_display_name(d));
        if (multi) m += snprintf(meta + m, sizeof meta - m, " %d", ch[c].bit);
        m += snprintf(meta + m, sizeof meta - m, "\n");
    }
    m += snprintf(meta + m, sizeof meta - m, "unitsize=%d\n", unit);

    Zip z = { .f = fopen(path, "wb") };
    if (!z.f) { snprintf(err, errlen, "cannot write %s", path); return -1; }
    zip_add(&z, "version", "2", 1);
    zip_add(&z, "metadata", meta, (uint32_t)m);

    /* The first cycle is given the length of the second. */
    uint8_t *chunk = malloc(SR_CHUNK);
    if (!chunk) { fclose(z.f); snprintf(err, errlen, "out of memory"); return -1; }
    uint64_t ns0 = probe_get(p, 0)->ns, ns1 = probe_get(p, 1)->ns;
    double t0 = (double)ns0 - (double)(ns1 - ns0);
    double dt = 1e9 / rate_hz;
    uint64_t nsamples = 0;
    uint8_t v[SR_MAX_CH / 8], smp[SR_MAX_CH / 8];
    uint32_t used = 0, k = 0, vk = UINT32_MAX;
    int nfile = 0, rc = 0;
    for (;;) {
        double t = t0 + (double)nsamples * dt;
        while (k < total && (double)probe_get(p, k)->ns <= t) k++;
        if (k == total) break;
        const ProbeSnapshot *snap = probe_get(p, k);
        if (k != vk) {                                /* values of cycle k */
            memset(v, 0, sizeof v);
            for (int c = 0; c < nch; c++)
                if (ch[c].bit >= 0 && ((probe_read_value(snap, ch[c].def) >> ch[c].bit) & 1))
                    v[c >> 3] |= (uint8_t)(1 << (c & 7));
            vk = k;
        }
        memcpy(smp, v, unit);
        for (int c = 0; c < nch; c++) {
            int early = (double)snap->ns - t > SR_CLK_LOW_NS;
            if ((ch[c].bit == -1 && early) ||
                (ch[c].bit == -2 && !early && probe_read_value(snap, ch[c].def)))
                smp[c >> 3] |= (uint8_t)(1 << (c & 7));
        }
        if (used + unit > SR_CHUNK) {
            char name[16];
            snprintf(name, sizeof name, "logic-1-%d", ++nfile);
            rc |= zip_add(&z, name, chunk, used);
            used = 0;
            if (z.n == 62) break;                     /* about 250 MB of samples */
        }
        memcpy(chunk + used, smp, unit);
        used += unit;
        nsamples++;
    }
    if (used || !nfile) {
        char name[16];
        snprintf(name, sizeof name, "logic-1-%d", ++nfile);
        rc |= zip_add(&z, name, chunk, used);
    }
    free(chunk);
    rc |= zip_close(&z);
    if (rc) { snprintf(err, errlen, "write error on %s", path); return -1; }
    return (int)(nsamples > 0x7FFFFFFF ? 0x7FFFFFFF : nsamples);
}

/* VCD scope and name: "K2-9 BUT SERVICE (1) H" is BUT_SERVICE_(1)_H in
 * scope K2-9, "K2:E102:9" is E102_9 in K2, an alias goes in its group */
static void vcd_names(const ProbeDef *d, char *scope, int smax, char *ref, int max) {
    const char *b = probe_def_board(d), *n = probe_display_name(d);
    if (b) {                                          /* sheet, or board */
        int k = 2;
        if (n[2] == '-') while (n[k] && n[k] != ' ') k++;
        snprintf(scope, smax, "%.*s", k, n);
        n += k;
        while (*n == ' ' || *n == ':') n++;
    } else if (d->name && !strncmp(d->name, "BUS ", 4)) {
        snprintf(scope, smax, "BUS");
        n += 4;
    } else {
        snprintf(scope, smax, "%s", d->group ? d->group : "ll34");
    }
    int k = 0;
    for (; *n && k < max - 1; n++) ref[k++] = (*n == ' ' || *n == ':') ? '_' : *n;
    ref[k] = '\0';
}

static void vcd_value(FILE *f, uint32_t v, int width, const char *id) {
    if (width == 1) { fprintf(f, "%u%s\n", v & 1, id); return; }
    char bits[33];
    int n = 0, top = width - 1;
    while (top > 0 && !((v >> top) & 1)) top--;
    for (int b = top; b >= 0; b--) bits[n++] = (char)('0' + ((v >> b) & 1));
    bits[n] = '\0';
    fprintf(f, "b%s %s\n", bits, id);
}

int probe_export_vcd(const Probe *p, const char *path, char *err, int errlen) {
    uint32_t total = probe_valid_count(p);
    if (total < 2) { snprintf(err, errlen, "need at least 2 samples"); return -1; }
    int ns = p->nsignals, width[PROBE_MAX_SIGNALS], clk[PROBE_MAX_SIGNALS];
    char id[PROBE_MAX_SIGNALS][4];
    const ProbeDef *def[PROBE_MAX_SIGNALS];
    for (int s = 0; s < ns; s++) {
        def[s] = probe_def_get(p->signal_idx[s]);
        width[s] = signal_width(p, def[s], total);
        clk[s] = is_clock(def[s]);
        id[s][0] = (char)('!' + s); id[s][1] = '\0';
    }
    FILE *f = fopen(path, "w");
    if (!f) { snprintf(err, errlen, "cannot write %s", path); return -1; }

    uint64_t ns0 = probe_get(p, 0)->ns, ns1 = probe_get(p, 1)->ns;
    uint64_t t0 = ns0 - (ns1 - ns0);
    fprintf(f, "$comment ll-34 " LL34_VERSION " emulator: %u micro-cycles from %llu ns $end\n",
            total, (unsigned long long)t0);
    fprintf(f, "$timescale 1 ns $end\n");
    char done[PROBE_MAX_SIGNALS] = {0};
    for (int s = 0; s < ns; s++) {                    /* one scope at a time */
        if (done[s]) continue;
        char scope[32], ref[64], sc2[32];
        vcd_names(def[s], scope, sizeof scope, ref, sizeof ref);
        fprintf(f, "$scope module %s $end\n", scope);
        for (int t = s; t < ns; t++) {
            vcd_names(def[t], sc2, sizeof sc2, ref, sizeof ref);
            if (done[t] || strcmp(sc2, scope)) continue;
            done[t] = 1;
            if (width[t] == 1) fprintf(f, "$var wire 1 %s %s $end\n", id[t], ref);
            else fprintf(f, "$var wire %d %s %s [%d:0] $end\n", width[t], id[t], ref, width[t] - 1);
        }
        fprintf(f, "$upscope $end\n");
    }
    fprintf(f, "$enddefinitions $end\n");

    /* Values change at the start of a cycle, the clock rises at its end. */
    uint32_t prev[PROBE_MAX_SIGNALS];
    for (uint32_t k = 0; k < total; k++) {
        const ProbeSnapshot *snap = probe_get(p, k);
        uint64_t start = k ? probe_get(p, k - 1)->ns : t0;
        fprintf(f, "#%llu\n", (unsigned long long)(start - t0));
        if (k == 0) fprintf(f, "$dumpvars\n");
        for (int s = 0; s < ns; s++) {
            uint32_t v = clk[s] ? (clk[s] == 1) : probe_read_value(snap, def[s]);
            if (width[s] < 32) v &= (1u << width[s]) - 1;
            if (k == 0 || v != prev[s]) vcd_value(f, v, width[s], id[s]);
            prev[s] = v;
        }
        if (k == 0) fprintf(f, "$end\n");
        uint64_t pulse = snap->ns > start + SR_CLK_LOW_NS ? snap->ns - SR_CLK_LOW_NS : start;
        int pulse_time = 0;
        for (int s = 0; s < ns; s++) {
            if (!clk[s] || (clk[s] == 2 && !probe_read_value(snap, def[s]))) continue;
            if (!pulse_time++) fprintf(f, "#%llu\n", (unsigned long long)(pulse - t0));
            prev[s] = clk[s] == 2;
            fprintf(f, "%u%s\n", prev[s], id[s]);
        }
    }
    fprintf(f, "#%llu\n", (unsigned long long)(probe_get(p, total - 1)->ns - t0));
    for (int s = 0; s < ns; s++)
        if (clk[s]) fprintf(f, "%d%s\n", clk[s] == 1, id[s]);
    if (fclose(f)) { snprintf(err, errlen, "write error on %s", path); return -1; }
    return (int)total;
}
