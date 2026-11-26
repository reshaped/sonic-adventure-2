#include <sg_syg2.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2DmaResume(SYG2DMA handle)
{
    struct sy_g2_dma *dma = (struct sy_g2_dma *)handle;
    /* @hack spelling: regs, set to the address of the registers' first
     * member, where sygG2WrkPtr->regs->dma[dma->type].suspend stored in
     * place would do */
    volatile struct sy_g2_dma_regs *regs;

    regs = (volatile struct sy_g2_dma_regs *)&sygG2WrkPtr->regs->dma[dma->type]
               .g2_adr;
    regs->suspend = 0;
    return SYE_G2_ERR_NOTHING;
}
