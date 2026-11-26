#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the pitch. sdDrvMidiSetPitch queues the pitch with one of
 * its own */
static struct sd_host_cmd midi_set_pitch_cmd;

SDE_ERR sdMidiSetPitch(SDMIDI handle, const Sint16 pitch,
                       const Sint32 fade_time)
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
    port->target.pitch = pitch * SD_LEVEL_UNIT;
    if (fade_time == 0) {
        port->cur.pitch = port->target.pitch;
        port->step.pitch = 0;
    } else if (fade_time < 0) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    } else {
        sdUtilCalculationFade(port->cur.pitch, port->target.pitch, fade_time,
                              &port->step.pitch);
        if (port->step.pitch != 0) {
            port->cur.pitch += port->step.pitch;
        } else {
            port->cur.pitch = port->target.pitch;
        }
    }
    result = sdDrvMidiSetPitch(handle, port->cur.pitch / SD_LEVEL_UNIT);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiSetPitch, result);
    }
    return result;
}
