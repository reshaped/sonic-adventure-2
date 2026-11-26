#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdMemBlkSetRealUseCallback(SDMEMBLK handle,
                                   SD_MEMBLK_CALLBACK_FUNC callback_func,
                                   void *callback_first_arg)
{
    SDE_ERR result;

    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        return result;
    }
    ((struct sd_memblk *)handle)->callback = callback_func;
    ((struct sd_memblk *)handle)->arg = callback_first_arg;
    return result;
}
