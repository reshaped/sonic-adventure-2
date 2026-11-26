#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

/* how many times the wait for a block in transfer would ask the DMA */
#define WAIT_MAX 30000

SDE_ERR sdMemBlkDestroy(SDMEMBLK handle)
{
    SDE_ERR result;
    Sint32 mask;
    SDE_MEMBLK_STAT stat;
    Bool busy;
    Sint32 found;
    Sint32 i;
    /* @hack spelling: blk, a local for the record, where handle cast at the
     * tag's test and at its store would do */
    struct sd_memblk *blk = (struct sd_memblk *)handle;

    mask = get_imask();
    set_imask(IMASK_ALL);
    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    result = sdMemBlkGetStat(handle, &stat);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    /* @hack spelling: no case for SDE_MEMBLK_STAT_TRANSFER_FINISHED, where
     * an empty one, or a default, would do */
    switch (stat) { /* NOLINT(clang-diagnostic-switch) */
    case SDE_MEMBLK_STAT_TRANSFER_REMAIN:
        /* @bug logic: a block still waiting in the queue is freed, and its
         * entry stays there. no statement sets found to 1, so a block found
         * in the queue leaves the switch as one not found does (BT at
         * 0x8c12e70c) */
        found = 0;
        for (i = 0; i < SDD_MEMBLK_QUEUE_NUM; i++) {
            if (sdgWrkPtr->blk_queue.entries[i].blk == handle) {
                break;
            }
        }
        if (found != 1) {
            break;
        }
    case SDE_MEMBLK_STAT_TRANSFER_PROGRESS:
        /* @bug logic: the wait never runs, so a block whose transfer is
         * under way is freed at once. its test i > WAIT_MAX fails for the
         * first i, 0 (TST R6,R6 at 0x8c12e71a) */
        for (i = 0; i > WAIT_MAX; i++) {
            /* @bug mismatched-call: passes the block's handle to
             * syG2DmaIsBusy, which takes a DMA channel's (MOV R13,R4 at
             * 0x8c12e726) */
            if (syG2DmaIsBusy((SYG2DMA)handle, &busy) == SYE_G2_ERR_NOTHING) {
                break;
            }
        }
        break;
    }
    if (blk->tag == SD_MEMBLK_MADE) {
        blk->tag = SD_MEMBLK_FREE;
        sdgWrkPtr->blocks.made--;
    } else {
        result = SDE_ERR_HANDLE_ILLEGAL_VALUE;
    }
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkDestroy, result);
    }
    return result;
}
