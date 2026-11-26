#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* what the command's first parameter holds for each mode */
#define PRM_MONO 0xff
#define PRM_STEREO 0

static struct sd_host_cmd snd_set_pan_mode_cmd;

SDE_ERR sdSndSetPanMode(const SDE_PAN_MODE pan_mode)
{
    SDE_ERR result;
    Uint8 *prm;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    snd_set_pan_mode_cmd.cmd = SD_HOST_CMD_SET_PAN_MODE;
    /* @hack spelling: prm, a pointer to the parameter taken ahead of the
     * switch, where SD_HOST_CMD_PRM(snd_set_pan_mode_cmd, 0) stored in each
     * case would do */
    prm = &SD_HOST_CMD_PRM(snd_set_pan_mode_cmd, 0);
    switch (pan_mode) {
    case SDE_PAN_MODE_MONO:
        *prm = PRM_MONO;
        break;
    case SDE_PAN_MODE_STEREO:
        *prm = PRM_STEREO;
        break;
    default:
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    result = sdHostCmdSet(&snd_set_pan_mode_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSndSetPanMode, result);
    }
    return result;
}
