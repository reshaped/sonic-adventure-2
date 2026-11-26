#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdSndStopAll(void)
{
    /* @hack spelling: the command as a static inside the function, where one
     * at file scope would do */
    static struct sd_host_cmd snd_stop_all_cmd;
    SDE_ERR result;
    Sint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    snd_stop_all_cmd.cmd = SD_HOST_CMD_SND_STOP_ALL;
    snd_stop_all_cmd.prm[0] = 1;
    snd_stop_all_cmd.prm[1] = 1;
    snd_stop_all_cmd.prm[2] = 1;
    snd_stop_all_cmd.prm[3] = 1;
    snd_stop_all_cmd.prm[4] = 0;
    snd_stop_all_cmd.prm[5] = 0;
    result = sdHostCmdSet(&snd_stop_all_cmd);
    if (result == SDE_ERR_NOTHING) {
        for (i = 0; i < SD_PSTM_PORT_NUM; i++) {
            sdPstmStopWrk((SDPSTM)&sdgWrkPtr->modules->by_kind.pstm[i]);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSndStopAll, result);
    }
    return result;
}
