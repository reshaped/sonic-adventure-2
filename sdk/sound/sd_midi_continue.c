#include <sd_work.h>
#include <sg_sd.h>

static struct sd_host_cmd midi_continue_cmd;

SDE_ERR sdMidiContinue(SDMIDI handle)
{
    SDE_ERR result;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    midi_continue_cmd.cmd = SD_HOST_CMD_MIDI_CONTINUE;
    SD_HOST_CMD_PRM(midi_continue_cmd, 0) =
        ((struct sd_port_head *)handle)->port;
    result = sdHostCmdSet(&midi_continue_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiContinue, result);
    }
    return result;
}
