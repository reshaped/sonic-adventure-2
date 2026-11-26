#include <sd_mw.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmSetUseChMw(SDPSTM handle, const Sint8 use_slot_num)
{
    return sdPstmSetUseSlotNum(handle, use_slot_num);
}
