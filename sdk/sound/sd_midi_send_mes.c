#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd midi_send_mes_cmd;

SDE_ERR sdMidiSendMes(SDMIDI handle, const SDS_MIDI_MES *midi_mes_ptr,
                      const Sint8 priority)
{
    SDE_ERR result;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (priority < 0 || priority > SD_MIDI_PRIORITY_MAX) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    midi_send_mes_cmd.cmd = SD_HOST_CMD_MIDI_SEND_MES;
    SD_HOST_CMD_PRM(midi_send_mes_cmd, 2) = midi_mes_ptr->m_Member[0];
    SD_HOST_CMD_PRM(midi_send_mes_cmd, 3) = midi_mes_ptr->m_Member[1];
    SD_HOST_CMD_PRM(midi_send_mes_cmd, 4) = midi_mes_ptr->m_Member[2];
    SD_HOST_CMD_PRM(midi_send_mes_cmd, 5) =
        midi_mes_ptr->m_Member[3] | (priority << SD_MIDI_PRIORITY_SHIFT);
    result = sdHostCmdSet(&midi_send_mes_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiSendMes, result);
    }
    return result;
}
