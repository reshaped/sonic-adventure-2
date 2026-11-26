/* LSC's lock around its handles, which masks every interrupt */
#include <lsc_crs.h>
#include <machine.h>
#include <sg_xpt.h>

void LSC_LockCrs(Sint32 *mask)
{
    *mask = get_imask();
    set_imask(IMASK_ALL);
}

void LSC_UnlockCrs(const Sint32 *mask)
{
    set_imask(*mask);
}
