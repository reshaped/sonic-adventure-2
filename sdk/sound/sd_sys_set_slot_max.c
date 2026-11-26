#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd sys_set_slot_max_cmd;

SDE_ERR sdSysSetSlotMax(Sint32 midi_slot_max, Sint32 shot_slot_max,
                        Sint32 pstm_slot_max)
{
    SDE_ERR result;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (midi_slot_max < 0 || midi_slot_max > SDD_MIDI_SLOT_NUM ||
        shot_slot_max < 0 || shot_slot_max > SDD_SHOT_SLOT_NUM ||
        pstm_slot_max < 0 || pstm_slot_max > SDD_PSTM_SLOT_NUM ||
        midi_slot_max + shot_slot_max + pstm_slot_max > SD_SLOT_NUM) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    sys_set_slot_max_cmd.cmd = SD_HOST_CMD_SET_SLOT_MAX;
    SD_HOST_CMD_PRM(sys_set_slot_max_cmd, 0) = midi_slot_max;
    SD_HOST_CMD_PRM(sys_set_slot_max_cmd, 1) = shot_slot_max;
    SD_HOST_CMD_PRM(sys_set_slot_max_cmd, 2) = pstm_slot_max;
    result = sdHostCmdSet(&sys_set_slot_max_cmd);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    sdgWrkPtr->midi_slot_max = midi_slot_max;
    sdgWrkPtr->shot_slot_max = shot_slot_max;
    sdgWrkPtr->pstm_slot_max = pstm_slot_max;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSysSetSlotMax, result);
    }
    return result;
}
