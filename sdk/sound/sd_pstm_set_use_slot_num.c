#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the count of slots. the only commands sdPstmSetUseSlotNum
 * queues are sdPstmResetPrm's */
static struct sd_host_cmd pstm_set_use_slot_num_cmd;

SDE_ERR sdPstmSetUseSlotNum(SDPSTM handle, const Sint8 use_slot_num)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 port;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (use_slot_num < 1 || use_slot_num > SDD_PSTM_PORT_SLOT_NUM) {
        result = SDE_ERR_PRM_OVER_RANGE;
    } else {
        /* @hack spelling: port, a local for the port's number, where
         * ((struct sd_port_head *)handle)->port as the index would do */
        port = ((struct sd_port_head *)handle)->port;
        ext = &sdgWrkPtr->pstm_ext[port];
        /* @hack redundant: a second test of use_slot_num for a negative,
         * where ext->slot_max < use_slot_num alone would do: use_slot_num
         * is 1 or more here */
        if (use_slot_num < 0 || ext->slot_max < use_slot_num) {
            result = SDE_ERR_PRM_OVER_RANGE;
        } else {
            ext->slot_num = use_slot_num;
            sdPstmResetPrm(handle, SDD_PSTM_PORT_SLOT_ALL);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmSetUseSlotNum, result);
    }
    return result;
}
