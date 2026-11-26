#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkExecuteCallback(SDMEMBLK handle)
{
    struct sd_memblk *blk = (struct sd_memblk *)handle;
    SDE_ERR result;

    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    if (blk->callback != NULL) {
        if (blk->callback == SDD_MEMBLK_SYNC_FUNC) {
            blk->ended = TRUE;
        } else {
            blk->callback(blk->arg);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkExecuteCallback, result);
    }
    return result;
}
