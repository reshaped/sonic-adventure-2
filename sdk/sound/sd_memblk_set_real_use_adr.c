#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdMemBlkSetRealUseAdr(SDMEMBLK handle, void *ptr)
{
    SDE_ERR result;

    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        return result;
    }
    ((struct sd_memblk *)handle)->ptr = ptr;
    return result;
}
