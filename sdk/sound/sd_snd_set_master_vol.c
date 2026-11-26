#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the shift that puts vol in the high four bits of the command's
 * parameter */
#define MASTER_VOL_SHIFT 4

static struct sd_host_cmd snd_set_master_vol_cmd;

SDE_ERR sdSndSetMasterVol(const Sint8 vol)
{
    SDE_ERR result;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    snd_set_master_vol_cmd.cmd = SD_HOST_CMD_SET_MASTER_VOL;
    SD_HOST_CMD_PRM(snd_set_master_vol_cmd, 0) = vol << MASTER_VOL_SHIFT;
    result = sdHostCmdSet(&snd_set_master_vol_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSndSetMasterVol, result);
    }
    return result;
}
