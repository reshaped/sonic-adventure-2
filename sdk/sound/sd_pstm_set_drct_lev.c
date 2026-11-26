#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd pstm_set_drct_lev_cmd;

SDE_ERR sdPstmSetDrctLev(SDPSTM handle, const Sint8 target_slot,
                         const Sint8 drct_lev)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 lev;
    Sint32 i;
    Sint32 port;

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
    if (target_slot != SDD_PSTM_PORT_SLOT_ALL &&
        (target_slot < 0 || ext->slot_num <= target_slot)) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    /* @hack spelling: lev, a local for the sum, set before the loop, where
     * the sum in its store would do */
    lev = drct_lev + SD_LEVEL_BIAS;
    for (i = 0; i < ext->slot_num; i++) {
        if (target_slot == i || target_slot == SDD_PSTM_PORT_SLOT_ALL) {
            SD_HOST_CMD_NO(pstm_set_drct_lev_cmd) = SD_HOST_CMD_PSTM_DRCT_LEV;
            SD_HOST_CMD_PRM(pstm_set_drct_lev_cmd, 0) = ext->slot_no[i];
            SD_HOST_CMD_PRM(pstm_set_drct_lev_cmd, 1) = lev;
            result = sdHostCmdSet(&pstm_set_drct_lev_cmd);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmSetDrctLev, result);
    }
    return result;
}
