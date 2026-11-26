#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void sdHostCmdPrimBufFlushCallback(void *first_arg);
/* NOLINTEND(misc-use-internal-linkage) */

SDE_ERR sdHostCmdPrimBufFlush(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (sdgWrkPtr->host_cmd.batch_num == 0) {
        goto exit;
    }
    /* a DMA transfer moves whole 32-byte units of two commands each. so an
     * odd batch takes the empty command after its last */
    if (sdgWrkPtr->host_cmd.batch_num & 1) {
        sdgWrkPtr->host_cmd.batch_num++;
    }
    result = sdMemBlkSetPrm(
        sdgWrkPtr->batch_blk, sdgWrkPtr->host_cmd.batch,
        sdgWrkPtr->host_cmd.batch_num * sizeof(struct sd_driver_cmd),
        sdHostCmdPrimBufFlushCallback, &sdgWrkPtr->host_cmd);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    result = sdMemBlkTransfer(sdgWrkPtr->batch_blk, sdgWrkPtr->host_cmd_adr);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    sdgWrkPtr->batches_started++;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdHostCmdPrimBufFlush, result);
    }
    return result;
}

/* the callback of the batch's transfer. it tells the sound driver the batch
 * waits, and empties the batch for the next send */
void sdHostCmdPrimBufFlushCallback(void *first_arg)
{
    Sint32 mask;
    Sint32 i;

    mask = get_imask();
    set_imask(IMASK_ALL);
    sdgWrkPtr->batches_ended++;
    sdSndMemWriteUint8(sdgWrkPtr->stat_adr, 1);
    for (i = 0; i < sdgWrkPtr->host_cmd.batch_num; i++) {
        sdgWrkPtr->host_cmd.batch[i].cmd = 0;
    }
    sdgWrkPtr->host_cmd.batch_num = 0;
    sdgWrkPtr->host_cmd.sending = FALSE;
    set_imask(mask);
}
