#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2DmaSetPrm(SYG2DMA handle, void *g2_adr, void *root_bus_adr,
                         Sint32 sz, SYE_G2_DMA_DIR dir, Bool transfer_ctl,
                         Bool suspend_mode, SYE_G2_CALLBACK_FUNC callback,
                         void *callback_1st_arg)
{
    Sint32 mask;
    struct sy_g2_dma *dma = (struct sy_g2_dma *)handle;

    mask = get_imask();
    set_imask(IMASK_ALL);
    dma->g2_adr = (Uint32)g2_adr & SY_G2_DMA_ADR_BITS;
    dma->root_bus_adr = (Uint32)root_bus_adr & SY_G2_DMA_ADR_BITS;
    dma->sz = (sz & SY_G2_DMA_SZ_BITS) | SY_G2_DMA_SZ_END;
    dma->dir = dir & SY_G2_DMA_DIR_BITS;
    if (transfer_ctl == TRUE) {
        dma->mode |= SY_G2_DMA_MODE_TRANSFER_CTL;
    } else {
        dma->mode &= ~SY_G2_DMA_MODE_TRANSFER_CTL;
    }
    if (suspend_mode == TRUE) {
        dma->mode |= SY_G2_DMA_MODE_SUSPEND;
    } else {
        dma->mode &= ~SY_G2_DMA_MODE_SUSPEND;
    }
    dma->mode &= SY_G2_DMA_MODE_BITS;
    dma->callback = callback;
    dma->callback_1st_arg = callback_1st_arg;
    dma->transfer_ended = FALSE;
    /* @hack redundant: a label nothing jumps to */
exit: /* NOLINT(clang-diagnostic-unused-label) */
    set_imask(mask);
    return SYE_G2_ERR_NOTHING;
}
