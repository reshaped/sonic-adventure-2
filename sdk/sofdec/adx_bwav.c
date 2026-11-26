/* the basic decoder's RIFF WAVE reader, for 8-bit or 16-bit PCM or 4-bit
 * samples. 4-bit samples go to the rings packed, four to a 16-bit word */
#include <adx_bfmt.h>
#include <adx_bsc.h>
#include <sg_xpt.h>
#include <string.h>

#include "../sofdec/adx_bendian.h"
#include "../sofdec/adx_bpcm_setup.h"

#define CHUNK_ID_BYTES 4
#define CHUNK_HEADER_BYTES 8
/* the fmt chunk's tag for PCM. the reader takes tags 0 and 1 */
#define WAVE_FORMAT_PCM 1

struct wave_format {
    Uint16 format;
    Sint16 channels;
    Uint8 sample_rate[4];
    Uint8 bytes_per_second[4];
    Sint16 block_align;
    Sint16 bits;
};

/* the IDs of the chunks the reader finds, and the form's type */
static const char *format_id = "fmt ";
static const char *wave_id = "WAVE";
static const char *data_id = "data";

static Sint32 ADX_DecodeInfoWav(Uint8 *header, Sint32 size, Sint16 *offset,
                                Sint8 *enc, Sint8 *bits, Sint8 *block_bytes,
                                Sint8 *channels, Sint32 *rate, Sint32 *total,
                                Sint32 *block_samples, Sint16 *encoding)
{
    Sint32 i;
    const struct wave_format *format;
    const Uint8 *p;
    Sint32 bytes;

    /* @bug memory: this scan and the data scan compare 4 bytes at every i
     * below size, reading up to 3 bytes past the header. their memcmp calls
     * at 0x8c176ce2 and 0x8c176d36 are bounded only by the tests of i < size
     * at 0x8c176cec and 0x8c176d40 */
#ifdef NONMATCHING
    /* this side repairs the reads: size drops to the count of offsets where
     * a whole ID fits, so neither scan reads past the header */
    size -= CHUNK_ID_BYTES - 1;
#endif
    for (i = 0; i < size; i++) {
        if (memcmp(header + i, format_id, CHUNK_ID_BYTES) == 0) {
            break;
        }
    }
    if (i == size || i % 4 != 0) {
        return -1;
    }
    format = (const struct wave_format *)(header + (i + CHUNK_HEADER_BYTES));
    if (format->format > WAVE_FORMAT_PCM) {
        return -1;
    }
    for (i = 0; i < size; i++) {
        if (memcmp(header + i, data_id, CHUNK_ID_BYTES) == 0) {
            break;
        }
    }
    if (i == size) {
        return -1;
    }
    p = header + (i + CHUNK_ID_BYTES);
    bytes = ADX_LE32(p);
    *offset = i + CHUNK_HEADER_BYTES;
    *enc = -1;
    p = format->sample_rate;
    *rate = ADX_LE32(p);
    *channels = format->channels;
    *bits = format->bits;
    *block_bytes = format->block_align;
    /* @bug arithmetic: a block_align of 0 reaches the division at
     * 0x8c176dbe before the test at 0x8c176e24 refuses it */
    *total = bytes / *block_bytes;
    *block_samples = 1;
    if (*bits == 16) {
        *encoding = ADXB_PCM16;
    } else if (*bits == 8) {
        *encoding = ADXB_PCM8;
    } else if (*bits == 4) {
        *block_bytes = *channels * 2;
        *block_samples = 4;
        /* @bug arithmetic: 4-bit data of 0 channels reaches the division
         * at 0x8c176e08 before the test at 0x8c176e2a refuses it */
        *total = ((Uint32)bytes >> 1) / *channels;
        *bits = 16;
        *encoding = ADXB_PCM4;
    }
    /* @hack spelling: a test and a return for each field, where one test
     * of all four would do */
    if (*bits == 0) {
        return -1;
    }
    if (*block_bytes == 0) {
        return -1;
    }
    if (*channels <= 0 || *channels > 2) {
        return -1;
    }
    if (*rate == 0) {
        return -1;
    }
    return 0;
}

Sint32 ADXB_DecodeHeaderWav(ADXB adxb, Uint8 *header, Sint32 size)
{
    Sint16 offset;
    Sint32 status;

    adxb->unk_02 = 1;
    status =
        ADX_DecodeInfoWav(header, size, &offset, &adxb->enc, &adxb->bps,
                          &adxb->blklen, &adxb->nch, &adxb->sfreq, &adxb->total,
                          &adxb->blksmpl, &adxb->pcm_encoding);
    if (status < 0) {
        return 0;
    }
    ADXB_SETUP_PCM(adxb, ADXB_FORMAT_WAV);
    memset(adxb->dly, 0, sizeof(adxb->dly));
    return offset;
}

#define PCM_FUNCTION static void ADXB_ExecOneWav16
#define PCM_INPUT Sint16
#define PCM_BYTES 2
#define PCM_LEFT(input, i) ((input)[(i) * 2])
#define PCM_RIGHT(input, i) ((input)[(i) * 2 + 1])
#define PCM_MONO(input, i) ((input)[i])
#include "../sofdec/adx_bpcm_impl.h"

#define PCM_FUNCTION static void ADXB_ExecOneWav8
#define PCM_INPUT Uint8
#define PCM_BYTES 1
/* an 8-bit WAVE sample is unsigned, with silence at WAVE_ZERO */
#define WAVE_ZERO 128
#define WAVE_SAMPLE(byte) (((byte) - WAVE_ZERO) << 8)
#define PCM_LEFT(input, i) WAVE_SAMPLE((input)[(i) * 2])
#define PCM_RIGHT(input, i) WAVE_SAMPLE((input)[(i) * 2 + 1])
#define PCM_MONO(input, i) WAVE_SAMPLE((input)[i])
#include "../sofdec/adx_bpcm_impl.h"

/* two channels' words interleave by byte: both low bytes, then both high
 * bytes */
#define PCM_FUNCTION static void ADXB_ExecOneWav4
#define PCM_INPUT Uint8
#define PCM_BYTES 2
#define PCM_LEFT(input, i) (((input)[(i) * 4 + 2] << 8) | (input)[(i) * 4])
#define PCM_RIGHT(input, i) (((input)[(i) * 4 + 3] << 8) | (input)[(i) * 4 + 1])
#define PCM_MONO(input, i) (((input)[(i) * 2 + 1] << 8) | (input)[(i) * 2])
#include "../sofdec/adx_bpcm_impl.h"

Bool ADXB_CheckWav(const Uint8 *header)
{
    return memcmp(header, "RIFF", CHUNK_ID_BYTES) == 0 &&
           memcmp(header + CHUNK_HEADER_BYTES, "WAVE", CHUNK_ID_BYTES) == 0;
}

void ADXB_ExecOneWav(ADXB adxb)
{
    if (adxb->pcm_encoding == ADXB_PCM4) {
        ADXB_ExecOneWav4(adxb);
    } else if (adxb->pcm_encoding == ADXB_PCM8) {
        ADXB_ExecOneWav8(adxb);
    } else if (adxb->pcm_encoding == ADXB_PCM16) {
        ADXB_ExecOneWav16(adxb);
    }
}
