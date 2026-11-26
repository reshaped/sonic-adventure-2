#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_host_cmd snd_set_fx_prg_cmd;

SDE_ERR sdSndSetFxPrg(const Sint8 new_fx_prg_num, const Sint8 new_fx_out_num)
{
    SDE_ERR result;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (new_fx_prg_num < 0 || new_fx_out_num < 0) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    snd_set_fx_prg_cmd.cmd = SD_HOST_CMD_SET_FX_PRG;
    SD_HOST_CMD_PRM(snd_set_fx_prg_cmd, 0) = new_fx_prg_num;
    result = sdHostCmdSet(&snd_set_fx_prg_cmd);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    snd_set_fx_prg_cmd.cmd = SD_HOST_CMD_SET_FX_OUT;
    SD_HOST_CMD_PRM(snd_set_fx_prg_cmd, 0) = new_fx_out_num;
    result = sdHostCmdSet(&snd_set_fx_prg_cmd);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSndSetFxPrg, result);
    }
    return result;
}
