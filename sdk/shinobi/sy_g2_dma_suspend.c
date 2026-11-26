#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2DmaSuspend(SYG2DMA handle)
{
    Sint32 mask;
    struct sy_g2_dma *dma = (struct sy_g2_dma *)handle;
    /* @hack spelling: regs, set to the address of the registers' first
     * member, where sygG2WrkPtr->regs->dma[dma->type].suspend stored in
     * place would do */
    volatile struct sy_g2_dma_regs *regs;

    mask = get_imask();
    set_imask(IMASK_ALL);
    regs = (volatile struct sy_g2_dma_regs *)&sygG2WrkPtr->regs->dma[dma->type]
               .g2_adr;
    regs->suspend = 1;
    /* @hack redundant: a label nothing jumps to */
exit: /* NOLINT(clang-diagnostic-unused-label) */
    set_imask(mask);
    return SYE_G2_ERR_NOTHING;
}
