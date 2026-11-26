#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdShotServer(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_port *port;
    Sint32 prev;
    Sint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        for (i = 0; i < SD_SHOT_PORT_NUM; i++) {
            /* @hack spelling: shot[i] indexed for the test of opened and
             * again to set port, where port set first and port->opened
             * tested would do */
            if (sdgWrkPtr->modules->by_kind.shot[i].opened != 1) {
                continue;
            }
            port = &sdgWrkPtr->modules->by_kind.shot[i];
            SD_STEP_LEVEL(port, prev, vol, sdDrvShotSetVol, SDSHOT);
            SD_STEP_LEVEL(port, prev, pan, sdDrvShotSetPan, SDSHOT);
            SD_STEP_LEVEL(port, prev, pitch, sdDrvShotSetPitch, SDSHOT);
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdShotServer, result);
    }
    return result;
}
