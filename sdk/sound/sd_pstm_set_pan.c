#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the pan. sdDrvPstmSetPan queues each channel's pan with
 * one of its own */
static struct sd_host_cmd pstm_set_pan_cmd;

SDE_ERR sdPstmSetPan(SDPSTM handle, const Sint8 target_slot, const Sint8 pan,
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
            ext->target[i].pan = pan * SD_LEVEL_UNIT;
            if (fade_time == 0) {
                ext->cur[i].pan = ext->target[i].pan;
                ext->step[i].pan = 0;
            } else if (fade_time < 0) {
                result = SDE_ERR_PRM_OVER_RANGE;
                goto exit;
            } else {
                sdUtilCalculationFade(ext->cur[i].pan, ext->target[i].pan,
                                      fade_time, &ext->step[i].pan);
                if (ext->step[i].pan != 0) {
                    ext->cur[i].pan += ext->step[i].pan;
                } else {
                    ext->cur[i].pan = ext->target[i].pan;
                }
            }
            result =
                sdDrvPstmSetPan(handle, i, ext->cur[i].pan / SD_LEVEL_UNIT);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmSetPan, result);
    }
    return result;
}
