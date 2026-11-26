/* the GSC's calls through its interface, mwg_gsc_i. each passes its
 * arguments to the interface's call of its name, and returns that call's
 * result. none records an error of its own. without an interface,
 * mwGscOpenFname and mwGscOpenRange return NULL, mwGscClose returns
 * GSC_ERR_NOIF, and mwGscExecServer does nothing */
#include <sg_mwgsc.h>
#include <sg_xpt.h>
#include <sj.h>

#define GSC_ERR_NOIF (-11) /* no interface is set */

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 mwGscFinish(void);
struct gsc_stream *mwGscOpenRange(Sint32 stsct, Sint32 nsct, SJ sj);

/* the GSC's identifier as a library */
UUID mwg_gsc_lid = {
    0x6c6af8c0,
    0x01f9,
    0x11d2,
    {0xa5, 0xa0, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},
};
/* the interface the calls go through, or null before mwGscInitOvl and
 * once a finish succeeds */
struct gsc_lif *mwg_gsc_i = NULL;
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 mwGscInitOvl(Sint32 nhn, void *work)
{
    /* @hack spelling: err, a local for the result, where a return of the
     * call would do */
    Sint32 err;

    mwg_gsc_i = &mwg_gsc_lif;
    err = mwg_gsc_i->Init(nhn, work);
    return err;
}

/* while mwg_gsc_execstatic is set, do nothing and return GSC_ERR_STATIC.
 * otherwise finish the GSC through the interface's Finish, clear mwg_gsc_i
 * and return 0. an error from Finish returns at once, and leaves mwg_gsc_i
 * set. mwg_gsc_i must not be null */
Sint32 mwGscFinish(void)
{
    Sint32 err;

    /* @hack spelling: the result built in err through an else, where a
     * return of each result as it is known would do */
    if (!mwg_gsc_execstatic) {
        err = mwg_gsc_i->Finish();
        if (err != 0) {
            return err;
        }
        /* @hack redundant: the second test of mwg_gsc_execstatic, which the
         * interface's Finish never sets */
        if (!mwg_gsc_execstatic) {
            mwg_gsc_i = NULL;
        } else {
            err = GSC_ERR_STATIC;
        }
    } else {
        err = GSC_ERR_STATIC;
    }
    return err;
}

/* count mwg_gsc_prevcnt down, to no lower than 0, and return 0 while it is
 * above 0. at 0, finish the GSC through the interface's Finish, and return
 * its result. a result of 0 also clears mwg_gsc_i. mwg_gsc_i must not be
 * null whenever the count is 0 */
Sint32 mwGscFinishStatic(void)
{
    Sint32 err;

    /* @hack spelling: err set to 0 first and returned at the end, where a
     * return of each result as it is known would do */
    err = 0;
    mwg_gsc_prevcnt--;
    if (mwg_gsc_prevcnt < 0) {
        mwg_gsc_prevcnt = 0;
    }
    if (mwg_gsc_prevcnt == 0) {
        err = mwg_gsc_i->Finish();
        if (err != 0) {
            return err;
        }
        mwg_gsc_i = NULL;
    }
    return err;
}

void mwGscExecServer(void)
{
    if (mwg_gsc_i != NULL) {
        mwg_gsc_i->ExecServer();
    }
}

struct gsc_stream *mwGscOpenFname(char *fname, SJ sj)
{
    /* @hack spelling: hn, a local for the result, where a return of the
     * call would do */
    struct gsc_stream *hn;

    if (mwg_gsc_i == NULL) {
        return NULL;
    }
    hn = mwg_gsc_i->OpenFname(fname, sj);
    return hn;
}

struct gsc_stream *mwGscOpenRange(Sint32 stsct, Sint32 nsct, SJ sj)
{
    /* @hack spelling: hn, a local for the result, where a return of the
     * call would do */
    struct gsc_stream *hn;

    if (mwg_gsc_i == NULL) {
        return NULL;
    }
    hn = mwg_gsc_i->OpenRange(stsct, nsct, sj);
    return hn;
}

Sint32 mwGscClose(struct gsc_stream *hn)
{
    /* @hack spelling: err, a local for the result, where a return of the
     * call would do */
    Sint32 err;

    if (mwg_gsc_i == NULL) {
        return GSC_ERR_NOIF;
    }
    err = mwg_gsc_i->Close(hn);
    return err;
}
