/* the GSC's calls on a stream's fields, and mwGscIsHnNull, its test for a
 * null stream. a call on a null stream fails with GSC_ERR_HN. mwGscGetSjHn
 * and mwGscGetFsHn instead return NULL for one, and record no error */
#include <sg_mwgsc.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 mwGscGetCurOfst(struct gsc_stream *hn, Sint32 *ofst);
Sint32 mwGscGetStmSize(struct gsc_stream *hn, Sint32 *size);
SJ mwGscGetSjHn(struct gsc_stream *hn);
Sint32 mwGscGetFlowLimit(struct gsc_stream *hn, Sint32 *low, Sint32 *high);
Sint32 mwGscGetRdSct(struct gsc_stream *hn, Sint32 *nsct);
Sint32 mwGscGetTrSct(struct gsc_stream *hn, Sint32 *nsct);
Sint32 mwGscSetFlowLimit(struct gsc_stream *hn, Sint32 low, Sint32 high);
GDFS mwGscGetFsHn(struct gsc_stream *hn);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 mwGscGetStat(struct gsc_stream *hn, Sint32 *stat)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    *stat = hn->stat;
    return 0;
}

Sint32 mwGscGetCurOfst(struct gsc_stream *hn, Sint32 *ofst)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    *ofst = hn->cur_ofst;
    return 0;
}

Sint32 mwGscGetStmSize(struct gsc_stream *hn, Sint32 *size)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    *size = hn->stm_size;
    return 0;
}

SJ mwGscGetSjHn(struct gsc_stream *hn)
{
    if (mwGscIsHnNull(hn)) {
        return NULL;
    }
    return hn->sj;
}

Sint32 mwGscGetFlowLimit(struct gsc_stream *hn, Sint32 *low, Sint32 *high)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    *low = hn->flow_low;
    *high = hn->flow_high;
    return 0;
}

Sint32 mwGscGetRdSct(struct gsc_stream *hn, Sint32 *nsct)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    *nsct = hn->rd_sct;
    return 0;
}

Sint32 mwGscGetTrSct(struct gsc_stream *hn, Sint32 *nsct)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    *nsct = hn->tr_sct;
    return 0;
}

/* set hn's flow limits, in bytes. unless 0 <= low <= high <= hn->bufsize,
 * it returns GSC_ERR_RANGE and sets neither */
Sint32 mwGscSetFlowLimit(struct gsc_stream *hn, Sint32 low, Sint32 high)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    /* @hack spelling: three tests, each with its own return, where one
     * test of the five conditions would do */
    if (high < 0 || high > hn->bufsize) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    if (low < 0 || low > hn->bufsize) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    if (low > high) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    hn->flow_low = low;
    hn->flow_high = high;
    return 0;
}

/* set hn's rd_sct. unless 0 <= nsct <= hn->bufsize / GDD_FS_SCTSIZE, it
 * returns GSC_ERR_RANGE and sets nothing */
Sint32 mwGscSetRdSct(struct gsc_stream *hn, Sint32 nsct)
{
    /* @hack spelling: max, a local for the bound, where the bound in the
     * test would do */
    Sint32 max;

    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    max = hn->bufsize / GDD_FS_SCTSIZE;
    if (nsct < 0 || nsct > max) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    hn->rd_sct = nsct;
    return 0;
}

/* set hn's tr_sct. unless 0 <= nsct <= hn->rd_sct, it returns
 * GSC_ERR_RANGE and sets nothing */
Sint32 mwGscSetTrSct(struct gsc_stream *hn, Sint32 nsct)
{
    /* @hack spelling: max, a local for the bound, where the bound in the
     * test would do */
    Sint32 max;

    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    max = hn->rd_sct;
    if (nsct < 0 || nsct > max) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    hn->tr_sct = nsct;
    return 0;
}

Sint32 mwGscSetOfstRange(struct gsc_stream *hn, Sint32 ofst, Sint32 end)
{
    /* @hack spelling: size, a local for the stream's size, where
     * hn->stm_size read at each use would do */
    Sint32 size;

    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    size = hn->stm_size;
    if (end == GSC_OFST_END) {
        end = size;
    }
    /* @hack spelling: three tests, each with its own return, where one
     * test of the five conditions would do */
    if (ofst < 0 || ofst > size) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    if (end < 0 || end > size) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    if (ofst > end) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    hn->ofst_start = ofst;
    hn->ofst_end = end;
    return 0;
}

Bool mwGscIsHnNull(void *hn)
{
    /* @hack spelling: the result through an if, where one return of the
     * comparison would do */
    if (hn == NULL) {
        return TRUE;
    }
    return FALSE;
}

GDFS mwGscGetFsHn(struct gsc_stream *hn)
{
    if (mwGscIsHnNull(hn)) {
        return NULL;
    }
    return hn->fs;
}
