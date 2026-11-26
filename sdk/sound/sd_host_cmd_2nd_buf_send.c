#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdHostCmd2ndBufSend(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_host_cmd_buf *buf;
    struct sd_host_cmd_entry *entry;
    Sint32 mask;
    Sint32 max;
    Sint32 queued;
    Sint32 sent;
    Sint32 i;
    Uint8 batch_waits;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    buf = &sdgWrkPtr->host_cmd;
    /* @hack spelling: max, a local for the queue's size, where queue_max
     * read in place would do */
    max = buf->queue_max;
    queued = buf->queue_num;
    if (max == 0) {
        goto exit;
    }
    sdSndMemReadUint8(sdgWrkPtr->stat_adr, &batch_waits);
    if (batch_waits != 0 || buf->sending == TRUE) {
        result = SDE_ERR_DRV_BUSY;
        goto exit;
    }
    /* @bug logic: a failure of sdHostCmdPrimBufSet or sdHostCmdPrimBufFlush
     * leaves sending TRUE, so each later send returns SDE_ERR_DRV_BUSY. only
     * the end of a batch's transfer clears sending, and neither failure
     * starts one. this store sets sending (MOV.L R2,@(28,R14) at 0x8c16b534),
     * and a failed sdHostCmdPrimBufSet leaves by the BF at 0x8c16b564 */
    buf->sending = TRUE;
    sent = 0;
    for (i = 0; i < max; i++) {
        entry = &buf->queue[buf->queue_read++];
        if (buf->queue_read == max) {
            buf->queue_read = 0;
        }
        if (entry->valid == TRUE) {
            result = sdHostCmdPrimBufSet(entry);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
            sent++;
            if (sent == SD_HOST_CMD_SEND_MAX || sent == queued) {
                break;
            }
        }
    }
    result = sdHostCmdPrimBufFlush();
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdHostCmd2ndBufSend, result);
    }
    return result;
}
