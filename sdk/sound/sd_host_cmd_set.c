#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <shinobi.h>

SDE_ERR sdHostCmdSet(struct sd_host_cmd *cmd)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;
    struct sd_host_cmd_buf *buf;
    struct sd_host_cmd_entry *entry;
    Sint32 num;
    Sint32 max;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    buf = &sdgWrkPtr->host_cmd;
    /* @hack spelling: num and max, locals for the queue's count and size,
     * where the fields read in place would do */
    num = buf->queue_num;
    max = buf->queue_max;
    if (num == SD_HOST_CMD_QUEUE_NUM) {
        result = SDE_ERR_HOST_CMD_BUF_NO_ENOUGH;
        goto exit;
    }
    entry = &buf->queue[buf->queue_write++];
    if (buf->queue_write == max) {
        buf->queue_write = 0;
    }
    entry->valid = TRUE;
    entry->cmd = *cmd;
    entry->time = syTmrGetCount();
    buf->queue_num++;
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdHostCmdSet, result);
    }
    return result;
}
