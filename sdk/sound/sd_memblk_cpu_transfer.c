#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkCpuTransfer(SDMEMBLK handle, void *dst)
{
    SDE_ERR result;
    Sint32 mask;
    struct sd_memblk *blk;

    mask = get_imask();
    set_imask(IMASK_ALL);
    result = sdMemBlkCheck(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    blk = (struct sd_memblk *)handle;
    blk->ended = FALSE;
    /* @bug logic: leaves the block the one in transfer where
     * sdMoveWrkMemToSndMem refuses the copy (MOV.L R14,@(R0,R2) at
     * 0x8c16b6ca). a refused copy calls no end handler, so in_transfer keeps
     * the block until another block's transfer ends */
    sdgWrkPtr->blocks.in_transfer = handle;
    sdgWrkPtr->blocks.transfers_started++;
    /* @hack redundant: a second store of handle to in_transfer, which holds
     * it already */
    sdgWrkPtr->blocks.in_transfer = handle;
    result = sdMoveWrkMemToSndMem(blk->ptr, (Sint32)dst, blk->size,
                                  sdMemBlkEndTransferHandller, handle);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkCpuTransfer, result);
    }
    set_imask(mask);
    return result;
}
