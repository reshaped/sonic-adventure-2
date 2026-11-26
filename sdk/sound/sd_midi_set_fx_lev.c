#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd midi_set_fx_lev_cmd;

SDE_ERR sdMidiSetFxLev(SDMIDI handle, const Sint8 fx_lev)
{
    struct sd_port *port = (struct sd_port *)handle;
    SDE_ERR result;
    Sint32 lev;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: lev, a local for the sum, set before the command's
     * number, where the sum in its store would do */
    lev = fx_lev + SD_LEVEL_BIAS;
    midi_set_fx_lev_cmd.cmd = SD_HOST_CMD_MIDI_SET_FX_LEV;
    /* @hack spelling: the port's number read through the handle, where
     * port->head.port would do */
    SD_HOST_CMD_PRM(midi_set_fx_lev_cmd, 0) =
        ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(midi_set_fx_lev_cmd, 1) = lev;
    result = sdHostCmdSet(&midi_set_fx_lev_cmd);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    port->target.fx_lev = fx_lev;
    port->cur.fx_lev = fx_lev;
    port->step.fx_lev = 0;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiSetFxLev, result);
    }
    return result;
}
