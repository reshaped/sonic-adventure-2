#include <machine.h>
#include <sd_work.h>
#include <sg_chain.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <string.h>

SDE_ERR sdSysFinish(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        if (sdgExistSndDrv == TRUE) {
            sdDrvFinish();
        }
        sdArm7ResetKeep();
        sdSndMemClear();
        syChainDeleteHandler(sdgWrkPtr->vsync_chain);
        syG2DmaClose(sdgWrkPtr->blocks.dma);
        memset(sdgWrkPtr, SD_UNSET_FILL, sizeof(struct sd_work));
        sdgWrkPtr = NULL;
        sdgExistSndDrv = FALSE;
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSysFinish, result);
    }
    set_imask(mask);
    return result;
}
