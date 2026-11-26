/* ADX's header and footer readers, and the coefficients of the filter that
 * predicts each sample */
#include <adx_dcd.h>
#include <math.h>
#include <mathf.h>
#include <sg_xpt.h>
#ifdef NONMATCHING
/* this side's division sets errno to EDIV for a zero divisor, as SHC's runtime
 * does */
#include <errno.h>
#endif

/* ADX_MARK as ADX_ScanInfoCode reads it: the bytes 0x80 0x00 as a
 * little-endian halfword */
#define INFO_CODE 0x0080
#define TWO_PI 6.283185307F
#define BYTE_BITS 8
/* found's value until ADX_ScanInfoCode finds a mark */
#define NOT_FOUND 0x7fffffff
/* 1.0 in the fixed point of k0 and k1 */
#define COEF_ONE ((float)(1 << ADX_COEF_BITS))
/* the bytes of a block's header, which its samples follow */
#define BLK_HDR_LEN 2

/* an ADX header, every field big-endian */
typedef struct {
    Uint16 mark;
    Sint16 dtofs;
    Sint8 enc;
    Sint8 blklen;
    Sint8 bps;
    Sint8 nch;
    Sint32 sfreq;
    Sint32 total;
    Uint16 cof;
    Sint8 ver;
    Sint8 rev;
    Sint16 ins_nsmpl;
    Sint16 nloop;
    Sint16 reserved;
    Sint16 type;
    Sint32 start_pos;
    Sint32 start_ofst;
    Sint32 end_pos;
    Sint32 end_ofst;
} ADX_HDR;
#define HDR(hdr) ((const ADX_HDR *)(hdr))
/* a footer's first fields, which lie where a header's mark and dtofs lie */
typedef struct {
    Uint16 mark;
    Sint16 len;
} ADX_FTR;
#define FTR(ftr) ((const ADX_FTR *)(ftr))

/* the big-endian halfword and word at p, read a byte at a time */
#define READ16(p) (((Uint8)(p)[0] << 8) | (Uint8)(p)[1])
#define READ32(p)                                                              \
    (((Uint8)(p)[0] << 24) | ((Uint8)(p)[1] << 16) | ((Uint8)(p)[2] << 8) |    \
     (Uint8)(p)[3])
/* a halfword or a word with its bytes swapped */
#define ADX_SWAP16(x) ((((x) >> 8) & 0xff) | (((x) << 8) & 0xff00))
#define ADX_SWAP32(x)                                                          \
    ((Uint8)((x) >> 24) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) |    \
     ((x) << 24))

void ADX_GetCoefficient(Sint32 cof, Sint32 sfreq, Sint16 *k0, Sint16 *k1)
{
    Float a;
    Float b;
    Float c;

    a = sqrtf(2.0F) - cos(TWO_PI * cof / sfreq);
    b = sqrtf(2.0F) - 1.0F;
    c = (a - sqrtf((a + b) * (a - b))) / b;
    *k0 = (Sint16)(c * (2.0F * COEF_ONE));
    *k1 = (Sint16)(-(c * c * COEF_ONE));
}

Sint32 ADX_ScanInfoCode(const Sint8 *data, Sint32 size, Sint16 *ofst)
{
    Sint32 i;
    Sint32 found = NOT_FOUND;
    /* @hack redundant: register on code, where a plain local would do */
    register Sint16 code;

    for (i = 0; i < size - 1; i += 2) {
        code = *(const Sint16 *)(data + i);
        if (code == INFO_CODE) {
            /* @hack spelling: the least of i and found, where i would do, found
             * being NOT_FOUND there */
            found = (i < found) ? i : found;
            break;
        }
    }
    /* @hack spelling: the else after a return, where the second return alone
     * would do */
    if (found != NOT_FOUND) {
        *ofst = found;
        return 0;
    } else {
        *ofst = 0;
        return -1;
    }
}

Sint32 ADX_DecodeInfo(Sint8 *hdr, Sint32 hdrsize, Sint16 *dtofs, Sint8 *enc,
                      Sint8 *bps, Sint8 *blklen, Sint8 *nch, Sint32 *sfreq,
                      Sint32 *total, Sint32 *blksmpl)
{
    /* @hack spelling: data, a copy of hdr that the fields after the mark
     * are read through, where hdr would do */
    Sint8 *data = hdr;
    Uint16 mark;
    Sint32 pos;

    if (hdrsize < ADX_HDR_INFO_LEN) {
        return -1;
    }
    mark = READ16(hdr);
    if (mark != ADX_MARK) {
        return -2;
    }
    pos = sizeof(Uint16);
    *dtofs = READ16(data + pos) + ADX_DTOFS_BASE;
    pos += sizeof(Sint16);
    *enc = data[pos++];
    *blklen = data[pos++];
    *bps = data[pos++];
    *nch = data[pos++];
    *sfreq = READ32(data + pos);
    pos += sizeof(Sint32);
    *total = READ32(data + pos);
#ifdef NONMATCHING
    /* this side keeps the shipped result for a header whose bps is 0. for a
     * zero divisor, SHC's runtime returns 0 and sets errno to EDIV */
    if (*bps == 0) {
        errno = EDIV;
        *blksmpl = 0;
    } else {
        *blksmpl = (*blklen - BLK_HDR_LEN) * BYTE_BITS / *bps;
    }
#else
    *blksmpl = (*blklen - BLK_HDR_LEN) * BYTE_BITS / *bps;
#endif
    return 0;
}

Sint32 ADX_DecodeInfoExADPCM2(const Sint8 *hdr, Sint32 hdrsize, Sint16 *cof)
{
    Sint32 dtofs;
    Uint16 mark;

    if (hdrsize < ADX_HDR_COF_LEN) {
        return -1;
    }
    mark = ADX_SWAP16(HDR(hdr)->mark);
    if (mark != ADX_MARK) {
        return -2;
    }
    dtofs = ADX_SWAP16(HDR(hdr)->dtofs);
    if (dtofs < ADX_HDR_COF_LEN - ADX_DTOFS_BASE) {
        return -1;
    }
    *cof = ADX_SWAP16(HDR(hdr)->cof);
    return 0;
}

Sint32 ADX_DecodeInfoExLoop(const Sint8 *hdr, Sint32 hdrsize, Sint32 *ins_nsmpl,
                            Sint16 *nloop, Sint16 *type, Sint32 *start_pos,
                            Sint32 *start_ofst, Sint32 *end_pos,
                            Sint32 *end_ofst)
{
    /* @hack spelling: ADX_HDR_LOOP_LEN through len, which the data's offset
     * then reuses, where the constant in the test and the offset in a local
     * of its own would do */
    Sint32 len;
    Uint16 mark;

    *nloop = 0;
    len = ADX_HDR_LOOP_LEN;
    if (hdrsize < len) {
        return -1;
    }
    mark = ADX_SWAP16(HDR(hdr)->mark);
    if (mark != ADX_MARK) {
        return -2;
    }
    len = ADX_SWAP16(HDR(hdr)->dtofs);
    if (len < ADX_HDR_LOOP_LEN - ADX_DTOFS_BASE) {
        return -1;
    }
    *ins_nsmpl = ADX_SWAP16(HDR(hdr)->ins_nsmpl);
    *nloop = ADX_SWAP16(HDR(hdr)->nloop);
    if (*nloop != 1) {
        return -2;
    }
    *type = ADX_SWAP16(HDR(hdr)->type);
    *start_pos = ADX_SWAP32(HDR(hdr)->start_pos);
    *start_ofst = ADX_SWAP32(HDR(hdr)->start_ofst);
    *end_pos = ADX_SWAP32(HDR(hdr)->end_pos);
    *end_ofst = ADX_SWAP32(HDR(hdr)->end_ofst);
    return 0;
}

Sint32 ADX_DecodeFooter(Sint8 *ftr, Sint32 ftrsize, Sint16 *len)
{
    /* @hack spelling: data, a copy of ftr that the length is read through,
     * where ftr would do */
    Sint8 *data = ftr;
    Uint16 mark;

    if (ftrsize < ADX_FTR_LEN) {
        return -1;
    }
    mark = ADX_SWAP16(FTR(ftr)->mark);
    if (mark != ADX_END_MARK) {
        return -2;
    }
    *len = ADX_SWAP16(FTR(data)->len) + ADX_DTOFS_BASE;
    return 0;
}
