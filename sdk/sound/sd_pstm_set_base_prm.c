#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* how many low bits of base_filter_lev a channel's starting filter level
 * drops */
#define FILTER_LEV_SHIFT 2

SDE_ERR sdPstmSetBasePrm(SDPSTM handle, const Sint8 target_slot,
                         const Sint8 base_vol, const Sint8 base_pan,
                         const Sint8 fx_in_ch, const Sint8 base_fx_lev,
                         const Sint8 base_drct_lev,
                         const Sint16 base_filter_lev,
                         const Sint8 base_filter_q)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 i;
    Sint32 port;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    if (target_slot != SDD_PSTM_PORT_SLOT_ALL &&
        (target_slot < 0 || ext->slot_num <= target_slot)) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    for (i = 0; i < SDD_PSTM_PORT_SLOT_NUM; i++) {
        if (target_slot == i || target_slot == SDD_PSTM_PORT_SLOT_ALL) {
            ext->base[i].pan = base_pan;
            ext->base[i].vol = base_vol;
            ext->base[i].fx_ch = fx_in_ch;
            ext->base[i].fx_lev = base_fx_lev;
            ext->base[i].drct_lev = base_drct_lev;
            if (base_filter_lev == SDD_PSTM_PORT_SLOT_FILTER_THROUGH) {
                ext->base_filter_lev[i] = SDD_PSTM_PORT_SLOT_FILTER_THROUGH;
            } else {
                ext->base_filter_lev[i] = base_filter_lev >> FILTER_LEV_SHIFT;
            }
            ext->base_filter_q[i] = base_filter_q;
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmSetBasePrm, result);
    }
    return result;
}
