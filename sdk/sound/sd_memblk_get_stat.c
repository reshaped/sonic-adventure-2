#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkGetStat(SDMEMBLK handle, SDE_MEMBLK_STAT *stat)
{
    SDE_ERR result;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    if (stat == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    *stat = ((struct sd_memblk *)handle)->stat;
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkGetStat, result);
    }
    return result;
}
