#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkCreate(SDMEMBLK *handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;
    /* @bug memory: reads through null before the library is started. the
     * initializer reads sdgWrkPtr's table ahead of the test below (MOV.L
     * @(R0,R5),R4 at 0x8c12e5f8) */
    struct sd_memblk *blk = sdgWrkPtr->blocks.table;
    Sint32 i;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    for (i = 0; i < SDD_MEMBLK_HANDLE_NUM; i++, blk++) {
        if (blk->tag != SD_MEMBLK_MADE) {
            goto found;
        }
    }
    *handle = NULL;
    result = SDE_ERR_HANDLE_NO_ENOUGH;
    goto exit;
found:
    blk->tag = SD_MEMBLK_MADE;
    *handle = (SDMEMBLK)blk;
    /* @hack spelling: the handle read back from *handle for both calls,
     * where blk would do */
    sdMemBlkSetStat(*handle, SDE_MEMBLK_STAT_TRANSFER_REMAIN);
    sdMemBlkSetPrm(*handle, NULL, 0, NULL, NULL);
    sdgWrkPtr->blocks.made++;
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkCreate, result);
    }
    return result;
}
