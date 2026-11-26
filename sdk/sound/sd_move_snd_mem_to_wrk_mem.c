#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

SDE_ERR sdMoveSndMemToWrkMem(Sint32 snd_mem_adr, void *wrk_mem_ptr, Sint32 size)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (((Uint32)wrk_mem_ptr & (SYE_G2_DATA_WIDTH_32BIT - 1)) ||
        (size & (SYE_G2_DATA_WIDTH_32BIT - 1))) {
        result = SDE_ERR_UNKNOW_NUM;
    } else {
        sdgG2LibResult = syG2Read((void *)(snd_mem_adr + SD_SND_MEM_BASE),
                                  wrk_mem_ptr, SYE_G2_DATA_WIDTH_32BIT,
                                  (size + (SYE_G2_DATA_WIDTH_32BIT - 1)) /
                                      SYE_G2_DATA_WIDTH_32BIT,
                                  SYE_G2_ADR_MODE_ADD, SYE_G2_ADR_MODE_ADD);
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMoveSndMemToWrkMem, result);
    }
    return result;
}
