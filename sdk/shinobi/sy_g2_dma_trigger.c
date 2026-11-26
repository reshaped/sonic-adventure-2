#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sy_g2_work.h>

/* address in P2, which reaches memory uncached */
#define P2(address) ((Uint32)(address) | P2_BASE)

SYE_G2_ERR syG2DmaTrigger(SYG2DMA handle, SYE_G2_DMA_TRIGGER trigger)
{
    /* @hack spelling: dma set after the interrupts are masked, where an
     * initializer would do */
    struct sy_g2_dma *dma;
    SYE_G2_ERR result = SYE_G2_ERR_NOTHING;
    Sint32 mask;
    /* @hack spelling: regs, set to the address of the registers' first
     * member, where &sygG2WrkPtr->regs->dma[dma->type] would do */
    volatile struct sy_g2_dma_regs *regs;
    Bool busy;
    /* @hack spelling: adr and ptr, locals each read once, where the
     * address worked out in syCacheOCWB's call would do */
    void *ptr;
    Uint32 adr;

    mask = get_imask();
    set_imask(IMASK_ALL);
    dma = (struct sy_g2_dma *)handle;
    regs = (volatile struct sy_g2_dma_regs *)&sygG2WrkPtr->regs->dma[dma->type]
               .g2_adr;
    syG2DmaIsBusy(handle, &busy);
    if (busy == TRUE) {
        result = SYE_G2_ERR_DMA_BUSY;
    } else {
        adr = dma->root_bus_adr;
        ptr = (void *)P2(adr & SY_G2_PHYSICAL_BITS);
        syCacheOCWB(ptr, dma->sz & SY_G2_DMA_SZ_BYTES);
        regs->enable = 0;
        regs->g2_adr = dma->g2_adr;
        regs->root_bus_adr = dma->root_bus_adr;
        regs->sz = dma->sz;
        regs->dir = dma->dir;
        regs->mode = dma->mode | (trigger == SYE_G2_DMA_TRIGGER_CPU
                                      ? SYE_G2_DMA_TRIGGER_CPU
                                      : SYE_G2_DMA_TRIGGER_INT);
        regs->enable = 1;
        dma->running = TRUE;
        sygG2WrkPtr->transfers_started[dma->type]++;
        regs->start = 1;
    }
    set_imask(mask);
    return result;
}
