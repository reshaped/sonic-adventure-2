#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the pan. sdDrvMidiSetPan queues the pan with one of its
 * own */
static struct sd_host_cmd midi_set_pan_cmd;

SDE_ERR sdMidiSetPan(SDMIDI handle, const Sint8 pan, const Sint32 fade_time)
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
    port->target.pan = pan * SD_LEVEL_UNIT;
    if (fade_time == 0) {
        port->cur.pan = port->target.pan;
        port->step.pan = 0;
    } else if (fade_time < 0) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    } else {
        sdUtilCalculationFade(port->cur.pan, port->target.pan, fade_time,
                              &port->step.pan);
        if (port->step.pan != 0) {
            port->cur.pan += port->step.pan;
        } else {
            port->cur.pan = port->target.pan;
        }
    }
    result = sdDrvMidiSetPan(handle, port->cur.pan / SD_LEVEL_UNIT);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiSetPan, result);
    }
    return result;
}
