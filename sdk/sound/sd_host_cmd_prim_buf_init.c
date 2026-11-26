#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the batch sdHostCmdPrimBufInit was last given */
void *sdgPrimBufPtr;
/* NOLINTEND(misc-use-internal-linkage) */

SDE_ERR sdHostCmdPrimBufInit(void *buf)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;
    Sint32 i;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        /* @hack redundant: both the goto and the else that follows it, where
         * either alone would do */
        goto exit;
    } else {
        sdgPrimBufPtr = buf;
        sdgWrkPtr->host_cmd.batch = buf;
        sdgWrkPtr->host_cmd.batch_num = 0;
        /* @hack spelling: sdgWrkPtr->host_cmd.batch in memset's call, where
         * buf, the same pointer, would do */
        memset(sdgWrkPtr->host_cmd.batch, SD_UNSET_FILL,
               SD_BATCH_MAX * sizeof(struct sd_driver_cmd));
        for (i = 0; i < SD_BATCH_MAX; i++) {
            sdgWrkPtr->host_cmd.batch[i].cmd = 0;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        /* @hack redundant: a goto to the label that follows it */
        if ((result = sdMemBlkCreate(&sdgWrkPtr->batch_blk)) !=
            SDE_ERR_NOTHING) {
            goto exit;
        }
    }
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdHostCmdPrimBufInit, result);
    }
    return result;
}
