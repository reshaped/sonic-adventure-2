#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <string.h>

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* sdMemBlkInit points it at sdgWrkPtr->blocks */
struct sd_memblk_table *sdgMemBlkDbPtr;
/* NOLINTEND(misc-use-internal-linkage) */

SDE_ERR sdMemBlkInit(struct sd_memblk *blk, Sint32 num)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;
    Sint32 i;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        sdgWrkPtr->blocks.table = blk;
        sdgWrkPtr->blocks.max = num;
        sdgWrkPtr->blocks.made = 0;
        sdgWrkPtr->blocks.transfers_started = 0;
        sdgWrkPtr->blocks.transfers_ended = 0;
        sdgWrkPtr->blocks.in_transfer = NULL;
        sdgMemBlkDbPtr = &sdgWrkPtr->blocks;
        /* @hack spelling: sdgWrkPtr->blocks.table in memset's call, where
         * blk, the same pointer, would do */
        memset(sdgWrkPtr->blocks.table, SD_UNSET_FILL,
               num * sizeof(struct sd_memblk));
        for (i = 0; i < num; i++) {
            /* @hack spelling: the table indexed at each of the record's four
             * stores, where a pointer to the record would do */
            sdgWrkPtr->blocks.table[i].tag = SD_MEMBLK_FREE;
            sdgWrkPtr->blocks.table[i].stat = SDE_MEMBLK_STAT_TRANSFER_REMAIN;
            sdgWrkPtr->blocks.table[i].start_time = SD_MEMBLK_NO_START;
            sdgWrkPtr->blocks.table[i].end_time = 0;
        }
        syG2DmaOpen(&sdgWrkPtr->blocks.dma, SYE_G2_DMA_TYPE_AICA);
    }
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMemBlkInit, result);
    }
    return result;
}
