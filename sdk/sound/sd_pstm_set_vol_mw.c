#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmSetVolMw(SDPSTM handle, const Sint8 target_slot, const Sint8 vol,
                       const Sint32 fade_time)
{
    return sdPstmSetVol(handle, target_slot, vol, fade_time);
}
