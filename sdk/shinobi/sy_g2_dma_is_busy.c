#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

/* the bit of a channel's start register that reads 1 while the channel's
 * transfer is under way */
#define START_BUSY 0x1

SYE_G2_ERR syG2DmaIsBusy(SYG2DMA handle, Bool *flg)
{
    Sint32 mask;
    struct sy_g2_dma *dma = (struct sy_g2_dma *)handle;
    /* @hack spelling: type and regs, locals for the channel's type and its
     * registers, where sygG2WrkPtr->regs->dma[dma->type].start read in place
     * would do */
    SYE_G2_DMA_TYPE type;
    volatile struct sy_g2_dma_regs *regs;

    mask = get_imask();
    set_imask(IMASK_ALL);
    type = dma->type;
    regs = &sygG2WrkPtr->regs->dma[type];
    /* @hack spelling: the test's result through ? TRUE : FALSE, where != 0
     * would do */
    *flg = (regs->start & START_BUSY) ? TRUE : FALSE;
    set_imask(mask);
    return SYE_G2_ERR_NOTHING;
}
