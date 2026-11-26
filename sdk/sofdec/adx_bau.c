/* the basic decoder's Sun AU reader: G.711 mu-law, or 8-bit or 16-bit linear
 * PCM. the header's words and 16-bit samples are big-endian */
#include <adx_bfmt.h>
#include <adx_bsc.h>
#include <sg_xpt.h>
#include <string.h>

#include "../sofdec/adx_bendian.h"
#include "../sofdec/adx_bpcm_setup.h"

/* ".snd", and the older ".sd" with a NUL, as ADX_LE32 reads them */
#define AU_MAGIC 0x646e732eUL
#define AU_MAGIC_OLD 0x0064732eUL
/* the header's encodings, by Sun's numbers */
#define AU_ULAW 1
#define AU_LINEAR8 2
#define AU_LINEAR16 3
/* the magic and the data's offset, which the reader reads before it checks
 * the offset against size */
#define AU_MIN_BYTES 8
/* the total for an encoding no branch reaches: more samples than any stream
 * holds */
#define ENDLESS 0x7fff0000

/* each mu-law code's 16-bit linear sample */
static Sint16 ulaw_table[256] = {
#include "sofdec/adx_bau/ulaw_table.inc"
};

static Uint8 *adxb_ParseAuHeader(Uint8 *header, Sint32 size, Sint32 *rate,
                                 Sint32 *channels, Sint32 *bits, Sint32 *total,
                                 Sint32 *encoding)
{
    Uint8 *p = header;
    Uint32 signature;
    Uint32 raw;
    Sint32 offset;
    Uint32 bytes;
    Uint32 format;

    signature = ADX_LE32(p);
    p += 4;
    if (signature != AU_MAGIC_OLD && signature != AU_MAGIC) {
        return NULL;
    }
    raw = ADX_LE32(p);
    p += 4;
    offset = ADX_SWAP32(raw);
    if (offset > size) {
        return NULL;
    }
    raw = ADX_LE32(p);
    bytes = ADX_SWAP32(raw);
    p += 4;
    raw = ADX_LE32(p);
    format = ADX_SWAP32(raw);
    p += 4;
    switch (format) {
    case AU_ULAW:
        *encoding = ADXB_ULAW;
        *bits = 8;
        break;
    case AU_LINEAR8:
        *encoding = ADXB_PCM8;
        *bits = 8;
        break;
    case AU_LINEAR16:
        *encoding = ADXB_PCM16;
        *bits = 16;
        break;
    default:
        return NULL;
    }
    *rate = ADX_LE32(p);
    *rate = ADX_SWAP32(*rate);
    p += 4;
    *channels = ADX_LE32(p);
    *channels = ADX_SWAP32(*channels);
    /* @hack spelling: a branch for each encoding and an else none reaches,
     * where one test for ADXB_PCM16 would do */
    if (*encoding == ADXB_ULAW) {
        *total = bytes / *channels;
    } else if (*encoding == ADXB_PCM8) {
        *total = bytes / *channels;
    } else if (*encoding == ADXB_PCM16) {
        *total = (bytes >> 1) / *channels;
    } else {
        *total = ENDLESS;
    }
    /* @hack type: offset added to header's address as a Uint32, where
     * header + offset would do */
    return (Uint8 *)((Uint32)header + offset);
}

Bool ADXB_CheckAu(const Sint8 *header)
{
    /* @hack spelling: an if that returns TRUE or FALSE, where returning the
     * test would do */
    if (memcmp(header, ".snd", 4) == 0 || memcmp(header, ".sd", 4) == 0) {
        return TRUE;
    }
    return FALSE;
}

static Sint32 ADX_DecodeInfoAu(Sint8 *header, Sint32 size, Sint16 *offset,
                               Sint8 *enc, Sint8 *bits, Sint8 *block_bytes,
                               Sint8 *channels, Sint32 *rate, Sint32 *total,
                               Sint32 *block_samples, Sint32 *encoding)
{
    Sint32 sample_rate;
    Sint32 channel_count;
    Sint32 sample_bits;
    Sint32 samples;
    Sint8 *data;
    Sint16 start;

    if (size < AU_MIN_BYTES) {
        *offset = 0;
        return -1;
    }
    data = (Sint8 *)adxb_ParseAuHeader((Uint8 *)header, size, &sample_rate,
                                       &channel_count, &sample_bits, &samples,
                                       encoding);
    if (!data) {
        return -1;
    }
    /* @hack redundant: the else after a return, where its block alone would
     * do */
    else {
        start = data - header;
        *offset = start;
        if (start <= 0) {
            return -1;
        }
    }
    *rate = sample_rate;
    *channels = channel_count;
    *bits = sample_bits;
    *total = samples;
    *enc = -1;
    *block_bytes = *channels * *bits / 8;
    *block_samples = 1;
    return 0;
}

Sint32 ADXB_DecodeHeaderAu(ADXB adxb, Sint8 *header, Sint32 size)
{
    Sint16 offset;
    Sint32 encoding;
    Sint32 status;

    adxb->unk_02 = 1;
    status = ADX_DecodeInfoAu(header, size, &offset, &adxb->enc, &adxb->bps,
                              &adxb->blklen, &adxb->nch, &adxb->sfreq,
                              &adxb->total, &adxb->blksmpl, &encoding);
    if (status < 0) {
        return 0;
    }
    ADXB_SETUP_PCM(adxb, ADXB_FORMAT_AU);
    adxb->pcm_encoding = encoding;
    memset(adxb->dly, 0, sizeof(adxb->dly));
    return offset;
}

#define PCM_FUNCTION static void ADXB_ExecOneAu16
#define PCM_INPUT Uint16
#define PCM_BYTES 2
#define PCM_BIG_ENDIAN
#define PCM_LEFT(input, i) ((input)[(i) * 2])
#define PCM_RIGHT(input, i) ((input)[(i) * 2 + 1])
#define PCM_MONO(input, i) ((input)[i])
#include "../sofdec/adx_bpcm_impl.h"

#define PCM_FUNCTION static void ADXB_ExecOneAu8
#define PCM_INPUT Uint8
#define PCM_BYTES 1
#define PCM_LEFT(input, i) ((input)[(i) * 2] << 8)
#define PCM_RIGHT(input, i) ((input)[(i) * 2 + 1] << 8)
#define PCM_MONO(input, i) ((input)[i] << 8)
#include "../sofdec/adx_bpcm_impl.h"

#define PCM_FUNCTION static void ADXB_ExecOneAuUlaw
#define PCM_INPUT Uint8
#define PCM_BYTES 1
#define PCM_LEFT(input, i) ulaw_table[(input)[(i) * 2]]
#define PCM_RIGHT(input, i) ulaw_table[(input)[(i) * 2 + 1]]
#define PCM_MONO(input, i) ulaw_table[(input)[i]]
#include "../sofdec/adx_bpcm_impl.h"

void ADXB_ExecOneAu(ADXB adxb)
{
    if (adxb->pcm_encoding == ADXB_ULAW) {
        ADXB_ExecOneAuUlaw(adxb);
    } else if (adxb->pcm_encoding == ADXB_PCM8) {
        ADXB_ExecOneAu8(adxb);
    } else {
        ADXB_ExecOneAu16(adxb);
    }
}
