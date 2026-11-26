#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

/* the member exports these names, though no header of the SDK declares
 * them. sdMemBlkQueueInit stores its queue in sdgMemBlkQueueBufPtr, its num
 * in sdgMemBlkQueueBufNum and &sdgWrkPtr->blk_queue in sdgMemBlkQueueDbPtr */
/* NOLINTBEGIN(misc-use-internal-linkage) */
struct sd_memblk_queue_entry *sdgMemBlkQueueBufPtr;
Sint32 sdgMemBlkQueueBufNum;
struct sd_memblk_queue *sdgMemBlkQueueDbPtr;
/* NOLINTEND(misc-use-internal-linkage) */

static struct sd_memblk_queue_entry empty_entry = {NULL, NULL};

SDE_ERR sdMemBlkQueueInit(struct sd_memblk_queue_entry *queue, Sint32 num)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;
    Sint32 i;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        sdgWrkPtr->blk_queue.entries = queue;
        sdgWrkPtr->blk_queue.max = num;
        sdgWrkPtr->blk_queue.write = 0;
        sdgWrkPtr->blk_queue.read = 0;
        sdgWrkPtr->blk_queue.num = 0;
        sdgMemBlkQueueDbPtr = &sdgWrkPtr->blk_queue;
        sdgMemBlkQueueBufPtr = queue;
        sdgMemBlkQueueBufNum = num;
        /* @hack spelling: sdgWrkPtr->blk_queue.entries in memset's call and
         * in the loop, where queue, the same pointer, would do */
        /* @hack redundant: the memset of the queue, whose every entry the
         * loop then overwrites */
        memset(sdgWrkPtr->blk_queue.entries, SD_UNSET_FILL,
               num * sizeof(struct sd_memblk_queue_entry));
        for (i = 0; i < num; i++) {
            sdgWrkPtr->blk_queue.entries[i] = empty_entry;
        }
    }
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkQueueInit, result);
    }
    return result;
}
