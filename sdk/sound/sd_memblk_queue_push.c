#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkQueuePush(SDMEMBLK handle, void *dst)
{
    SDE_ERR result;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    result = sdMemBlkCheck(handle);
    /* @hack redundant: the empty first arm, where if (result ==
     * SDE_ERR_NOTHING) around the other two would do */
    /* @hack spelling: max <= num, where num >= max would do */
    if (result != SDE_ERR_NOTHING) {
    } else if (sdgWrkPtr->blk_queue.max <= sdgWrkPtr->blk_queue.num) {
        result = SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH;
    } else {
        /* @hack spelling: the queue indexed at both of the entry's uses,
         * where a pointer to the entry would do */
        sdgWrkPtr->blk_queue.entries[sdgWrkPtr->blk_queue.write].blk = handle;
        sdgWrkPtr->blk_queue.entries[sdgWrkPtr->blk_queue.write].dst = dst;
        sdgWrkPtr->blk_queue.write++;
        if (sdgWrkPtr->blk_queue.write == sdgWrkPtr->blk_queue.max) {
            sdgWrkPtr->blk_queue.write = 0;
        }
        sdgWrkPtr->blk_queue.num++;
    }
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkQueuePush, result);
    }
    return result;
}
