#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd shot_set_pitch_cmd;

SDE_ERR sdDrvShotSetPitch(SDSHOT handle, Sint16 pitch)
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
    biased = pitch + SD_PITCH_BIAS;
    shot_set_pitch_cmd.cmd = SD_HOST_CMD_SHOT_PITCH;
    SD_HOST_CMD_PRM(shot_set_pitch_cmd, 0) =
        ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(shot_set_pitch_cmd, 2) = biased;
    SD_HOST_CMD_PRM(shot_set_pitch_cmd, 3) = biased >> 8;
    result = sdHostCmdSet(&shot_set_pitch_cmd);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvShotSetPitch, result);
    }
    return result;
}
