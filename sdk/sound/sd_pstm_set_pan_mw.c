#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmSetPanMw(SDPSTM handle, const Sint8 target_slot, const Sint8 pan,
                       const Sint32 fade_time)
{
    return sdPstmSetPan(handle, target_slot, pan, fade_time);
}
