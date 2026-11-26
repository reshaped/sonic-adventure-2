#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmStop(SDPSTM handle)
{
    /* @hack spelling: the command as a static inside the function, where one
     * at file scope would do */
    static struct sd_host_cmd pstm_stop_cmd;
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint8 port;
    Sint32 slot_num;
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
    /* @hack spelling: slot_num, a local for the count of channels, where
     * ext->slot_num in the loop's test would do */
    slot_num = ext->slot_num;
    for (i = 0; i < slot_num; i++) {
        pstm_stop_cmd.cmd = SD_HOST_CMD_PSTM_STOP;
        pstm_stop_cmd.prm[0] = ext->slot_no[i];
        result = sdHostCmdSet(&pstm_stop_cmd);
        if (result != SDE_ERR_NOTHING) {
            goto exit;
        }
    }
    sdPstmStopWrk(handle);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmStop, result);
    }
    return result;
}
