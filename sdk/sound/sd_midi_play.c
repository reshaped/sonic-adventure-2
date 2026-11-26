#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd midi_play_cmd;

SDE_ERR sdMidiPlay(SDMIDI handle, const Sint8 bank_num, const Sint8 data_num,
                   const Sint8 priority)
{
    SDE_ERR result;
    Sint32 bank_ptr;
    Sint32 bank_size;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (bank_num < 0 || data_num < 0 || priority < 0 ||
        priority > SD_MIDI_PRIORITY_MAX) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    sdSndMemGetBankStat(SDE_DATA_TYPE_MIDI_SEQ_BANK, bank_num, &bank_ptr,
                        &bank_size);
    /* @bug uninitialized: reads bank_ptr and bank_size unset for a bank_num
     * of SD_BANK_MAX or more (MOV.L @R15,R2 at 0x8c12ee7a). the test above
     * refuses only a negative bank_num. sdSndMemGetBankStat refuses a larger
     * one and stores nothing */
    if (bank_ptr == 0 || bank_size == 0) {
        result = SDE_ERR_BANK_NO_MAPPING;
        goto exit;
    }
    midi_play_cmd.cmd = SD_HOST_CMD_MIDI_PLAY;
    SD_HOST_CMD_PRM(midi_play_cmd, 0) = ((struct sd_port_head *)handle)->port;
    SD_HOST_CMD_PRM(midi_play_cmd, 1) = bank_num;
    SD_HOST_CMD_PRM(midi_play_cmd, 2) = data_num;
    SD_HOST_CMD_PRM(midi_play_cmd, 3) = priority << SD_MIDI_PRIORITY_SHIFT;
    result = sdHostCmdSet(&midi_play_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiPlay, result);
    }
    return result;
}
