#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMidiGetFlg(SDMIDI handle, Uint32 *flg)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_port *port = (struct sd_port *)handle;
    Sint32 address;
    Uint8 value;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (flg == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    /* @hack spelling: address, a local for the read's first argument, where
     * the expression in the call would do */
    address = (Sint32)&SD_SND_MIDI_STATUS(handle)->flg;
    sdSndMemReadUint8(address, &value);
    *flg = value;
    if (port->step.vol != 0) {
        *flg |= SDD_PORT_FLG_CHG_VOL;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_VOL;
    }
    if (port->step.pan != 0) {
        *flg |= SDD_PORT_FLG_CHG_PAN;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_PAN;
    }
    if (port->step.speed != 0) {
        *flg |= SDD_PORT_FLG_CHG_SPEED;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_SPEED;
    }
    if (port->step.pitch != 0) {
        *flg |= SDD_PORT_FLG_CHG_PITCH;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_PITCH;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiGetFlg, result);
    }
    return result;
}
