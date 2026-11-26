#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkQueuePop(SDMEMBLK *handle, void **dst)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        /* @hack spelling: the queue indexed at each of the entry's four uses,
         * where a pointer to the entry would do */
        *handle = sdgWrkPtr->blk_queue.entries[sdgWrkPtr->blk_queue.read].blk;
        *dst = sdgWrkPtr->blk_queue.entries[sdgWrkPtr->blk_queue.read].dst;
        sdgWrkPtr->blk_queue.entries[sdgWrkPtr->blk_queue.read].blk = NULL;
        sdgWrkPtr->blk_queue.entries[sdgWrkPtr->blk_queue.read].dst = NULL;
        sdgWrkPtr->blk_queue.read++;
        if (sdgWrkPtr->blk_queue.read == sdgWrkPtr->blk_queue.max) {
            sdgWrkPtr->blk_queue.read = 0;
        }
        sdgWrkPtr->blk_queue.num--;
    }
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkQueuePop, result);
    }
    return result;
}
