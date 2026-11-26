#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

SDE_ERR sdHostCmdPrimBufSet(struct sd_host_cmd_entry *entry)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_driver_cmd *to;

    if (sdgWrkPtr->host_cmd.batch_num >= SD_BATCH_MAX) {
        result = SDE_ERR_HOST_CMD_BUF_NO_ENOUGH;
        goto exit;
    }
    to = &sdgWrkPtr->host_cmd.batch[sdgWrkPtr->host_cmd.batch_num++];
    to->cmd = entry->cmd.cmd;
    memcpy(to->prm, entry->cmd.prm, sizeof(to->prm));
    entry->valid = FALSE;
    sdgWrkPtr->host_cmd.queue_num--;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdHostCmdPrimBufSet, result);
    }
    return result;
}
