#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the volume. sdDrvPstmSetVol queues each channel's volume
 * with one of its own */
static struct sd_host_cmd pstm_set_vol_cmd;

SDE_ERR sdPstmSetVol(SDPSTM handle, const Sint8 target_slot, const Sint8 vol,
                     const Sint32 fade_time)
{
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
    if (target_slot != SDD_PSTM_PORT_SLOT_ALL &&
        (target_slot < 0 || ext->slot_num <= target_slot)) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    for (i = 0; i < SDD_PSTM_PORT_SLOT_NUM; i++) {
        if (target_slot == i || target_slot == SDD_PSTM_PORT_SLOT_ALL) {
            ext->target[i].vol = vol * SD_LEVEL_UNIT;
            if (fade_time == 0) {
                ext->cur[i].vol = ext->target[i].vol;
                ext->step[i].vol = 0;
            } else if (fade_time < 0) {
                result = SDE_ERR_PRM_OVER_RANGE;
                goto exit;
            } else {
                sdUtilCalculationFade(ext->cur[i].vol, ext->target[i].vol,
                                      fade_time, &ext->step[i].vol);
                if (ext->step[i].vol != 0) {
                    ext->cur[i].vol += ext->step[i].vol;
                } else {
                    ext->cur[i].vol = ext->target[i].vol;
                }
            }
            result =
                sdDrvPstmSetVol(handle, i, ext->cur[i].vol / SD_LEVEL_UNIT);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmSetVol, result);
    }
    return result;
}
