/* the movie renderer's frames, which pass through a stream joint as chunks
 * of sizeof(struct mw_mvf) bytes. the joint's free line holds the free
 * frames, and its data line the frames to show */
#ifndef MW_MVF_H
#define MW_MVF_H
#include <sg_xpt.h>
#include <sj.h>

/* the formats of a frame's image */
#define MVF_FMT_YUV420 0 /* 12 bits a pixel */
#define MVF_FMT_YUV422 1 /* 16 bits a pixel */
#define MVF_FMT_PLT8 2 /* 8 bits a pixel, an index into a palette */

struct mw_mvf;
/* a frame's functions. lock returns its image, and unlock gives the image
 * up */
struct mw_mvf_vtbl {
    void *(*lock)(struct mw_mvf *mvf);
    void (*unlock)(struct mw_mvf *mvf);
};
/* a frame. vtbl may be null, and so may either function in it */
struct mw_mvf {
    struct mw_mvf_vtbl *vtbl;
    Uint8 unk_04;
    Sint8 fmt; /* an MVF_FMT_ format */
    Uint8 unk_06[2];
    Sint32 width;
    Sint32 height;
    Sint32 time;
    Sint32 tunit;
    void *img; /* the image mwMvfLock last returned */
    Uint8 unk_1c[4];
};

/* the bytes of a width by height image in fmt. it hangs for an fmt that is
 * none of the three */
Sint32 mwMvfCalcImgSize(Sint32 fmt, Sint32 width, Sint32 height);
/* mwMvfGetDispFrm takes the next frame to show off sj's data line.
 * mwMvfGetFreeFrm takes the next free frame off its free line. each returns
 * NULL where its line holds no frame */
struct mw_mvf *mwMvfGetDispFrm(SJ sj);
struct mw_mvf *mwMvfGetFreeFrm(SJ sj);
/* mwMvfUngetDispFrm puts mvf back at the front of the data line.
 * mwMvfRelDispFrm puts a shown frame at the end of the free line.
 * mwMvfRelFreeFrm puts a filled frame at the end of the data line */
void mwMvfUngetDispFrm(SJ sj, struct mw_mvf *mvf);
void mwMvfRelDispFrm(SJ sj, struct mw_mvf *mvf);
void mwMvfRelFreeFrm(SJ sj, struct mw_mvf *mvf);
/* put mvf at the end of the free line, as mwMvfRelDispFrm does. unlike
 * mwMvfUngetDispFrm, it doesn't put the frame back at its line's front */
void mwMvfUngetFreeFrm(SJ sj, struct mw_mvf *mvf);
/* mwMvfLock locks mvf's image, keeps it in mvf->img and returns it. where
 * mvf has no lock, it returns mvf->img as it was. mwMvfUnlock gives the image
 * up, and does nothing where mvf has no unlock */
void *mwMvfLock(struct mw_mvf *mvf);
void mwMvfUnlock(struct mw_mvf *mvf);
void mwMvfSetSize(struct mw_mvf *mvf, Sint32 width, Sint32 height);
void mwMvfSetFmt(struct mw_mvf *mvf, Sint32 fmt);
void mwMvfSetTime(struct mw_mvf *mvf, Sint32 time, Sint32 tunit);
void mwMvfGetFmt(struct mw_mvf *mvf, Sint32 *fmt);
void mwMvfGetTime(struct mw_mvf *mvf, Sint32 *time, Sint32 *tunit);

#endif
