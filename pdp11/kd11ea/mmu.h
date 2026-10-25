/* Memory management of the KD11-EA. It is part of the CPU board (K1-6,
 * K1-7). The PAR and PDR are 8554 RAMs, and an abort reaches E52 as KTE,
 * which traps to 250. */

#ifndef MMU_H
#define MMU_H

#include <stdint.h>

typedef struct {
    uint16_t par[16];   /* kernel pages 0-7, then user pages 0-7 */
    uint16_t pdr[16];
    uint16_t sr0;
    uint16_t sr2;       /* virtual PC of the last instruction fetch */
    int8_t   page;      /* page of the last relocation, -1 if none */
    uint8_t  mode;      /* mode of the last relocation */
} MMU;

#define SR0_ENABLE       0x0001
#define SR0_MAINT        0x0100  /* maintenance, destinations only */
#define SR0_ABORT_NR     0x8000  /* non-resident */
#define SR0_ABORT_PL     0x4000  /* page length */
#define SR0_ABORT_RO     0x2000  /* read-only */
#define SR0_ABORT_MASK   0xE000

/* The 11/34 has a 2-bit ACF and no PDR bit 0. */
#define PDR_ACF_MASK     0x0006
#define PDR_ACF_SHIFT    1
#define PDR_ED           0x0008
#define PDR_W            0x0040
#define PDR_PLF_SHIFT    8
#define PDR_PLF_MASK     0x7F00
#define PDR_WR_MASK      0x7F4E

#define ACF_NON_RESIDENT 0
#define ACF_READ_ONLY    1
#define ACF_UNUSED       2       /* aborts every access on the 11/34 */
#define ACF_READ_WRITE   3

void mmu_init(MMU *mmu);
/* RCD INIT (K1-8) clears the SR0 latches. The PAR and PDR RAMs and SR2
 * keep their contents. */
void mmu_bus_init(void *dev, int init_l);

int mmu_kt_write(uint32_t pba, int c0);

/* Translate a virtual address. With bus = MMU_MAP only the address is
 * computed, as when BA is loaded. Otherwise bus is the transfer type, which
 * E83 sees on UBUS C1 C0 when the transfer starts. An abort is then latched
 * in SR0 and -1 is returned. */
#define MMU_MAP (-1)
int mmu_translate(MMU *mmu, uint16_t vba, int mode, int bus, int relocate,
                  uint16_t pc, uint32_t *pba_out);

int mmu_read(void *dev, uint32_t addr, uint16_t *data);
int mmu_write(void *dev, uint32_t addr, uint16_t data, int is_byte);

#endif
