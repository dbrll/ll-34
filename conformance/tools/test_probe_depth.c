/* Probe capacity and depth regression tests. No CPU or ROM changes. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdp11/probe/probe.h"

static void fill(Probe *p) {
    ProbeSnapshot sample = {0};
    probe_arm(p);
    for (uint32_t i = 0; i < p->depth; i++) {
        sample.ns = i;
        probe_sample(p, &sample);
    }
    assert(p->state == PROBE_DONE);
    assert(probe_valid_count(p) == p->depth);
    assert(probe_get(p, 0)->ns == 0);
    assert(probe_get(p, p->depth - 1)->ns == p->depth - 1);
    assert(probe_get(p, p->depth) == NULL);
}

int main(void) {
    Probe p;
    ProbeSnapshot small[17];
    memset(small, 0xa5, sizeof small);
    ProbeSnapshot guard = small[16];
    probe_init(&p, small, 16);
    probe_set_depth(&p, UINT32_MAX);
    assert(p.depth == 16 && p.capacity == 16);
    fill(&p);
    assert(memcmp(&guard, &small[16], sizeof guard) == 0);

    /* A depth change invalidates a completed or active ring. */
    probe_set_depth(&p, 7);
    assert(p.depth == 4 && p.state == PROBE_IDLE);
    assert(probe_valid_count(&p) == 0);
    fill(&p);
    probe_arm(&p);
    probe_set_depth(&p, 8);
    assert(p.state == PROBE_IDLE && probe_valid_count(&p) == 0);
    fill(&p);
    probe_set_depth(&p, 0);
    assert(p.depth == 1);
    fill(&p);

    /* The backend also protects fixed-size browser and test buffers. */
    probe_init(&p, small, 15);
    assert(p.capacity == 15 && p.depth == 8);
    fill(&p);

    ProbeSnapshot *large = malloc((PROBE_MAX_DEPTH + 1u) * sizeof(*large));
    assert(large);
    memset(&large[PROBE_MAX_DEPTH], 0x5a, sizeof guard);
    guard = large[PROBE_MAX_DEPTH];
    probe_init(&p, large, PROBE_MAX_DEPTH);
    assert(p.depth == 262144);
    fill(&p);
    probe_set_depth(&p, 4096);
    fill(&p);
    probe_set_depth(&p, UINT32_MAX);
    assert(p.depth == PROBE_MAX_DEPTH);
    fill(&p);
    assert(memcmp(&guard, &large[PROBE_MAX_DEPTH], sizeof guard) == 0);
    free(large);
    puts("Probe depth: capacity bounds, resizing and 262144-sample capture OK");
    return 0;
}
