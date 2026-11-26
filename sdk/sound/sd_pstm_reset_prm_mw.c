#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmResetPrmMw(SDPSTM handle, const Sint8 target_slot)
{
    return sdPstmResetPrm(handle, target_slot);
}
