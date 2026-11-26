#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2DmaReset(struct sy_g2_dma *dma)
{
    Sint32 mask;
    /* @hack spelling: regs, set to the address of the registers' first
     * member, where sygG2WrkPtr->regs->dma[dma->type].enable stored in place
     * would do */
    volatile struct sy_g2_dma_regs *regs;

    mask = get_imask();
    set_imask(IMASK_ALL);
    /* @hack spelling: the record looked up again for each of its twelve
     * stores, where a pointer to it would do */
    sygG2WrkPtr->dma[dma->type].type = dma->type;
    sygG2WrkPtr->dma[dma->type].open = FALSE;
    sygG2WrkPtr->dma[dma->type].running = FALSE;
    sygG2WrkPtr->dma[dma->type].transfer_ended = FALSE;
    sygG2WrkPtr->dma[dma->type].g2_adr = 0;
    sygG2WrkPtr->dma[dma->type].root_bus_adr = 0;
    sygG2WrkPtr->dma[dma->type].sz = 0;
    sygG2WrkPtr->dma[dma->type].dir = 0;
    sygG2WrkPtr->dma[dma->type].mode = 0;
    sygG2WrkPtr->dma[dma->type].unk_18 = 3;
    sygG2WrkPtr->dma[dma->type].callback = NULL;
    sygG2WrkPtr->dma[dma->type].callback_1st_arg = NULL;
    regs = (volatile struct sy_g2_dma_regs *)&sygG2WrkPtr->regs->dma[dma->type]
               .g2_adr;
    regs->enable = 0;
    set_imask(mask);
    return SYE_G2_ERR_NOTHING;
}
