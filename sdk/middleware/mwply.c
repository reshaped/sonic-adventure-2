/* the player's calls on the whole middleware. each calls the manager's
 * call of the same work: mwPlyInitSystem calls mwMngInit, and
 * mwPlyExitSystem mwMngFinish */
#include <sg_mw.h>
#include <sg_mwply.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mwPlyInitSystem(void);
void mwPlyExitSystem(void);

char *mwg_ply_build = "\nmwPly Ver.2.05 Build:Jan 19 2001 18:50:59\n";
/* NOLINTEND(misc-use-internal-linkage) */

void mwPlyStartFrame(void)
{
    mwMngStartFrame();
}

void mwPlyEntryErrFunc(MW_PLY_ERRFN errfn, void *obj)
{
    mwMngEntryErrFn(errfn, obj);
}

void mwPlyExecServer(void)
{
    mwMngExecServer();
}

void mwPlyInitSystem(void)
{
    mwMngInit();
}

void mwPlyExitSystem(void)
{
    mwMngFinish();
}
