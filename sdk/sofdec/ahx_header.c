/* the reader of the ADX header that comes before an AHX stream. every field
 * is big-endian */
#include <adx_dcd.h>
#include <ahx_header.h>
#include <sg_xpt.h>

/* the "(c)CRI" a header ends with */
#define COPYRIGHT_BYTES 6
#define FORMAT_BYTES 16
#define LOOP_COUNT_BYTES 4
#define LOOP_BYTES 20
#define CHANNEL_BYTES 12

/* a header's fields can be unaligned, so these read a byte at a time */
#define BE16(p) (((p)[0] << 8) | (p)[1])
#define BE32(p)                                                                \
    (((Uint32)(p)[0] << 24) | ((p)[1] << 16) | ((p)[2] << 8) | (p)[3])

static void read_format(const Uint8 *p, struct mpeg_stream_info *info)
{
    info->encoding = *p++;
    info->block_bytes = *p++;
    info->sample_bits = *p++;
    info->channels = *p++;
    info->sample_rate = BE32(p);
    p += 4;
    info->total_samples = BE32(p);
    p += 4;
    info->cutoff = BE16(p);
}

static void read_loop_count(const Uint8 *p, struct mpeg_stream_info *info)
{
    p += 2;
    info->loop_count = BE16(p);
}

static void read_loop(const Uint8 *p, struct ahx_loop *loop)
{
    loop->number = BE16(p);
    p += 2;
    loop->start_sample = BE32(p);
    p += 4;
    loop->start_offset = BE32(p);
    p += 4;
    loop->end_sample = BE32(p);
    p += 4;
    loop->end_offset = BE32(p);
}

/* @hack spelling: the four options read one by one, where a loop over them
 * would do */
#define READ_OPTION(cursor, option)                                            \
    do {                                                                       \
        (option).present = *(cursor)++;                                        \
        if ((option).present > 0) {                                            \
            (option).value = *(cursor);                                        \
        }                                                                      \
        (cursor)++;                                                            \
    } while (0)

static void read_channel(const Uint8 *p, struct mpeg_channel *channel)
{
    channel->out_vol_present = BE16(p);
    p += 2;
    if (channel->out_vol_present > 0) {
        channel->out_vol = BE16(p);
    }
    p += 2;
    READ_OPTION(p, channel->option[OPTION_DRCT_LVL]);
    READ_OPTION(p, channel->option[OPTION_FXCH]);
    READ_OPTION(p, channel->option[OPTION_FXLEV]);
    READ_OPTION(p, channel->option[OPTION_PAN]);
}

Sint32 ahx_read_header(const Uint8 *buffer, Sint32 size, Sint32 *length,
                       struct mpeg_stream_info *info)
{
    const Uint8 *p = buffer;
    Uint16 mark;
    Uint16 remaining;
    Sint32 channel;

    if (length) {
        *length = 0;
    }
    if (size < AHX_HEADER_LEAD) {
        return -1;
    }
    mark = BE16(p);
    p += 2;
    if (mark != ADX_MARK) {
        return -4;
    }
    remaining = BE16(p);
    p += 2;
    if (length) {
        *length = remaining;
    }
    if (!info) {
        return 0;
    }
    if (size < remaining + AHX_HEADER_LEAD) {
        return -2;
    }
    /* @bug memory: a header length under COPYRIGHT_BYTES wraps remaining, a
     * Uint16, to a large count. the subtraction at 0x8c1774d6 and the
     * conversion at 0x8c1774da let read_format at 0x8c1774e2 read past size */
    remaining -= COPYRIGHT_BYTES;
    if (remaining < FORMAT_BYTES) {
        return -2;
    }
    read_format(p, info);
    p += FORMAT_BYTES;
    remaining -= FORMAT_BYTES;
    if (remaining < LOOP_COUNT_BYTES) {
        return -2;
    }
    read_loop_count(p, info);
    p += LOOP_COUNT_BYTES;
    if (remaining < info->loop_count * LOOP_BYTES) {
        return -3;
    }
    /* @bug memory: read_loop at 0x8c177514 reads a loop even where the count
     * is 0, for which the check at 0x8c177506 asks no loop bytes */
    read_loop(p, &info->loop);
    /* @bug logic: read_channel reads the first channel from the loop records
     * where loops are present. 0x8c177522 takes the loops off remaining, but p
     * stays at their start, and 0x8c17753e passes it to read_channel */
    remaining -= info->loop_count * LOOP_BYTES;
    /* @bug memory: a header of three channels or more writes past
     * info->channel's two records. 0x8c177548 tests the header's count alone,
     * and 0x8c17753c calls read_channel */
#ifdef NONMATCHING
    /* this side repairs the write: it refuses a header of more channels than
     * info->channel holds, as it refuses fields that run past the header */
    if (info->channels >
        (Sint32)(sizeof(info->channel) / sizeof(info->channel[0]))) {
        return -2;
    }
#endif
    for (channel = 0; channel < info->channels; channel++) {
        if (remaining < CHANNEL_BYTES) {
            return 0;
        }
        read_channel(p, &info->channel[channel]);
        p += CHANNEL_BYTES;
        remaining -= CHANNEL_BYTES;
    }
    return 0;
}
