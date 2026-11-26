#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* an empty entry of the queue */
struct sd_host_cmd_entry sdgHostCmd2ndDefault = {
    FALSE, 0xffffffff, {0, {0}}, {0}};

/* the queue sdHostCmd2ndBufInit was last given */
struct sd_host_cmd_entry *sdg2ndHostCmdBufPtr;
/* NOLINTEND(misc-use-internal-linkage) */

SDE_ERR sdHostCmd2ndBufInit(struct sd_host_cmd_entry *buf, const Sint32 num)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Uint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        sdgWrkPtr->host_cmd.queue = buf;
        sdgWrkPtr->host_cmd.queue_max = num;
        sdgWrkPtr->host_cmd.queue_num = 0;
        sdgWrkPtr->host_cmd.queue_read = 0;
        sdgWrkPtr->host_cmd.queue_write = 0;
        sdgWrkPtr->host_cmd.sending = FALSE;
        sdg2ndHostCmdBufPtr = buf;
        /* @hack redundant: the memset of the queue, whose every entry the
         * loop then overwrites */
        memset(buf, 0, num * sizeof(struct sd_host_cmd_entry));
        for (i = 0; i < (Uint32)num; i++) {
            sdgWrkPtr->host_cmd.queue[i] = sdgHostCmd2ndDefault;
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdHostCmd2ndBufInit, result);
    }
    return result;
}
