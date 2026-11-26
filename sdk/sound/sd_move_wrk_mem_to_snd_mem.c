#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

SDE_ERR sdMoveWrkMemToSndMem(void *wrk_mem_ptr, Sint32 snd_mem_adr, Sint32 size,
                             SD_MEMBLK_CALLBACK_FUNC func, void *arg)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    /* @bug logic: doesn't refuse a copy that runs past sound memory's end.
     * the address and the size are each tested against SD_SND_MEM_SIZE
     * (CMP/GT R2,R12 at 0x8c174f40 and CMP/GT R2,R13 at 0x8c174f4a). their
     * sum is never tested */
    /* @hack spelling: three ifs that each set SDE_ERR_PRM_OVER_RANGE, where
     * one if over their tests joined by || would do */
    if (snd_mem_adr < 0 || snd_mem_adr > SD_SND_MEM_SIZE) {
        result = SDE_ERR_PRM_OVER_RANGE;
    } else if (size < 0 || size > SD_SND_MEM_SIZE) {
        result = SDE_ERR_PRM_OVER_RANGE;
    } else if (((Uint32)wrk_mem_ptr & (SYE_G2_DATA_WIDTH_32BIT - 1)) ||
               (size & (SYE_G2_DATA_WIDTH_32BIT - 1))) {
        result = SDE_ERR_PRM_OVER_RANGE;
    } else {
        sdgG2LibResult =
            syG2Write(wrk_mem_ptr, (void *)(snd_mem_adr + SD_SND_MEM_BASE),
                      SYE_G2_DATA_WIDTH_32BIT, size / SYE_G2_DATA_WIDTH_32BIT,
                      SYE_G2_ADR_MODE_ADD, SYE_G2_ADR_MODE_ADD);
        if (func != NULL && func != SDD_MEMBLK_SYNC_FUNC) {
            func(arg);
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMoveWrkMemToSndMem, result);
    }
    return result;
}
