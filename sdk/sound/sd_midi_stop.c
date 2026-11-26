#include <sd_work.h>
#include <sg_sd.h>

static struct sd_host_cmd midi_stop_cmd;

SDE_ERR sdMidiStop(SDMIDI handle)
{
    struct sd_port *port = (struct sd_port *)handle;
    SDE_ERR result;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    midi_stop_cmd.cmd = SD_HOST_CMD_MIDI_STOP;
    /* @hack spelling: the port's number read through the handle, where
     * port->head.port would do */
    SD_HOST_CMD_PRM(midi_stop_cmd, 0) = ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(midi_stop_cmd, 1) = port->unk_68 == 0 ? 0 : 1;
    result = sdHostCmdSet(&midi_stop_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiStop, result);
    }
    return result;
}
