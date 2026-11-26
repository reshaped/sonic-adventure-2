/* the ADX encoder. a block holds one channel's scale and a code for each of
 * its samples, 4 bits by default. a code stands for the sample's residual
 * from a prediction from the two samples before it. a channel's prediction
 * filter makes the predictions. its iir filter decodes each code as a decoder
 * will. the next prediction reads the samples it decodes */
#include <adx_crs.h>
#include <adx_dcd.h>
#include <adx_sje.h>
#include <sg_xpt.h>
#include <sj.h>
#include <stdlib.h>
#include <string.h>

#include "../sofdec/adx_bendian.h"

#define ADXSJE_MAX_OBJ 8
#define ADXSJE_MAX_CH 2
#define ADXSJE_MAX_BLKSMPL 32
/* the filters of each kind, two for each encoder */
#define ADXSJE_MAX_FLT 16
/* a block's bytes: its scale's, then its codes' */
#define SCALE_BYTES 2
#define CODE_BYTES 16
#define ADXSJE_BLKLEN (SCALE_BYTES + CODE_BYTES)
#define BYTE_BITS 8

#define SAMPLE_MIN (-32768)
#define SAMPLE_MAX 32767
#define CODE_MIN (-8)
#define CODE_MAX 7
/* a code's step, in a residual scaled to SAMPLE_MAX. HALF_STEP rounds a
 * scaled residual to the nearest code */
#define CODE_STEP (SAMPLE_MAX / CODE_MAX)
#define HALF_STEP (CODE_STEP / 2)

/* the bytes of the header's fields from ADX_DTOFS_BASE through its
 * revision */
#define FORMAT_BYTES 16
/* the bytes of the loops' lead-in and count, and of each loop's record */
#define LOOP_COUNT_BYTES 4
#define LOOP_BYTES 20
/* the header's version and revision, and the type of each loop it gives */
#define HEADER_VERSION 3
#define HEADER_REVISION 0
#define LOOP_TYPE 1
/* the encoding ADXSJE_Create sets. a header of it or of the next encoding
 * holds a loop's lead-in and count */
#define ADX_ENCODING 3
/* the least data offset that leaves the header room for the cutoff, which
 * ADX_DecodeInfoExADPCM2 requires */
#define COF_DTOFS (ADX_HDR_COF_LEN - ADX_DTOFS_BASE)
/* a header's length rounds up to a multiple of HEADER_ALIGN */
#define HEADER_ALIGN 4
/* the data offset ADXSJE_SetConfigSfa gives a header */
#define SFA_HDINFO_SIZE 0x11c
#define SECTOR_BYTES 2048
/* total_samples and limit as ADXSJE_Create sets them: more samples than any
 * stream holds */
#define ENDLESS 0x7fff0000
/* the sample rate, code bits and cutoff in hertz ADXSJE_Create sets */
#define DEFAULT_SAMPLE_RATE 44100
#define DEFAULT_BITS 4
#define DEFAULT_CUTOFF 500
/* 1.00 */
#define ADXSJE_VERSION 0x100

#define ADXSJE_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ADXSJE_MIN(a, b) (((a) < (b)) ? (a) : (b))

#define ADXSJE_FLOOR(value, lo) (((value) < (lo)) ? (lo) : (value))
#define ADXSJE_CLIP16(value)                                                   \
    ((ADXSJE_FLOOR(value, SAMPLE_MIN) > SAMPLE_MAX)                            \
         ? SAMPLE_MAX                                                          \
         : ADXSJE_FLOOR(value, SAMPLE_MIN))
#define ADXSJE_CLIP4(value)                                                    \
    ((ADXSJE_FLOOR(value, CODE_MIN) < CODE_MAX)                                \
         ? ADXSJE_FLOOR(value, CODE_MIN)                                       \
         : CODE_MAX)

/* a decoder's filter. it adds each residual to its prediction from the two
 * samples before, recent and previous */
struct iir_filter {
    Sint8 used;
    Uint8 reserved[3];
    Sint16 coefficient1;
    Sint16 coefficient2;
    Sint16 recent;
    Sint16 previous;
};

/* one channel's predictor and its block. the block holds each sample's
 * residual from the prediction, its code, and the block's scale and gain */
struct prediction_filter {
    Sint8 used;
    Uint8 reserved0[3];
    Sint16 coefficient1;
    Sint16 coefficient2;
    Sint16 recent;
    Sint16 previous;
    Sint32 reserved1[2];
    Sint32 samples;
    Sint16 residual[ADXSJE_MAX_BLKSMPL];
    Sint8 quantized[ADXSJE_MAX_BLKSMPL];
    Sint32 peak; /* the largest residual's magnitude */
    Sint16 scale;
    Sint16 reserved2;
    Float gain;
    struct iir_filter *iir;
};

/* an encoder. samples is the sample at which encoding ends, and
 * total_samples the count the header gives. recent and previous hold each
 * channel's last two samples as a decoder reproduces them */
struct adxsje_object {
    Sint8 used;
    Sint8 state;
    Sint8 max_channels;
    Sint8 end_requested;
    SJ inputs[ADXSJE_MAX_CH];
    SJ output;
    Sint32 reserved0[6];
    Sint32 count;
    Sint32 encoded_bytes;
    Sint32 encoded_samples;
    Sint32 limit;
    Sint32 reserved1;
    Sint32 samples;
    Sint32 padded_samples;
    Sint32 samples_per_block;
    Sint32 header_info_size; /* the data offset the header gives */
    Sint32 encoding;
    Sint32 block_bytes;
    Sint32 bits_per_sample;
    Sint32 channels;
    Sint32 sample_rate;
    Sint32 total_samples;
    Sint32 cutoff;
    Sint32 loop_leadin;
    Sint32 loop_count;
    Sint32 loop_start;
    Sint32 loop_start_offset;
    Sint32 loop_end;
    Sint32 loop_end_offset;
    struct prediction_filter *filters[ADXSJE_MAX_CH];
    Sint16 recent[ADXSJE_MAX_CH];
    Sint16 previous[ADXSJE_MAX_CH];
    Sint16 pcm[ADXSJE_MAX_CH][ADXSJE_MAX_BLKSMPL];
    Sint16 residual[ADXSJE_MAX_CH][ADXSJE_MAX_BLKSMPL];
    Sint16 scaled_residual[ADXSJE_MAX_CH][ADXSJE_MAX_BLKSMPL];
    Sint16 decoded[ADXSJE_MAX_CH][ADXSJE_MAX_BLKSMPL];
    Sint16 scale[ADXSJE_MAX_CH];
    Float gain[ADXSJE_MAX_CH];
    Uint8 encoded_block[ADXSJE_MAX_CH][CODE_BYTES];
};

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 adxsje_write68(void *buf, Sint32 size, Sint32 count, SJ sj);
void iirflt_init(void);
struct iir_filter *iirflt_create(void);
void iirflt_destroy(struct iir_filter *filter);
void iirflt_set_coef(struct iir_filter *filter, Sint16 coefficient1,
                     Sint16 coefficient2);
void iirflt_set_delay(struct iir_filter *filter, Sint16 recent,
                      Sint16 previous);
void iirflt_get_delay(struct iir_filter *filter, Sint16 *recent,
                      Sint16 *previous);
Sint32 iirflt_put_sig(struct iir_filter *filter, Sint32 sig);
struct prediction_filter *pflt_create(Sint32 samples);
void pflt_destroy(struct prediction_filter *filter);
void pflt_set_coef(struct prediction_filter *filter, Sint16 coefficient1,
                   Sint16 coefficient2);
void pflt_calc_coef(struct prediction_filter *filter, Sint32 cutoff,
                    Sint32 sample_rate);
void pflt_set_delay(struct prediction_filter *filter, Sint16 recent,
                    Sint16 previous);
void pflt_put_sig(struct prediction_filter *filter, Sint32 i, Sint16 sig);
void pflt_calc_gain(struct prediction_filter *filter);
Sint32 pflt_get_rsig(struct prediction_filter *filter, Sint32 i);
Sint32 pflt_get_rsig_q(struct prediction_filter *filter, Sint32 i);
void pflt_set_rsig_q(struct prediction_filter *filter, Sint32 i,
                     Sint8 quantized_sample);
Sint32 adxsje_get_pcm_data(ADXSJE encoder, Sint32 pos, Sint32 count,
                           Sint16 **buf);
Sint32 adxsje_calc_rsig(ADXSJE encoder, Sint32 channel);
void adxsje_set_rsig(ADXSJE encoder, Sint32 channel);
Sint32 adxsje_encode_blk(ADXSJE encoder);
Sint32 adxsje_output_sdata(ADXSJE encoder);
Sint32 adxsje_encode_data(ADXSJE encoder);
Sint32 adxsje_write_end_code(ADXSJE encoder);
Sint32 adxsje_output_header(ADXSJE encoder, SJ sjo_in);
void adxsje_adjust_hdinfo_size(ADXSJE encoder);
void adxsje_encode_prep(ADXSJE encoder);
void adxsje_encode_exec(ADXSJE encoder);

struct adxsje_object adxsje_obj[ADXSJE_MAX_OBJ];
struct iir_filter adxsje_iirflt_obj[ADXSJE_MAX_FLT];
struct prediction_filter adxsje_prdflt_obj[ADXSJE_MAX_FLT];
/* NOLINTEND(misc-use-internal-linkage) */

static const char adxsje_ver_str[] = "ADXENC.DLL Ver.1.00 Jun 23 2000";
char *adxenc_build = "\nADXENC Ver.1.00 Build:Jun 23 2000 11:59:33\n";
static Sint32 adxsje_max_scale = 0x1000;
/* the text every header ends with */
static char *adxsje_copyright = "(c)CRI";

Sint32 ADXSJE_GetVersion(void)
{
    return ADXSJE_VERSION;
}

const char *ADXSJE_GetVerStr(void)
{
    return adxsje_ver_str;
}

Sint32 ADXSJE_GetInfo(Sint8 *header, Sint32 header_bytes, Sint32 *bits,
                      Sint32 *channels, Sint32 *sample_rate, Sint32 *samples)
{
    Sint8 bps;
    Sint8 channel;
    Sint16 dtofs;
    Sint8 enc;
    Sint8 block_bytes;
    Sint32 samples_per_block;
    Sint32 ret;

#ifdef NONMATCHING
    /* a failed ADX_DecodeInfo stores nothing, and the shipped code passes
     * channel and bps on unset */
    channel = 0;
    bps = 0;
#endif
    ret = ADX_DecodeInfo(header, header_bytes, &dtofs, &enc, &bps, &block_bytes,
                         &channel, sample_rate, samples, &samples_per_block);
    *channels = channel;
    *bits = bps;
    return ret;
}

/* put count items of size bytes from buf on sj's data line, each in the
 * 68000's big-endian order. size is 1, 2 or 4, and any other size hangs.
 * returns count, or 0, writing nothing, where the free line's first run is
 * shorter than the items */
Sint32 adxsje_write68(void *buf, Sint32 size, Sint32 count, SJ sj)
{
    SJCK chunk;
    Sint32 i;
    Sint8 *destination;

    SJ_GetChunk(sj, SJ_LIN_FREE, size * count, &chunk);
    if (chunk.len < size * count) {
        SJ_UngetChunk(sj, SJ_LIN_FREE, &chunk);
        return 0;
    }
    destination = (Sint8 *)chunk.data;
#ifdef NONMATCHING
    /* a run of a joint can start at any address. this side moves each item
     * through an aligned local, and returns 0 for another size */
    if (size == 4) {
        for (i = 0; i < count; i++) {
            Uint32 word;
            memcpy(&word, (Uint8 *)buf + (i * size), sizeof(word));
            word = ADX_SWAP32(word);
            memcpy(destination + (i * size), &word, sizeof(word));
        }
    } else if (size == 2) {
        for (i = 0; i < count; i++) {
            Uint16 halfword;
            memcpy(&halfword, (Uint8 *)buf + (i * size), sizeof(halfword));
            halfword = ADX_SWAP16(halfword);
            memcpy(destination + (i * size), &halfword, sizeof(halfword));
        }
    } else if (size == 1) {
        memcpy(destination, buf, count);
    } else {
        SJ_UngetChunk(sj, SJ_LIN_FREE, &chunk);
        return 0;
    }
#else
    if (size == 4) {
        Sint32 *source_words = buf;
        Sint32 *output_words = (Sint32 *)destination;

        for (i = 0; i < count; i++) {
            *output_words = ADX_SWAP32(*source_words);
            source_words++;
            output_words++;
        }
    } else if (size == 2) {
        Sint16 *source_halves = buf;
        Sint16 *output_halves = (Sint16 *)destination;

        for (i = 0; i < count; i++) {
            *output_halves = ADX_SWAP16(*source_halves);
            source_halves++;
            output_halves++;
        }
    } else if (size == 1) {
        memcpy(destination, buf, count);
    } else {
        for (;;) {
        }
    }
#endif
    SJ_PutChunk(sj, SJ_LIN_DATA, &chunk);
    return count;
}

void iirflt_init(void)
{
    memset(adxsje_iirflt_obj, 0, sizeof(adxsje_iirflt_obj));
}

/* return the first free filter, or NULL. it never marks the filter used, so
 * every call returns adxsje_iirflt_obj[0], which every prediction filter then
 * shares. adxsje_calc_rsig loads the filter's delay before each block. the
 * sharing then changes no output while every encoder takes the same
 * coefficients */
struct iir_filter *iirflt_create(void)
{
    struct iir_filter *filter;
    Sint32 i;

    for (i = 0; i < ADXSJE_MAX_FLT; i++) {
        filter = &adxsje_iirflt_obj[i];
        if (filter->used == 0) {
            break;
        }
    }
    if (i < ADXSJE_MAX_FLT) {
        return filter;
    }
    return NULL;
}

void iirflt_destroy(struct iir_filter *filter)
{
    if (filter != NULL) {
        /* @hack redundant: used cleared before the memset that clears it */
        filter->used = 0;
        memset(filter, 0, sizeof(struct iir_filter));
    }
}

void iirflt_set_coef(struct iir_filter *filter, Sint16 coefficient1,
                     Sint16 coefficient2)
{
    filter->coefficient1 = coefficient1;
    filter->coefficient2 = coefficient2;
}

void iirflt_set_delay(struct iir_filter *filter, Sint16 recent, Sint16 previous)
{
    filter->recent = recent;
    filter->previous = previous;
}

void iirflt_get_delay(struct iir_filter *filter, Sint16 *recent,
                      Sint16 *previous)
{
    *recent = filter->recent;
    *previous = filter->previous;
}

/* the prediction plus the residual sig, clipped to 16 bits, which becomes the
 * recent sample. a NULL filter returns sig */
Sint32 iirflt_put_sig(struct iir_filter *filter, Sint32 sig)
{
    Sint32 sample;

    if (filter != NULL) {
        sample = ((filter->coefficient1 * filter->recent >> ADX_COEF_BITS) +
                  (filter->coefficient2 * filter->previous >> ADX_COEF_BITS)) +
                 (Sint16)sig;
        sample = ADXSJE_MIN(ADXSJE_MAX(sample, SAMPLE_MIN), SAMPLE_MAX);
        filter->previous = filter->recent;
        filter->recent = sample;
        return sample;
    }
    return sig;
}

struct prediction_filter *pflt_create(Sint32 samples)
{
    struct iir_filter *iir;
    struct prediction_filter *filter;
    Sint32 i;

    iir = iirflt_create();
    if (iir != NULL) {
        for (i = 0; i < ADXSJE_MAX_FLT; i++) {
            filter = &adxsje_prdflt_obj[i];
            if (filter->used == 0) {
                break;
            }
        }
        filter->samples = samples;
        filter->iir = iir;
        if (i < ADXSJE_MAX_FLT) {
            filter->used = 1;
            return filter;
        }
    }
    return NULL;
}

void pflt_destroy(struct prediction_filter *filter)
{
    if (filter != NULL) {
        /* @hack redundant: used cleared before the memset that clears it */
        filter->used = 0;
        memset(filter, 0, sizeof(struct prediction_filter));
    }
}

void pflt_set_coef(struct prediction_filter *filter, Sint16 coefficient1,
                   Sint16 coefficient2)
{
    filter->coefficient1 = coefficient1;
    filter->coefficient2 = coefficient2;
}

void pflt_calc_coef(struct prediction_filter *filter, Sint32 cutoff,
                    Sint32 sample_rate)
{
    Sint16 coefficient1;
    Sint16 coefficient2;

    /* the cutoff as the header's 16-bit field holds it, so the coefficients
     * agree with a decoder's */
    ADX_GetCoefficient((Sint16)cutoff, sample_rate, &coefficient1,
                       &coefficient2);
    pflt_set_coef(filter, coefficient1, coefficient2);
    iirflt_set_coef(filter->iir, coefficient1, coefficient2);
}

void pflt_set_delay(struct prediction_filter *filter, Sint16 recent,
                    Sint16 previous)
{
    filter->recent = recent;
    filter->previous = previous;
}

/* store sig's residual from the prediction, clipped to 16 bits, as the
 * block's sample i. peak rises to its magnitude. sample 0 clears the block
 * first, and sig becomes the recent sample */
void pflt_put_sig(struct prediction_filter *filter, Sint32 i, Sint16 sig)
{
    Sint32 j;
    Sint32 sample;
    Sint16 error;

    if (i == 0) {
        for (j = 0; j < filter->samples; j++) {
            filter->residual[j] = 0;
        }
        filter->peak = 0;
    }
    if (filter == NULL) {
        return;
    }
    sample = sig - (filter->coefficient1 * filter->recent >> ADX_COEF_BITS) -
             (filter->coefficient2 * filter->previous >> ADX_COEF_BITS);
    filter->residual[i] = error = ADXSJE_CLIP16(sample);
    if (abs(error) > filter->peak) {
        filter->peak = abs(error);
    }
    filter->previous = filter->recent;
    filter->recent = sig;
}

/* set scale, the residual a code of 1 stands for, and gain, which takes the
 * peak to SAMPLE_MAX */
void pflt_calc_gain(struct prediction_filter *filter)
{
    Sint32 scale;

    scale = ((filter->peak - 1) / CODE_MAX) + 1;
    filter->scale = ADXSJE_MAX(ADXSJE_MIN(scale, adxsje_max_scale), 1);
    filter->gain = (Float)SAMPLE_MAX / filter->peak;
}

Sint32 pflt_get_rsig(struct prediction_filter *filter, Sint32 i)
{
    if (filter == NULL) {
        return 0;
    }
    return filter->residual[i];
}

Sint32 pflt_get_rsig_q(struct prediction_filter *filter, Sint32 i)
{
    if (filter == NULL) {
        return 0;
    }
    return filter->quantized[i];
}

void pflt_set_rsig_q(struct prediction_filter *filter, Sint32 i,
                     Sint8 quantized_sample)
{
    filter->quantized[i] = quantized_sample;
}

/* copy count samples of each channel's input into buf. return the samples it
 * copied of the last channel. where an input holds fewer than count, it
 * copies nothing and returns 0. each chunk it asks for is count samples long.
 * where a channel's data wraps its ring, it then copies and returns more than
 * count */
Sint32 adxsje_get_pcm_data(ADXSJE encoder, Sint32 pos, Sint32 count,
                           Sint16 **buf)
{
    Sint32 channel;
    Sint32 chunk_samples;
    SJ *inputs;
    SJCK chunk;

#ifdef NONMATCHING
    /* with no channels, the shipped code returns chunk_samples unset */
    chunk_samples = 0;
#endif
    inputs = encoder->inputs;
    for (channel = 0; channel < encoder->channels; channel++) {
        chunk_samples =
            SJ_GetNumData(inputs[channel], SJ_LIN_DATA) / sizeof(Sint16);
        if (chunk_samples < count) {
            return 0;
        }
    }
    for (channel = 0; channel < encoder->channels; channel++) {
#ifdef NONMATCHING
        /* this side asks each chunk for the bytes still wanted, which may end
         * inside a sample. it stops on an empty chunk */
        Sint32 bytes = count * (Sint32)sizeof(Sint16);
        Sint32 copied = 0;
        while (copied < bytes) {
            SJ_GetChunk(inputs[channel], SJ_LIN_DATA, bytes - copied, &chunk);
            if (chunk.len == 0) {
                SJ_UngetChunk(inputs[channel], SJ_LIN_DATA, &chunk);
                return 0;
            }
            memcpy((Uint8 *)buf[channel] + copied, chunk.data, chunk.len);
            copied += chunk.len;
            SJ_PutChunk(inputs[channel], SJ_LIN_FREE, &chunk);
        }
        chunk_samples = count;
#else
        for (chunk_samples = 0; chunk_samples < count;) {
            SJ_GetChunk(inputs[channel], SJ_LIN_DATA, count * sizeof(Sint16),
                        &chunk);
            memcpy(&buf[channel][chunk_samples], chunk.data, chunk.len);
            chunk_samples += chunk.len / sizeof(Sint16);
            SJ_PutChunk(inputs[channel], SJ_LIN_FREE, &chunk);
        }
#endif
    }
    return chunk_samples;
}

Sint32 adxsje_calc_rsig(ADXSJE encoder, Sint32 channel)
{
    struct prediction_filter *filter;
    struct iir_filter *iir;
    Sint32 i;
    Sint16 sig;
    Sint16 recent;
    Sint16 previous;
    Sint16 residual;
    Sint32 sample;
    Sint32 quantized_sample;

    filter = encoder->filters[channel];
    iir = filter->iir;
    pflt_set_delay(filter, encoder->recent[channel],
                   encoder->previous[channel]);
    for (i = 0; i < encoder->samples_per_block; i++) {
        pflt_put_sig(filter, i, encoder->pcm[channel][i]);
    }
    pflt_calc_gain(filter);
    iirflt_set_delay(filter->iir, encoder->recent[channel],
                     encoder->previous[channel]);
    for (i = 0; i < encoder->samples_per_block; i++) {
        sig = encoder->pcm[channel][i];
        iirflt_get_delay(iir, &recent, &previous);
        pflt_set_delay(filter, recent, previous);
        pflt_put_sig(filter, i, sig);
        /* @hack spelling: the residual copied through residual, where
         * pflt_get_rsig's result times gain would do */
        sample = pflt_get_rsig(filter, i);
        residual = sample;
        sample = residual * filter->gain;
        /* @hack spelling: the clip stored inside the sign test, whose 1 or 0
         * picks the rounding, where a statement of its own and residual < 0
         * would do */
        /* NOLINTNEXTLINE(readability-avoid-nested-conditional-operator) */
        quantized_sample = ((((residual = ADXSJE_CLIP16(sample)) < 0) ? 1 : 0)
                                ? residual - HALF_STEP
                                : residual + HALF_STEP) /
                           CODE_STEP;
        quantized_sample = ADXSJE_CLIP4(quantized_sample);
        pflt_set_rsig_q(filter, i, quantized_sample);
        /* @hack redundant: a do-while (0) and a continue around the multiply,
         * where the statement alone would do */
        do {
            quantized_sample = quantized_sample * filter->scale;
            /* NOLINTNEXTLINE(*-continue,readability-redundant-control-flow) */
            continue;
        } while (0);
        iirflt_put_sig(filter->iir, ADXSJE_CLIP16(quantized_sample));
    }
    return 0;
}

void adxsje_set_rsig(ADXSJE encoder, Sint32 channel)
{
    Sint16 *residual;
    Sint16 *scaled_residual;
    Sint16 *decoded;
    Uint8 *encoded_block;
    Sint32 codes_per_byte;
    struct prediction_filter *filter;
    Sint32 byte_index;
    Sint32 sample_in_byte;
    Sint32 i;
    Sint32 sample;
    Sint32 quantized_sample;
    Sint32 decoded_sample;
    Sint8 code;
    Sint32 value;

#ifdef NONMATCHING
    /* sample 0 sets it before its first use, which GCC can't see */
    sample_in_byte = 1;
#endif
    residual = encoder->residual[channel];
    scaled_residual = encoder->scaled_residual[channel];
    decoded = encoder->decoded[channel];
    encoded_block = encoder->encoded_block[channel];
    codes_per_byte = BYTE_BITS / encoder->bits_per_sample;
    filter = encoder->filters[channel];
    for (byte_index = -1, i = 0; i < encoder->samples_per_block; i++) {
        residual[i] = pflt_get_rsig(filter, i);
        sample = residual[i];
        /* @hack spelling: two stores joined by a comma, where two statements
         * would do */
        sample = sample * filter->gain, value = ADXSJE_CLIP16(sample);
        scaled_residual[i] = value;
        quantized_sample = (scaled_residual[i] < 0)
                               ? (value - HALF_STEP) / CODE_STEP
                               : (value + HALF_STEP) / CODE_STEP;
        code = ADXSJE_CLIP4(quantized_sample);
        decoded_sample = code * filter->scale * filter->gain;
        decoded_sample = ADXSJE_CLIP16(decoded_sample);
        decoded[i] = decoded_sample;
        if (i % codes_per_byte == 0) {
            encoded_block[++byte_index] = 0;
            sample_in_byte = 1;
        }
#ifdef NONMATCHING
        /* a left shift of a negative value is undefined, so this side shifts
         * the code as a Uint8 */
        code = (Uint8)((Uint8)code << (BYTE_BITS - encoder->bits_per_sample)) >>
               (BYTE_BITS - encoder->bits_per_sample);
#else
        code = (Uint8)(code << (BYTE_BITS - encoder->bits_per_sample)) >>
               (BYTE_BITS - encoder->bits_per_sample);
#endif
        /* @hack redundant: a read of bits_per_sample, whose value goes
         * unused */
        (void)encoder->bits_per_sample;
        encoded_block[byte_index] |= code
                                     << ((codes_per_byte - sample_in_byte) *
                                         encoder->bits_per_sample);
        sample_in_byte++;
    }
}

/* encode a block of each channel, a short last block padded with zeros.
 * returns a block's samples, or 0 where none are left or the inputs hold too
 * few */
Sint32 adxsje_encode_blk(ADXSJE encoder)
{
    Sint16 recent;
    Sint16 previous;
    Sint32 pos;
    Sint16 *buf[ADXSJE_MAX_CH];
    Sint32 count;
    Sint32 channel;
    struct prediction_filter *filter;
    struct iir_filter *iir;

    buf[0] = encoder->pcm[0];
    buf[1] = encoder->pcm[1];
    pos = encoder->encoded_samples;
    count = ADXSJE_MIN(encoder->samples_per_block,
                       encoder->samples - encoder->encoded_samples);
    if (adxsje_get_pcm_data(encoder, pos, count, buf) == 0) {
        return 0;
    }
    if (count < encoder->samples_per_block) {
        for (channel = 0; channel < encoder->channels; channel++) {
            if (buf[channel] != NULL) {
                memset(&buf[channel][count], 0,
                       (encoder->samples_per_block - count) * sizeof(Sint16));
            }
        }
    }
    encoder->encoded_samples += encoder->samples_per_block;
    for (channel = 0; channel < encoder->channels; channel++) {
        adxsje_calc_rsig(encoder, channel);
        filter = encoder->filters[channel];
        iir = filter->iir;
        encoder->scale[channel] = filter->scale;
        encoder->gain[channel] = filter->gain;
        iirflt_get_delay(iir, &recent, &previous);
        encoder->recent[channel] = recent;
        encoder->previous[channel] = previous;
        adxsje_set_rsig(encoder, channel);
    }
    return encoder->samples_per_block;
}

/* put each channel's block on the output: its scale less 1, big-endian, then
 * its codes. returns the bytes */
Sint32 adxsje_output_sdata(ADXSJE encoder)
{
    SJ output;
    Sint32 channel;
    Sint32 len;
    Sint16 scale;
    Sint8 byte;

    output = encoder->output;
    for (channel = 0, len = 0; channel < encoder->channels; channel++) {
        /* @hack spelling: scale set to 0 and then or-ed with the scale less
         * 1, where storing the scale less 1 would do */
        scale = 0;
        scale |= encoder->scale[channel] - 1;
        byte = scale >> BYTE_BITS;
        adxsje_write68(&byte, 1, 1, output);
        byte = scale;
        adxsje_write68(&byte, 1, 1, output);
        adxsje_write68(encoder->encoded_block[channel], 1, CODE_BYTES, output);
        len += ADXSJE_BLKLEN;
    }
    return len;
}

/* encode blocks while samples remain and the output has room for a block of
 * each channel. returns the bytes written */
Sint32 adxsje_encode_data(ADXSJE encoder)
{
    SJ output;
    Sint32 len;
    Sint32 nblk;

    len = 0;
    output = encoder->output;
    do {
        nblk = SJ_GetNumData(output, SJ_LIN_FREE) / ADXSJE_BLKLEN /
               encoder->channels;
        if (nblk <= 0) {
            break;
        }
        if (adxsje_encode_blk(encoder) == 0) {
            break;
        }
        nblk = adxsje_output_sdata(encoder);
        len += nblk;
    } while (encoder->encoded_samples < encoder->samples);
    return len;
}

/* put the footer on the output: ADX_END_MARK, its length, and that many
 * zeros. returns the footer's bytes, or 0, writing nothing, where the output
 * has less room. without loops the footer takes block_bytes. with them it
 * runs from pos to the first sector boundary at or past pos + block_bytes.
 * pos holds encoded_samples, a count of samples, where a sector boundary
 * wants the stream's bytes, which encoded_bytes counts */
Sint32 adxsje_write_end_code(ADXSJE encoder)
{
    SJ output;
    Sint32 len;
    Sint32 pad;
    Sint32 pos;
    Sint32 nfree;
    Sint32 i;
    Sint16 halfword;
    Sint8 zero;

    output = encoder->output;
    pos = encoder->encoded_samples;
    if (encoder->loop_count <= 0) {
        len = encoder->block_bytes;
    } else {
        len = ((encoder->block_bytes + pos + SECTOR_BYTES - 1) / SECTOR_BYTES *
               SECTOR_BYTES) -
              pos;
    }
    pad = len - ADX_DTOFS_BASE;
    nfree = SJ_GetNumData(output, SJ_LIN_FREE);
    if (nfree < len) {
        return 0;
    }
    halfword = ADX_END_MARK;
    adxsje_write68(&halfword, 2, 1, output);
    halfword = pad;
    adxsje_write68(&halfword, 2, 1, output);
    zero = 0;
    for (i = 0; i < pad; i++) {
        adxsje_write68(&zero, 1, 1, output);
    }
    return len;
}

/* put the header on sjo_in. returns the data offset it gives, its bytes past
 * the first ADX_DTOFS_BASE, or 0 where a write fails */
Sint32 adxsje_output_header(ADXSJE encoder, SJ sjo_in)
{
    Sint32 word;
    Sint16 halfword;
    Sint8 byte;
    Sint32 copyright_bytes;
    Sint32 len;
    Sint32 i;
    Sint32 hdsize;
    SJ output;

    copyright_bytes = strlen(adxsje_copyright);
    output = sjo_in;
    halfword = ADX_MARK;
    if (adxsje_write68(&halfword, 2, 1, output) != 1) {
        goto err;
    }
    halfword = encoder->header_info_size;
    if (adxsje_write68(&halfword, 2, 1, output) != 1) {
        goto err;
    }
    byte = encoder->encoding;
    if (adxsje_write68(&byte, 1, 1, output) != 1) {
        goto err;
    }
    byte = encoder->block_bytes;
    if (adxsje_write68(&byte, 1, 1, output) != 1) {
        goto err;
    }
    byte = encoder->bits_per_sample;
    if (adxsje_write68(&byte, 1, 1, output) != 1) {
        goto err;
    }
    byte = encoder->channels;
    if (adxsje_write68(&byte, 1, 1, output) != 1) {
        goto err;
    }
    word = encoder->sample_rate;
    if (adxsje_write68(&word, 4, 1, output) != 1) {
        goto err;
    }
    word = encoder->total_samples;
    if (adxsje_write68(&word, 4, 1, output) != 1) {
        goto err;
    }
    halfword = encoder->cutoff;
    if (adxsje_write68(&halfword, 2, 1, output) != 1) {
        goto err;
    }
    byte = HEADER_VERSION;
    if (adxsje_write68(&byte, 1, 1, output) != 1) {
        goto err;
    }
    byte = HEADER_REVISION;
    if (adxsje_write68(&byte, 1, 1, output) != 1) {
        goto err;
    }
    len = FORMAT_BYTES;
    if (encoder->loop_count > 0) {
        halfword = encoder->loop_leadin;
        if (adxsje_write68(&halfword, 2, 1, output) != 1) {
            goto err;
        }
        len += 2;
        halfword = encoder->loop_count;
        if (adxsje_write68(&halfword, 2, 1, output) != 1) {
            goto err;
        }
        len += 2;
        for (i = 0; i < encoder->loop_count; i++) {
            halfword = i;
            if (adxsje_write68(&halfword, 2, 1, output) != 1) {
                goto err;
            }
            len += 2;
            halfword = LOOP_TYPE;
            if (adxsje_write68(&halfword, 2, 1, output) != 1) {
                goto err;
            }
            len += 2;
            word = encoder->loop_start;
            if (adxsje_write68(&word, 4, 1, output) != 1) {
                goto err;
            }
            len += 4;
            word = encoder->loop_start_offset;
            if (adxsje_write68(&word, 4, 1, output) != 1) {
                goto err;
            }
            len += 4;
            word = encoder->loop_end;
            if (adxsje_write68(&word, 4, 1, output) != 1) {
                goto err;
            }
            len += 4;
            word = encoder->loop_end_offset;
            if (adxsje_write68(&word, 4, 1, output) != 1) {
                goto err;
            }
            len += 4;
        }
    }
    byte = 0;
    hdsize = encoder->header_info_size;
    for (; len < hdsize - copyright_bytes; len++) {
        if (adxsje_write68(&byte, 1, 1, output) != 1) {
            goto err;
        }
    }
    if (adxsje_write68(adxsje_copyright, 1, copyright_bytes, output) !=
        copyright_bytes) {
        goto err;
    }
    len += copyright_bytes;
    return len;
err:
    return 0;
}

/* lay out a looped stream. lead-in samples move the loop's start to a
 * block's start. the header then grows until the loop's first block starts a
 * sector. a header length that isn't a multiple of HEADER_ALIGN takes a block
 * more of lead-in. it sets the loop's byte offsets and padded_samples, but
 * stores no header length. no code calls it */
void adxsje_adjust_hdinfo_size(ADXSJE encoder)
{
    Sint32 len;
    Sint32 value;
    Sint32 offset;

    len = strlen(adxsje_copyright) + FORMAT_BYTES;
    if (encoder->encoding == ADX_ENCODING ||
        encoder->encoding == ADX_ENCODING + 1) {
        len += LOOP_COUNT_BYTES;
    }
    if (encoder->loop_count > 0) {
        value = encoder->loop_start % encoder->samples_per_block;
        len += 6;
        if (value != 0) {
            value = encoder->samples_per_block - value;
            encoder->samples += value - encoder->loop_leadin;
            encoder->loop_start += value - encoder->loop_leadin;
            encoder->loop_end += value - encoder->loop_leadin;
            encoder->loop_leadin = value;
        }
        len += encoder->loop_count * LOOP_BYTES;
        for (;;) {
            if (encoder->loop_start % encoder->samples_per_block != 0) {
                for (;;) {
                }
            }
            value = encoder->loop_start / encoder->samples_per_block *
                    encoder->channels;
            offset =
                ((((value * encoder->block_bytes) + len) / SECTOR_BYTES) + 1) *
                SECTOR_BYTES;
            encoder->loop_start_offset = offset;
            len = encoder->loop_start_offset - (value * encoder->block_bytes);
            value = len % HEADER_ALIGN;
            if (value == 0) {
                break;
            }
            if (value % 2 != 0) {
                for (;;) {
                }
            }
            encoder->loop_leadin += encoder->samples_per_block;
            encoder->samples += encoder->samples_per_block;
            encoder->loop_start += encoder->samples_per_block;
            encoder->loop_end += encoder->samples_per_block;
        }
        value = ((((encoder->loop_end - 1) / encoder->samples_per_block) + 1) *
                 encoder->channels * encoder->block_bytes) +
                len;
        encoder->loop_end_offset = value;
        if (len - ADX_DTOFS_BASE < COF_DTOFS) {
            for (;;) {
            }
        }
    } else {
        len = (len + HEADER_ALIGN - 1) / HEADER_ALIGN * HEADER_ALIGN;
        value = ADXSJE_MAX(len - ADX_DTOFS_BASE, COF_DTOFS);
    }
    encoder->padded_samples =
        (((encoder->total_samples - 1) / encoder->samples_per_block) + 1) *
        encoder->samples_per_block;
}

void ADXSJE_Init(void)
{
    memset(adxsje_obj, 0, sizeof(adxsje_obj));
}

void ADXSJE_Finish(void)
{
    memset(adxsje_obj, 0, sizeof(adxsje_obj));
}

ADXSJE ADXSJE_Create(Sint32 max_channels, SJ *inputs, SJ output)
{
    ADXSJE encoder;
    Sint32 copyright_bytes;
    Sint32 i;
    Sint32 channel;

    copyright_bytes = strlen(adxsje_copyright);
    for (i = 0; i < ADXSJE_MAX_OBJ; i++) {
        if (adxsje_obj[i].used == 0) {
            break;
        }
    }
    if (i == ADXSJE_MAX_OBJ) {
        return NULL;
    }
    encoder = &adxsje_obj[i];
    encoder->max_channels = max_channels;
    for (channel = 0; channel < max_channels; channel++) {
        encoder->inputs[channel] = inputs[channel];
    }
    encoder->output = output;
    encoder->state = ADXSJE_STAT_STOP;
    encoder->count = 0;
    encoder->encoded_bytes = 0;
    encoder->encoded_samples = 0;
    encoder->limit = ENDLESS;
    encoder->header_info_size =
        copyright_bytes + (FORMAT_BYTES + ADX_DTOFS_BASE);
    encoder->header_info_size = (encoder->header_info_size + HEADER_ALIGN - 1) /
                                HEADER_ALIGN * HEADER_ALIGN;
    encoder->header_info_size =
        (encoder->header_info_size - ADX_DTOFS_BASE > COF_DTOFS)
            ? encoder->header_info_size - ADX_DTOFS_BASE
            : COF_DTOFS;
    encoder->encoding = ADX_ENCODING;
    encoder->channels = max_channels;
    encoder->sample_rate = DEFAULT_SAMPLE_RATE;
    encoder->bits_per_sample = DEFAULT_BITS;
    encoder->block_bytes = ADXSJE_BLKLEN;
    encoder->samples_per_block = (encoder->block_bytes - SCALE_BYTES) *
                                 BYTE_BITS / encoder->bits_per_sample;
    encoder->total_samples = ENDLESS;
    encoder->cutoff = DEFAULT_CUTOFF;
    encoder->loop_leadin = 0;
    encoder->loop_count = 0;
    encoder->loop_start = 0;
    encoder->loop_start_offset = 0;
    encoder->loop_end = 0;
    encoder->loop_end_offset = 0;
    encoder->reserved1 = 0;
    encoder->samples = 0;
    encoder->padded_samples =
        (((encoder->total_samples - 1) / encoder->samples_per_block) + 1) *
        encoder->samples_per_block;
    encoder->filters[0] = pflt_create(encoder->samples_per_block);
    encoder->filters[1] = pflt_create(encoder->samples_per_block);
    encoder->used = 1;
    return encoder;
}

void ADXSJE_Destroy(ADXSJE encoder)
{
    if (encoder == NULL) {
        return;
    }
    ADXCRS_Lock();
    if (encoder->filters[0] != NULL) {
        pflt_destroy(encoder->filters[0]);
    }
    if (encoder->filters[1] != NULL) {
        pflt_destroy(encoder->filters[1]);
    }
    /* ADXSJE_Create sets each field from inputs on */
    memset(encoder, 0, (Uint32)(&((struct adxsje_object *)0)->inputs));
    ADXCRS_Unlock();
}

Sint32 ADXSJE_GetStat(ADXSJE encoder)
{
    return encoder->state;
}

void ADXSJE_Start(ADXSJE encoder)
{
    encoder->count = 0;
    encoder->encoded_bytes = 0;
    encoder->encoded_samples = 0;
    encoder->end_requested = 0;
    encoder->state = ADXSJE_STAT_PREP;
}

void ADXSJE_Stop(ADXSJE encoder)
{
    encoder->end_requested = 1;
}

void ADXSJE_SetSfreq(ADXSJE encoder, Sint32 sample_rate)
{
    encoder->sample_rate = sample_rate;
}

void ADXSJE_SetHdInfoSize(ADXSJE encoder, Sint32 size)
{
    encoder->header_info_size = size;
}

void ADXSJE_SetNumChan(ADXSJE encoder, Sint32 channels)
{
    encoder->channels = channels;
}

void ADXSJE_SetOutBps(ADXSJE encoder, Sint32 bps)
{
    encoder->bits_per_sample = bps;
}

void ADXSJE_SetBlkSmpl(ADXSJE encoder, Sint32 samples)
{
    encoder->samples_per_block = samples;
    if (encoder->filters[0] != NULL) {
        pflt_destroy(encoder->filters[0]);
    }
    encoder->filters[0] = pflt_create(encoder->samples_per_block);
    if (encoder->filters[1] != NULL) {
        pflt_destroy(encoder->filters[1]);
    }
    encoder->filters[1] = pflt_create(encoder->samples_per_block);
}

void ADXSJE_SetBlkLen(ADXSJE encoder, Sint32 len)
{
    encoder->block_bytes = len;
}

void ADXSJE_SetTotalNumSmpl(ADXSJE encoder, Sint32 samples)
{
    encoder->samples = samples;
    encoder->total_samples = samples;
}

void ADXSJE_SetCof(ADXSJE encoder, Sint32 cutoff)
{
    encoder->cutoff = cutoff;
}

void ADXSJE_SetNumLoop(ADXSJE encoder, Sint32 loop_count)
{
    encoder->loop_count = loop_count;
}

void ADXSJE_SetLpInsNsmpl(ADXSJE encoder, Sint32 samples)
{
    encoder->loop_leadin = samples;
}

void ADXSJE_SetLpStartPos(ADXSJE encoder, Sint32 pos)
{
    encoder->loop_start = pos;
}

void ADXSJE_SetLpStartOfst(ADXSJE encoder, Sint32 ofst)
{
    encoder->loop_start_offset = ofst;
}

void ADXSJE_SetLpEndPos(ADXSJE encoder, Sint32 pos)
{
    encoder->loop_end = pos;
}

void ADXSJE_SetLpEndOfst(ADXSJE encoder, Sint32 ofst)
{
    encoder->loop_end_offset = ofst;
}

void ADXSJE_SetConfigSfa(ADXSJE encoder, Sint32 channels, Sint32 sample_rate,
                         Sint32 samples)
{
    ADXSJE_SetNumChan(encoder, channels);
    ADXSJE_SetSfreq(encoder, sample_rate);
    ADXSJE_SetTotalNumSmpl(encoder, samples);
    ADXSJE_SetHdInfoSize(encoder, SFA_HDINFO_SIZE);
}

Sint32 ADXSJE_GetEncDtLen(ADXSJE encoder)
{
    return encoder->encoded_bytes;
}

Sint32 ADXSJE_GetEncNumSmpl(ADXSJE encoder)
{
    return encoder->encoded_samples;
}

void ADXSJE_SetEncPos(ADXSJE encoder, Sint32 pos)
{
    encoder->encoded_samples = pos;
}

Sint32 ADXSJE_GetEncPos(ADXSJE encoder)
{
    return encoder->encoded_samples;
}

void ADXSJE_SetInSj(ADXSJE encoder, Sint32 channel, SJ sj)
{
    if (channel >= 0 && channel < ADXSJE_MAX_CH) {
        encoder->inputs[channel] = sj;
    }
}

void ADXSJE_SetOutSj(ADXSJE encoder, SJ sj)
{
    encoder->output = sj;
}

/* put the header on the output and set each channel's filter. where a write
 * fails, the state stays ADXSJE_STAT_PREP. the next step then writes the
 * header again from its start */
void adxsje_encode_prep(ADXSJE encoder)
{
    Sint32 len;
    Sint32 channel;

    if ((len = adxsje_output_header(encoder, encoder->output)) == 0) {
        return;
    }
    encoder->encoded_bytes += len;
    for (channel = 0; channel < encoder->channels; channel++) {
        pflt_calc_coef(encoder->filters[channel], encoder->cutoff,
                       encoder->sample_rate);
    }
    encoder->state = ADXSJE_STAT_EXEC;
}

/* encode until the samples run out or ADXSJE_Stop asks, then put the footer
 * on the output */
void adxsje_encode_exec(ADXSJE encoder)
{
    Sint32 len;

    /* @hack spelling: a test, a do-while (1) and a goto back to the test,
     * where a while loop would do */
top:
    if (encoder->end_requested == 0) {
        do {
            if ((len = adxsje_encode_data(encoder)) == 0) {
                return;
            }
            encoder->encoded_bytes += len;
            if (encoder->encoded_samples >= encoder->samples) {
                encoder->end_requested = 1;
                goto top;
            }
        } while (1);
    }
    if (adxsje_write_end_code(encoder) > 0) {
        encoder->state = ADXSJE_STAT_END;
    }
}

void ADXSJE_ExecHndl(ADXSJE encoder)
{
    if (encoder->state == ADXSJE_STAT_PREP) {
        adxsje_encode_prep(encoder);
    } else if (encoder->state == ADXSJE_STAT_EXEC) {
        adxsje_encode_exec(encoder);
    }
}

void ADXSJE_ExecServer(void)
{
    ADXSJE encoder;
    Sint32 i;

    for (i = 0; i < ADXSJE_MAX_OBJ; i++) {
        encoder = &adxsje_obj[i];
        if (encoder->used == 1) {
            ADXSJE_ExecHndl(encoder);
        }
    }
}
