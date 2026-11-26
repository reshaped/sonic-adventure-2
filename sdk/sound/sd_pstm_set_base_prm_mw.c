#include <sd_mw.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the call takes a pan around 0 at twice a base pan's scale, which runs
 * around SD_PSTM_BASE_PAN_CENTER */
#define PAN_SCALE 2

SDE_ERR sdPstmSetBasePrmMw(SDPSTM handle, const Sint8 target_slot,
                           const Sint8 base_vol, const Sint8 base_pan,
                           const Sint8 fx_in_ch, const Sint8 base_fx_lev,
                           const Sint8 base_drct_lev)
{
    /* @hack redundant: pan declared register, where a plain local would do */
    /* @hack spelling: pan, a local, where base_pan / PAN_SCALE +
     * SD_PSTM_BASE_PAN_CENTER in the call would do */
    register Sint8 pan = (base_pan / PAN_SCALE) + SD_PSTM_BASE_PAN_CENTER;

    return sdPstmSetBasePrm(handle, target_slot, base_vol, pan, fx_in_ch,
                            base_fx_lev, base_drct_lev,
                            SDD_PSTM_PORT_SLOT_FILTER_THROUGH, 0);
}
