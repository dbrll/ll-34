/*
 * mmu.h: KD11-EA Memory Management Unit
 *
 * The KD11-EA has the MMU built into the CPU board (K1-6, K1-7).
 * There is no separate KT11 option card on the PDP-11/34, the PAR/PDR
 * RAMs, relocation adders, and abort logic are always present.
 *
 * Translation is combinatorial: it happens in the VBA→PBA path at bus
 * cycle time (schematic K1-6), controlled by SR0 bit 0 (RELOCATE ENB).
 *
 * Hardware: 3× 74S283 adders (E45/E40/E47), PAR/PDR in 8554 RAMs (K1-7),
 * abort logic feeds E52 KTE input for trap to vector 250.
 * SR0 bit 0 → E100 (MS74 flip-flop) → RELOCATE ENB → E113 → RELOCATE H.
 */

#ifndef MMU_H
#define MMU_H

#include <stdint.h>

/* PAR/PDR register layout:
 *   Index 0-7  = Kernel pages 0-7
 *   Index 8-15 = User pages 0-7
 *
 * PAR format: bits [11:0] = PAF (Page Address Field, 12 bits)
 *
 * PDR format (KT11, PDP-11/34):
 *   Bit 14:8  = PLF (Page Length Field, 7 bits)
 *   Bit 6     = W (Written / dirty bit, read-only)
 *   Bit 3     = ED (Expansion Direction: 0=up, 1=down)
 *   Bit 2:1   = ACF (Access Control Field, 2 bits on KT11)
 *                00 = non-resident
 *                01 = read-only
 *                10 = unused (abort all on 11/34, trap-on-read on 11/45)
 *                11 = read/write
 *   Bit 0     = not used on 11/34 (always 0)
 *
 * SR0 format:
 *   Bit 15    = abort: non-resident
 *   Bit 14    = abort: page length exceeded
 *   Bit 13    = abort: read-only violation
 *   Bit 8     = maintenance / destination mode (not on 11/34)
 *   Bit 6:5   = mode (00=kernel, 11=user)
 *   Bit 3:1   = page number
 *   Bit 0     = enable (RELOCATE)
 */

typedef struct {
    uint16_t par[16];   /* PAR: 8 kernel (0-7) + 8 user (8-15) */
    uint16_t pdr[16];   /* PDR: 8 kernel (0-7) + 8 user (8-15) */
    uint16_t sr0;       /* Status Register 0 */
    uint16_t sr2;       /* Virtual PC saved on abort */
} MMU;

/* SR0 bit definitions */
#define SR0_ENABLE       0x0001  /* bit 0: MMU enable (RELOCATE) */
#define SR0_ABORT_NR     0x8000  /* bit 15: non-resident abort */
#define SR0_ABORT_PL     0x4000  /* bit 14: page length abort */
#define SR0_ABORT_RO     0x2000  /* bit 13: read-only abort */
#define SR0_ABORT_MASK   0xE000  /* bits 15:13, any abort frozen */

/* PDR bit definitions (KT11, PDP-11/34) */
#define PDR_ACF_MASK     0x0006  /* bits 2:1, 2-bit ACF */
#define PDR_ACF_SHIFT    1       /* ACF is at bits 2:1 */
#define PDR_ED           0x0008  /* bit 3: expansion direction */
#define PDR_W            0x0040  /* bit 6: written (dirty) */
#define PDR_PLF_SHIFT    8       /* bits 14:8 */
#define PDR_PLF_MASK     0x7F00  /* 7-bit PLF */

/* PDR write mask for 11/34: PLF[14:8] + W[6] + ED[3] + ACF[2:1]
 * Bit 0 is not implemented.  W is set by hardware, cleared by write. */
#define PDR_WR_MASK      0x7F4E  /* = SimH PDR_1134 = 0077516 */

/* ACF values (2-bit encoding at bits 2:1 of PDR) */
#define ACF_NON_RESIDENT 0       /* 00: non-resident */
#define ACF_READ_ONLY    1       /* 01: read-only */
#define ACF_UNUSED       2       /* 10: unused (abort all on 11/34) */
#define ACF_READ_WRITE   3       /* 11: read/write */

/* Initialize MMU (all registers to 0, disabled) */
void mmu_init(MMU *mmu);

/* Translate VBA → PBA.
 * Returns 0 on success, -1 on abort (sr0 updated with abort info).
 *
 * mmu:     MMU state
 * vba:     16-bit virtual bus address
 * mode:    CPU mode (0=kernel, 3=user) from PSW[15:14]
 * access:  0=read, 1=write
 * pc:      current virtual PC (saved in SR2 on abort)
 * pba_out: resulting 18-bit physical bus address */
int mmu_translate(MMU *mmu, uint16_t vba, int mode, int access,
                  uint16_t pc, uint32_t *pba_out);

/* Bus read/write handlers for MMU registers (PAR/PDR/SR0/SR2).
 * Registered on the UNIBUS at the appropriate I/O page addresses. */
int mmu_read(void *dev, uint32_t addr, uint16_t *data);
int mmu_write(void *dev, uint32_t addr, uint16_t data, int is_byte);

#endif /* MMU_H */
