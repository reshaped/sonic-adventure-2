#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* what *count_num holds where no driver's count is read */
#define NO_COUNT 0xffffffff

SDE_ERR sdDrvGetExecuteCounter(Uint32 *count_num)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (count_num == NULL) {
        result = SDE_ERR_PTR_NULL;
        /* @hack redundant: both the goto and the else that follows it, where
         * either alone would do */
        goto exit;
    } else {
        *count_num = NO_COUNT;
        if (sdgWrkPtr == NULL || sdgExistSndDrv != TRUE) {
            result = SDE_ERR_NO_INIT;
            /* @hack redundant: a second store of NO_COUNT to *count_num,
             * which holds it already */
            *count_num = NO_COUNT;
        } else {
            sdSndMemReadUint32(sdgWrkPtr->stat_adr + SD_DRV_COUNTER_AT,
                               count_num);
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvGetExecuteCounter, result);
    }
    return result;
}
