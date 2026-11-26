#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdMidiOpenPort(SDMIDI *handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else if (handle != NULL) {
        result = sdModuleOpenPort(handle, SD_PORT_KIND_MIDI);
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiOpenPort, result);
    }
    return result;
}
