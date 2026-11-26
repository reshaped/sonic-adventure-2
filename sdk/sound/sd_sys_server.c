#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdSysServer(void)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr != NULL && sdgExistSndDrv == TRUE) {
        sdHostCmdServer();
        sdMemBlkServer();
    }
    set_imask(mask);
    return SDE_ERR_NOTHING;
}
