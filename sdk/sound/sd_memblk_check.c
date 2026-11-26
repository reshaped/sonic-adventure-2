#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdMemBlkCheck(SDMEMBLK handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    /* @hack spelling: blk, a local for the record, where handle cast in each
     * test of the tag would do */
    struct sd_memblk *blk = (struct sd_memblk *)handle;

    if (sdgWrkPtr == NULL || sdgWrkPtr == (struct sd_work *)-1) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL || handle == (SDMEMBLK)-1) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (blk->tag != SD_MEMBLK_MADE && blk->tag != SD_MEMBLK_FREE) {
        result = SDE_ERR_HANDLE_ILLEGAL_VALUE;
        /* @hack redundant: a goto to the label that follows it */
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkCheck, result);
    }
    return result;
}
