/* the basic decoder's SPSD reader. every format's data goes to the rings as
 * 16-bit words, and pcm_encoding keeps its width for ADXB_GetOutBps */
#include <adx_bfmt.h>
#include <adx_bsc.h>
#include <sg_xpt.h>
#include <string.h>

#include "../sofdec/adx_bpcm_setup.h"

/* the header's format byte. 2 and 3 both hold 4-bit samples */
#define SPSD_16BIT 0
#define SPSD_8BIT 1
#define SPSD_4BIT 2
#define SPSD_4BIT_ALT 3
/* a paragraph is 16 bytes */
#define PARAGRAPH_SHIFT 4
/* the bits of the channels byte that hold the channels less 1 */
#define CHANNELS_MASK 3

struct spsd_header {
    char id[4];
    Uint8 reserved04[3];
    Uint8 data_paragraph; /* the data's offset in paragraphs */
    Uint8 format;
    Uint8 channels;
    Uint8 reserved0a[2];
    Sint32 bytes;
    Uint8 reserved10[26];
    Uint16 sample_rate;
};

static Sint32 ADX_DecodeInfoSpsd(struct spsd_header *header, Sint32 size,
                                 Sint16 *offset, Sint8 *enc, Sint8 *bits,
                                 Sint8 *block_bytes, Sint8 *channels,
                                 Sint32 *rate, Sint32 *total,
                                 Sint32 *block_samples, Sint16 *encoding)
{
    /* @hack spelling: data_paragraph and channels read through p, a copy of
     * header, where header would do */
    struct spsd_header *p = header;
    Sint32 *bytes;
    Uint8 format;

    *offset = p->data_paragraph << PARAGRAPH_SHIFT;
    *channels = (p->channels & CHANNELS_MASK) + 1;
    *rate = header->sample_rate;
    format = header->format;
    /* @hack spelling: bytes set by a comma in the switch's expression, where
     * a statement of its own would do */
    switch (bytes = &header->bytes, format) {
    case SPSD_16BIT:
        *bits = 16;
        *block_bytes = *channels * 2;
        *block_samples = 1;
        *total = *bytes / 2;
        *encoding = ADXB_PCM16;
        break;
    case SPSD_8BIT:
        *bits = 8;
        *block_bytes = *channels;
        *block_samples = 1;
        *total = *bytes;
        *encoding = ADXB_PCM8;
        break;
    case SPSD_4BIT:
    case SPSD_4BIT_ALT:
        *bits = 4;
        *block_bytes = *channels;
        *block_samples = 2;
        *total = *bytes * 2;
        *encoding = ADXB_PCM4;
        break;
    }
    /* these replace every store of the switch but *encoding's */
    *block_bytes = 2;
    *block_samples = 1;
    *total = *bytes / 2;
    *bits = 16;
    *enc = -1;
    return 0;
}

Sint32 ADXB_DecodeHeaderSpsd(ADXB adxb, Uint8 *header, Sint32 size)
{
    Sint16 offset;
    Sint32 status;

    adxb->unk_02 = 1;
    status = ADX_DecodeInfoSpsd((struct spsd_header *)header, size, &offset,
                                &adxb->enc, &adxb->bps, &adxb->blklen,
                                &adxb->nch, &adxb->sfreq, &adxb->total,
                                &adxb->blksmpl, &adxb->pcm_encoding);
    if (status < 0) {
        return 0;
    }
    ADXB_SETUP_PCM(adxb, ADXB_FORMAT_SPSD);
    memset(adxb->dly, 0, sizeof(adxb->dly));
    return offset;
}

#define PCM_FUNCTION void ADXB_ExecOneSpsd
#define PCM_INPUT Sint16
#define PCM_BYTES 2
#define PCM_LEFT(input, i) ((input)[(i) * 2])
#define PCM_RIGHT(input, i) ((input)[(i) * 2 + 1])
#define PCM_MONO(input, i) ((input)[i])
#include "../sofdec/adx_bpcm_impl.h"

Bool ADXB_CheckSpsd(const Uint8 *header)
{
    /* @hack spelling: an if that returns TRUE or FALSE, where returning the
     * test would do */
    if (memcmp(header, "SPSD", 4) == 0) {
        return TRUE;
    }
    return FALSE;
}
