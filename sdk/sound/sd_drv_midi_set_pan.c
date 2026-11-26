#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd midi_set_pan_cmd;

SDE_ERR sdDrvMidiSetPan(SDMIDI handle, Sint32 pan)
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
    /* @hack type: the cast of pan to Sint8, where pan alone would do: the
     * command takes only the sum's low byte */
    biased = (Sint8)pan + SD_LEVEL_BIAS;
    midi_set_pan_cmd.cmd = SD_HOST_CMD_MIDI_PAN;
    SD_HOST_CMD_PRM(midi_set_pan_cmd, 0) =
        ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(midi_set_pan_cmd, 1) = biased;
    result = sdHostCmdSet(&midi_set_pan_cmd);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvMidiSetPan, result);
    }
    return result;
}
