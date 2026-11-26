#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmGetFlg(SDPSTM handle, Sint8 ch, Uint32 *flg)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 port;
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
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    if (ch < 0 || ext->slot_num <= ch) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    /* @hack spelling: address, a local for the read's first argument, where
     * the expression in the call would do */
    address = (Sint32)&SD_SND_SLOT_STATUS(ext->slot_no[ch])->flg;
    sdSndMemReadUint8(address, &value);
    *flg = value;
    if (ext->step[ch].vol) {
        *flg |= SDD_PORT_FLG_CHG_VOL;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_VOL;
    }
    if (ext->step[ch].pan) {
        *flg |= SDD_PORT_FLG_CHG_PAN;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_PAN;
    }
    if (ext->step[ch].speed) {
        *flg |= SDD_PORT_FLG_CHG_SPEED;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_SPEED;
    }
    if (ext->step[ch].pitch) {
        *flg |= SDD_PORT_FLG_CHG_PITCH;
    } else {
        *flg &= ~SDD_PORT_FLG_CHG_PITCH;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmGetFlg, result);
    }
    return result;
}
