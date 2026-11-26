#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2DmaClose(SYG2DMA handle)
{
    SYE_G2_ERR result = SYE_G2_ERR_NOTHING;
    Sint32 mask;
    struct sy_g2_dma *dma = (struct sy_g2_dma *)handle;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (dma->open != TRUE) {
        result = SYE_G2_ERR_DMA_NOT_OPEN;
    } else {
        dma->open = FALSE;
    }
    set_imask(mask);
    return result;
}
