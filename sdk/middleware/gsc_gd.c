/* the GSC's file system layer, which works on a stream's file through the
 * mwGscFsi calls */
#include <machine.h>
#include <sg_mwgsc.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the layer's errors */
#define GD_ERR_NUMRD (-100) /* a negative count of bytes read */
#define GD_ERR_INIT (-102) /* a failed mwGscFsiInit */
#define GD_ERR_REINIT (-103) /* a failed mwGscFsiReinit */
#define GD_ERR_OPEN (-104) /* a file that won't open */
#define GD_ERR_FSIZE (-105) /* a file whose size can't be read */
#define GD_ERR_OPEN_RANGE (-106) /* sectors that won't open */
#define GD_ERR_FS (-107) /* a stream with no file */
#define GD_ERR_SEEK (-108) /* a failed seek */
#define GD_ERR_REQRD (-109) /* a failed read request */
/* a status outside GDD_STAT_IDLE to GDD_STAT_ERR */
#define GD_ERR_STAT (-110)

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 mwGscGdIsComplete(struct gsc_stream *hn);
Sint32 gscGdLock(void);
void gscGdUnlock(Sint32 mask);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 mwGscGdInit(void)
{
    /* @hack spelling: err, a local for the call's result, where the call
     * tested in place would do */
    Sint32 err;

    err = mwGscFsiInit();
    if (err != 0) {
        return mwGscSetErrCode(GD_ERR_INIT);
    }
    return 0;
}

Sint32 mwGscGdReset(void)
{
    /* @hack spelling: err, a local for the call's result, where the call
     * tested in place would do */
    Sint32 err;

    err = mwGscFsiReinit();
    if (err != 0) {
        return mwGscSetErrCode(GD_ERR_REINIT);
    }
    return 0;
}

void mwGscGdFinish(void)
{
    mwGscFsiFinish();
}

GDFS mwGscGdOpenFname(char *fname, Sint32 *fsize)
{
    GDFS fs;
    /* @hack spelling: ok, a local for the call's result, where the call
     * tested in place would do */
    Bool ok;

    fs = mwGscFsiOpen(fname, NULL);
    if (fs == NULL) {
        mwGscSetErrCode(GD_ERR_OPEN);
        return NULL;
    }
    ok = mwGscFsiGetFileSize(fs, fsize);
    if (ok != TRUE) {
        mwGscSetErrCode(GD_ERR_FSIZE);
        return NULL;
    }
    return fs;
}

GDFS mwGscGdOpenRange(Sint32 stsct, Sint32 nsct, Sint32 *fsize)
{
    GDFS fs;

    fs = mwGscFsiOpenRange(stsct, nsct);
    if (fs == NULL) {
        mwGscSetErrCode(GD_ERR_OPEN_RANGE);
        return NULL;
    }
    *fsize = nsct * GDD_FS_SCTSIZE;
    return fs;
}

Sint32 mwGscGdClose(struct gsc_stream *hn)
{
    GDFS fs;
    Sint32 err;

    fs = hn->fs;
    if (mwGscIsHnNull(fs)) {
        return mwGscSetErrCode(GD_ERR_FS);
    }
    err = mwGscGdStop(hn);
    mwGscFsiClose(fs);
    return err;
}

Sint32 mwGscGdStop(struct gsc_stream *hn)
{
    /* @hack spelling: fs, a local for hn->fs, where hn->fs tested in place
     * would do */
    GDFS fs;

    fs = hn->fs;
    if (mwGscIsHnNull(fs)) {
        return mwGscSetErrCode(GD_ERR_FS);
    }
    return 0;
}

Sint32 mwGscGdReqRd(struct gsc_stream *hn, Sint32 nsct, void *buf, Sint32 *nreq)
{
    GDFS fs;
    Sint32 nsct_req;

    fs = hn->fs;
    if (mwGscIsHnNull(fs)) {
        return mwGscSetErrCode(GD_ERR_FS);
    }
    nsct_req = mwGscFsiReqRd32(fs, nsct, buf);
    if (nsct_req < 0) {
        return mwGscSetErrCode(GD_ERR_REQRD);
    }
    *nreq = nsct_req;
    return 0;
}

Sint32 mwGscGdGetNumRd(struct gsc_stream *hn, Sint32 *nbyte)
{
    GDFS fs;

    fs = hn->fs;
    if (mwGscIsHnNull(fs)) {
        return mwGscSetErrCode(GD_ERR_FS);
    }
    *nbyte = mwGscFsiGetNumRd(fs);
    if (*nbyte < 0) {
        return mwGscSetErrCode(GD_ERR_NUMRD);
    }
    return 0;
}

Sint32 mwGscGdGetFsiStat(struct gsc_stream *hn, Sint32 *stat)
{
    Sint32 err;
    GDFS fs;

    /* @hack spelling: err set to 0 first and returned after the switch,
     * where a return from each arm would do */
    err = 0;
    fs = hn->fs;
    if (mwGscIsHnNull(fs)) {
        return mwGscSetErrCode(GD_ERR_FS);
    }
    *stat = mwGscFsiGetStat(fs);
    switch (*stat) {
    case GDD_STAT_IDLE:
    case GDD_STAT_COMPLETE:
    case GDD_STAT_READ:
    case GDD_STAT_SEEK:
    case GDD_STAT_BUSY:
    case GDD_STAT_ERR:
        break;
    default:
        err = mwGscSetErrCode(GD_ERR_STAT);
        break;
    }
    return err;
}

Sint32 mwGscGdIsFsiStatErr(struct gsc_stream *hn)
{
    GDFS fs;
    /* @hack spelling: stat, a local for the call's result, where the call
     * as the switch's operand would do */
    Sint32 stat;
    Bool failed;

    fs = hn->fs;
    if (mwGscIsHnNull(fs)) {
        return mwGscSetErrCode(GD_ERR_FS);
    }
    stat = mwGscFsiGetStat(fs);
    /* @hack spelling: the result through a switch, where one return of the
     * comparison would do */
    switch (stat) {
    case GDD_STAT_ERR:
        failed = TRUE;
        break;
    default:
        failed = FALSE;
        break;
    }
    return failed;
}

/* 1 when the file's status is GDD_STAT_COMPLETE, and else 0 */
Sint32 mwGscGdIsComplete(struct gsc_stream *hn)
{
    Sint32 stat;
    Sint32 complete;

    /* @bug uninitialized: tests stat, which mwGscGdGetFsiStat leaves unset
     * for a stream with no file. that call returns GD_ERR_FS before it
     * stores stat (MOV #-107,R4 at 0x8c10a70c), and this one tests stat all
     * the same (MOV.L @R15,R0 at 0x8c10a7c8). nothing in the image calls
     * this function */
    mwGscGdGetFsiStat(hn, &stat);
    /* @hack spelling: the result through a switch, where one return of the
     * comparison would do */
    switch (stat) {
    case GDD_STAT_COMPLETE:
        complete = 1;
        break;
    default:
        complete = 0;
        break;
    }
    return complete;
}

Sint32 mwGscGdSeek(struct gsc_stream *hn, Sint32 sct)
{
    GDFS fs;
    /* @hack spelling: err, a local for the call's result, where the call
     * tested in place would do */
    Sint32 err;

    fs = hn->fs;
    if (mwGscIsHnNull(fs)) {
        return mwGscSetErrCode(GD_ERR_FS);
    }
    err = mwGscFsiSeek(fs, sct, GDD_SEEK_SET);
    if (err != 0) {
        return mwGscSetErrCode(GD_ERR_SEEK);
    }
    return 0;
}

void mwGscGdExecServer(void)
{
    Sint32 mask;

    mask = gscGdLock();
    mwGscFsiExecServer();
    gscGdUnlock(mask);
    /* @hack spelling: a return at the end of the body, where the end alone
     * would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* the layer's lock is the interrupt mask. gscGdLock saves the interrupt
 * mask, masks every interrupt, and returns the saved mask. gscGdUnlock sets
 * the interrupt mask to mask again */
Sint32 gscGdLock(void)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    return mask;
}

void gscGdUnlock(Sint32 mask)
{
    set_imask(mask);
}
