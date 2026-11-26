#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMidiServer(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_port *port;
    Sint32 prev;
    Sint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        for (i = 0; i < SDD_MIDI_PORT_NUM; i++) {
            /* @hack spelling: midi[i] indexed for the test of opened and again
             * to set port, where port set first and port->opened tested would
             * do */
            if (sdgWrkPtr->modules->by_kind.midi[i].opened != 1) {
                continue;
            }
            port = &sdgWrkPtr->modules->by_kind.midi[i];
            SD_STEP_LEVEL(port, prev, vol, sdDrvMidiSetVol, SDMIDI);
            SD_STEP_LEVEL(port, prev, pan, sdDrvMidiSetPan, SDMIDI);
            SD_STEP_LEVEL(port, prev, speed, sdDrvMidiSetSpeed, SDMIDI);
            SD_STEP_LEVEL(port, prev, pitch, sdDrvMidiSetPitch, SDMIDI);
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiServer, result);
    }
    return result;
}
