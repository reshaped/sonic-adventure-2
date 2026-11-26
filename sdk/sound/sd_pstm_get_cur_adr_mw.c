#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmGetCurAdrMw(SDPSTM handle, const Sint8 target_slot,
                          Sint32 *cur_smp_adr)
{
    return sdPstmGetCurAdr(handle, target_slot, cur_smp_adr);
}
