/* ADX's header reader, and the coefficients of the filter that predicts
 * each sample */
#ifndef ADX_DCD_H
#define ADX_DCD_H
#include <sg_xpt.h>

/* store in *k0 and *k1 the coefficients that predict a sample from the two
 * before it, for a cutoff of cof hertz at sfreq samples a second. a
 * prediction is k0 / 4096 of the sample before plus k1 / 4096 of the one
 * before that. the call works in floats:
 *
 *   a = sqrt(2) - cos(2 * pi * cof / sfreq)
 *   b = sqrt(2) - 1
 *   c = (a - sqrt((a + b) * (a - b))) / b
 *
 * *k0 gets 2c and *k1 gets -c * c, each times 4096 and truncated toward 0 */
void ADX_GetCoefficient(Sint32 cof, Sint32 sfreq, Sint16 *k0, Sint16 *k1);
/* k0 and k1 hold ADX_COEF_BITS bits of fraction, so 4096 stands for 1 */
#define ADX_COEF_BITS 12
/* the big-endian halfwords that open an ADX header and an ADX footer, each
 * ADX_MARK_LEN bytes. a header's data offset and a footer's length count from
 * byte ADX_DTOFS_BASE, past the mark and the halfword that holds them */
#define ADX_MARK 0x8000
#define ADX_END_MARK 0x8001
#define ADX_MARK_LEN 2
#define ADX_DTOFS_BASE 4
/* the shortest header or footer each reader takes. ADX_HDR_INFO_LEN bytes
 * hold the sample format, and ADX_HDR_COF_LEN bytes the cutoff. the loop's
 * fields end at byte 44, and ADX_HDR_LOOP_LEN asks 4 bytes past them */
#define ADX_HDR_INFO_LEN 16
#define ADX_HDR_COF_LEN 18
#define ADX_HDR_LOOP_LEN 48
#define ADX_FTR_LEN 16
/* read the format of the ADX header at hdr, of hdrsize bytes, and return 0.
 * the call returns -1 where hdrsize is under ADX_HDR_INFO_LEN. it returns -2
 * where hdr opens with no header's mark. a failed call stores nothing.
 *
 * *dtofs gets the bytes from the header's start to the data. *enc gets the
 * encoding, and *nch the channels. *blklen gets a block's bytes, and *bps a
 * sample's bits. *sfreq gets the samples a second, and *total the samples a
 * channel. *blksmpl gets a block's samples: (*blklen - 2) * 8 / *bps */
Sint32 ADX_DecodeInfo(Sint8 *hdr, Sint32 hdrsize, Sint16 *dtofs, Sint8 *enc,
                      Sint8 *bps, Sint8 *blklen, Sint8 *nch, Sint32 *sfreq,
                      Sint32 *total, Sint32 *blksmpl);

/* find the mark that opens an ADX header in the size bytes at data. data
 * must lie on a 2-byte boundary. the call reads each halfword at an even
 * offset. where one holds the bytes 0x80 and 0x00, *ofst gets its offset,
 * and the call returns 0. where none does, *ofst gets 0, and the call returns
 * -1. *ofst keeps an offset's low 16 bits */
Sint32 ADX_ScanInfoCode(const Sint8 *data, Sint32 size, Sint16 *ofst);

/* store the bytes of the ADX footer at ftr, of ftrsize bytes, in *len, and
 * return 0. the call returns -1 where ftrsize is under ADX_FTR_LEN, and -2
 * where ftr opens with no footer's mark. a failed call stores nothing */
Sint32 ADX_DecodeFooter(Sint8 *ftr, Sint32 ftrsize, Sint16 *len);

/* store the header's cutoff in hertz in *cof, and return 0. the call returns
 * -1 where hdrsize is under ADX_HDR_COF_LEN, or where the data starts before
 * byte ADX_HDR_COF_LEN. it returns -2 where hdr opens with no header's mark. a
 * failed call stores nothing */
Sint32 ADX_DecodeInfoExADPCM2(const Sint8 *hdr, Sint32 hdrsize, Sint16 *cof);
/* read the header's loop. the call first stores 0 in *nloop. it returns -1
 * where hdrsize is under ADX_HDR_LOOP_LEN, or where the data starts before
 * byte ADX_HDR_LOOP_LEN. it returns -2 where hdr opens with no header's
 * mark.
 *
 * the call then stores *ins_nsmpl, the samples inserted ahead of the data.
 * it stores *nloop, the header's count of loops, and returns -2 where that
 * count isn't 1. it then stores the loop's type, and returns 0 after it
 * stores the loop's start and end. each is a sample, and a byte offset */
Sint32 ADX_DecodeInfoExLoop(const Sint8 *hdr, Sint32 hdrsize, Sint32 *ins_nsmpl,
                            Sint16 *nloop, Sint16 *type, Sint32 *start_pos,
                            Sint32 *start_ofst, Sint32 *end_pos,
                            Sint32 *end_ofst);

#endif
