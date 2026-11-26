#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdHostCmdServer(void)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr != NULL && sdgExistSndDrv == TRUE) {
        /* @hack spelling: the test of the queue's count in an if of its own,
         * where && with the tests above would do */
        if (sdgWrkPtr->host_cmd.queue_num > 0) {
            sdHostCmd2ndBufSend();
        }
    }
    set_imask(mask);
    return SDE_ERR_NOTHING;
}
