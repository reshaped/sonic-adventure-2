#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmResetPrm(SDPSTM handle, const Sint8 target_slot)
{
    /* @hack spelling: the command as a static inside the function, where one
     * at file scope would do */
    static struct sd_host_cmd pstm_reset_prm_cmd;
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 port;
    Sint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    for (i = 0; i < SDD_PSTM_PORT_SLOT_NUM; i++) {
        if (target_slot == i || target_slot == SDD_PSTM_PORT_SLOT_ALL) {
            if (ext->slot_no[i] < 0) {
                continue;
            }
            pstm_reset_prm_cmd.cmd = SD_HOST_CMD_PSTM_RESET_PRM;
            pstm_reset_prm_cmd.prm[0] = ext->slot_no[i];
            result = sdHostCmdSet(&pstm_reset_prm_cmd);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
            sdPstmResetPrmWrk(handle, i);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmResetPrm, result);
    }
    return result;
}
