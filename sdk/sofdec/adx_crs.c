/* CRI ADX's critical sections: a lock that nests, and masks every interrupt
 * while it is held */
#include <adx_crs.h>
#include <machine.h>
#include <sg_xpt.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the ADXCRS_Lock calls that no ADXCRS_Unlock has answered */
Sint32 adxcrs_lvl = 0;
/* the interrupt mask the outermost ADXCRS_Lock replaced */
Sint32 adxcrs_msk = 0;
/* NOLINTEND(misc-use-internal-linkage) */

void ADXCRS_Init(void)
{
    adxcrs_lvl = 0;
}

void ADXCRS_Lock(void)
{
    /* @hack spelling: mask, a local that holds the mask until set_imask has
     * run, where get_imask's result stored in adxcrs_msk at once would do */
    Sint32 mask;

    if (adxcrs_lvl == 0) {
        mask = get_imask();
        set_imask(IMASK_ALL);
        adxcrs_msk = mask;
    }
    adxcrs_lvl++;
}

void ADXCRS_Unlock(void)
{
    adxcrs_lvl--;
    if (adxcrs_lvl == 0) {
        set_imask(adxcrs_msk);
    }
}
