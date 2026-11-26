#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a driver is old where its major version is OLD_VER_MAJOR, its minor
 * version at most OLD_VER_MINOR_MAX and its local version at most
 * OLD_VER_LOCAL_MAX */
#define OLD_VER_MAJOR 2
#define OLD_VER_MINOR_MAX 2
#define OLD_VER_LOCAL_MAX 5
/* the err bit in which an old driver reports SDD_PORT_ERR_PRIORITY_LOW */
#define OLD_PRIORITY_LOW ((Uint32)SDD_BIT(1))

SDE_ERR sdMidiGetErr(SDMIDI handle, Uint32 *err)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_snd_midi_status *status;
    Uint16 value;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (handle == (SDMIDI)-1) {
        result = SDE_ERR_HANDLE_ILLEGAL_VALUE;
        goto exit;
    }
    if (err == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    if (err == (Uint32 *)-1) {
        result = SDE_ERR_PTR_ILLEGAL_VALUE;
        goto exit;
    }
    /* @hack redundant: a store of 0 to *err, which the value read then
     * overwrites */
    *err = 0;
    /* @hack spelling: status, a local for the port's record, where
     * SD_SND_MIDI_STATUS(handle)->err in the call would do */
    status = SD_SND_MIDI_STATUS(handle);
    sdSndMemReadUint16((Sint32)&status->err, &value);
    *err = value;
    /* @hack spelling: the local version tested in an if of its own, where &&
     * in the test above would do */
    if (sdgWrkPtr->ver_major == OLD_VER_MAJOR &&
        sdgWrkPtr->ver_minor <= OLD_VER_MINOR_MAX) {
        if (sdgWrkPtr->ver_local <= OLD_VER_LOCAL_MAX) {
            *err &= ~SDD_PORT_ERR_PRIORITY_LOW;
            *err |= ((*err & OLD_PRIORITY_LOW) == 0)
                        ? 0
                        : SDD_PORT_ERR_PRIORITY_LOW;
        }
    }
    *err &= ~OLD_PRIORITY_LOW;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiGetErr, result);
    }
    return result;
}
