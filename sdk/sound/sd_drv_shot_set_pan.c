#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd shot_set_pan_cmd;

SDE_ERR sdDrvShotSetPan(SDSHOT handle, Sint8 pan)
{
    SDE_ERR result;
    Sint32 biased;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: biased, a local for the sum, set before the command's
     * number, where the sum in its store would do */
    biased = pan + SD_LEVEL_BIAS;
    shot_set_pan_cmd.cmd = SD_HOST_CMD_SHOT_PAN;
    SD_HOST_CMD_PRM(shot_set_pan_cmd, 0) =
        ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(shot_set_pan_cmd, 1) = biased;
    result = sdHostCmdSet(&shot_set_pan_cmd);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvShotSetPan, result);
    }
    return result;
}
