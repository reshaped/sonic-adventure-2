/* the GSC's calls on the joint a stream reads into */
#include <sg_mwgsc.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>

/* the boundary a joint's free line must start on, for the drive's
 * transfers */
#define SJ_BUF_ALIGN 32

/* the calls' errors */
#define SJ_ERR_HN (-15) /* mwGscSjChkBuf's null joint */
#define SJ_ERR_BSIZE (-16) /* a line of a negative size */
#define SJ_ERR_SCT (-200) /* a joint whose size isn't whole sectors */
#define SJ_ERR_ALIGN (-201) /* a free line off SJ_BUF_ALIGN */
#define SJ_ERR_NUMDATA (-202) /* GetNumData's negative count */
#define SJ_ERR_NULL (-203) /* gsc_sj_line_bytes's null joint */
#define SJ_ERR_CHUNK (-204) /* a free chunk short of its sectors */

static Sint32 gsc_sj_line_bytes(SJ sj, Sint32 id, Sint32 *nbyte);
static Bool gsc_sj_is_get_chunk(SJ sj, Sint32 id, Sint32 nsct, Sint32 *rsct);

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 mwGscSjGetFreeBsize(SJ sj, Sint32 *nbyte);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 mwGscSjChkBuf(SJ sj)
{
    Sint32 bsize;
    char *buf;
    SJCK ck;
    Sint32 err;

    if (mwGscIsHnNull(sj)) {
        return mwGscSetErrCode(SJ_ERR_HN);
    }
    err = mwGscSjGetTotBufSize(sj, &bsize);
    if (err != 0) {
        return err;
    }
    if (bsize % GDD_FS_SCTSIZE != 0) {
        return mwGscSetErrCode(SJ_ERR_SCT);
    }
    SJ_GetChunk(sj, SJ_LIN_FREE, bsize, &ck);
    buf = ck.data;
    SJ_UngetChunk(sj, SJ_LIN_FREE, &ck);
    if ((Uint32)buf % SJ_BUF_ALIGN != 0) {
        return mwGscSetErrCode(SJ_ERR_ALIGN);
    }
    return 0;
}

Sint32 mwGscSjGetTotBufSize(SJ sj, Sint32 *nbyte)
{
    Sint32 nfree;
    Sint32 ndata;
    Sint32 err;

    err = mwGscSjGetFreeBsize(sj, &nfree);
    if (err != 0) {
        return err;
    }
    err = mwGscSjGetDataBsize(sj, &ndata);
    if (err != 0) {
        return err;
    }
    /* @hack redundant: the test of the two sizes, which gsc_sj_line_bytes
     * never leaves negative */
    if (nfree < 0 || ndata < 0) {
        return mwGscSetErrCode(SJ_ERR_BSIZE);
    }
    *nbyte = nfree + ndata;
    return 0;
}

/* store in *nbyte the bytes on the joint's free line. a failure leaves
 * *nbyte unset */
Sint32 mwGscSjGetFreeBsize(SJ sj, Sint32 *nbyte)
{
    /* @hack spelling: err, a local for the result, where a return of the
     * call would do */
    Sint32 err;

    err = gsc_sj_line_bytes(sj, SJ_LIN_FREE, nbyte);
    return err;
}

/* store in *nbyte the bytes on line id. a null sj or a negative count
 * fails, and leaves *nbyte unset */
static Sint32 gsc_sj_line_bytes(SJ sj, Sint32 id, Sint32 *nbyte)
{
    Sint32 n;

    if (mwGscIsHnNull(sj)) {
        return mwGscSetErrCode(SJ_ERR_NULL);
    }
    n = SJ_GetNumData(sj, id);
    if (n < 0) {
        return mwGscSetErrCode(SJ_ERR_NUMDATA);
    }
    *nbyte = n;
    return 0;
}

Sint32 mwGscSjGetDataBsize(SJ sj, Sint32 *nbyte)
{
    /* @hack spelling: err, a local for the result, where a return of the
     * call would do */
    Sint32 err;

    err = gsc_sj_line_bytes(sj, SJ_LIN_DATA, nbyte);
    return err;
}

Sint32 mwGscSjIsGetFreeChunk(SJ sj, Sint32 nsct, Sint32 limit, Sint32 *rsct)
{
    Sint32 ndata;
    Sint32 nfit;
    /* @hack spelling: nget, a local for the count, where the count passed
     * in place would do */
    Sint32 nget;
    Sint32 err;

    *rsct = 0;
    /* @hack spelling: err, a local for the call's result, where the call
     * tested in place would do */
    err = mwGscSjGetDataBsize(sj, &ndata);
    if (err != 0) {
        return 0;
    }
    nfit = (limit - ndata) / GDD_FS_SCTSIZE;
    if (nfit <= 0) {
        return 0;
    }
    nget = nfit < nsct ? nfit : nsct;
    return gsc_sj_is_get_chunk(sj, SJ_LIN_FREE, nget, rsct);
}

/* whether line id can give nsct sectors as one chunk. *rsct gets nsct where
 * it can, and otherwise the whole sectors of the chunk it can give */
static Bool gsc_sj_is_get_chunk(SJ sj, Sint32 id, Sint32 nsct, Sint32 *rsct)
{
    Sint32 rbyte;
    /* @hack spelling: nbyte, a local for the bytes, where the product
     * passed in place would do */
    Sint32 nbyte;
    /* @hack spelling: whole, a local for the call's result, where the call
     * tested in place would do */
    Sint32 whole;

    nbyte = nsct * GDD_FS_SCTSIZE;
    *rsct = nsct;
    whole = sj->vtbl->IsGetChunk(sj, id, nbyte, &rbyte);
    if (whole == TRUE) {
        return TRUE;
    }
    *rsct = rbyte / GDD_FS_SCTSIZE;
    return FALSE;
}

Sint32 mwGscSjGetFreeChunk(SJ sj, Sint32 nsct, SJCK *ck)
{
    Sint32 nbyte;
    SJCK got;

    nbyte = nsct * GDD_FS_SCTSIZE;
    SJ_GetChunk(sj, SJ_LIN_FREE, nbyte, &got);
    if (nbyte == got.len) {
        *ck = got;
        return 1;
    }
    mwGscSetErrCode(SJ_ERR_CHUNK);
    return 0;
}

void mwGscSjPutDataChunk(SJ sj, Sint32 nbyte, SJCK *ck)
{
    SJCK ck1;
    SJCK ck2;

    SJ_SplitChunk(ck, nbyte, &ck1, &ck2);
    SJ_PutChunk(sj, SJ_LIN_DATA, &ck1);
    *ck = ck2;
}

void mwGscSjUngetFreeChunk(SJ sj, SJCK *ck)
{
    if (ck->len > 0) {
        SJ_UngetChunk(sj, SJ_LIN_FREE, ck);
        ck->data = NULL;
        ck->len = 0;
    }
}

void mwGscSjReset(SJ sj)
{
    SJ_Reset(sj);
    /* @hack spelling: a return at the end of the body, where the end alone
     * would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}
