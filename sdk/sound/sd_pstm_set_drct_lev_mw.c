#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmSetDrctLevMw(SDPSTM handle, const Sint8 target_slot,
                           const Sint8 drct_lev)
{
    return sdPstmSetDrctLev(handle, target_slot, drct_lev);
}
