#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMidiGetPitch(SDMIDI handle, Sint16 *pitch)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 address;
    Sint32 value;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (pitch == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    /* @hack spelling: address, a local for the read's first argument, where
     * the expression in the call would do */
    address = (Sint32)&SD_SND_MIDI_STATUS(handle)->pitch;
    sdSndMemReadSint32(address, &value);
    *pitch = value - SD_PITCH_BIAS;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiGetPitch, result);
    }
    /* @bug logic: returns SDE_ERR_NOTHING after a failure too (MOV #0,R0 at
     * 0x8c154ae0). only sdSysErrTrap learns of the failure */
    return SDE_ERR_NOTHING;
}
