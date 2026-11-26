#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd pstm_set_pitch_cmd;

SDE_ERR sdDrvPstmSetPitch(SDPSTM handle, Sint8 ch, Sint16 pitch)
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
    biased = pitch + SD_PITCH_BIAS;
    for (i = 0; i < ext->slot_num; i++) {
        if (ch == i || ch == SDD_PSTM_PORT_SLOT_ALL) {
            SD_HOST_CMD_NO(pstm_set_pitch_cmd) = SD_HOST_CMD_PSTM_PITCH;
            SD_HOST_CMD_PRM(pstm_set_pitch_cmd, 0) = ext->slot_no[i];
            SD_HOST_CMD_PRM(pstm_set_pitch_cmd, 2) = biased;
            SD_HOST_CMD_PRM(pstm_set_pitch_cmd, 3) = biased >> 8;
            result = sdHostCmdSet(&pstm_set_pitch_cmd);
            if (result != SDE_ERR_NOTHING) {
                break;
            }
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        /* @bug logic: passes sdPstmSetPitch's address to sdSysErrTrap, where
         * its own belongs (the pool word at 0x8c175288) */
        sdSysErrTrap((void *)sdPstmSetPitch, result);
    }
    return result;
}
