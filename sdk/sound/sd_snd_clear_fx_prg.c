#include <sd_work.h>
#include <sg_sd.h>

static struct sd_host_cmd snd_clear_fx_prg_cmd;

SDE_ERR sdSndClearFxPrg(void)
{
    SDE_ERR result;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    snd_clear_fx_prg_cmd.cmd = SD_HOST_CMD_CLEAR_FX_PRG;
    result = sdHostCmdSet(&snd_clear_fx_prg_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSndClearFxPrg, result);
    }
    return result;
}
