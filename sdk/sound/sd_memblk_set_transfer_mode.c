#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the member exports this name, though no header of the SDK declares it.
 * by its name, the current transfer mode, which sdMemBlkSetTransferMode
 * keeps in the work's transfer_mode instead */
/* NOLINTBEGIN(misc-use-internal-linkage) */
SDE_MEMBLK_TRANSFER_MODE sdgCurMemBlkTransferMode;
/* NOLINTEND(misc-use-internal-linkage) */

SDE_ERR sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE transfer_mode)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    /* @hack spelling: an early return that restores the mask itself, where
     * if (sdgWrkPtr != NULL) around the wait and the store would do */
    if (sdgWrkPtr == NULL) {
        set_imask(mask);
        return SDE_ERR_NOTHING;
    }
    while (sdgWrkPtr->blocks.in_transfer != NULL) {
        sdSysServer();
    }
    sdgWrkPtr->transfer_mode = transfer_mode;
    set_imask(mask);
    return SDE_ERR_NOTHING;
}
