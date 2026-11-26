#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkQueueGetPushNum(Sint32 *num)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    *num = sdgWrkPtr->blk_queue.num;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkQueueGetPushNum, result);
    }
    return result;
}
