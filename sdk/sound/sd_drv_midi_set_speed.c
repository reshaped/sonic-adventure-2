#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the driver's speed is DRV_SPEED_SCALE times the call's */
#define DRV_SPEED_SCALE 4

static struct sd_host_cmd midi_set_speed_cmd;

SDE_ERR sdDrvMidiSetSpeed(SDMIDI handle, Sint16 speed)
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
    biased = (speed * DRV_SPEED_SCALE) + SD_SPEED_BIAS;
    midi_set_speed_cmd.cmd = SD_HOST_CMD_MIDI_SPEED;
    SD_HOST_CMD_PRM(midi_set_speed_cmd, 0) =
        ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(midi_set_speed_cmd, 2) = biased;
    SD_HOST_CMD_PRM(midi_set_speed_cmd, 3) = biased >> 8;
    result = sdHostCmdSet(&midi_set_speed_cmd);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvMidiSetSpeed, result);
    }
    return result;
}
