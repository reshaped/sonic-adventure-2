#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdSysHostCmdBufFlush(const SDE_HOST_CMD_FLUSH_MODE mode)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (sdgWrkPtr->host_cmd.queue_num >= 0) {
        switch (mode) {
        case SDE_HOST_CMD_FLUSH_MODE_NORMAL:
            result = sdHostCmdServer();
            break;
        case SDE_HOST_CMD_FLUSH_MODE_FORCE:
            /* @bug logic: never retries, so it flushes once, as
             * SDE_HOST_CMD_FLUSH_MODE_NORMAL does. the loop repeats only on
             * SDE_ERR_DRV_BUSY (CMP/EQ R13,R14 at 0x8c12f7ea), which
             * sdHostCmdServer never returns (MOV #0,R0 at 0x8c153adc) */
            do {
                result = sdHostCmdServer();
            } while (result == SDE_ERR_DRV_BUSY);
            break;
        case SDE_HOST_CMD_FLUSH_MODE_FORCE_ALL:
            /* sdHostCmdServer never returns SDE_ERR_DRV_BUSY, so the inner
             * loop runs once each pass. this loop still calls it until the
             * queue is empty */
            while (sdgWrkPtr->host_cmd.queue_num > 0) {
                /* @hack spelling: an endless loop left by a break, where a
                 * do-while like the one in SDE_HOST_CMD_FLUSH_MODE_FORCE's case
                 * would do */
                for (;;) {
                    result = sdHostCmdServer();
                    if (result != SDE_ERR_DRV_BUSY) {
                        break;
                    }
                }
            }
            break;
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSysHostCmdBufFlush, result);
    }
    return result;
}
