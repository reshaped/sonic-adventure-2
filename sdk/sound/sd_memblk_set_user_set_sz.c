#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkSetUserSetSz(SDMEMBLK handle, Sint32 sz)
{
    SDE_ERR result;

    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        return result;
    }
    ((struct sd_memblk *)handle)->set_size = sz;
    return result;
}
