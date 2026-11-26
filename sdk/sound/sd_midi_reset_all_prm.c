#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd midi_reset_all_prm_cmd;

SDE_ERR sdMidiResetAllPrm(void)
{
    SDE_ERR result;
    Sint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        midi_reset_all_prm_cmd.cmd = SD_HOST_CMD_MIDI_RESET_PRM;
        SD_HOST_CMD_PRM(midi_reset_all_prm_cmd, 0) = SD_PORT_ALL;
        result = sdHostCmdSet(&midi_reset_all_prm_cmd);
        if (result == SDE_ERR_NOTHING) {
            for (i = 0; i < SDD_MIDI_PORT_NUM; i++) {
                if (sdgWrkPtr->modules->by_kind.midi[i].opened == 1) {
                    result =
                        sdMidiResetPrmWrk(&sdgWrkPtr->modules->by_kind.midi[i]);
                    /* @bug logic: resets the levels of only the first open
                     * port: the loop ends once a reset succeeds (BT at
                     * 0x8c12ef68) */
                    if (result == SDE_ERR_NOTHING) {
                        break;
                    }
                }
            }
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiResetAllPrm, result);
    }
    return result;
}
