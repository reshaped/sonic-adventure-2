/* the MPEG audio decoder that AHX data plays through. it decodes each MPEG-1
 * or MPEG-2 frame as layer II, a granule at a time */
#include <ahx_header.h>
#include <ahxbsr.h>
#include <mpeg_alloc.h>
#include <mpeg_audio.h>
#include <mpeg_synth.h>
#include <mpeg_timer.h>
#include <mw_sdsb.h>
#include <sg_xpt.h>
#include <string.h>

#define MPEG_DEC_MAX 2
/* the channels of a mono frame and of a stereo one */
#define NCH_MONO 1
#define NCH_STEREO 2
/* a granule holds SUBBAND_SAMPLES consecutive samples of each subband. a
 * frame holds FRAME_GRANULES granules */
#define SUBBAND_SAMPLES 3
#define GRANULE_SAMPLES (SUBBAND_SAMPLES * SBLIMIT)
#define FRAME_GRANULES 12
/* a frame's granules fall in SCALE_PARTS parts of 1 << PART_SHIFT granules.
 * a subband has a scale factor for each part */
#define SCALE_PARTS 3
#define PART_SHIFT 2
/* the scale factor index of a subband that codes no samples */
#define SCALE_SILENT 63
/* the bits of a scfsi, and of a scale factor index */
#define SCFSI_BITS 2
#define SCALE_BITS 6
#define MPEG_SAMPLE_BITS 16
/* the header's modes for joint stereo and for one channel */
#define MODE_JOINT 1
#define MODE_MONO 3
/* a quantizer's group where each sample takes a code of its own */
#define GROUP_NONE 3
/* the bits of a frame header's fields after its sync word, in order. a layer
 * code c stands for layer LAYER_CODES - c */
#define VERSION_BITS 1
#define LAYER_BITS 2
#define PROTECTION_BITS 1
#define BITRATE_BITS 4
#define FREQ_BITS 2
#define PADDING_BITS 1
#define EXTENSION_BITS 1
#define MODE_BITS 2
#define MODE_EXT_BITS 2
#define COPYRIGHT_BITS 1
#define ORIGINAL_BITS 1
#define EMPHASIS_BITS 2
#define LAYER_CODES 4
/* the layer this decoder is for */
#define LAYER_II 2
/* the hertz in a kilohertz */
#define HZ_PER_KHZ 1000.0F
/* the boundary a decoder's fraction starts at */
#define FRACTION_ALIGN 32
/* the grouped quantizers of 3, 5 and 9 steps read 5, 7 and 10 bits a code.
 * these are the codes those bits hold */
#define GROUP3_CODES 32
#define GROUP5_CODES 128
#define GROUP9_CODES 1024
/* the quantizers quant_c and quant_d hold an entry for */
#define QUANTS 17
/* the most bits of a GROUP_NONE code for which mpeg_decode_samples reads a
 * subband's three codes in one read */
#define PACKED_BITS_MAX 8
/* the effect level mpeg_fx gives where the stream header gives none, the
 * level a sound buffer opens with */
#define FXLEV_NONE MW_SB_FX_LVL_OPEN
/* the stream header holds a pan at 1 << PAN_SHIFT times its setting.
 * PAN_LEFT and PAN_RIGHT are the hard left and right pans. a stereo frame's
 * channels 0 and 1 take them where the header holds none */
#define PAN_SHIFT 3
#define PAN_LEFT (-MW_SB_PAN_MAX)
#define PAN_RIGHT MW_SB_PAN_MAX

/* a frame's header. each field holds its bits as the header codes them, but
 * for layer and error_protection */
struct mpeg_header {
    Sint32 version; /* 1 for MPEG-1, 0 for MPEG-2 */
    Sint32 layer; /* 1 to 3, or 4 for the reserved code 0 */
    /* 1 where a CRC follows the header. mpeg_read_frame_header reads no CRC,
     * so it decodes only frames without one */
    Sint32 error_protection;
    Sint32 bitrate_index;
    Sint32 sampling_frequency;
    Sint32 padding;
    Sint32 extension;
    Sint32 mode;
    Sint32 mode_extension;
    Sint32 copyright;
    Sint32 original;
    Sint32 emphasis;
};

/* what a frame's decode takes from the frame's header and the stream's own.
 * where has_info is set, channels and sample_rate are the stream header's,
 * and otherwise the frame header's */
struct mpeg_frame {
    Bool has_info; /* mpeg_read_stream_header read the stream header */
    Sint32 channels;
    Sint32 sample_rate; /* in hertz */
    Sint32 total_samples; /* the stream header's, or 0 */
    Sint32 sblimit; /* the subbands the frame codes */
    Sint32 jsbound; /* the first subband whose samples the channels share */
    MPEG_ALLOC_TABLE *alloc;
    struct mpeg_channel channel[NCH_STEREO]; /* the stream header's settings */
};

struct mpeg_decoder {
    /* the granule's subband samples by channel, sample and subband, at
     * fraction_store's first FRACTION_ALIGN boundary */
    Float (*fraction)[SUBBAND_SAMPLES][SBLIMIT];
    Uint8 fraction_store[(NCH_STEREO * GRANULE_SAMPLES * sizeof(Float)) +
                         FRACTION_ALIGN];
    Uint8 used; /* mpeg_create handed the decoder out */
    Uint8 ready; /* mpeg_read_frame_header read a frame's header */
    Sint32 granule; /* the frame's granules decoded */
    AHXBSR bsr;
    MPEG_SYNTH *synth;
    struct mpeg_header header;
    struct mpeg_frame frame;
    Uint32 bit_alloc[NCH_STEREO][SBLIMIT];
    Uint32 scfsi[NCH_STEREO][SBLIMIT];
    Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT];
    Uint32 unk_8a0[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT];
};

/* the member exports these functions, though no header declares them.
 * nothing calls the _args forms, mpeg_buffer_sample, mpeg_denormalize_sample
 * or the three dequantizers. mpeg_x_args takes as arguments the fields of fr
 * that mpeg_x reads */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the samples of each channel in a granule, and in a frame */
Sint32 mpeg_granule_samples(void);
Sint32 mpeg_frame_samples(void);
/* read the rest of a frame's header, once ahxbsr_find_frame has read its sync
 * word */
void mpeg_decode_info(AHXBSR bsr, struct mpeg_header *hdr);
/* point *alloc at allocation table 0, whatever hdr and channels, and return
 * the subbands the table codes */
Sint32 mpeg_pick_table(struct mpeg_header *hdr, Sint32 channels,
                       MPEG_ALLOC_TABLE **alloc);
/* set fr's subbands, joint stereo bound and table from hdr. fr's channels and
 * sample rate come from hdr only where has_info is clear. a layer II frame
 * takes mpeg_pick_table's table and subbands. a frame of another layer takes
 * SBLIMIT subbands and keeps fr's table. the sample rate is s_freq's entry
 * times 1000, truncated. the 22.05 and 44.1 kilohertz entries give 22050
 * and 44100 hertz with round to nearest, or 22049 and 44099 hertz with
 * round toward zero */
void mpeg_set_frame(struct mpeg_header *hdr, struct mpeg_frame *fr);
/* read a frame's bit allocations. below jsbound, each subband takes a code for
 * each channel. from jsbound to sblimit, each takes one code that both
 * channels share. each channel's codes from sblimit on are 0 */
void mpeg_decode_bitalloc_args(AHXBSR bsr, Sint32 channels, Sint32 sblimit,
                               Sint32 jsbound, MPEG_ALLOC_TABLE *table,
                               Uint32 bit_alloc[NCH_STEREO][SBLIMIT]);
void mpeg_decode_bitalloc(AHXBSR bsr, struct mpeg_frame *fr,
                          Uint32 bit_alloc[NCH_STEREO][SBLIMIT]);
/* read a frame's scfsi, its scale factor selections, and then its scale
 * factor indexes. each channel's subband below sblimit that has an allocation
 * reads both. every other subband takes SCALE_SILENT for each part, and each
 * scfsi from sblimit on is 0. a subband's scfsi says which of its parts share
 * an index:
 *
 *   0  none
 *   1  parts 0 and 1
 *   2  all three
 *   3  parts 1 and 2 */
void mpeg_decode_scale_args(
    AHXBSR bsr, Uint32 bit_alloc[NCH_STEREO][SBLIMIT], Sint32 channels,
    Sint32 sblimit, Uint32 scfsi[NCH_STEREO][SBLIMIT],
    Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT]);
void mpeg_decode_scale(AHXBSR bsr, struct mpeg_frame *fr,
                       Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
                       Uint32 scfsi[NCH_STEREO][SBLIMIT],
                       Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT]);
/* read a granule's codes into sample, by channel, sample and subband. a
 * GROUP_NONE quantizer codes each sample in its bits. any other quantizer
 * codes the subband's three samples as one number in its bits. sample k is
 * the number's digit k in base steps, the quantizer's steps. a subband
 * without an allocation, and each from sblimit on, takes 0. from jsbound on,
 * channel 1 takes channel 0's codes. mpeg_buffer_sample also clears
 * ahxbsr_read_count, and times each read into mpeg_read_ticks and
 * mpeg_read_total */
void mpeg_buffer_sample_args(
    AHXBSR bsr, Uint32 bit_alloc[NCH_STEREO][SBLIMIT], Sint32 channels,
    Sint32 sblimit, Sint32 jsbound, MPEG_ALLOC_TABLE *table,
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT]);
void mpeg_buffer_sample(AHXBSR bsr, struct mpeg_frame *fr,
                        Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
                        Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT]);
/* multiply each channel's values in fraction below sblimit by their
 * subband's scale factor for part */
void mpeg_denormalize_sample_args(
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT], Sint32 sblimit,
    Sint32 channels, Sint32 part);
void mpeg_denormalize_sample(
    struct mpeg_frame *fr, Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT],
    Sint32 part, Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT]);
/* store in fraction the value, from -1 to 1, that each of a granule's codes
 * stands for. code s stands for (2s + 1) / steps - 1, with steps the
 * quantizer's. that value is the middle of interval s when -1 to 1 is cut
 * into steps equal intervals. mpeg_dequantize_sample computes it so.
 * mpeg_dequantize_sample_args computes it from quant_c and quant_d, which
 * give the same value but for rounding. a subband without an allocation, and
 * each from sblimit on, takes 0 */
void mpeg_dequantize_sample_args(
    Sint32 channels, Sint32 sblimit,
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    MPEG_ALLOC_TABLE *table);
void mpeg_dequantize_sample(
    struct mpeg_frame *fr, Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT]);
/* do as mpeg_dequantize_sample and then mpeg_denormalize_sample do, in one
 * pass. code s becomes (c * s + d) * scale. c and d are the fields of the
 * code's struct mpeg_quantizer, not quant_c's and quant_d's entries. scale is
 * the subband's scale factor for part. FMAC rounds c * s + d once, and FMUL
 * rounds the product with scale */
void mpeg_dequantize_scale_sample(
    struct mpeg_frame *fr, Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT],
    Sint32 part, Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT]);
/* read a granule's codes as mpeg_buffer_sample does. store each sample's code
 * s in fraction as (c * scale) * s + d * scale. c, d and scale are those
 * of mpeg_dequantize_scale_sample. FMUL rounds c * scale and d * scale, and
 * FMAC rounds the value once. two defects its body tags set its results apart
 * from the three steps'. a code of 5 steps gives its samples from the wrong
 * digits. channel 1 of a joint stereo subband takes channel 0's scale
 * factors. the call clears mpeg_read_total and ahxbsr_read_count, and times
 * nothing */
void mpeg_decode_samples(AHXBSR bsr, struct mpeg_frame *fr,
                         Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
                         Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT],
                         Sint32 part,
                         Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT]);
/* NOLINTEND(misc-use-internal-linkage) */

static MPEG_DEC mpeg_dec[MPEG_DEC_MAX];
/* the stream's own header, as mpeg_read_stream_header last read it */
static Uint8 mpeg_stream_header[MPEG_STREAM_HEADER_MAX];
/* the slots of mpeg_granule_ticks, the timer's counts at the steps of the
 * last mpeg_decode_granule. slots 1 and 2 hold no step, and no code reads any
 * slot */
enum {
    TICK_START = 0, /* before the granule's codes are read */
    TICK_READ = 3, /* after they are read */
    TICK_LEFT = 4, /* after channel 0's synthesis */
    TICK_RIGHT = 5, /* after the granule's last synthesis */
    GRANULE_TICKS = 6
};
static Uint32 mpeg_granule_ticks[GRANULE_TICKS];
/* the timer's counts before and after mpeg_buffer_sample's last read.
 * mpeg_read_total holds the counts that all the granule's reads took */
static Uint32 mpeg_read_ticks[2];
static Uint32 mpeg_read_total;

/* the sampling frequencies in kilohertz, by version and sampling_frequency */
static Float s_freq[2][4] = {
    {22.05F, 24.0F, 16.0F, 0.0F},
    {44.1F, 48.0F, 32.0F, 0.0F},
};

/* the bitrates in kilobits a second, by version, layer - 1 and
 * bitrate_index */
static Sint32 bitrate[2][3][15] = {
    {
        {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256},
        {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},
        {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},
    },
    {
        {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448},
        {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384},
        {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320},
    },
};

/* the joint stereo bounds, by layer - 1 and mode_extension */
static Sint32 jsb_table[3][4] = {
    {4, 8, 12, 16},
    {4, 8, 12, 16},
    {0, 4, 8, 16},
};

/* the scale factors: index i stands for 2^(1 - i / 3), but SCALE_SILENT for
 * 1e-20 */
#define SCALES(q)                                                              \
    2.0F / (1L << (q)), 1.587401F / (1L << (q)), 1.2599211F / (1L << (q))
static Float scale_factors[1 << SCALE_BITS] = {
    SCALES(0),  SCALES(1),  SCALES(2),  SCALES(3),  SCALES(4),  SCALES(5),
    SCALES(6),  SCALES(7),  SCALES(8),  SCALES(9),  SCALES(10), SCALES(11),
    SCALES(12), SCALES(13), SCALES(14), SCALES(15), SCALES(16), SCALES(17),
    SCALES(18), SCALES(19), SCALES(20), 1e-20F,
};

/* the mpeg_init calls that no mpeg_finish has closed */
static Sint32 mpeg_init_count = 0;

/* group3, group5 and group9 split a grouped code of 3, 5 or 9 steps into its
 * three samples. row k holds each code's sample k, its digit k in base steps.
 * row 2 holds the code divided by steps^2, with no remainder taken. so a code
 * of steps^3 or more gives steps or more there */
#define DIGIT(steps, k, c)                                                     \
    ((k) == 0   ? (c) % (steps)                                                \
     : (k) == 1 ? (c) / (steps) % (steps)                                      \
                : (c) / ((steps) * (steps)))
#define DIGITS_4(steps, k, c)                                                  \
    DIGIT(steps, k, c), DIGIT(steps, k, (c) + 1), DIGIT(steps, k, (c) + 2),    \
        DIGIT(steps, k, (c) + 3)
#define DIGITS_32(steps, k, c)                                                 \
    DIGITS_4(steps, k, c), DIGITS_4(steps, k, (c) + 4),                        \
        DIGITS_4(steps, k, (c) + 8), DIGITS_4(steps, k, (c) + 12),             \
        DIGITS_4(steps, k, (c) + 16), DIGITS_4(steps, k, (c) + 20),            \
        DIGITS_4(steps, k, (c) + 24), DIGITS_4(steps, k, (c) + 28)
#define DIGITS_128(steps, k, c)                                                \
    DIGITS_32(steps, k, c), DIGITS_32(steps, k, (c) + 32),                     \
        DIGITS_32(steps, k, (c) + 64), DIGITS_32(steps, k, (c) + 96)
#define DIGITS_1024(steps, k)                                                  \
    DIGITS_128(steps, k, 0), DIGITS_128(steps, k, 128),                        \
        DIGITS_128(steps, k, 256), DIGITS_128(steps, k, 384),                  \
        DIGITS_128(steps, k, 512), DIGITS_128(steps, k, 640),                  \
        DIGITS_128(steps, k, 768), DIGITS_128(steps, k, 896)
static Sint8 group3[SUBBAND_SAMPLES][GROUP3_CODES] = {
    {DIGITS_32(3, 0, 0)},
    {DIGITS_32(3, 1, 0)},
    {DIGITS_32(3, 2, 0)},
};
static Sint8 group5[SUBBAND_SAMPLES][GROUP5_CODES] = {
    {DIGITS_128(5, 0, 0)},
    {DIGITS_128(5, 1, 0)},
    {DIGITS_128(5, 2, 0)},
};
static Sint8 group9[SUBBAND_SAMPLES][GROUP9_CODES] = {
    {DIGITS_1024(9, 0)},
    {DIGITS_1024(9, 1)},
    {DIGITS_1024(9, 2)},
};

/* ISO/IEC 11172-3's dequantization, by a quantizer's quant. code s stands for
 * quant_c[quant] * (s / 2^(x - 1) - 1 + quant_d[quant]). 2^x is the least
 * power of 2 at or above the quantizer's steps */
static Float quant_c[QUANTS] = {
    1.33333333333F, 1.60000000000F, 1.14285714286F, 1.77777777777F,
    1.06666666666F, 1.03225806452F, 1.01587301587F, 1.00787401575F,
    1.00392156863F, 1.00195694716F, 1.00097751711F, 1.00048851979F,
    1.00024420024F, 1.00012208522F, 1.00006103888F, 1.00003051851F,
    1.00001525902F,
};
static Float quant_d[QUANTS] = {
    0.50000000000F, 0.50000000000F, 0.25000000000F, 0.50000000000F,
    0.12500000000F, 0.06250000000F, 0.03125000000F, 0.01562500000F,
    0.00781250000F, 0.00390625000F, 0.00195312500F, 0.00097656250F,
    0.00048828125F, 0.00024414063F, 0.00012207031F, 0.00006103516F,
    0.00003051758F,
};

/* low_bits[n] masks the low n bits of a word */
static Sint32 low_bits[PACKED_BITS_MAX + 1] = {
    0, 1, 3, 7, 15, 31, 63, 127, 255,
};

void mpeg_init(void)
{
    if (mpeg_init_count == 0) {
        mpeg_synth_init();
        memset(mpeg_dec, 0, sizeof(mpeg_dec));
    }
    mpeg_init_count++;
}

void mpeg_finish(void)
{
    mpeg_init_count--;
    if (mpeg_init_count == 0) {
        memset(mpeg_dec, 0, sizeof(mpeg_dec));
        mpeg_synth_finish();
    }
}

MPEG_DEC *mpeg_create(void)
{
    MPEG_DEC *dec;
    Sint32 i;

    for (i = 0; i < MPEG_DEC_MAX; i++) {
        dec = &mpeg_dec[i];
        if (dec->used == FALSE) {
            break;
        }
    }
    if (i == MPEG_DEC_MAX) {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((dec->synth = mpeg_synth_create()) == NULL) {
        return NULL;
    }
    dec->fraction = (Float(*)[SUBBAND_SAMPLES][SBLIMIT])(
        ((Uint32)dec->fraction_store + FRACTION_ALIGN - 1) &
        ~(FRACTION_ALIGN - 1));
    dec->used = TRUE;
    return dec;
}

void mpeg_destroy(MPEG_DEC *dec)
{
    if (dec->synth != NULL) {
        mpeg_synth_destroy(dec->synth);
    }
    memset(dec, 0, sizeof(*dec));
    /* @hack redundant: the store that clears used, which memset has already
     * cleared */
    dec->used = FALSE;
}

void mpeg_reset(MPEG_DEC *dec)
{
    memset(&dec->header, 0, sizeof(dec->header));
    memset(&dec->frame, 0, sizeof(dec->frame));
    memset(dec->bit_alloc, 0, sizeof(dec->bit_alloc));
    memset(dec->scfsi, 0, sizeof(dec->scfsi));
    memset(dec->scale_index, 0, sizeof(dec->scale_index));
    memset(dec->fraction, 0, NCH_STEREO * sizeof(*dec->fraction));
    memset(dec->unk_8a0, 0, sizeof(dec->unk_8a0));
    dec->ready = FALSE;
    dec->granule = 0;
    if (dec->synth != NULL) {
        mpeg_synth_destroy(dec->synth);
        dec->synth = mpeg_synth_create();
    }
}

void mpeg_set_stream(MPEG_DEC *dec, AHXBSR bsr)
{
    mpeg_reset(dec);
    dec->bsr = bsr;
}

void mpeg_read_stream_header(MPEG_DEC *dec)
{
    AHXBSR bsr;
    struct mpeg_frame *fr;
    Uint8 *p;
    Uint8 *end;
    Sint32 length;
    struct mpeg_stream_info info;
    Sint32 result;

    bsr = dec->bsr;
    fr = &dec->frame;
    if (bsr == NULL) {
        return;
    }
    memset(&info, 0, sizeof(info));
    length = 0;
    p = mpeg_stream_header;
    end = p + AHX_HEADER_LEAD;
    while (p != end) {
        *p++ = AHXBSR_GetBitStm(bsr, 8);
    }
    ahx_read_header(mpeg_stream_header, AHX_HEADER_LEAD, &length, NULL);
    /* @hack spelling: end = end + length, where end += length would do */
    end = end + length;
    while (p != end) {
        *p++ = AHXBSR_GetBitStm(bsr, 8);
    }
    /* @hack spelling: result, a local for the call's result, where the call
     * in the test would do */
    result = ahx_read_header(mpeg_stream_header, MPEG_STREAM_HEADER_MAX,
                             &length, &info);
    if (result >= 0) {
        fr->channels = info.channels;
        fr->sample_rate = info.sample_rate;
        fr->total_samples = info.total_samples;
        fr->channel[0] = info.channel[0];
        fr->channel[1] = info.channel[1];
        fr->has_info = TRUE;
    }
}

Sint32 mpeg_read_frame_header(MPEG_DEC *dec)
{
    Sint32 result;

    if (dec->bsr != NULL) {
        /* @hack spelling: result, a local for the call's result, where the
         * call in the test would do */
        result = ahxbsr_find_frame(dec->bsr);
        if (result != AHXBSR_SYNC) {
            return 1;
        }
        mpeg_decode_info(dec->bsr, &dec->header);
        mpeg_set_frame(&dec->header, &dec->frame);
        mpeg_decode_bitalloc(dec->bsr, &dec->frame, dec->bit_alloc);
        mpeg_decode_scale(dec->bsr, &dec->frame, dec->bit_alloc, dec->scfsi,
                          dec->scale_index);
        dec->ready = TRUE;
        dec->granule = 0;
    }
    return 0;
}

Bool mpeg_frame_done(MPEG_DEC *dec)
{
    /* @hack spelling: the result returned by two returns, where returning the
     * comparison would do */
    if (dec->granule == FRAME_GRANULES) {
        return TRUE;
    }
    return FALSE;
}

Sint32 mpeg_decode_granule(MPEG_DEC *dec, Sint16 *left, Sint16 *right,
                           Sint32 samples)
{
    Sint32 channels;
    Sint32 k;

    /* @hack spelling: channels, read ahead of the test that may return, where
     * dec->frame.channels read at its use would do */
    channels = dec->frame.channels;
    if (dec->granule == FRAME_GRANULES) {
        return 0;
    }
    mpeg_granule_ticks[TICK_START] = mpeg_timer_count();
    mpeg_decode_samples(dec->bsr, &dec->frame, dec->bit_alloc, dec->scale_index,
                        dec->granule >> PART_SHIFT, dec->fraction);
    mpeg_granule_ticks[TICK_READ] = mpeg_timer_count();
    for (k = 0; k < SUBBAND_SAMPLES; k++) {
        mpeg_synth_pcm(dec->synth, dec->fraction[0][k], 0,
                       left + (k * SBLIMIT));
    }
    mpeg_granule_ticks[TICK_LEFT] = mpeg_timer_count();
    if (channels >= NCH_STEREO) {
        for (k = 0; k < SUBBAND_SAMPLES; k++) {
            mpeg_synth_pcm(dec->synth, dec->fraction[1][k], 1,
                           right + (k * SBLIMIT));
        }
    }
    mpeg_granule_ticks[TICK_RIGHT] = mpeg_timer_count();
    dec->granule++;
    return GRANULE_SAMPLES;
}

Sint32 mpeg_sample_rate(MPEG_DEC *dec)
{
    if (!dec->ready) {
        return 0;
    }
    return dec->frame.sample_rate;
}

Sint32 mpeg_channels(MPEG_DEC *dec)
{
    if (!dec->ready) {
        return 0;
    }
    return dec->frame.channels;
}

Sint32 mpeg_sample_bits(MPEG_DEC *dec)
{
    return MPEG_SAMPLE_BITS;
}

Sint32 mpeg_block_samples(MPEG_DEC *dec)
{
    return mpeg_granule_samples();
}

Sint32 mpeg_total_samples(MPEG_DEC *dec)
{
    if (!dec->ready) {
        return 0;
    }
    return dec->frame.total_samples;
}

Sint32 mpeg_granule_samples(void)
{
    return GRANULE_SAMPLES;
}

Sint32 mpeg_frame_samples(void)
{
    return mpeg_granule_samples() * FRAME_GRANULES;
}

Sint32 mpeg_out_vol(MPEG_DEC *dec)
{
    /* @hack spelling: the two tests in two ifs, where one if that joins them
     * by ||, as mpeg_drct_lvl's does, would do */
    if (!dec->ready) {
        return 0;
    }
    if (!dec->frame.channel[0].out_vol_present) {
        return 0;
    }
    return dec->frame.channel[0].out_vol;
}

Sint32 mpeg_drct_lvl(MPEG_DEC *dec)
{
    if (!dec->ready || !dec->frame.channel[0].option[OPTION_DRCT_LVL].present) {
        return 0;
    }
    return dec->frame.channel[0].option[OPTION_DRCT_LVL].value;
}

void mpeg_fx(MPEG_DEC *dec, Sint32 *fxch, Sint32 *fxlev)
{
    *fxch = 0;
    *fxlev = FXLEV_NONE;
    /* @hack spelling: ready compared with 0, where !dec->ready, as the other
     * functions test it, would do */
    if (dec->ready == 0) {
        return;
    }
    if (dec->frame.channel[0].option[OPTION_FXCH].present > 0) {
        *fxch = dec->frame.channel[0].option[OPTION_FXCH].value;
    }
    if (dec->frame.channel[0].option[OPTION_FXLEV].present > 0) {
        *fxlev = dec->frame.channel[0].option[OPTION_FXLEV].value;
    }
}

Sint32 mpeg_channel_pan(MPEG_DEC *dec, Sint32 channel)
{
    if (dec->ready) {
        if (dec->frame.channels == NCH_STEREO) {
            if (channel == 1) {
                if (dec->frame.channel[1].option[OPTION_PAN].present > 0) {
                    return dec->frame.channel[1].option[OPTION_PAN].value >>
                           PAN_SHIFT;
                }
                return PAN_RIGHT;
            }
            if (dec->frame.channel[0].option[OPTION_PAN].present > 0) {
                return dec->frame.channel[0].option[OPTION_PAN].value >>
                       PAN_SHIFT;
            }
            return PAN_LEFT;
        }
        if (dec->frame.channel[0].option[OPTION_PAN].present > 0) {
            return dec->frame.channel[0].option[OPTION_PAN].value >> PAN_SHIFT;
        }
    }
    return 0;
}

void mpeg_decode_info(AHXBSR bsr, struct mpeg_header *hdr)
{
    hdr->version = AHXBSR_GetBitStm(bsr, VERSION_BITS);
    hdr->layer = LAYER_CODES - AHXBSR_GetBitStm(bsr, LAYER_BITS);
    hdr->error_protection = !AHXBSR_GetBitStm(bsr, PROTECTION_BITS);
    hdr->bitrate_index = AHXBSR_GetBitStm(bsr, BITRATE_BITS);
    hdr->sampling_frequency = AHXBSR_GetBitStm(bsr, FREQ_BITS);
    hdr->padding = AHXBSR_GetBitStm(bsr, PADDING_BITS);
    hdr->extension = AHXBSR_GetBitStm(bsr, EXTENSION_BITS);
    hdr->mode = AHXBSR_GetBitStm(bsr, MODE_BITS);
    hdr->mode_extension = AHXBSR_GetBitStm(bsr, MODE_EXT_BITS);
    hdr->copyright = AHXBSR_GetBitStm(bsr, COPYRIGHT_BITS);
    hdr->original = AHXBSR_GetBitStm(bsr, ORIGINAL_BITS);
    hdr->emphasis = AHXBSR_GetBitStm(bsr, EMPHASIS_BITS);
}

Sint32 mpeg_pick_table(struct mpeg_header *hdr, Sint32 channels,
                       MPEG_ALLOC_TABLE **alloc)
{
    Sint32 table;
    Sint32 version;
    Sint32 channel_rate;
    Sint32 freq_khz;
    Sint32 layer_row;
    Sint32 rate_index;
    Sint32 freq_index;
    Sint32 sblim;

    version = hdr->version;
    layer_row = hdr->layer - 1;
    rate_index = hdr->bitrate_index;
    channel_rate = bitrate[version][layer_row][rate_index] / channels;
    freq_index = hdr->sampling_frequency;
    freq_khz = s_freq[version][freq_index];
    /* @hack redundant: the choice of the standard's table for an MPEG-1
     * frame, which table = 0 below overrides */
    /* @hack spelling: hdr->version read again, where version would do */
    if (hdr->version == 1) {
        if ((freq_khz == 48 && channel_rate >= 56) ||
            (channel_rate >= 56 && channel_rate <= 80)) {
            table = 0;
        } else if (freq_khz != 48 && channel_rate >= 96) {
            table = 1;
        } else if (freq_khz != 32 && channel_rate <= 48) {
            table = 2;
        } else {
            table = 3;
        }
    } else {
        table = 0;
    }
    table = 0;
    /* @hack spelling: sblim, a local for the call's result, where returning
     * the call would do */
    sblim = mpeg_read_bit_alloc(table, alloc);
    return sblim;
}

void mpeg_set_frame(struct mpeg_header *hdr, struct mpeg_frame *fr)
{
    if (!fr->has_info) {
        if (hdr->mode == MODE_MONO) {
            fr->channels = NCH_MONO;
        } else {
            fr->channels = NCH_STEREO;
        }
    }
    if (hdr->layer == LAYER_II) {
        fr->sblimit = mpeg_pick_table(hdr, fr->channels, &fr->alloc);
    } else {
        fr->sblimit = SBLIMIT;
    }
    if (hdr->mode == MODE_JOINT) {
        /* @bug logic: indexes jsb_table by layer, not layer - 1, so a frame
         * takes the next layer's row. a layer II frame takes layer III's
         * bounds, {0, 4, 8, 16}, where its own are {4, 8, 12, 16}. MOV.L
         * @(4,R13),R0 at 0x8c16ee2a reads the layer that indexes the row */
        /* @bug memory: reads past jsb_table's end for a joint stereo frame of
         * layer III or the reserved layer, which nothing refuses. the bound
         * is then a word of scale_factors, a float's bits over 10^9. MOV.L
         * @R3,R1 at 0x8c16ee3a reads it */
#ifdef NONMATCHING
        /* this side repairs the read past jsb_table: a frame of layer III
         * or the reserved layer takes sblimit as its bound, as one that isn't
         * joint stereo does. layers I and II keep the next layer's row, as
         * the logic tag says */
        if (hdr->layer < (Sint32)(sizeof(jsb_table) / sizeof(jsb_table[0]))) {
            fr->jsbound = jsb_table[hdr->layer][hdr->mode_extension];
        } else {
            fr->jsbound = fr->sblimit;
        }
#else
        fr->jsbound = jsb_table[hdr->layer][hdr->mode_extension];
#endif
    } else {
        fr->jsbound = fr->sblimit;
    }
    if (!fr->has_info) {
        /* @hack spelling: version and freq_index, locals of a block of their
         * own, where hdr's fields read in the index would do */
        Sint32 version = hdr->version;
        Sint32 freq_index = hdr->sampling_frequency;

        fr->sample_rate = HZ_PER_KHZ * s_freq[version][freq_index];
    }
}

void mpeg_decode_bitalloc_args(AHXBSR bsr, Sint32 channels, Sint32 sblimit,
                               Sint32 jsbound, MPEG_ALLOC_TABLE *table,
                               Uint32 bit_alloc[NCH_STEREO][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    /* @hack spelling: alloc, a local that holds table, where table itself
     * would do */
    MPEG_ALLOC_TABLE *alloc = table;

    for (i = 0; i < jsbound; i++) {
        for (j = 0; j < channels; j++) {
            bit_alloc[j][i] = AHXBSR_GetBitStm(bsr, (*alloc)[i][0].bits);
        }
    }
    for (i = jsbound; i < sblimit; i++) {
        bit_alloc[0][i] = bit_alloc[1][i] =
            AHXBSR_GetBitStm(bsr, (*alloc)[i][0].bits);
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            bit_alloc[j][i] = 0;
        }
    }
}

void mpeg_decode_bitalloc(AHXBSR bsr, struct mpeg_frame *fr,
                          Uint32 bit_alloc[NCH_STEREO][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 sblimit = fr->sblimit;
    Sint32 channels = fr->channels;
    Sint32 jsbound = fr->jsbound;
    MPEG_ALLOC_TABLE *alloc = fr->alloc;

    for (i = 0; i < jsbound; i++) {
        for (j = 0; j < channels; j++) {
            bit_alloc[j][i] = AHXBSR_GetBitStm(bsr, (*alloc)[i][0].bits);
        }
    }
    for (i = jsbound; i < sblimit; i++) {
        bit_alloc[0][i] = bit_alloc[1][i] =
            AHXBSR_GetBitStm(bsr, (*alloc)[i][0].bits);
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            bit_alloc[j][i] = 0;
        }
    }
}

void mpeg_decode_scale_args(
    AHXBSR bsr, Uint32 bit_alloc[NCH_STEREO][SBLIMIT], Sint32 channels,
    Sint32 sblimit, Uint32 scfsi[NCH_STEREO][SBLIMIT],
    Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT])
{
    Sint32 i;
    Sint32 j;

    for (i = 0; i < sblimit; i++) {
        for (j = 0; j < channels; j++) {
            if (bit_alloc[j][i]) {
                scfsi[j][i] = AHXBSR_GetBitStm(bsr, SCFSI_BITS);
            }
        }
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            scfsi[j][i] = 0;
        }
    }
    for (i = 0; i < sblimit; i++) {
        for (j = 0; j < channels; j++) {
            if (bit_alloc[j][i]) {
                switch (scfsi[j][i]) {
                case 0:
                    scale_index[j][0][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][1][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][2][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                case 1:
                    scale_index[j][0][i] = scale_index[j][1][i] =
                        AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][2][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                /* @hack spelling: the case for scfsi 3 ahead of the case for
                 * scfsi 2, where the cases in order would do */
                case 3:
                    scale_index[j][0][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][1][i] = scale_index[j][2][i] =
                        AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                case 2:
                    scale_index[j][0][i] = scale_index[j][1][i] =
                        scale_index[j][2][i] =
                            AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                }
            } else {
                scale_index[j][0][i] = scale_index[j][1][i] =
                    scale_index[j][2][i] = SCALE_SILENT;
            }
        }
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            scale_index[j][0][i] = scale_index[j][1][i] = scale_index[j][2][i] =
                SCALE_SILENT;
        }
    }
}

void mpeg_decode_scale(AHXBSR bsr, struct mpeg_frame *fr,
                       Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
                       Uint32 scfsi[NCH_STEREO][SBLIMIT],
                       Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 sblimit = fr->sblimit;
    Sint32 channels = fr->channels;

    for (i = 0; i < sblimit; i++) {
        for (j = 0; j < channels; j++) {
            if (bit_alloc[j][i]) {
                scfsi[j][i] = AHXBSR_GetBitStm(bsr, SCFSI_BITS);
            }
        }
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            scfsi[j][i] = 0;
        }
    }
    for (i = 0; i < sblimit; i++) {
        for (j = 0; j < channels; j++) {
            if (bit_alloc[j][i]) {
                switch (scfsi[j][i]) {
                case 0:
                    scale_index[j][0][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][1][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][2][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                case 1:
                    scale_index[j][0][i] = scale_index[j][1][i] =
                        AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][2][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                /* @hack spelling: the case for scfsi 3 ahead of the case for
                 * scfsi 2, where the cases in order would do */
                case 3:
                    scale_index[j][0][i] = AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    scale_index[j][1][i] = scale_index[j][2][i] =
                        AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                case 2:
                    scale_index[j][0][i] = scale_index[j][1][i] =
                        scale_index[j][2][i] =
                            AHXBSR_GetBitStm(bsr, SCALE_BITS);
                    break;
                }
            } else {
                scale_index[j][0][i] = scale_index[j][1][i] =
                    scale_index[j][2][i] = SCALE_SILENT;
            }
        }
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            scale_index[j][0][i] = scale_index[j][1][i] = scale_index[j][2][i] =
                SCALE_SILENT;
        }
    }
}

void mpeg_buffer_sample_args(
    AHXBSR bsr, Uint32 bit_alloc[NCH_STEREO][SBLIMIT], Sint32 channels,
    Sint32 sblimit, Sint32 jsbound, MPEG_ALLOC_TABLE *table,
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 k;
    Sint32 m;
    /* @hack spelling: alloc, a local that holds table, where table itself
     * would do */
    MPEG_ALLOC_TABLE *alloc = table;

    for (i = 0; i < sblimit; i++) {
        for (j = 0; j < ((i < jsbound) ? channels : 1); j++) {
            if (bit_alloc[j][i]) {
                if ((*alloc)[i][bit_alloc[j][i]].group == GROUP_NONE) {
                    for (m = 0; m < SUBBAND_SAMPLES; m++) {
                        /* @hack spelling: k, a local that holds the bits for
                         * each read, where the quantizer's bits in the call
                         * would do */
                        k = (*alloc)[i][bit_alloc[j][i]].bits;
                        sample[j][m][i] = AHXBSR_GetBitStm(bsr, k);
                    }
                } else {
                    Uint32 steps;
                    Uint32 code;

                    steps = (*alloc)[i][bit_alloc[j][i]].steps;
                    /* @hack spelling: k, a local that holds the bits for the
                     * read, where the quantizer's bits in the call would do */
                    k = (*alloc)[i][bit_alloc[j][i]].bits;
                    code = AHXBSR_GetBitStm(bsr, k);
                    for (k = 0; k < SUBBAND_SAMPLES; k++) {
                        sample[j][k][i] = code % steps;
                        code /= steps;
                    }
                }
            } else {
                for (k = 0; k < SUBBAND_SAMPLES; k++) {
                    sample[j][k][i] = 0;
                }
            }
            if (channels == NCH_STEREO && i >= jsbound) {
                for (k = 0; k < SUBBAND_SAMPLES; k++) {
                    sample[1][k][i] = sample[0][k][i];
                }
            }
        }
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            for (k = 0; k < SUBBAND_SAMPLES; k++) {
                sample[j][k][i] = 0;
            }
        }
    }
}

void mpeg_buffer_sample(AHXBSR bsr, struct mpeg_frame *fr,
                        Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
                        Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 k;
    Sint32 m;
    Sint32 channels = fr->channels;
    Sint32 sblimit = fr->sblimit;
    Sint32 jsbound = fr->jsbound;
    MPEG_ALLOC_TABLE *alloc = fr->alloc;

    mpeg_read_total = 0;
    ahxbsr_read_count = 0;
    for (i = 0; i < sblimit; i++) {
        for (j = 0; j < ((i < jsbound) ? channels : 1); j++) {
            if (bit_alloc[j][i]) {
                if ((*alloc)[i][bit_alloc[j][i]].group == GROUP_NONE) {
                    for (m = 0; m < SUBBAND_SAMPLES; m++) {
                        /* @hack spelling: k, a local that holds the bits for
                         * each read, where the quantizer's bits in the call
                         * would do */
                        k = (*alloc)[i][bit_alloc[j][i]].bits;
                        mpeg_read_ticks[0] = mpeg_timer_count();
                        sample[j][m][i] = AHXBSR_GetBitStm(bsr, k);
                        mpeg_read_ticks[1] = mpeg_timer_count();
                        mpeg_read_total += mpeg_timer_elapsed(
                            mpeg_read_ticks[0], mpeg_read_ticks[1]);
                    }
                } else {
                    Uint32 steps;
                    Uint32 code;

                    steps = (*alloc)[i][bit_alloc[j][i]].steps;
                    /* @hack spelling: k, a local that holds the bits for the
                     * read, where the quantizer's bits in the call would do */
                    k = (*alloc)[i][bit_alloc[j][i]].bits;
                    mpeg_read_ticks[0] = mpeg_timer_count();
                    code = AHXBSR_GetBitStm(bsr, k);
                    mpeg_read_ticks[1] = mpeg_timer_count();
                    mpeg_read_total += mpeg_timer_elapsed(mpeg_read_ticks[0],
                                                          mpeg_read_ticks[1]);
                    if (steps == 3) {
                        sample[j][0][i] = group3[0][code];
                        sample[j][1][i] = group3[1][code];
                        sample[j][2][i] = group3[2][code];
                    } else if (steps == 5) {
                        sample[j][0][i] = group5[0][code];
                        sample[j][1][i] = group5[1][code];
                        sample[j][2][i] = group5[2][code];
                    } else if (steps == 9) {
                        sample[j][0][i] = group9[0][code];
                        sample[j][1][i] = group9[1][code];
                        sample[j][2][i] = group9[2][code];
                    } else {
                        for (k = 0; k < SUBBAND_SAMPLES; k++) {
                            sample[j][k][i] = code % steps;
                            code /= steps;
                        }
                    }
                }
            } else {
                for (k = 0; k < SUBBAND_SAMPLES; k++) {
                    sample[j][k][i] = 0;
                }
            }
            if (channels == NCH_STEREO && i >= jsbound) {
                for (k = 0; k < SUBBAND_SAMPLES; k++) {
                    sample[1][k][i] = sample[0][k][i];
                }
            }
        }
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            for (k = 0; k < SUBBAND_SAMPLES; k++) {
                sample[j][k][i] = 0;
            }
        }
    }
}

void mpeg_denormalize_sample_args(
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT], Sint32 sblimit,
    Sint32 channels, Sint32 part)
{
    Sint32 i;
    Sint32 j;

    for (j = 0; j < channels; j++) {
        for (i = 0; i < sblimit; i++) {
            fraction[j][0][i] *= scale_factors[scale_index[j][part][i]];
            fraction[j][1][i] *= scale_factors[scale_index[j][part][i]];
            fraction[j][2][i] *= scale_factors[scale_index[j][part][i]];
        }
    }
}

void mpeg_denormalize_sample(
    struct mpeg_frame *fr, Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT],
    Sint32 part, Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 sblimit = fr->sblimit;
    Sint32 channels = fr->channels;

    for (j = 0; j < channels; j++) {
        for (i = 0; i < sblimit; i++) {
            fraction[j][0][i] *= scale_factors[scale_index[j][part][i]];
            fraction[j][1][i] *= scale_factors[scale_index[j][part][i]];
            fraction[j][2][i] *= scale_factors[scale_index[j][part][i]];
        }
    }
}

void mpeg_dequantize_sample_args(
    Sint32 channels, Sint32 sblimit,
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    MPEG_ALLOC_TABLE *table)
{
    Sint32 i;
    Sint32 j;
    Sint32 k;
    Sint32 x;
    /* @hack spelling: alloc, a local that holds table, where table itself
     * would do */
    MPEG_ALLOC_TABLE *alloc = table;

    for (k = 0; k < channels; k++) {
        for (j = 0; j < SUBBAND_SAMPLES; j++) {
            for (i = 0; i < sblimit; i++) {
                if (bit_alloc[k][i]) {
                    x = 0;
                    while ((1 << x) < (*alloc)[i][bit_alloc[k][i]].steps) {
                        x++;
                    }
                    if (((sample[k][j][i] >> (x - 1)) & 1) == 1) {
                        fraction[k][j][i] = 0.0F;
                    } else {
                        fraction[k][j][i] = -1.0F;
                    }
                    fraction[k][j][i] +=
                        (Float)(sample[k][j][i] & ((1 << (x - 1)) - 1)) /
                        (Float)(1 << (x - 1));
                    fraction[k][j][i] +=
                        quant_d[(*alloc)[i][bit_alloc[k][i]].quant];
                    fraction[k][j][i] *=
                        quant_c[(*alloc)[i][bit_alloc[k][i]].quant];
                } else {
                    fraction[k][j][i] = 0.0F;
                }
            }
        }
    }
    for (k = 0; k < channels; k++) {
        for (j = 0; j < SUBBAND_SAMPLES; j++) {
            for (i = sblimit; i < SBLIMIT; i++) {
                fraction[k][j][i] = 0.0F;
            }
        }
    }
}

void mpeg_dequantize_sample(
    struct mpeg_frame *fr, Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 k;
    MPEG_ALLOC_TABLE *alloc = fr->alloc;
    Sint32 channels = fr->channels;
    Sint32 sblimit = fr->sblimit;
    Float step;
    Float scale;

    for (k = 0; k < channels; k++) {
        for (j = 0; j < SUBBAND_SAMPLES; j++) {
            for (i = 0; i < sblimit; i++) {
                if (bit_alloc[k][i]) {
                    step = 1.0F / (*alloc)[i][bit_alloc[k][i]].steps;
                    scale = step * 2.0F;
                    fraction[k][j][i] = (scale * sample[k][j][i]) + step - 1.0F;
                } else {
                    fraction[k][j][i] = 0.0F;
                }
            }
        }
    }
    for (k = 0; k < channels; k++) {
        for (j = 0; j < SUBBAND_SAMPLES; j++) {
            for (i = sblimit; i < SBLIMIT; i++) {
                fraction[k][j][i] = 0.0F;
            }
        }
    }
}

void mpeg_dequantize_scale_sample(
    struct mpeg_frame *fr, Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT],
    Sint32 part, Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
    Uint32 sample[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT],
    Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 k;
    MPEG_ALLOC_TABLE *alloc = fr->alloc;
    Sint32 channels = fr->channels;
    Sint32 sblimit = fr->sblimit;
    Float cq;
    Float dq;
    Float scale;

    for (k = 0; k < channels; k++) {
        for (i = 0; i < sblimit; i++) {
            if (bit_alloc[k][i]) {
                /* @hack spelling: d read before c, where the quantizer's
                 * order would do */
                dq = (*alloc)[i][bit_alloc[k][i]].d;
                cq = (*alloc)[i][bit_alloc[k][i]].c;
                scale = scale_factors[scale_index[k][part][i]];
                for (j = 0; j < SUBBAND_SAMPLES; j++) {
                    fraction[k][j][i] = ((cq * sample[k][j][i]) + dq) * scale;
                }
            } else {
                for (j = 0; j < SUBBAND_SAMPLES; j++) {
                    fraction[k][j][i] = 0.0F;
                }
            }
        }
    }
    for (k = 0; k < channels; k++) {
        for (i = sblimit; i < SBLIMIT; i++) {
            for (j = 0; j < SUBBAND_SAMPLES; j++) {
                fraction[k][j][i] = 0.0F;
            }
        }
    }
}

void mpeg_decode_samples(AHXBSR bsr, struct mpeg_frame *fr,
                         Uint32 bit_alloc[NCH_STEREO][SBLIMIT],
                         Uint32 scale_index[NCH_STEREO][SCALE_PARTS][SBLIMIT],
                         Sint32 part,
                         Float fraction[NCH_STEREO][SUBBAND_SAMPLES][SBLIMIT])
{
    Sint32 i;
    Sint32 j;
    Sint32 k;
    Sint32 channels = fr->channels;
    Sint32 sblimit = fr->sblimit;
    Sint32 jsbound = fr->jsbound;
    MPEG_ALLOC_TABLE *alloc = fr->alloc;

    mpeg_read_total = 0;
    ahxbsr_read_count = 0;
    for (i = 0; i < sblimit; i++) {
        /* @hack redundant: a store of 0 to scale_index under if (0), which
         * never runs */
        if (0) {
            scale_index = 0;
        }
        /* @hack redundant: the same store again, in a block of its own */
        if (0) {
            scale_index = 0;
        }
        for (j = 0; j < ((i < jsbound) ? channels : 1); j++) {
            /* @hack spelling: ba, a local that holds the allocation code,
             * where bit_alloc[j][i] at each use would do */
            Uint32 ba = bit_alloc[j][i];

            if (ba) {
                /* @hack redundant: register on q, cq, scale, cs and ds, where
                 * plain locals would do */
                register struct mpeg_quantizer *q;
                Float dq;
                register Float cq;
                register Float scale;
                register Float cs;
                register Float ds;

                q = &(*alloc)[i][ba];
                /* @hack spelling: d read before c, where the quantizer's order
                 * would do */
                dq = q->d;
                cq = q->c;
                scale = scale_factors[scale_index[j][part][i]];
                cs = cq * scale;
                ds = dq * scale;

                if (q->group == GROUP_NONE) {
                    /* @hack redundant: register on bits and v, where plain
                     * locals would do */
                    register Sint32 bits = q->bits;
                    register Sint32 v;

                    if (bits <= PACKED_BITS_MAX) {
                        Sint32 mask;

                        /* @hack spelling: bits + bits + bits, where 3 * bits
                         * would do */
                        v = AHXBSR_GetBitStm(bsr, bits + bits + bits);
                        mask = low_bits[bits];
                        fraction[j][2][i] = (cs * (v & mask)) + ds;
                        v >>= bits;
                        fraction[j][1][i] = (cs * (v & mask)) + ds;
                        v >>= bits;
                        fraction[j][0][i] = (cs * v) + ds;
                    } else {
                        v = AHXBSR_GetBitStm(bsr, bits);
                        fraction[j][0][i] = (cs * v) + ds;
                        v = AHXBSR_GetBitStm(bsr, bits);
                        fraction[j][1][i] = (cs * v) + ds;
                        v = AHXBSR_GetBitStm(bsr, bits);
                        fraction[j][2][i] = (cs * v) + ds;
                    }
                } else {
                    Sint32 steps = q->steps;
                    Sint32 v;
                    /* @hack redundant: register on p and s, where plain locals
                     * would do */
                    register Sint8 *p;
                    register Sint32 s;

                    /* @hack spelling: k, a local that holds the bits for the
                     * read, where q->bits in the call would do */
                    k = q->bits;
                    v = AHXBSR_GetBitStm(bsr, k);

                    /* @hack spelling: p, a pointer stepped from row to row of
                     * the digits, where the table indexed by row would do */
                    if (steps == 3) {
                        p = group3[0];
                        s = p[v];
                        p += sizeof(group3[0]);
                        fraction[j][0][i] = (cs * s) + ds;
                        s = p[v];
                        p += sizeof(group3[0]);
                        fraction[j][1][i] = (cs * s) + ds;
                        s = p[v];
                        fraction[j][2][i] = (cs * s) + ds;
                    } else if (steps == 5) {
                        /* @bug logic: takes samples 0 and 1 of a code of 5
                         * steps both from its digit 0, and sample 2 from its
                         * digit 1. p moves to group5's next row after the
                         * second read, not after the first. MOV.B @R6,R4 at
                         * 0x8c1701be and at 0x8c1701d2 read the same row */
                        p = group5[0];
                        s = p[v];
                        fraction[j][0][i] = (cs * s) + ds;
                        s = p[v];
                        p += sizeof(group5[0]);
                        fraction[j][1][i] = (cs * s) + ds;
                        s = p[v];
                        fraction[j][2][i] = (cs * s) + ds;
                    } else if (steps == 9) {
                        p = group9[0];
                        s = p[v];
                        p += sizeof(group9[0]);
                        fraction[j][0][i] = (cs * s) + ds;
                        s = p[v];
                        p += sizeof(group9[0]);
                        fraction[j][1][i] = (cs * s) + ds;
                        s = p[v];
                        fraction[j][2][i] = (cs * s) + ds;
                    } else {
                        for (k = 0; k < SUBBAND_SAMPLES; k++) {
                            s = v % steps;
                            fraction[j][k][i] = (cs * s) + ds;
                            v /= steps;
                        }
                    }
                }
            } else {
                fraction[j][0][i] = 0.0F;
                fraction[j][1][i] = 0.0F;
                fraction[j][2][i] = 0.0F;
            }
            /* @bug logic: scales channel 1 of a joint stereo subband by
             * channel 0's scale factors. from jsbound on, the loop above
             * decodes channel 0 alone, and this copies its scaled values to
             * channel 1. channel 1's own scale factors, which
             * mpeg_decode_scale reads, go unused. mpeg_denormalize_sample
             * scales each channel by its own. FMOV.S FR3,@(R0,R5) at
             * 0x8c1702b0 stores channel 0's value for channel 1 */
            if (channels == NCH_STEREO && i >= jsbound) {
                for (k = 0; k < SUBBAND_SAMPLES; k++) {
                    fraction[1][k][i] = fraction[0][k][i];
                }
            }
        }
    }
    for (i = sblimit; i < SBLIMIT; i++) {
        for (j = 0; j < channels; j++) {
            for (k = 0; k < SUBBAND_SAMPLES; k++) {
                fraction[j][k][i] = 0.0F;
            }
        }
    }
}
