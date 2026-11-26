#include <sd_work.h>
#include <sg_sd.h>

static struct sd_port_levels midi_reset_levels = {0};

SDE_ERR sdMidiResetPrmWrk(struct sd_port *port)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else if (port == NULL) {
        result = SDE_ERR_HANDLE_NULL;
    } else {
        port->target = midi_reset_levels;
        port->cur = midi_reset_levels;
        port->step = midi_reset_levels;
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiResetPrmWrk, result);
    }
    return result;
}
