#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* a DMA transfer the library asked syG2DmaTrigger to start. g2_adr is the
 * bus address of its destination, start_time is in microseconds, and
 * result is the trigger's */
struct dma_record {
    Uint32 g2_adr;
    void *ptr;
    Sint32 size;
    Uint32 start_time;
    SYE_G2_ERR result;
    Uint8 unk_14[0xc];
};
#define DMA_RECORDS 256
/* DMA carries only a block whose size is a multiple of DMA_UNIT bytes */
#define DMA_UNIT 32

/* the member exports these names, though no header of the SDK declares
 * them. sdlMemBlkG2DmaErrNum counts the failed calls of syG2DmaSetPrm and
 * syG2DmaTrigger. sdlG2DmaHistory is a ring of the last DMA_RECORDS
 * transfers, and sdlG2DmaHistoryIndex is the entry the next one fills */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 sdlMemBlkG2DmaErrNum = 0;
struct dma_record sdlG2DmaHistory[DMA_RECORDS];
Sint32 sdlG2DmaHistoryIndex;
/* NOLINTEND(misc-use-internal-linkage) */

static void memblk_count_g2_error(void);

SDE_ERR sdMemBlkDmaTransfer(SDMEMBLK handle, Sint32 dst_adr)
{
    SDE_ERR result;
    Sint32 mask;
    struct sd_memblk *blk;

    mask = get_imask();
    set_imask(IMASK_ALL);
    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    blk = (struct sd_memblk *)handle;
    if (blk->size & (DMA_UNIT - 1)) {
        result = SDE_ERR_PRM_ILLEGAL_VALUE;
        goto exit;
    }
    /* @hack redundant: a store of SYE_G2_ERR_NOTHING to sdgG2LibResult,
     * which syG2DmaSetPrm's result replaces */
    sdgG2LibResult = SYE_G2_ERR_NOTHING;
    blk->ended = FALSE;
    /* @hack spelling: the bus address as a pointer sum, where
     * (void *)(dst_adr + SD_SND_MEM_BASE) would do */
    sdgG2LibResult =
        syG2DmaSetPrm(sdgWrkPtr->blocks.dma, (Sint8 *)SD_SND_MEM_BASE + dst_adr,
                      blk->ptr, blk->size, SYE_G2_DMA_DIR_ROOT_TO_G2, TRUE,
                      TRUE, sdMemBlkEndTransferHandller, blk);
    if (sdgG2LibResult != SYE_G2_ERR_NOTHING) {
        memblk_count_g2_error();
    }
    blk->start_time = syTmrCountToMicro(syTmrGetCount());
    sdgG2LibResult =
        syG2DmaTrigger(sdgWrkPtr->blocks.dma, SYE_G2_DMA_TRIGGER_CPU);
    if (sdgG2LibResult != SYE_G2_ERR_NOTHING) {
        memblk_count_g2_error();
    }
    /* @bug logic: makes the block the one in transfer even where
     * syG2DmaTrigger refused to start it, as it does while the channel's
     * earlier transfer runs (MOV.L R10,@(R0,R2) at 0x8c16b7e0). syG2DmaSetPrm
     * has made the block its end callback's argument, so that earlier
     * transfer's end marks the block ended, though its bytes never moved */
    sdgWrkPtr->blocks.in_transfer = handle;
    sdgWrkPtr->blocks.transfers_started++;
    /* @hack spelling: sdlG2DmaHistory indexed at each of the record's five
     * stores, where a pointer to the record would do */
    sdlG2DmaHistory[sdlG2DmaHistoryIndex].g2_adr = dst_adr + SD_SND_MEM_BASE;
    sdlG2DmaHistory[sdlG2DmaHistoryIndex].ptr = blk->ptr;
    sdlG2DmaHistory[sdlG2DmaHistoryIndex].size = blk->size;
    sdlG2DmaHistory[sdlG2DmaHistoryIndex].start_time = blk->start_time;
    sdlG2DmaHistory[sdlG2DmaHistoryIndex].result = sdgG2LibResult;
    sdlG2DmaHistoryIndex = (sdlG2DmaHistoryIndex + 1) % DMA_RECORDS;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkDmaTransfer, result);
    }
    set_imask(mask);
    return result;
}

static void memblk_count_g2_error(void)
{
    sdlMemBlkG2DmaErrNum++;
}
