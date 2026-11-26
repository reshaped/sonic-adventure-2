#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

/* what mask holds once sdMemBlkTransfer has restored the interrupt mask
 * itself. the function's exit restores only a mask of 0 or more */
#define MASK_RESTORED (-1)

static void memblk_serve_once(void);

SDE_ERR sdMemBlkTransfer(SDMEMBLK handle, Sint32 dst_adr)
{
    SDE_ERR result;
    Sint32 mask;
    struct sd_memblk *blk;
    SDE_MEMBLK_STAT stat;
    SDMEMBLK queued;
    void *queued_dst;
    SDE_MEMBLK_STAT end_stat;

    mask = get_imask();
    set_imask(IMASK_ALL);
    blk = (struct sd_memblk *)handle;
    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    sdMemBlkGetStat(handle, &stat);
    /* @hack redundant: an empty else */
    if (stat == SDE_MEMBLK_STAT_TRANSFER_PROGRESS) {
        result = SDE_ERR_MEMBLK_ALREADY_TRANSFER;
        goto exit;
    } else {
    }
    sdgG2LibResult = SYE_G2_ERR_NOTHING;
    blk->ended = FALSE;
    /* @bug logic: a transfer that fails below leaves its block marked under
     * way. this call marks it before the transfer starts (JSR at
     * 0x8c153f32), and no failure's path takes the mark off. a transfer_mode
     * that is neither CPU nor DMA leaves the mark too (BRA at 0x8c153f4c) */
    result = sdMemBlkSetStat(handle, SDE_MEMBLK_STAT_TRANSFER_PROGRESS);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    switch (sdgWrkPtr->transfer_mode) {
    case SDE_MEMBLK_TRANSFER_MODE_CPU:
        result = sdMemBlkCpuTransfer(handle, (void *)dst_adr);
        /* @hack spelling: a break on success, then the goto, where a goto on
         * failure, then the break, would do */
        if (result == SDE_ERR_NOTHING) {
            break;
        }
        goto exit;
    case SDE_MEMBLK_TRANSFER_MODE_DMA:
        result = sdMemBlkQueuePush(handle, (void *)dst_adr);
        if (result != SDE_ERR_NOTHING) {
            goto exit;
        }
        if (sdgWrkPtr->blocks.in_transfer != NULL) {
            /* @hack spelling: the goto in the else of the wait's test, where a
             * goto under the opposite test would do */
            if (blk->callback == SDD_MEMBLK_SYNC_FUNC) {
                do {
                    sdMemBlkServer();
                } while (sdgWrkPtr->blocks.in_transfer != NULL);
            } else {
                goto exit;
            }
        } else {
            sdMemBlkQueuePop(&queued, &queued_dst);
            result = sdMemBlkDmaTransfer(queued, (Sint32)queued_dst);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
        }
        if (blk->callback == SDD_MEMBLK_SYNC_FUNC) {
            do {
                sdMemBlkGetStat(handle, &end_stat);
                memblk_serve_once();
            } while (end_stat != SDE_MEMBLK_STAT_TRANSFER_FINISHED);
        }
        set_imask(mask);
        mask = MASK_RESTORED;
        break;
    }
exit:
    if (mask >= 0) {
        set_imask(mask);
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkTransfer, result);
    }
    return result;
}

static void memblk_serve_once(void)
{
    sdMemBlkServer();
}
