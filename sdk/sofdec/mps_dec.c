/* the parser's reading of pack, system and packet headers, through
 * sfd_bits.h's bit reader */
#include <mps.h>
#include <sg_xpt.h>

#include "../sofdec/sfd_bits.h"

/* a start code's bytes, and those of its 0x000001 prefix */
#define START_CODE_BYTES 4
#define PREFIX_BYTES 3
/* an MPEG-1 pack header's bytes, its start code included */
#define PACK_BYTES 12

/* READ_BITS(2, out), with out shifted in statements of its own. a 2-bit
 * field crosses words only from current's last bit, so its shifts by bit are
 * by 1 */
#define READ_TWO(out)                                                          \
    do {                                                                       \
        if (bit >= 30) {                                                       \
            bit -= 30;                                                         \
            if (bit != 0) {                                                    \
                current |= lookahead >> 1;                                     \
                (out) = current;                                               \
                (out) >>= 30;                                                  \
                current = lookahead << 1;                                      \
            } else {                                                           \
                (out) = current;                                               \
                (out) >>= 30;                                                  \
                current = lookahead;                                           \
            }                                                                  \
            READ_WORD(lookahead);                                              \
        } else {                                                               \
            bit += 2;                                                          \
            (out) = current;                                                   \
            (out) >>= 30;                                                      \
            current <<= 2;                                                     \
        }                                                                      \
    } while (0)

/* a stamp's 3, 15 and 15-bit fields, joined and divided by 4 as struct
 * mps_packet's pts holds it */
#define TIMESTAMP(high, middle, low)                                           \
    (((high) << 28) | ((middle) << 13) | ((low) >> 2))
#define READ_TIMESTAMP(out)                                                    \
    {                                                                          \
        SKIP_BITS(4, WORD_BITS);                                               \
        READ_BITS(3, high);                                                    \
        SKIP_BITS(1, WORD_BITS);                                               \
        READ_BITS(15, middle);                                                 \
        SKIP_BITS(1, WORD_BITS);                                               \
        READ_BITS(15, low);                                                    \
        SKIP_BITS(1, WORD_BITS);                                               \
        (out) = TIMESTAMP(high, middle, low);                                  \
    }

static Bool mpsdec_DecOneHd(struct mps_handle *handle, const Uint8 *data,
                            Sint32 available, Sint32 *used, Sint32 *kind);
static void mpsdec_DecPackHd(struct mps_pack *header, const Uint8 *data,
                             Sint32 *used);
static void mpsdec_DecSysHd(struct mps_system *header, const Uint8 *data,
                            Sint32 *used);
static void mpsdec_DecPketHd(struct mps_packet *header, const Uint8 *data,
                             Sint32 *used, Sint32 length_bytes);

void MPSDEC_Init(void)
{
}

void MPSDEC_Finish(void)
{
}

Sint32 MPS_SetPketLenField(struct mps_handle *handle, Sint32 bytes)
{
    if (MPSLIB_CheckHn(handle) != 0) {
        return MPSLIB_SetErr(NULL, MPS_ERR_LENGTH);
    }
    handle->length_bytes = bytes;
    return 0;
}

Sint32 MPS_DecHd(struct mps_handle *handle, const Uint8 *data, Sint32 available,
                 Sint32 *used, Sint32 *kinds)
{
    Sint32 size;
    Sint32 kind;
    Bool again;
    Sint32 system_kind;

    *used = 0;
    *kinds = MPS_NONE;
    if (MPSLIB_CheckHn(handle) != 0) {
        return MPSLIB_SetErr(NULL, MPS_ERR_DECODE);
    }
    while (available >= START_CODE_BYTES) {
        again = mpsdec_DecOneHd(handle, data, available, &size, &kind);
        *kinds |= kind;
        data += size;
        available -= size;
        *used += size;
        if (!again) {
            break;
        }
    }
    if (*kinds & MPS_SYSTEM) {
        struct mps_system *system = &handle->last_system;
        if (system->audio_bound != 0) {
            system_kind = MPS_AUDIO;
        } else if (system->video_bound != 0) {
            system_kind = MPS_VIDEO;
        } else {
            system_kind = MPS_OTHER;
        }
        handle->systems[system_kind] = handle->last_system;
    }
    return 0;
}

static Bool mpsdec_DecOneHd(struct mps_handle *handle, const Uint8 *data,
                            Sint32 available, Sint32 *used, Sint32 *kind)
{
    Bool again = FALSE;

    *used = 0;
    *kind = MPSDEL_CheckDelim(data);
    switch (*kind) {
    /* @hack redundant: the empty MPS_END case */
    case MPS_END:
        break;
    case MPS_PACK:
        mpsdec_DecPackHd(&handle->pack, data, used);
        again = TRUE;
        break;
    case MPS_SYSTEM:
        mpsdec_DecSysHd(&handle->last_system, data, used);
        again = TRUE;
        break;
    case MPS_PACKET:
        mpsdec_DecPketHd(&handle->packet, data, used, handle->length_bytes);
        break;
    }
    return again;
}

static void mpsdec_DecPackHd(struct mps_pack *header, const Uint8 *data,
                             Sint32 *used)
{
    Uint32 marker;
    Uint32 rate;
    Sint32 high;
    Sint32 middle;
    Sint32 low;
    /* @hack redundant: cursor, bit and lookahead declared register, where plain
     * locals would do */
    register const Uint8 *cursor;
    register Sint32 bit;
    Uint32 current;
    register Uint32 lookahead;
    /* @hack spelling: a word's bits through word_bits, where WORD_BITS in
     * each SKIP_BITS would do */
    Sint32 word_bits;

    /* @hack spelling: data's address through bit, and cursor from bit, where
     * cursor from data, as in mpsdec_DecSysHd, would do */
    bit = (Sint32)data;
    word_bits = WORD_BITS;
    cursor =
        (const Uint8 *)((Uint32)(bit + START_CODE_BYTES) & ~(WORD_BYTES - 1UL));
    bit += START_CODE_BYTES;
    bit -= (Sint32)cursor;
    bit *= BYTE_BITS;
    READ_WORD(current);
    READ_WORD(lookahead);
    current <<= bit;
    /* @hack spelling: READ_TWO, where READ_BITS(2, marker) would do */
    READ_TWO(marker);
    SKIP_BITS(2, word_bits);
    READ_BITS(3, high);
    SKIP_BITS(1, word_bits);
    READ_BITS(15, middle);
    SKIP_BITS(1, word_bits);
    READ_BITS(15, low);
    SKIP_BITS(1, word_bits);
    SKIP_BITS(1, word_bits);
    READ_BITS(22, rate);
    SKIP_BITS(1, word_bits);
    /* @hack redundant: a test of high in a block no path reaches */
    if (0) {
        if (high) {
        }
    }
    header->mpeg1 = marker ? FALSE : TRUE;
    header->clock = TIMESTAMP(high, middle, low);
    header->mux_rate = rate;
    *used = PACK_BYTES;
}

static void mpsdec_DecSysHd(struct mps_system *header, const Uint8 *data,
                            Sint32 *used)
{
    const Uint8 *end;
    const Uint8 *cursor;
    Sint32 bit;
    Bool more;
    Uint32 current;
    Uint32 lookahead;

    cursor = (const Uint8 *)((Uint32)(data + START_CODE_BYTES) &
                             ~(WORD_BYTES - 1UL));
    bit = (Sint32)data;
    bit += START_CODE_BYTES;
    bit -= (Sint32)cursor;
    bit *= BYTE_BITS;
    READ_WORD(current);
    READ_WORD(lookahead);
    current <<= bit;
    READ_BITS(16, header->length);
    SKIP_BITS(1, WORD_BITS);
    READ_BITS(22, header->rate_bound);
    SKIP_BITS(1, WORD_BITS);
    READ_BITS(6, header->audio_bound);
    READ_FLAG(header->fixed_rate);
    READ_FLAG(header->constrained);
    READ_FLAG(header->audio_lock);
    READ_FLAG(header->video_lock);
    SKIP_BITS(1, WORD_BITS);
    READ_BITS(5, header->video_bound);
    SKIP_BITS(8, WORD_BITS);
    for (;;) {
        more = (Sint32)current < 0;
        if (!more) {
            break;
        }
        SKIP_BITS(24, WORD_BITS);
    }
    bit += BYTE_BITS - 1;
    bit >>= BYTE_SHIFT;
    cursor -= AHEAD_BYTES;
    end = cursor + bit;
    *used = end - data;
    if (MPSDEL_CheckDelim(end) == MPS_NONE) {
        if (MPSDEL_CheckDelim(end + 1) == MPS_PACKET) {
            ++*used;
        }
    }
}

/* a packet header's stuffing byte, and the codes of its optional fields */
#define STUFFING 0xff
#define STD_BUFFER 1 /* 2 bits, before the STD buffer's size */
#define PTS_ONLY 2 /* 4 bits, before a PTS */
#define PTS_AND_DTS 3 /* 4 bits, before a PTS and a DTS */
/* the STD buffer's size counts 128 bytes, or 1024 with its scale bit set */
#define BUFFER_UNIT_SHIFT 7
#define BUFFER_SCALE_SHIFT 3

static void mpsdec_DecPketHd(struct mps_packet *header, const Uint8 *data,
                             Sint32 *used, Sint32 length_bytes)
{
    Sint32 field;
    Sint32 stream;
    Sint32 kind;
    Sint32 channel;
    Sint32 scale;
    Sint32 buffer_size;
    Sint32 high;
    Sint32 middle;
    Sint32 low;
    /* the offset of the length field's end, from which length counts */
    Sint32 length_end;
    const Uint8 *end;
    /* @hack redundant: the reader's four locals declared register, where plain
     * locals would do */
    register Sint32 bit;
    register Uint32 current;
    register Uint32 lookahead;
    register const Uint8 *cursor;

    cursor =
        (const Uint8 *)((Uint32)(data + PREFIX_BYTES) & ~(WORD_BYTES - 1UL));
    bit = (Sint32)data;
    bit += PREFIX_BYTES;
    bit -= (Sint32)cursor;
    bit *= BYTE_BITS;
    READ_WORD(current);
    READ_WORD(lookahead);
    current <<= bit;
    GET_BITS(8, stream);
    header->stream_id = stream;
    if (stream >= MPS_STREAM_VIDEO_FIRST && stream <= MPS_STREAM_VIDEO_LAST) {
        kind = MPS_KIND_VIDEO;
        channel = stream - MPS_STREAM_VIDEO_FIRST;
    } else if (stream >= MPS_STREAM_AUDIO_FIRST &&
               stream <= MPS_STREAM_AUDIO_LAST) {
        kind = MPS_KIND_AUDIO;
        channel = stream - MPS_STREAM_AUDIO_FIRST;
    } else if (stream == MPS_STREAM_PRIVATE1) {
        kind = MPS_KIND_PRIVATE;
        channel = 1;
    } else if (stream == MPS_STREAM_PRIVATE2) {
        kind = MPS_KIND_PRIVATE;
        channel = 2;
    } else if (stream == MPS_STREAM_PADDING) {
        kind = MPS_KIND_PADDING;
        channel = 0;
    } else {
        kind = MPS_KIND_OTHER;
        channel = 0;
    }
    header->kind = kind;
    header->channel = channel;
    if (length_bytes == MPS_LENGTH_BYTES) {
        READ_BITS(16, header->length);
        length_end = START_CODE_BYTES + MPS_LENGTH_BYTES;
    } else {
        if (bit != 0) {
            header->length = current | (lookahead >> (32 - bit));
            current = lookahead << bit;
        } else {
            header->length = current;
            current = lookahead;
        }
        READ_WORD(lookahead);
        length_end = START_CODE_BYTES + WORD_BYTES;
    }
    if (stream == MPS_STREAM_PRIVATE2 || stream == MPS_STREAM_PADDING) {
        *used = length_end;
        header->payload_length = header->length;
        return;
    }
    for (;;) {
        PEEK_BITS(8, field);
        if (field != STUFFING) {
            break;
        }
        SKIP_BITS(8, WORD_BITS);
    }
    PEEK_BITS(2, field);
    if (field == STD_BUFFER) {
        SKIP_BITS(2, WORD_BITS);
        READ_FLAG(scale);
        READ_BITS(13, buffer_size);
        buffer_size <<= BUFFER_UNIT_SHIFT;
        if (scale) {
            buffer_size <<= BUFFER_SCALE_SHIFT;
        }
        header->buffer_size = buffer_size;
    }
    PEEK_BITS(4, field);
    if (field == PTS_ONLY) {
        READ_TIMESTAMP(header->pts);
        header->dts = MPS_NO_STAMP;
    } else if (field == PTS_AND_DTS) {
        READ_TIMESTAMP(header->pts);
        READ_TIMESTAMP(header->dts);
    } else {
        SKIP_BITS(8, WORD_BITS);
        header->pts = MPS_NO_STAMP;
        header->dts = MPS_NO_STAMP;
    }
    /* @hack type: the cursor and the byte count added as Sint32s, where
     * adding the count to the pointer would do */
    end = (const Uint8 *)((Sint32)(cursor - AHEAD_BYTES) +
                          ((bit + BYTE_BITS - 1) >> BYTE_SHIFT));
    *used = end - data;
    header->payload_length = header->length + length_end - *used;
}
