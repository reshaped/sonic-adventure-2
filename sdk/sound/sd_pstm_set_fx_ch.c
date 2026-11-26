#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the fx_in_ch that names no effect channel, and the command's level byte that
 * feeds none */
#define FX_IN_CH_NONE (-1)
#define FX_LEV_NONE 0xff

static struct sd_host_cmd pstm_set_fx_ch_cmd;

SDE_ERR sdPstmSetFxCh(SDPSTM handle, const Sint8 target_slot,
                      const Sint8 fx_in_ch, const Sint8 fx_base_lev)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 fx_ch;
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
    fx_ch = fx_in_ch;
    lev = fx_base_lev + SD_LEVEL_BIAS;
    /* a fx_base_lev of 127 makes FX_LEV_NONE too, so that level goes as 126 */
    if (lev == FX_LEV_NONE) {
        lev = FX_LEV_NONE - 1;
    }
    if (fx_in_ch == FX_IN_CH_NONE) {
        fx_ch = 0;
        lev = FX_LEV_NONE;
    }
    for (i = 0; i < ext->slot_num; i++) {
        if (target_slot == i || target_slot == SDD_PSTM_PORT_SLOT_ALL) {
            SD_HOST_CMD_NO(pstm_set_fx_ch_cmd) = SD_HOST_CMD_PSTM_FX_CH;
            SD_HOST_CMD_PRM(pstm_set_fx_ch_cmd, 0) = ext->slot_no[i];
            SD_HOST_CMD_PRM(pstm_set_fx_ch_cmd, 1) = fx_ch;
            SD_HOST_CMD_PRM(pstm_set_fx_ch_cmd, 2) = lev;
            result = sdHostCmdSet(&pstm_set_fx_ch_cmd);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmSetFxCh, result);
    }
    return result;
}
