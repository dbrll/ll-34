/* KD11-EA memory management. */

#include <string.h>
#include "mmu.h"
#include "rom_wiring.h"
#include "../trace.h"

/* OC outputs idle high. */
static uint8_t read_e74_e75(uint32_t pba, uint8_t c1, uint8_t c0,
                            uint8_t e74)
{
    if (!m8265_e54_io_page_enabled(pba) ||
        !m8265_e81_general_enabled(pba))
        return 0x0f;
    return (e74 ? m8265_e74_rom : m8265_e75_rom)
        [m8265_e74_e75_addr(pba, c1, c0, e74)];
}

static uint8_t read_e76_e77(uint32_t pba, uint8_t c1, uint8_t c0,
                            uint8_t e76)
{
    if (!m8265_e54_io_page_enabled(pba) ||
        !m8265_e118_par_pdr_enabled(pba))
        return 0x0f;
    return (e76 ? m8265_e76_rom : m8265_e77_rom)
        [m8265_e76_e77_addr(pba, c1, c0)];
}

static unsigned apr_index(uint32_t pba)
{
    /* PBA08 selects the register set. */
    return (((pba >> 8) & 1) << 3) | ((pba >> 1) & 7);
}


void mmu_init(MMU *mmu) {
    memset(mmu, 0, sizeof(*mmu));
}


/* PBA = ((PAF + block) << 6) | displacement. */

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


    int compare = block > plf || (ed && block == plf);
    int illegal_mode = ((mode >> 1) ^ mode) & 1;
    uint8_t e83 = m8265_e83_rom[m8265_e83_addr(
        access ? 1 : 0, 0, 1, illegal_mode, acf, compare, ed)];
    uint16_t aborts = 0;

    if (M8265_E83_NR(e83))
        aborts |= SR0_ABORT_NR;
    if (M8265_E83_PL(e83))
        aborts |= SR0_ABORT_PL;
    if (M8265_E83_RO(e83))
        aborts |= SR0_ABORT_RO;

    if (aborts) {
        /* SR0 keeps flags from the fault. */
        if (!(mmu->sr0 & SR0_ABORT_MASK))
            mmu->sr0 |= aborts | ((mode & 3) << 5) | (page << 1);
        trace("[MMU] ABORT: VBA=%06o page=%d mode=%d acf=%d flags=%06o PC=%06o\n",
              vba, page, mode, acf, aborts, pc);
        return -1;
    }

    if (access)
        mmu->pdr[idx] |= PDR_W;

    uint32_t pba = ((uint32_t)((paf + block) & 0x1FFF) << 6 | disp) & 0x3FFFF;

    *pba_out = pba;
    return 0;
}


/* Register decode ROMs. */

int mmu_read(void *dev, uint32_t addr, uint16_t *data)
{
    MMU *mmu = (MMU *)dev;
    uint8_t d;

    addr &= 0x3ffff;

    d = read_e76_e77(addr, 0, 0, 1); /* DATI */
    if (!M8265_E76_PAR_PDR_L(d)) {
        unsigned idx = apr_index(addr);
        *data = M8265_E76_KT_MUX_S0_L(d) ? mmu->par[idx] : mmu->pdr[idx];
        return 0;
    }

    d = read_e74_e75(addr, 0, 0, 1); /* DATI */
    if (!M8265_E74_INT_SSYN_L(d)) {
        *data = M8265_E74_KT_MUX_S0_L(d) ? mmu->sr0 : mmu->sr2;
        return 0;
    }

    *data = 0;
    return 0;
}

int mmu_write(void *dev, uint32_t addr, uint16_t data, int is_byte)
{
    MMU *mmu = (MMU *)dev;
    uint8_t c0 = is_byte ? 1 : 0;
    uint8_t d;

    addr &= 0x3ffff;

    d = read_e76_e77(addr, 1, c0, 0); /* DATO/DATOB */
    if (!M8265_E77_LOAD_PAR_HIGH_L(d) ||
        !M8265_E77_LOAD_PAR_LOW_L(d)) {
        unsigned idx = apr_index(addr);
        mmu->par[idx] = data & 0x0fff;
        trace("[MMU] %cPAR%u <- %06o\n", idx < 8 ? 'K' : 'U',
              idx & 7, mmu->par[idx]);
        return 0;
    }
    if (!M8265_E77_LOAD_PDR_HIGH_L(d) ||
        !M8265_E77_LOAD_PDR_LOW_L(d)) {
        unsigned idx = apr_index(addr);
        mmu->pdr[idx] = data & PDR_WR_MASK;
        trace("[MMU] %cPDR%u <- %06o (ACF=%d ED=%d PLF=%03o)\n",
              idx < 8 ? 'K' : 'U', idx & 7, mmu->pdr[idx],
              (mmu->pdr[idx] >> 1) & 3, (mmu->pdr[idx] >> 3) & 1,
              (mmu->pdr[idx] >> 8) & 0x7f);
        return 0;
    }

    d = read_e74_e75(addr, 1, c0, 0); /* DATO/DATOB */
    if (!M8265_E75_LOAD_SR0_HIGH_L(d) ||
        !M8265_E75_LOAD_SR0_LOW_L(d)) {
        uint16_t writable = 0xe101; /* bits 15:13 + 8 + 0 */
        mmu->sr0 = (mmu->sr0 & ~writable) | (data & writable);
        trace("[MMU] SR0 <- %06o (MMU %s)\n", mmu->sr0,
              (mmu->sr0 & SR0_ENABLE) ? "ON" : "OFF");
    }

    return 0;
}
