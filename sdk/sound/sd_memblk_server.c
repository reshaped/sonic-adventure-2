#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

SDE_ERR sdMemBlkServer(void)
{
    Sint32 mask;
    Bool busy;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (mask == IMASK_ALL) {
        syG2DmaIsBusy(sdgWrkPtr->blocks.dma, &busy);
        if (!busy && sdgWrkPtr->blocks.in_transfer != NULL) {
            syG2DmaEndIntrruptHandler(sdgWrkPtr->blocks.dma);
        }
    }
    set_imask(mask);
    return SDE_ERR_NOTHING;
}
