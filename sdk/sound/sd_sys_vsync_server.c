#include <machine.h>
#include <sd_work.h>
#include <sg_xpt.h>

void sdSysVsyncServer(void *arg)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr != NULL && sdgExistSndDrv == TRUE) {
        if (sdgWrkPtr->host_cmd.queue_num > 0) {
            sdHostCmd2ndBufSend();
        }
        sdMidiServer();
        sdShotServer();
        sdPstmServer();
        sdMemBlkServer();
    }
    set_imask(mask);
}
