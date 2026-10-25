/* Program loaders. Each returns the number of words or bytes loaded, or
 * -1 on error. */
#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include "ram.h"

/* Text made of the M9301 console commands L, D and S, as written by
 * obj2bin.pl --ascii. */
int loader_asc(Ram *ram, const char *filename, uint16_t *start_addr);

/* DEC absolute loader format (.lda or .bin). */
int loader_lda(Ram *ram, const char *filename, uint16_t *start_addr);

/* MACRO-11 listing of an absolute program. The start address comes from
 * .END, when there is one. */
int loader_lst(Ram *ram, const char *filename, uint16_t *start_addr,
               int *has_start, uint16_t *lo, uint16_t *hi);

#endif
