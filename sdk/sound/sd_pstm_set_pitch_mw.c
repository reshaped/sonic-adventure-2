#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmSetPitchMw(SDPSTM handle, const Sint8 target_slot,
                         const Sint16 pitch, const Sint32 fade_time)
{
    return sdPstmSetPitch(handle, target_slot, pitch, fade_time);
}
