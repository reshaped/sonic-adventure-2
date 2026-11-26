#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2DmaOpen(SYG2DMA *handle, SYE_G2_DMA_TYPE type)
{
    SYE_G2_ERR result = SYE_G2_ERR_NOTHING;
    Sint32 mask;
    struct sy_g2_dma *dma;

    mask = get_imask();
    set_imask(IMASK_ALL);
    *handle = NULL;
    dma = &sygG2WrkPtr->dma[type];
    if (dma->open == TRUE) {
        result = SYE_G2_ERR_DMA_ALREADY_OPEN;
    } else {
        /* @hack spelling: the record looked up again for each of its twelve
         * stores, where a pointer to it would do */
        /* @hack redundant: a store of the type to the record it was read
         * from */
        sygG2WrkPtr->dma[dma->type].type = dma->type;
        /* @hack redundant: a store of FALSE to open, which the TRUE below
         * replaces */
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
        dma->open = TRUE;
        *handle = (SYG2DMA)dma;
    }
    set_imask(mask);
    return result;
}
