#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <shinobi.h>

void sdMemBlkEndTransferHandller(void *first_arg)
{
    SDMEMBLK next;
    void *dst;
    Sint32 num;
    Sint32 mask;
    struct sd_memblk *blk;

    mask = get_imask();
    set_imask(IMASK_ALL);
    /* @hack spelling: blk set after the mask is raised, where an initializer
     * would do */
    blk = first_arg;
    if (blk->stat == SDE_MEMBLK_STAT_TRANSFER_FINISHED) {
        goto exit;
    }
    blk->end_time = syTmrCountToMicro(syTmrGetCount());
    blk->stat = SDE_MEMBLK_STAT_TRANSFER_FINISHED;
    sdMemBlkExecuteCallback((SDMEMBLK)blk);
    sdgWrkPtr->blocks.in_transfer = NULL;
    sdgWrkPtr->blocks.transfers_ended++;
    sdMemBlkQueueGetPushNum(&num);
    if (num > 0) {
        sdMemBlkQueuePop(&next, &dst);
        switch (sdgWrkPtr->transfer_mode) {
        case SDE_MEMBLK_TRANSFER_MODE_CPU:
            sdMemBlkCpuTransfer(next, dst);
            break;
        case SDE_MEMBLK_TRANSFER_MODE_DMA:
            sdMemBlkDmaTransfer(next, (Sint32)dst);
            break;
        }
    }
exit:
    set_imask(mask);
}
