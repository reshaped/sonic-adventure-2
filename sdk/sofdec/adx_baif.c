/* the basic decoder's AIFF reader. COMM gives the format, with the rate as
 * an 80-bit float, and SSND the data's offset. the two may come in either
 * order */
#include <adx_bfmt.h>
#include <adx_bsc.h>
#include <sg_xpt.h>
#include <string.h>

#include "../sofdec/adx_bendian.h"
#include "../sofdec/adx_bpcm_setup.h"

/* "FORM", "AIFF", "COMM" and "SSND" as ADX_LE32 reads them */
#define FORM_ID 0x4d524f46UL
#define AIFF_ID 0x46464941UL
#define COMM_ID 0x4d4d4f43UL
#define SSND_ID 0x444e5353UL
/* an ID's bytes, which form_size counts for the form's AIFF_ID */
#define ID_BYTES 4
#define COMM_BYTES 18
/* the fewest bytes ADX_DecodeInfoAiff parses. adxb_ParseAiffHeader takes no
 * size of its own */
#define AIFF_MIN_BYTES 0x1000
/* the exponent of a rate whose integer part is its mantissa's top 16 bits.
 * it is the 80-bit format's bias, 0x3fff, plus 15 */
#define RATE_EXPONENT 0x400e
/* a chunk's bytes with the pad that makes them even */
#define PADDED(bytes) (((bytes) + 1) & ~1)

static Uint8 *adxb_ParseAiffHeader(Uint8 *header, Sint32 *rate,
                                   Sint32 *channels, Sint32 *bits,
                                   Sint32 *samples)
{
    Uint8 *p = header;
    Uint8 *end;
    Sint32 form;
    /* @bug uninitialized: a header without SSND returns data unset. the
     * load at 0x8c175e68 returns the frame word that held form, written at
     * 0x8c175bd8 */
#ifdef NONMATCHING
    /* this side repairs the result: data starts at NULL, so a header without
     * SSND returns NULL, which ADX_DecodeInfoAiff refuses */
    Uint8 *data = NULL;
#else
    Uint8 *data;
#endif
    Sint32 form_size;
    Sint32 id;
    Sint32 bytes;
    Uint32 offset;
    Bool have_format = FALSE;
    Bool have_sound = FALSE;
    Uint16 exponent;
    Uint16 mantissa;

    form = ADX_LE32(p);
    /* @hack spelling: p = p + 4, where p += 4 would do */
    p = p + 4;
    form_size = ADX_LE32(p);
    form_size = ADX_SWAP32(form_size);
    p += 4;
    id = ADX_LE32(p);
    p += 4;
    /* @hack spelling: a test of each ID, where one test of both would do */
    if (form != FORM_ID) {
        return NULL;
    }
    if (id != AIFF_ID) {
        return NULL;
    }
    end = p + form_size - ID_BYTES;
    while (p < end) {
        id = ADX_LE32(p);
        p += 4;
        bytes = ADX_LE32(p);
        bytes = ADX_SWAP32(bytes);
        p += 4;
        switch (id) {
        case COMM_ID:
            if (!have_format) {
                if (bytes < COMM_BYTES) {
                    return NULL;
                }
                *channels = ADX_LE16(p);
                *channels = ADX_SWAP16(*channels);
                p += 2;
                *samples = ADX_LE32(p);
                *samples = ADX_SWAP32(*samples);
                p += 4;
                *bits = ADX_LE16(p);
                *bits = ADX_SWAP16(*bits);
                p += 2;
                exponent = ADX_LE16(p);
                exponent = ADX_SWAP16(exponent);
                p += 2;
                mantissa = ADX_LE16(p);
                mantissa = ADX_SWAP16(mantissa);
                *rate = mantissa >> (RATE_EXPONENT - exponent);
                p += 8;
                have_format = TRUE;
                /* @hack redundant: the test of have_format, just set, where
                 * have_sound alone would do */
                if (have_format && have_sound) {
                    return data;
                }
            }
            break;
        case SSND_ID:
            if (!have_sound) {
                /* @hack spelling: the offset read through sound, a copy of
                 * p, where p would do */
                Uint8 *sound = p;

                offset = ADX_LE32(sound);
                offset = ADX_SWAP32(offset);
                sound += 4;
                data = sound + offset;
                p = sound;
                have_sound = TRUE;
                /* @hack redundant: the test of have_sound, just set, where
                 * have_format alone would do */
                if (have_format && have_sound) {
                    return data;
                }
            }
            break;
        default:
            p += PADDED(bytes);
            break;
        }
    }
    return data;
}

Bool ADXB_CheckAiff(const Sint8 *header)
{
    return memcmp(header, "FORM", 4) == 0 && memcmp(header + 8, "AIFF", 4) == 0;
}

static Sint32 ADX_DecodeInfoAiff(Sint8 *header, Sint32 size, Sint16 *offset,
                                 Sint8 *enc, Sint8 *bits, Sint8 *block_bytes,
                                 Sint8 *channels, Sint32 *rate, Sint32 *total,
                                 Sint32 *block_samples)
{
    Sint32 sample_rate;
    Sint32 channel_count;
    Sint32 sample_bits;
    Sint32 samples;
    Sint8 *data;

    if (size < AIFF_MIN_BYTES) {
        *offset = 0;
        return -1;
    }
    data = (Sint8 *)adxb_ParseAiffHeader(
        (Uint8 *)header, &sample_rate, &channel_count, &sample_bits, &samples);
    if (!data) {
        return -1;
    }
    /* @hack spelling: the store of *offset inside the test, where a
     * statement of its own would do */
    if ((*offset = data - header) <= 0) {
        return -1;
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

Sint32 ADXB_DecodeHeaderAiff(ADXB adxb, Sint8 *header, Sint32 size)
{
    Sint16 offset;
    Sint32 status;

    adxb->unk_02 = 1;
    status = ADX_DecodeInfoAiff(header, size, &offset, &adxb->enc, &adxb->bps,
                                &adxb->blklen, &adxb->nch, &adxb->sfreq,
                                &adxb->total, &adxb->blksmpl);
    if (status < 0) {
        return 0;
    }
    ADXB_SETUP_PCM(adxb, ADXB_FORMAT_AIFF);
    if (adxb->bps == 8) {
        adxb->pcm_encoding = ADXB_PCM8;
    } else {
        adxb->pcm_encoding = ADXB_PCM16;
    }
    memset(adxb->dly, 0, sizeof(adxb->dly));
    return offset;
}

#define PCM_FUNCTION static void ADXB_ExecOneAiff16
#define PCM_INPUT Uint16
#define PCM_BYTES 2
#define PCM_BIG_ENDIAN
#define PCM_LEFT(input, i) ((input)[(i) * 2])
#define PCM_RIGHT(input, i) ((input)[(i) * 2 + 1])
#define PCM_MONO(input, i) ((input)[i])
#include "../sofdec/adx_bpcm_impl.h"

#define PCM_FUNCTION static void ADXB_ExecOneAiff8
#define PCM_INPUT Uint8
#define PCM_BYTES 1
#define PCM_LEFT(input, i) ((input)[(i) * 2] << 8)
#define PCM_RIGHT(input, i) ((input)[(i) * 2 + 1] << 8)
#define PCM_MONO(input, i) ((input)[i] << 8)
#include "../sofdec/adx_bpcm_impl.h"

void ADXB_ExecOneAiff(ADXB adxb)
{
    if (adxb->pcm_encoding == ADXB_PCM8) {
        ADXB_ExecOneAiff8(adxb);
    } else {
        ADXB_ExecOneAiff16(adxb);
    }
}
