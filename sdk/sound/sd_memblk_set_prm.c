#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkSetPrm(SDMEMBLK handle, const void *src_blk_ptr,
                       const Sint32 src_blk_sz,
                       const SD_MEMBLK_CALLBACK_FUNC callback_func,
                       const void *callback_first_arg)
{
    SDE_ERR result;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    result = sdMemBlkSetRealUsePrm(handle, (void *)src_blk_ptr, src_blk_sz,
                                   callback_func, (void *)callback_first_arg);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    result = sdMemBlkSetUserSetPrm(handle, src_blk_ptr, src_blk_sz,
                                   callback_func, callback_first_arg);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkSetPrm, result);
    }
    return result;
}
