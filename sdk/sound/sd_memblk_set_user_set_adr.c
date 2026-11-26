#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdMemBlkSetUserSetAdr(SDMEMBLK handle, const void *ptr)
{
    SDE_ERR result;

    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        return result;
    }
    ((struct sd_memblk *)handle)->set_ptr = ptr;
    return result;
}
