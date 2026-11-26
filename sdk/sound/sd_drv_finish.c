#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdDrvFinish(void)
{
    /* @hack spelling: the command as a static inside the function, where one
     * at file scope would do */
    static struct sd_host_cmd drv_finish_cmd;
    SDE_ERR result;
    Sint32 mask;
    Sint32 num;
    Uint32 stat;
    Uint8 batch_waits;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    do {
        sdMemBlkQueueGetPushNum(&num);
        sdSysServer();
    } while (num > 0);
    result = sdDrvCheckExecute();
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    sdSndStopAll();
    sdSndClearFxPrg();
    do {
        result = sdSysFlushHostCmd();
        sdSysServer();
    } while (result != SDE_ERR_NOTHING);
    do {
        sdSndMemReadUint8(sdgWrkPtr->stat_adr, &batch_waits);
        sdSysServer();
    } while (batch_waits != 0);
    while (sdgWrkPtr->blocks.in_transfer != NULL) {
        sdSysServer();
    }
    drv_finish_cmd.cmd = SD_HOST_CMD_DRV_FINISH;
    drv_finish_cmd.prm[2] = 'S';
    drv_finish_cmd.prm[3] = 'E';
    drv_finish_cmd.prm[4] = 'G';
    drv_finish_cmd.prm[5] = 'A';
    result = sdHostCmdSet(&drv_finish_cmd);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    do {
        result = sdSysFlushHostCmd();
        sdSysServer();
    } while (result != SDE_ERR_NOTHING);
    while (sdgWrkPtr->blocks.in_transfer != NULL) {
        sdSysServer();
    }
    do {
        sdSndMemReadUint8(sdgWrkPtr->stat_adr, &batch_waits);
        sdSysServer();
    } while (batch_waits != 0);
    do {
        sdSndMemReadUint32(SD_DRV_INFO_ADR(sdgWrkPtr->drv_info_adr, stat),
                           &stat);
        sdSysServer();
    } while (stat != SD_DRV_STAT_FINISHED);
    sdgExistSndDrv = FALSE;
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvFinish, result);
    }
    return result;
}
