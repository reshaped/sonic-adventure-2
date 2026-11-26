#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMidiGetVol(SDMIDI handle, Sint8 *vol)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 address;
    Sint8 value;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (vol == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    /* @hack spelling: address, a local for the read's first argument, where
     * the expression in the call would do */
    address = (Sint32)&SD_SND_MIDI_STATUS(handle)->vol;
    sdSndMemReadSint8(address, &value);
    *vol = value - SD_LEVEL_BIAS;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiGetVol, result);
    }
    return result;
}
