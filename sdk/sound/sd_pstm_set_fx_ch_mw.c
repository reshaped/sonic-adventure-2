#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmSetFxChMw(SDPSTM handle, const Sint8 target_slot,
                        const Sint8 fx_in_ch, const Sint8 fx_base_lev)
{
    return sdPstmSetFxCh(handle, target_slot, fx_in_ch, fx_base_lev);
}
