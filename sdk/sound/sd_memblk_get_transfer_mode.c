#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdMemBlkGetTransferMode(SDE_MEMBLK_TRANSFER_MODE *transfer_mode)
{
    /* @hack spelling: a goto past the store, where if (sdgWrkPtr != NULL)
     * around it would do */
    if (sdgWrkPtr == NULL) {
        goto exit;
    }
    *transfer_mode = sdgWrkPtr->transfer_mode;
exit:
    return SDE_ERR_NOTHING;
}
