#include <machine.h>
#include <sd_work.h>
#include <sg_chain.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

/* sdSysVsyncServer's priority among the vertical blank's handlers */
#define VSYNC_PRIORITY 96

SDE_ERR sdLibInit(void *wrk_ptr, Sint32 mem_blk_handle_max,
                  Sint32 second_host_cmd_max)
{
    SDE_ERR result;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr != NULL) {
        result = SDE_ERR_ALREADY_INIT;
    } else {
        result = sdSysWrkInit(SDD_MEMBLK_HANDLE_NUM, SDD_MEMBLK_QUEUE_NUM,
                              SD_HOST_CMD_QUEUE_NUM);
        sdgWrkPtr->vsync_chain =
            syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK, sdSysVsyncServer,
                              VSYNC_PRIORITY, NULL);
        /* @hack redundant: a call of strlen whose result nothing takes */
        strlen(sdgBuild);
    }
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdLibInit, result);
    }
    return result;
}
