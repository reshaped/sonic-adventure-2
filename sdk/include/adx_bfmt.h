/* the basic decoder's record, which its five format readers share.
 * ADXB_DecodeHeader picks the reader by the header's signature, and
 * ADXB_ExecHndl runs a step of the reader it picked */
#ifndef ADX_BFMT_H
#define ADX_BFMT_H
#include <adx_bsc.h>
#include <adx_xpnd.h>
#include <sg_xpt.h>

typedef void (*ADXB_ADDWR)(void *obj, Sint32 bytes, Sint32 samples);

/* a step's input, at ibuf, and the rings it writes. obufsize, obufdist,
 * wpos, nroom and nleft count samples of one channel */
typedef struct {
    Sint8 *ibuf;
    Sint32 nblk;
    Sint32 nch;
    Sint32 blklen;
    Sint32 blksmpl;
    Sint16 *obuf;
    Sint32 obufsize;
    Sint32 obufdist;
    Sint32 wpos;
    Sint32 nroom;
    Sint32 nleft;
    Sint16 coef[2];
    Sint16 *dly;
    Sint32 unk_34;
} ADXB_DEC;

struct adxb_obj {
    Sint16 used;
    Sint16 unk_02;
    Sint32 stat;
    ADXPD xpd;
    Sint8 enc;
    Sint8 bps;
    Sint8 nch;
    Sint8 blklen;
    Sint32 blksmpl;
    Sint32 sfreq;
    Sint32 total;
    Sint16 cof;
    Sint16 unk_1e;
    Sint32 lp_ins_nsmpl;
    Sint16 nloop;
    Sint16 lp_type;
    Sint32 lp_start_pos;
    Sint32 lp_start_ofst;
    Sint32 lp_end_pos;
    Sint32 lp_end_ofst;
    Sint32 maxnch;
    Sint16 *pcmbuf;
    Sint32 pcmbsize;
    Sint32 pcmbdist;
    Sint16 k0;
    Sint16 k1;
    Sint16 dly[2];
    ADXB_DEC dec;
    ADXB_GETWR get_wr;
    void *get_obj;
    ADXB_ADDWR add_wr;
    void *add_obj;
    Sint32 nsmpl;
    Sint32 wpos;
    Sint32 dec_nsmpl;
    Sint32 dec_dtlen;
    Sint16 format;
    Sint16 pcm_encoding;
};
/* pcm_encoding's codes: a PCM format's sample width, or the AU reader's
 * mu-law, which shares ADXB_PCM4's code */
#define ADXB_PCM16 0
#define ADXB_PCM8 1
#define ADXB_PCM4 2
#define ADXB_ULAW 2

Bool ADXB_CheckWav(const Uint8 *header);
Bool ADXB_CheckSpsd(const Uint8 *header);
Bool ADXB_CheckAiff(const Sint8 *header);
Bool ADXB_CheckAu(const Sint8 *header);
Sint32 ADXB_DecodeHeaderWav(ADXB adxb, Uint8 *header, Sint32 size);
Sint32 ADXB_DecodeHeaderSpsd(ADXB adxb, Uint8 *header, Sint32 size);
Sint32 ADXB_DecodeHeaderAiff(ADXB adxb, Sint8 *header, Sint32 size);
Sint32 ADXB_DecodeHeaderAu(ADXB adxb, Sint8 *header, Sint32 size);
void ADXB_ExecOneWav(ADXB adxb);
void ADXB_ExecOneSpsd(ADXB adxb);
void ADXB_ExecOneAiff(ADXB adxb);
void ADXB_ExecOneAu(ADXB adxb);

#endif
