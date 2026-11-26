/* the lock around the middleware's sound output: every interrupt masked */
#include <machine.h>
#include <mw_rna.h>
#include <sg_xpt.h>

void mwRnaInitCs(void)
{
}

void mwRnaFinishCs(void)
{
}

void mwRnaLockCs(Sint32 *mask)
{
    *mask = get_imask();
    set_imask(IMASK_ALL);
}

void mwRnaUnlockCs(const Sint32 *mask)
{
    set_imask(*mask);
}
