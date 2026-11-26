#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkSetUserSetPrm(SDMEMBLK handle, const void *ptr, Sint32 sz,
                              SD_MEMBLK_CALLBACK_FUNC callback_func,
                              const void *callback_first_arg)
{
    SDE_ERR result;
    SDE_MEMBLK_STAT stat;

    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    sdMemBlkGetStat(handle, &stat);
    if (stat == SDE_MEMBLK_STAT_TRANSFER_PROGRESS) {
        result = SDE_ERR_MEMBLK_ALREADY_TRANSFER;
        goto exit;
    }
    sdMemBlkSetUserSetAdr(handle, ptr);
    sdMemBlkSetUserSetSz(handle, sz);
    sdMemBlkSetUserSetCallback(handle, callback_func, callback_first_arg);
    sdMemBlkSetStat(handle, SDE_MEMBLK_STAT_TRANSFER_REMAIN);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkSetUserSetPrm, result);
    }
    return result;
}
