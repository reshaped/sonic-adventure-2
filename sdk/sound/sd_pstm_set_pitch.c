#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the pitch. sdDrvPstmSetPitch queues each channel's pitch
 * with one of its own */
static struct sd_host_cmd pstm_set_pitch_cmd;

SDE_ERR sdPstmSetPitch(SDPSTM handle, const Sint8 target_slot,
                       const Sint16 pitch, const Sint32 fade_time)
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
    if (fade_time < 0) {
        result = SDE_ERR_PRM_OVER_RANGE;
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
            ext->target[i].pitch = pitch * SD_LEVEL_UNIT;
            if (fade_time == 0) {
                ext->cur[i].pitch = ext->target[i].pitch;
                ext->step[i].pitch = 0;
            } else {
                sdUtilCalculationFade(ext->cur[i].pitch, ext->target[i].pitch,
                                      fade_time, &ext->step[i].pitch);
                if (ext->step[i].pitch != 0) {
                    ext->cur[i].pitch += ext->step[i].pitch;
                } else {
                    ext->cur[i].pitch = ext->target[i].pitch;
                }
            }
            result =
                sdDrvPstmSetPitch(handle, i, ext->cur[i].pitch / SD_LEVEL_UNIT);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmSetPitch, result);
    }
    return result;
}
