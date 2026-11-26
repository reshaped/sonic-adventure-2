/* convert the luma of Sofdec's decoded macroblocks, signed samples, into a
 * twiddled texture of 8-bit palette indexes. YUV_ALPHA3 and YUV_ALPHA5 also
 * write each sample they read back into the source. they map it through a luma
 * table of their mode's */
#ifndef SOFDEC_YUV_H
#define SOFDEC_YUV_H
#include <sg_xpt.h>

enum sofdec_yuv_mode { YUV_LUMINANCE, YUV_ALPHA3, YUV_ALPHA5, YUV_ALPHA256 };
void mwYuvSetPltTbl(const Sint8 *src, Sint32 mode);
void mwYuvGetPltTbl(Sint8 *dest, Sint32 mode);
void mwYuvConvMblkToTwdlRctngl(Sint8 *dest, Sint8 *src, Sint32 pitch, Sint32 x,
                               Sint32 y, Sint32 width, Sint32 height,
                               Sint32 mode);
void mwYuvConvMblkToTwdlRctngl2(Sint8 *dest, Sint8 *src, Sint32 pitch, Sint32 x,
                                Sint32 y, Sint32 width, Sint32 height,
                                Sint32 mode);
void mwYuvConvMblkToTwdl256(Sint8 *dest, Sint8 *src, Sint32 mode);
/* the index of column x and row y in twiddled order: the low 8 bits of
 * each, interleaved, y's in the even bits */
Sint32 mwYuvCombLct(Sint32 x, Sint32 y);
#endif
