/*
 * mmu.c: KD11-EA Memory Management Unit
 *
 * The MMU is built into the KD11-EA CPU board.
 * Combinatorial address translation:
 *   K1-6: 3× 74S283 adders compute PBA = (PAF + block_number) << 6 | disp
 *   K1-7: PAR/PDR stored in 8554 RAMs, indexed by mode + page
 *   K1-8: E100 (MS74) = SR0 bit 0 = RELOCATE ENB → E113 → RELOCATE H
 *   Abort logic blocks MSYN and signals KTE to E52 priority encoder
 */

#include <string.h>
#include "mmu.h"
#include "../trace.h"

/* MMU I/O page register addresses (18-bit PBA).
 * Supervisor regs (772200-772256) respond to avoid NXM but are inactive. */
#define KS_BLOCK_BASE  0x3F480  /* 772200 */
#define KS_BLOCK_END   0x3F5FE  /* 772776, covers all kernel/super MMU space */

#define KPDR_BASE   0x3F4C0     /* 772300 */
#define KPDR_END    0x3F4CE     /* 772316, 8 words */
#define KPAR_BASE   0x3F4E0     /* 772340 */
#define KPAR_END    0x3F4EE     /* 772356, 8 words */

/* User PAR/PDR: 777600-777676 */
#define U_BLOCK_BASE   0x3FF80  /* 777600 */
#define U_BLOCK_END    0x3FFBE  /* 777676 */

#define UPDR_BASE   0x3FF80     /* 777600 */
#define UPDR_END    0x3FF8E     /* 777616, 8 words */
#define UPAR_BASE   0x3FFA0     /* 777640 */
#define UPAR_END    0x3FFAE     /* 777656, 8 words */

/* Status registers: 777572-777576 (18-bit PBA) */
#define SR0_ADDR    0x3FF7A     /* 777572 */
#define SR1_ADDR    0x3FF7C     /* 777574, not implemented, reads 0 */
#define SR2_ADDR    0x3FF7E     /* 777576 */


void mmu_init(MMU *mmu) {
    memset(mmu, 0, sizeof(*mmu));
}


/* Address translation (K1-6 adders: E45/E40/E47).
 * PBA = ((PAF + block) << 6) | displacement */

int mmu_translate(MMU *mmu, uint16_t vba, int mode, int access,
                  uint16_t pc, uint32_t *pba_out)
{
    (void)pc;  /* used only by trace() */

    /* MMU disabled: pass-through, I/O page shortcut via E121 (K1-6) */
    if (!(mmu->sr0 & SR0_ENABLE)) {
        if ((vba & 0xE000) == 0xE000)
            *pba_out = 0x30000 | (uint32_t)vba;
        else
            *pba_out = vba;
        return 0;
    }

    int page  = (vba >> 13) & 7;       /* VBA[15:13] */
    int block = (vba >> 6) & 0x7F;     /* VBA[12:6] */
    int disp  = vba & 0x3F;            /* VBA[5:0] */

    int idx = (mode == 3 ? 8 : 0) + page;

    uint16_t par = mmu->par[idx];
    uint16_t pdr = mmu->pdr[idx];
    int paf = par & 0x0FFF;
    int acf = (pdr >> PDR_ACF_SHIFT) & 3;
    int plf = (pdr & PDR_PLF_MASK) >> PDR_PLF_SHIFT;
    int ed  = (pdr & PDR_ED) ? 1 : 0;

    if (acf == ACF_NON_RESIDENT || acf == ACF_UNUSED) {
        if (!(mmu->sr0 & SR0_ABORT_MASK)) {
            mmu->sr0 |= SR0_ABORT_NR | ((mode & 3) << 5) | (page << 1);
        }
        trace("[MMU] ABORT NR: VBA=%06o page=%d mode=%d acf=%d PC=%06o\n",
              vba, page, mode, acf, pc);
        return -1;
    }

    if (acf == ACF_READ_ONLY && access) {
        if (!(mmu->sr0 & SR0_ABORT_MASK)) {
            mmu->sr0 |= SR0_ABORT_RO | ((mode & 3) << 5) | (page << 1);
        }
        return -1;
    }

    /* Page length check */
    if (ed == 0) {
        if (block > plf) {
            if (!(mmu->sr0 & SR0_ABORT_MASK)) {
                mmu->sr0 |= SR0_ABORT_PL | ((mode & 3) << 5) | (page << 1);
            }
            return -1;
        }
    } else {
        if (block < plf) {
            if (!(mmu->sr0 & SR0_ABORT_MASK)) {
                mmu->sr0 |= SR0_ABORT_PL | ((mode & 3) << 5) | (page << 1);
            }
            return -1;
        }
    }

    if (access)
        mmu->pdr[idx] |= PDR_W;

    uint32_t pba = ((uint32_t)((paf + block) & 0x1FFF) << 6 | disp) & 0x3FFFF;

    *pba_out = pba;
    return 0;
}


/* MMU register bus interface */

int mmu_read(void *dev, uint32_t addr, uint16_t *data)
{
    MMU *mmu = (MMU *)dev;
    addr &= 0x3FFFF;

    /* Kernel PAR 0-7 */
    if (addr >= KPAR_BASE && addr <= KPAR_END) {
        int idx = (addr - KPAR_BASE) >> 1;
        *data = mmu->par[idx];
        return 0;
    }

    /* Kernel PDR 0-7 */
    if (addr >= KPDR_BASE && addr <= KPDR_END) {
        int idx = (addr - KPDR_BASE) >> 1;
        *data = mmu->pdr[idx];
        return 0;
    }

    /* User PAR 0-7 */
    if (addr >= UPAR_BASE && addr <= UPAR_END) {
        int idx = 8 + ((addr - UPAR_BASE) >> 1);
        *data = mmu->par[idx];
        return 0;
    }

    /* User PDR 0-7 */
    if (addr >= UPDR_BASE && addr <= UPDR_END) {
        int idx = 8 + ((addr - UPDR_BASE) >> 1);
        *data = mmu->pdr[idx];
        return 0;
    }

    /* SR0 */
    if (addr == SR0_ADDR) {
        *data = mmu->sr0;
        return 0;
    }

    /* SR1: not implemented on KD11-EA */
    if (addr == SR1_ADDR) {
        *data = 0;
        return 0;
    }

    /* SR2 */
    if (addr == SR2_ADDR) {
        *data = mmu->sr2;
        return 0;
    }

    *data = 0;
    return 0;
}

int mmu_write(void *dev, uint32_t addr, uint16_t data, int is_byte)
{
    MMU *mmu = (MMU *)dev;
    addr &= 0x3FFFF;
    (void)is_byte;  /* PAR/PDR are word-only in practice */
    /* Kernel PAR 0-7 */
    if (addr >= KPAR_BASE && addr <= KPAR_END) {
        int idx = (addr - KPAR_BASE) >> 1;
        mmu->par[idx] = data & 0x0FFF;  /* 12-bit PAF */
        trace("[MMU] KPAR%d <- %06o\n", idx, mmu->par[idx]);
        return 0;
    }

    /* Kernel PDR 0-7 */
    if (addr >= KPDR_BASE && addr <= KPDR_END) {
        int idx = (addr - KPDR_BASE) >> 1;
        mmu->pdr[idx] = data & PDR_WR_MASK;
        trace("[MMU] KPDR%d <- %06o (ACF=%d ED=%d PLF=%03o)\n",
              idx, mmu->pdr[idx], (mmu->pdr[idx] >> 1) & 3,
              (mmu->pdr[idx] >> 3) & 1, (mmu->pdr[idx] >> 8) & 0x7F);
        return 0;
    }

    /* User PAR 0-7 */
    if (addr >= UPAR_BASE && addr <= UPAR_END) {
        int idx = 8 + ((addr - UPAR_BASE) >> 1);
        mmu->par[idx] = data & 0x0FFF;
        return 0;
    }

    /* User PDR 0-7 */
    if (addr >= UPDR_BASE && addr <= UPDR_END) {
        int idx = 8 + ((addr - UPDR_BASE) >> 1);
        mmu->pdr[idx] = data & PDR_WR_MASK;
        return 0;
    }

    /* SR0: writable bits 15:13, 8, 0 */
    if (addr == SR0_ADDR) {
        uint16_t writable = 0xE101;  /* bits 15:13 + 8 + 0 */
        mmu->sr0 = (mmu->sr0 & ~writable) | (data & writable);
        trace("[MMU] SR0 <- %06o (MMU %s)\n",
              mmu->sr0, (mmu->sr0 & SR0_ENABLE) ? "ON" : "OFF");

        return 0;
    }

    /* SR1: not implemented, ignore writes */
    if (addr == SR1_ADDR)
        return 0;

    /* SR2: read-only */
    if (addr == SR2_ADDR)
        return 0;

    return 0;
}
