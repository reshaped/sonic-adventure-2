#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2DmaSuspendAll(void)
{
    Sint32 i;
    /* @hack spelling: dma, a local for each channel's record, where
     * &sygG2WrkPtr->dma[i] in the call would do */
    struct sy_g2_dma *dma;

    for (i = 0; i < SY_G2_DMA_NUM; i++) {
        dma = &sygG2WrkPtr->dma[i];
        syG2DmaSuspend((SYG2DMA)dma);
    }
    sygG2DmaSuspendAll = TRUE;
    return SYE_G2_ERR_NOTHING;
}
