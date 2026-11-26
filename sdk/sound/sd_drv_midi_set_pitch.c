#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd midi_set_pitch_cmd;

SDE_ERR sdDrvMidiSetPitch(SDMIDI handle, Sint32 pitch)
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
    /* @hack type: the cast of pitch to Sint16, where pitch alone would do:
     * the command takes only the sum's low two bytes */
    biased = (Sint16)pitch + SD_PITCH_BIAS;
    midi_set_pitch_cmd.cmd = SD_HOST_CMD_MIDI_PITCH;
    SD_HOST_CMD_PRM(midi_set_pitch_cmd, 0) =
        ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(midi_set_pitch_cmd, 2) = biased;
    SD_HOST_CMD_PRM(midi_set_pitch_cmd, 3) = biased >> 8;
    result = sdHostCmdSet(&midi_set_pitch_cmd);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvMidiSetPitch, result);
    }
    return result;
}
