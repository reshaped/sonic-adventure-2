#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd pstm_set_vol_cmd;

SDE_ERR sdDrvPstmSetVol(SDPSTM handle, Sint8 ch, Sint8 vol)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 port;
    Sint32 i;
    Sint32 biased;

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
    biased = vol + SD_LEVEL_BIAS;
    for (i = 0; i < ext->slot_num; i++) {
        if (ch == i || ch == SDD_PSTM_PORT_SLOT_ALL) {
            SD_HOST_CMD_NO(pstm_set_vol_cmd) = SD_HOST_CMD_PSTM_VOL;
            SD_HOST_CMD_PRM(pstm_set_vol_cmd, 0) = ext->slot_no[i];
            SD_HOST_CMD_PRM(pstm_set_vol_cmd, 1) = biased;
            result = sdHostCmdSet(&pstm_set_vol_cmd);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        /* @bug logic: passes sdPstmSetVol's address to sdSysErrTrap, where
         * its own belongs (the pool word at 0x8c175348) */
        sdSysErrTrap((void *)sdPstmSetVol, result);
    }
    return result;
}
