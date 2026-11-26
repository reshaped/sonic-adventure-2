/* the movie renderer's frames. a joint of frames holds the free frames on
 * its free line. its data line holds the frames to show. a frame goes through
 * the joint as a chunk of sizeof(struct mw_mvf) bytes */
#include <sg_xpt.h>
#include <sj.h>

#include <mw_mvf.h>

Sint32 mwMvfCalcImgSize(Sint32 fmt, Sint32 width, Sint32 height)
{
    if (fmt == MVF_FMT_YUV420) {
        return width * height * 3 / 2;
    }
    if (fmt == MVF_FMT_YUV422) {
        return width * height * 2;
    }
    if (fmt == MVF_FMT_PLT8) {
        return width * height;
    }
    for (;;) {
    }
}

struct mw_mvf *mwMvfGetDispFrm(SJ sj)
{
    SJCK ck;

    SJ_GetChunk(sj, SJ_LIN_DATA, sizeof(struct mw_mvf), &ck);
    if (ck.data == NULL || ck.len == 0) {
        return NULL;
    }
    return (struct mw_mvf *)ck.data;
}

void mwMvfUngetDispFrm(SJ sj, struct mw_mvf *mvf)
{
    SJCK ck;

    ck.data = (char *)mvf;
    ck.len = sizeof(struct mw_mvf);
    SJ_UngetChunk(sj, SJ_LIN_DATA, &ck);
}

void mwMvfRelDispFrm(SJ sj, struct mw_mvf *mvf)
{
    SJCK ck;

    ck.data = (char *)mvf;
    ck.len = sizeof(struct mw_mvf);
    SJ_PutChunk(sj, SJ_LIN_FREE, &ck);
}

struct mw_mvf *mwMvfGetFreeFrm(SJ sj)
{
    SJCK ck;

    SJ_GetChunk(sj, SJ_LIN_FREE, sizeof(struct mw_mvf), &ck);
    if (ck.data == NULL || ck.len == 0) {
        return NULL;
    }
    return (struct mw_mvf *)ck.data;
}

void mwMvfUngetFreeFrm(SJ sj, struct mw_mvf *mvf)
{
    SJCK ck;

    ck.data = (char *)mvf;
    ck.len = sizeof(struct mw_mvf);
    SJ_PutChunk(sj, SJ_LIN_FREE, &ck);
}

void mwMvfRelFreeFrm(SJ sj, struct mw_mvf *mvf)
{
    SJCK ck;

    ck.data = (char *)mvf;
    ck.len = sizeof(struct mw_mvf);
    SJ_PutChunk(sj, SJ_LIN_DATA, &ck);
}

void *mwMvfLock(struct mw_mvf *mvf)
{
    if (mvf->vtbl != NULL && mvf->vtbl->lock != NULL) {
        mvf->img = mvf->vtbl->lock(mvf);
    }
    return mvf->img;
}

void mwMvfUnlock(struct mw_mvf *mvf)
{
    if (mvf->vtbl != NULL && mvf->vtbl->unlock != NULL) {
        mvf->vtbl->unlock(mvf);
    }
}

void mwMvfSetSize(struct mw_mvf *mvf, Sint32 width, Sint32 height)
{
    mvf->width = width;
    mvf->height = height;
}

void mwMvfSetFmt(struct mw_mvf *mvf, Sint32 fmt)
{
    mvf->fmt = (Sint8)fmt;
}

void mwMvfSetTime(struct mw_mvf *mvf, Sint32 time, Sint32 tunit)
{
    mvf->time = time;
    mvf->tunit = tunit;
}

void mwMvfGetFmt(struct mw_mvf *mvf, Sint32 *fmt)
{
    *fmt = mvf->fmt;
}

void mwMvfGetTime(struct mw_mvf *mvf, Sint32 *time, Sint32 *tunit)
{
    *time = mvf->time;
    *tunit = mvf->tunit;
}
