#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the volume. sdDrvMidiSetVol queues the volume with one of
 * its own */
static struct sd_host_cmd midi_set_vol_cmd;

SDE_ERR sdMidiSetVol(SDMIDI handle, const Sint8 vol, const Sint32 fade_time)
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
    port->target.vol = vol * SD_LEVEL_UNIT;
    if (fade_time == 0) {
        port->cur.vol = port->target.vol;
        port->step.vol = 0;
    } else if (fade_time < 0) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    } else {
        sdUtilCalculationFade(port->cur.vol, port->target.vol, fade_time,
                              &port->step.vol);
        if (port->step.vol != 0) {
            port->cur.vol += port->step.vol;
        } else {
            port->cur.vol = port->target.vol;
        }
    }
    result = sdDrvMidiSetVol(handle, port->cur.vol / SD_LEVEL_UNIT);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiSetVol, result);
    }
    return result;
}
