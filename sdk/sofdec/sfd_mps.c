/* the system stage, which demultiplexes the input ring's MPEG program
 * stream. its outputs are the video and audio rings and the user channels. a
 * packet whose output has no room for all of its payload stays in the input
 * ring */
#include <machine.h>
#include <mps.h>
#include <sfd.h>
#include <sg_xpt.h>
#include <sj.h>
#include <sofdec.h>
#include <sofdec_uty.h>
#include "../sofdec/sfd_stage.h"

#define PARSERS 32
/* a server call reads units until it has read this many bytes */
#define BYTE_BUDGET 0x19000
/* a private stream 2 payload's offset in its pack, past the pack's headers */
#define PAYLOAD_OFFSET (MPS_PACK_HEADER_SIZE + MPS_PACKET_HEADER_SIZE)
/* the user channel that receives the pack that holds the stream header */
#define HEADER_CHANNEL 0
#define NO_PTS 0x7fffffff
/* the bytes of an MPEG start code, as the program end code is */
#define START_CODE_SIZE 4
/* the bytes a second one unit of a pack header's mux_rate counts */
#define MUX_RATE_BYTES 50
/* a packet copy's results, other than an error. NO_ROOM leaves the packet
 * for a later call. TAKEN means the payload is copied or dropped */
#define NO_ROOM 0
#define TAKEN 1
#define INIT_FAILED ((Sint32)0xff000d01)
#define DECODE_FAILED ((Sint32)0xff000d03)
#define NOT_PACKET ((Sint32)0xff000d05)
#define INVALID_PACKET ((Sint32)0xff000d06)
#define CREATE_FAILED ((Sint32)0xff000d08)
#define INVALID_CALLBACK ((Sint32)0xff000d09)
#define DESTROY_FAILED ((Sint32)0xff000d0a)
#define NO_RING ((Sint32)0xff000d0b)
#define REPARSE_FAILED ((Sint32)0xff000d0d)

typedef Sint32 (*packet_copy_fn)(struct sfd_handle *, Sint32, char *, Sint32,
                                 Sint32);
static struct {
    struct mps_library library;
    struct mps_handle additional[PARSERS - 1];
} sfmps_libwork;
/* the copies into a joint whose second chunk came up short */
static Sint32 short_sj_copies;

static Bool sfmps_ChkFatal(void);
static Sint32 sfmps_DecodeSomeUnit(struct sfd_handle *handle);
static Sint32 sfmps_RingGetRead(struct sfd_handle *handle, char **data,
                                Sint32 *size);
static Sint32 sfmps_RingAddRead(struct sfd_handle *handle, Sint32 size);
static Sint32 sfmps_DecodeOneUnit(struct sfd_handle *handle, char *data,
                                  Sint32 size, Sint32 *used);
static void sfmps_SkipZeroPack(const struct sfd_handle *handle, char *data,
                               Sint32 size, Sint32 *used);
static Sint32 sfmps_CopyPketData(struct sfd_handle *handle, char *data,
                                 Sint32 size, Sint32 *used);
static Sint32 sfmps_CopyUsrSj(struct sfd_handle *handle, Sint32 channel,
                              char *data, Sint32 size);
static Sint32 sfmps_CopySj(SJ joint, char *data, Sint32 size);
static Sint32 sfmps_ExecCopySj(SJ joint, char *data, Sint32 size);
static Sint32 sfmps_CopyDstBuft(struct sfd_handle *handle, Sint32 buffer,
                                char *data, Sint32 size);
static void sfmps_SwitchCopy(char *destination, char *source, Sint32 size);
static void sfmps_SetTermDst(struct sfd_handle *handle, Bool value);
static Bool sfmps_GetTermDst(struct sfd_handle *handle);
static void sfmps_Concat(struct sfd_handle *handle);
static Bool sfmps_IsTerm(struct sfd_handle *handle, Sint32 size, Sint32 found);
static void sfmps_ProcPrep(struct sfd_handle *handle);
static void sfmps_ChkPrepFlg(struct sfd_handle *handle);
static Bool sfmps_GetPrepDst(struct sfd_handle *handle);
static void sfmps_SetPrepDst(struct sfd_handle *handle, Bool value);
static Bool sfmps_IsPrepEnd(struct sfd_handle *handle, Sint32 buffer);
static void sfmps_AdjustAvPlay(struct sfd_handle *handle);
static void sfmps_GetStmNum(struct sfd_handle *handle, Sint32 *audio,
                            Sint32 *video);
static void sfmps_SetMvInf(struct sfd_handle *handle);
static void sfmps_SetMpsHd(struct sfd_handle *handle);
static void sfmps_SetMpsRaw(struct sfd_handle *handle,
                            struct mps_handle *parser, char *data, Sint32 size);
static void sfmps_InitInf(struct sfd_system_work *system);
static void sfmps_ErrFn(void *user, Sint32 error);
static Sint32 sfmps_DestroySub(struct mps_handle *parser);
static void sfmps_SetCustomPketLen(struct sfd_handle *handle);
static Sint32 sfmps_ReprocessHdr(struct sfd_handle *handle,
                                 struct sfd_system_work *system,
                                 struct sfd_system_seek *saved);
static struct sfd_system_seek *sfmps_GetHd(struct sfd_handle *handle);
static const packet_copy_fn sfmps_CopyPketFn[MPS_KIND_OTHER];

static Sint32 SFMPS_Init(struct sfd_handle *handle, Sint32 first, Sint32 second,
                         Sint32 third)
{
    Sint32 error;
    if (sfmps_ChkFatal()) {
        for (;;) {
        }
    }
    error = MPS_Init(PARSERS, &sfmps_libwork);
    if (error != 0) {
        return SFLIB_SetErr(NULL, INIT_FAILED);
    }
    short_sj_copies = 0;
    return 0;
}
static Bool sfmps_ChkFatal(void)
{
    return FALSE;
}
static Sint32 SFMPS_Finish(struct sfd_handle *handle, Sint32 first,
                           Sint32 second, Sint32 third)
{
    MPS_Finish();
    return 0;
}
static Sint32 SFMPS_ExecServer(struct sfd_handle *handle, Sint32 first,
                               Sint32 second, Sint32 third)
{
    Sint32 error;
    if (sfmps_GetTermDst(handle) == TRUE) {
        return 0;
    }
    error = sfmps_DecodeSomeUnit(handle);
    if (handle->status == SFD_PREPARING) {
        sfmps_ProcPrep(handle);
    }
    return error;
}
static Sint32 sfmps_DecodeSomeUnit(struct sfd_handle *handle)
{
    Sint32 used;
    char *data;
    Sint32 size;
    Sint32 total;
    Sint32 unit;
    Sint32 error;
    total = 0;
    do {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((error = sfmps_RingGetRead(handle, &data, &size)) != 0) {
            break;
        }
        error = sfmps_DecodeOneUnit(handle, data, size, &used);
        if (error != 0) {
            break;
        }
        if (used == 0) {
            break;
        }
        /* the ring moves on by a whole read unit, whatever part of it the
         * headers and payload took */
        /* @hack spelling: the read unit held in unit, where used could take
         * it directly */
        unit = handle->params.read_unit;
        used = unit;
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((error = sfmps_RingAddRead(handle, used)) != 0) {
            break;
        }
        total += used;
    } while (total < BYTE_BUDGET);
    return error;
}
static Sint32 sfmps_RingGetRead(struct sfd_handle *handle, char **data,
                                Sint32 *size)
{
    struct sfd_transfer *stage;
    Sint32 buffer;
    struct sfd_ring_info ring;
    Sint32 error;
    stage = &handle->stages[SFD_SYSTEM];
    buffer = stage->input;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((error = SFBUF_RingGetRead(handle, buffer, &ring)) != 0) {
        return error;
    }
    *data = ring.head.data;
    *size = ring.head.size;
    return 0;
}
static Sint32 sfmps_RingAddRead(struct sfd_handle *handle, Sint32 size)
{
    struct sfd_transfer *stage;
    Sint32 buffer;
    Sint32 error;
    stage = &handle->stages[SFD_SYSTEM];
    buffer = stage->input;
    /* @hack spelling: a test of the call's result that returns it or 0,
     * where returning it would do */
    if ((error = SFBUF_RingAddRead(handle, buffer, size)) != 0) {
        return error;
    }
    return 0;
}
static Sint32 sfmps_DecodeOneUnit(struct sfd_handle *handle, char *data,
                                  Sint32 size, Sint32 *used)
{
    Sint32 error;
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    struct mps_handle *parser;
    Sint32 found;
    Sint32 consumed;
    Sint32 copied;
    Sint32 skipped;
    Sint32 result;
    error = 0;
    *used = 0;
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    parser = system->parser;
    result = MPS_DecHd(parser, (const Uint8 *)data, size, &consumed, &found);
    if (result != 0) {
        error = SFLIB_SetErr(handle, DECODE_FAILED);
    }
    if (found & MPS_SYSTEM) {
        sfmps_SetMpsRaw(handle, parser, data, size);
    }
    if (found == MPS_END && SFCON_IsEndcodeSkip(handle)) {
        sfmps_Concat(handle);
        *used = START_CODE_SIZE;
    } else if (sfmps_IsTerm(handle, size, found)) {
        sfmps_SetTermDst(handle, TRUE);
    } else if (found == MPS_NONE) {
        sfmps_SkipZeroPack(handle, data, size, &skipped);
        *used = skipped;
    } else if (!(found & MPS_PACKET)) {
        error = SFLIB_SetErr(handle, NOT_PACKET);
    } else {
        data += consumed;
        size -= consumed;
        error = sfmps_CopyPketData(handle, data, size, &copied);
        if (copied != 0) {
            *used = consumed + copied;
        }
    }
    return error;
}
static void sfmps_SkipZeroPack(const struct sfd_handle *handle, char *data,
                               Sint32 size, Sint32 *used)
{
    Sint32 unit;
    unit = handle->params.read_unit;
    if (size >= unit) {
        *used = unit;
    } else {
        *used = 0;
    }
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}
static Sint32 sfmps_CopyPketData(struct sfd_handle *handle, char *data,
                                 Sint32 size, Sint32 *used)
{
    struct sfd_transfer *stage;
    struct mps_handle *parser;
    struct mps_packet packet;
    Sint32 kind;
    Sint32 channel;
    Sint32 payload;
    Sint32 pts;
    Sint32 error;
    Sint32 result;
    error = 0;
    *used = 0;
    stage = &handle->stages[SFD_SYSTEM];
    parser = stage->work.system.parser;
    /* @bug uninitialized: packet's fields are read even where MPS_GetPketHd
     * refuses parser and leaves packet unwritten. MPS_GetPketHd is at
     * 0x8c54e64e, and sfmps_CopyPketData at 0x8c547d9a */
    if (MPS_GetPketHd(parser, &packet) != 0) {
        error = SFLIB_SetErr(handle, INVALID_PACKET);
    }
    kind = packet.kind;
    channel = packet.channel;
    pts = packet.pts;
    payload = packet.payload_length;
    if (size < payload) {
        return 0;
    }
    /* @bug memory: calls the word past sfmps_CopyPketFn's end for a packet of
     * MPS_KIND_OTHER, a stream id that isn't audio, video, private or
     * padding (sfmps_CopyPketData at 0x8c547d9a) */
#ifdef NONMATCHING
    /* this side repairs the call: a packet of MPS_KIND_OTHER is dropped, as
     * sfmps_CopyPadding drops padding */
    if (kind == MPS_KIND_OTHER) {
        result = TAKEN;
    } else {
        result = sfmps_CopyPketFn[kind](handle, channel, data, payload, pts);
    }
#else
    result = sfmps_CopyPketFn[kind](handle, channel, data, payload, pts);
#endif
    switch (result) {
    case TAKEN:
        *used = payload;
        break;
    case NO_ROOM:
        break;
    default:
        error = result;
        break;
    }
    return error;
}

/* define function, which copies an audio or video packet's payload to
 * output's ring. it keeps the stream's earliest time stamp in first_pts, a
 * field of struct sfd_system_work. it drops the payload instead, returning
 * TAKEN, while the condition enabled is off. it drops it too where channel
 * isn't the one the condition channel_select selects */
#define COPY_STREAM(function, enabled, channel_select, output, first_pts)      \
    static Sint32 function(struct sfd_handle *handle, Sint32 channel,          \
                           char *data, Sint32 size, Sint32 pts)                \
    {                                                                          \
        Sint32 selected;                                                       \
        struct sfd_transfer *stage;                                            \
        Sint32 buffer;                                                         \
        struct sfd_system_work *system;                                        \
        if (!SFSET_GetCond(handle, enabled)) {                                 \
            return TAKEN;                                                      \
        }                                                                      \
        selected = SFSET_GetCond(handle, channel_select);                      \
        if (selected != SFD_ANY_CHANNEL && selected != channel) {              \
            return TAKEN;                                                      \
        }                                                                      \
        stage = &handle->stages[SFD_SYSTEM];                                   \
        buffer = stage->outputs[output];                                       \
        system = &stage->work.system;                                          \
        if (pts >= 0 && pts < system->first_pts) {                             \
            system->first_pts = pts;                                           \
        }                                                                      \
        return sfmps_CopyDstBuft(handle, buffer, data, size);                  \
    }
COPY_STREAM(sfmps_CopyAudio, SFD_COND_AUDIO, SFD_COND_AUDIO_CHANNEL,
            SFD_SYSTEM_AUDIO, audio_pts)
COPY_STREAM(sfmps_CopyVideo, SFD_COND_VIDEO, SFD_COND_VIDEO_CHANNEL,
            SFD_SYSTEM_VIDEO, video_pts)
static Sint32 sfmps_CopyPrvate(struct sfd_handle *handle, Sint32 channel,
                               char *data, Sint32 size, Sint32 pts)
{
    Bool saved;
    if (SFHDS_SetHdr(handle, channel, (const Uint8 *)data, size, &saved)) {
        sfmps_SetCustomPketLen(handle);
        if (saved) {
            data -= PAYLOAD_OFFSET;
            size += PAYLOAD_OFFSET;
            sfmps_CopyUsrSj(handle, HEADER_CHANNEL, data, size);
        }
        return TAKEN;
    }
    return sfmps_CopyUsrSj(handle, channel, data, size);
}
static Sint32 sfmps_CopyUsrSj(struct sfd_handle *handle, Sint32 channel,
                              char *data, Sint32 size)
{
    struct sfd_transfer *stage;
    Sint32 buffer;
    struct sfd_user_channel output;
    SJ joint;
    void (*notify)(struct sfd_handle *, Sint32);
    Sint32 mask;
    Sint32 result;
    stage = &handle->stages[SFD_SYSTEM];
    buffer = stage->outputs[SFD_SYSTEM_PRIVATE];
    if (buffer == SFD_NO_BUFFER) {
        return TAKEN;
    }
    SFBUF_GetUoch(handle, buffer, channel, &output);
    joint = output.stream;
    notify = output.notify;
    if (joint == NULL) {
        return TAKEN;
    }
    SFLIB_LockCs(&mask);
    result = sfmps_CopySj(joint, data, size);
    if (result == TAKEN && notify != NULL) {
        notify(handle, channel);
    }
    SFLIB_UnlockCs(&mask);
    return result;
}
static Sint32 sfmps_CopySj(SJ joint, char *data, Sint32 size)
{
    Sint32 copied;
    if (SJ_GetNumData(joint, SJ_LIN_FREE) < size) {
        return NO_ROOM;
    }
    copied = sfmps_ExecCopySj(joint, data, size);
    if (copied == 0) {
        return NO_ROOM;
    }
    size -= copied;
    data += copied;
    if (size > 0) {
        copied = sfmps_ExecCopySj(joint, data, size);
        if (copied != size) {
            short_sj_copies++;
        }
    }
    return TAKEN;
}
static Sint32 sfmps_ExecCopySj(SJ joint, char *data, Sint32 size)
{
    SJCK chunk;
    SJ_GetChunk(joint, SJ_LIN_FREE, size, &chunk);
    sfmps_SwitchCopy(chunk.data, data, chunk.len);
    SJ_PutChunk(joint, SJ_LIN_DATA, &chunk);
    return chunk.len;
}
static Sint32 sfmps_CopyPadding(struct sfd_handle *handle, Sint32 channel,
                                char *data, Sint32 size, Sint32 pts)
{
    return TAKEN;
}
static Sint32 sfmps_CopyDstBuft(struct sfd_handle *handle, Sint32 buffer,
                                char *data, Sint32 size)
{
    struct sfd_ring_info ring;
    char *head;
    Sint32 head_size;
    char *wrap;
    Sint32 wrap_size;
    Sint32 written;
    Sint32 error;
    error = SFBUF_RingGetWrite(handle, buffer, &ring);
    if (error != 0) {
        return error;
    }
    head = ring.head.data;
    head_size = ring.head.size;
    wrap = ring.wrap.data;
    wrap_size = ring.wrap.size;
    written = ring.written;
    if (size > head_size + wrap_size) {
        return NO_ROOM;
    }
    if (size <= head_size) {
        sfmps_SwitchCopy(head, data, size);
    } else {
        sfmps_SwitchCopy(head, data, head_size);
        sfmps_SwitchCopy(wrap, data + head_size, size - head_size);
    }
    error = SFBUF_RingAddWrite(handle, buffer, size, written);
    if (error != 0) {
        return error;
    }
    return TAKEN;
}
static void sfmps_SwitchCopy(char *destination, char *source, Sint32 size)
{
    Sint32 alignment;
    if (size != 0) {
        prefetch(source);
        alignment = (Sint32)destination | (Sint32)source | size;
        if ((alignment & 1) == 0) {
            if ((alignment & 3) == 0) {
                UTY_MemcpyDword((Uint32 *)destination, (const Uint32 *)source,
                                (Uint32)(size >> 2));
            } else {
                UTY_MemcpyWord((Uint16 *)destination, (const Uint16 *)source,
                               (Uint32)(size >> 1));
            }
        } else {
            UTY_MemcpyByte((Uint8 *)destination, (const Uint8 *)source, size);
        }
    }
}

#define SET_OUTPUT_FLAGS(function, set_flag)                                   \
    static void function(struct sfd_handle *handle, Bool value)                \
    {                                                                          \
        struct sfd_transfer *stage;                                            \
        stage = &handle->stages[SFD_SYSTEM];                                   \
        set_flag(handle, stage->outputs[SFD_SYSTEM_AUDIO], value);             \
        set_flag(handle, stage->outputs[SFD_SYSTEM_VIDEO], value);             \
        set_flag(handle, stage->outputs[SFD_SYSTEM_PRIVATE], value);           \
    }
#define GET_OUTPUT_FLAGS(function, get_flag, merge)                            \
    static Bool function(struct sfd_handle *handle)                            \
    {                                                                          \
        struct sfd_transfer *stage;                                            \
        Bool audio;                                                            \
        Bool video;                                                            \
        Bool user;                                                             \
        stage = &handle->stages[SFD_SYSTEM];                                   \
        audio = get_flag(handle, stage->outputs[SFD_SYSTEM_AUDIO]);            \
        video = get_flag(handle, stage->outputs[SFD_SYSTEM_VIDEO]);            \
        user = get_flag(handle, stage->outputs[SFD_SYSTEM_PRIVATE]);           \
        return audio merge video merge user;                                   \
    }

SET_OUTPUT_FLAGS(sfmps_SetTermDst, SFBUF_SetTermFlg)
GET_OUTPUT_FLAGS(sfmps_GetTermDst, SFBUF_GetTermFlg, &)

static void sfmps_Concat(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    system->concat_count++;
    /* @hack redundant: the system stage's address, formed again and
     * discarded */
    (void)&handle->stages[SFD_SYSTEM];
}
static Bool sfmps_IsTerm(struct sfd_handle *handle, Sint32 size, Sint32 found)
{
    struct sfd_create_params *params;
    struct sfd_transfer *stage;
    Sint32 buffer;
    Sint32 unit;
    if (found == MPS_END) {
        return TRUE;
    }
    params = &handle->params;
    unit = params->read_unit;
    if (size >= unit) {
        return FALSE;
    }
    stage = &handle->stages[SFD_SYSTEM];
    buffer = stage->input;
    if (SFBUF_GetTermFlg(handle, buffer) == TRUE) {
        return TRUE;
    }
    return FALSE;
}
static void sfmps_ProcPrep(struct sfd_handle *handle)
{
    sfmps_ChkPrepFlg(handle);
    sfmps_SetMvInf(handle);
    sfmps_AdjustAvPlay(handle);
    sfmps_SetMpsHd(handle);
}
static void sfmps_ChkPrepFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 buffer;
    if (sfmps_GetPrepDst(handle) == TRUE) {
        return;
    }
    stage = &handle->stages[SFD_SYSTEM];
    buffer = stage->input;
    if (SFBUF_GetPrepFlg(handle, buffer) == TRUE &&
        sfmps_IsPrepEnd(handle, buffer)) {
        sfmps_SetPrepDst(handle, TRUE);
    }
}
GET_OUTPUT_FLAGS(sfmps_GetPrepDst, SFBUF_GetPrepFlg, |)
SET_OUTPUT_FLAGS(sfmps_SetPrepDst, SFBUF_SetPrepFlg)
static Bool sfmps_IsPrepEnd(struct sfd_handle *handle, Sint32 buffer)
{
    return TRUE;
}
static void sfmps_AdjustAvPlay(struct sfd_handle *handle)
{
    Sint32 audio;
    Sint32 video;
    sfmps_GetStmNum(handle, &audio, &video);
    if (SFSET_GetCond(handle, SFD_COND_AUDIO) == TRUE && audio == 0 &&
        SFTRN_GetPrepFlg(handle, SFD_VIDEO_OUT)) {
        SFSET_SetCond(handle, SFD_COND_AUDIO, 0);
    }
    if (SFSET_GetCond(handle, SFD_COND_VIDEO) == TRUE && video == 0 &&
        SFTRN_GetPrepFlg(handle, SFD_AUDIO_OUT)) {
        SFSET_SetCond(handle, SFD_COND_VIDEO, 0);
    }
}
static void sfmps_GetStmNum(struct sfd_handle *handle, Sint32 *audio,
                            Sint32 *video)
{
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    struct mps_handle *parser;
    struct mps_system header;
    Sint32 i;
    Sint32 audio_count;
    Sint32 video_count;
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    parser = system->parser;
    audio_count = 0;
    video_count = 0;
    for (i = 0; i < MPS_SYSTEM_KINDS; i++) {
        MPS_GetSysHd(parser, &header, i);
        audio_count =
            audio_count > header.audio_bound ? audio_count : header.audio_bound;
        video_count =
            video_count > header.video_bound ? video_count : header.video_bound;
    }
    system->audio_count = audio_count;
    system->video_count = video_count;
    *audio = system->audio_count;
    *video = system->video_count;
}
static void sfmps_SetMvInf(struct sfd_handle *handle)
{
    struct sfd_movie_info *movie;
    struct sfd_transfer *stage;
    struct mps_handle *parser;
    struct mps_pack pack;
    struct mps_system header;
    Sint32 rate;
    Bool fixed;
    movie = &handle->movie;
    stage = &handle->stages[SFD_SYSTEM];
    parser = stage->work.system.parser;
    MPS_GetPackHd(parser, &pack);
    rate = pack.mux_rate;
    /* @hack redundant: the test of rate against MPS_UNKNOWN, which rate > 0
     * already excludes */
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    if (rate != MPS_UNKNOWN && rate > 0) {
        movie->mux_rate = rate;
    }
    MPS_GetSysHd(parser, &header, MPS_VIDEO);
    fixed = header.fixed_rate;
    if (fixed != MPS_UNKNOWN) {
        movie->fixed = fixed;
    }
}
static void sfmps_SetMpsHd(struct sfd_handle *handle)
{
    struct sfd_system_seek *saved;
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    struct sfd_timer *timer;
    struct sfd_movie_info *movie;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((saved = sfmps_GetHd(handle)) == NULL) {
        return;
    }
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    timer = &handle->timer;
    movie = &handle->movie;
    timer->concat_offset = system->audio_pts - saved->audio_pts;
    if (saved->saved) {
        return;
    }
    saved->byte_rate = movie->mux_rate * MUX_RATE_BYTES;
    saved->fixed = movie->fixed;
    saved->video_count = system->video_count;
    saved->audio_count = system->audio_count;
    saved->video_pts = system->video_pts;
    saved->audio_pts = system->audio_pts;
}
static void sfmps_SetMpsRaw(struct sfd_handle *handle,
                            struct mps_handle *parser, char *data, Sint32 size)
{
    struct sfd_system_seek *saved;
    struct sfd_system_packets *packets;
    struct mps_system header;
    Uint8 *destination;
    Uint32 count;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((saved = sfmps_GetHd(handle)) == NULL || saved->saved) {
        return;
    }
    packets = &saved->packets;
    MPS_GetLastSysHd(parser, &header);
    count = size < SFD_RAW_PACKET_SIZE ? size : SFD_RAW_PACKET_SIZE;
    if (header.video_bound > 0) {
        packets->video_size = count;
        destination = packets->video;
    } else if (header.audio_bound > 0) {
        packets->audio_size = count;
        destination = packets->audio;
    } else {
        return;
    }
    UTY_MemcpyByte(destination, (const Uint8 *)data, count);
}
static Sint32 SFMPS_Create(struct sfd_handle *handle, Sint32 first,
                           Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    struct mps_handle *parser;
    Sint32 error;
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    sfmps_InitInf(system);
    parser = MPS_Create();
    if (parser == NULL) {
        return SFLIB_SetErr(handle, CREATE_FAILED);
    }
    error = MPS_SetErrFn(parser, sfmps_ErrFn, handle);
    if (error != 0) {
        sfmps_DestroySub(parser);
        return SFLIB_SetErr(handle, INVALID_CALLBACK);
    }
    system->parser = parser;
    return 0;
}
static void sfmps_InitInf(struct sfd_system_work *system)
{
    system->parser = NULL;
    system->video_count = 0;
    system->audio_count = 0;
    system->video_pts = NO_PTS;
    system->audio_pts = NO_PTS;
    system->concat_count = 0;
}
static void sfmps_ErrFn(void *user, Sint32 error)
{
    SFLIB_SetErr((struct sfd_handle *)user, error);
}
static Sint32 SFMPS_Destroy(struct sfd_handle *handle, Sint32 first,
                            Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    struct mps_handle *parser;
    Sint32 error;
    stage = &handle->stages[SFD_SYSTEM];
    parser = stage->work.system.parser;
    error = sfmps_DestroySub(parser);
    if (error != 0) {
        return SFLIB_SetErr(handle, DESTROY_FAILED);
    }
    return 0;
}
static Sint32 sfmps_DestroySub(struct mps_handle *parser)
{
    return MPS_Destroy(parser);
}
/* clang-format off */
static SFD_STAGE_NOOP(SFMPS_Standby)
static SFD_STAGE_NOOP(SFMPS_Start)
static SFD_STAGE_NOOP(SFMPS_Stop)
static SFD_STAGE_NOOP(SFMPS_Pause)
SFD_STAGE_REJECT(SFMPS_GetWrite, NO_RING)
SFD_STAGE_REJECT(SFMPS_AddWrite, NO_RING)
SFD_STAGE_REJECT(SFMPS_GetRead, NO_RING)
SFD_STAGE_REJECT(SFMPS_AddRead, NO_RING)

static Sint32 SFMPS_Seek(struct sfd_handle *handle, Sint32 first, Sint32 second,
                         Sint32 third)
/* clang-format on */
{
    struct sfd_system_seek *saved;
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    Sint32 error;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((saved = sfmps_GetHd(handle)) == NULL || !saved->saved) {
        return 0;
    }
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    SFHDS_ReprocessHdr(handle);
    sfmps_SetCustomPketLen(handle);
    error = sfmps_ReprocessHdr(handle, system, saved);
    if (error != 0) {
        return error;
    }
    return 0;
}
static void sfmps_SetCustomPketLen(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    /* @hack redundant: the parser's address, formed and discarded */
    (void)&system->parser;
}
static Sint32 sfmps_ReprocessHdr(struct sfd_handle *handle,
                                 struct sfd_system_work *system,
                                 struct sfd_system_seek *saved)
{
    struct sfd_system_packets *packets;
    struct mps_handle *parser;
    Sint32 used;
    Sint32 found;
    Sint32 video_error;
    Sint32 audio_error;
    packets = &saved->packets;
    parser = system->parser;
    video_error =
        MPS_DecHd(parser, packets->video, packets->video_size, &used, &found);
    audio_error =
        MPS_DecHd(parser, packets->audio, packets->audio_size, &used, &found);
    if (video_error != 0 || audio_error != 0) {
        return SFLIB_SetErr(handle, REPARSE_FAILED);
    }
    return 0;
}
static struct sfd_system_seek *sfmps_GetHd(struct sfd_handle *handle)
{
    if (handle->seek.info == NULL) {
        return NULL;
    }
    if (SFMPS_GetConcatCnt(handle) > 0) {
        return NULL;
    }
    return &handle->seek.info->head.system;
}
Sint32 SFMPS_GetConcatCnt(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_system_work *system;
    stage = &handle->stages[SFD_SYSTEM];
    system = &stage->work.system;
    return system->concat_count;
}

const struct sfd_stage_ops SFD_tr_sd_mps = {
    {SFMPS_Init, SFMPS_Finish, SFMPS_ExecServer, SFMPS_Create, SFMPS_Destroy,
     SFMPS_Standby, SFMPS_Start, SFMPS_Stop, SFMPS_Pause, SFMPS_GetWrite,
     SFMPS_AddWrite, SFMPS_GetRead, SFMPS_AddRead, SFMPS_Seek}};
/* by struct mps_packet's kind, for each kind but MPS_KIND_OTHER */
static const packet_copy_fn sfmps_CopyPketFn[] = {
    sfmps_CopyAudio, sfmps_CopyVideo, sfmps_CopyPrvate, sfmps_CopyPadding};
