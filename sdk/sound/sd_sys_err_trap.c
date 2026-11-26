#include <sd_work.h>
#include <sg_sd.h>

/* the member exports sdgErrApi and sdgErrResult, though no header declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void *sdgErrApi = NULL;
SDE_ERR sdgErrResult = SDE_ERR_NOTHING;
/* NOLINTEND(misc-use-internal-linkage) */

void sdSysErrTrap(void *api, SDE_ERR err)
{
    sdgErrApi = api;
    sdgErrResult = err;
    /* @bug memory: reads err_callback through sdgWrkPtr untested (MOV.L
     * @(R0,R14),R3 at 0x8c15501c). the library's calls test sdgWrkPtr and
     * report SDE_ERR_NO_INIT here where it is null, as sdUtilCalculationFade
     * does (TST R3,R3 at 0x8c1551d8, JSR at 0x8c155206). the read then goes
     * through null, to err_callback's offset from address 0. a nonzero long
     * found there is called */
    if (sdgWrkPtr->err_callback != NULL) {
        sdgWrkPtr->err_callback(api, err, NULL);
    }
}
