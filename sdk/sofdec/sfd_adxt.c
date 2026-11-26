/* the Sofdec audio decoder stage, through ADXT. each server call copies the
 * stream's audio from the stage's input ring into a stream joint. an ADXT
 * player decodes and plays it from there. after a seek, audio->copy first drops
 * any leading silence, header or partial frame. it then drops or pads whole
 * frames until the audio starts with the video */
#include <adx_sfa.h>
#include <cri_adxt.h>
#include <sfd.h>
#include <sg_xpt.h>
#include <sj.h>
#include <sofdec.h>
#include <sofdec_uty.h>
#include <string.h>
#include "../sofdec/sfd_stage.h"

/* an ADX frame: a channel's FRAME_SAMPLES samples in FRAME_BYTES bytes */
#define FRAME_BYTES 18
#define FRAME_SAMPLES 32
/* the step between the offsets sfadxt_SearchAlign tries */
#define SEARCH_STEP 2
/* the bytes sfadxt_ExcludeHdr waits for before it looks for a header */
#define MIN_HEADER_INPUT 0x120
/* the most bytes one copy puts on the joint */
#define COPY_LIMIT 0x19000
/* the first multiplexer version whose audio the stage takes as it comes after
 * a seek. it then neither drops leading silence nor searches for a frame's
 * start */
#define ALIGNED_MUX_VERSION 108
/* the ticks a second of the demultiplexer's stamps: MPEG's 90 kHz clock
 * divided by 4 */
#define PTS_UNIT 22500
/* the boundary SFD_SetAdxtPara rounds the ring and the work up to */
#define BUFFER_ALIGN 32
/* the time and unit the stage holds until the player gives one. NO_TIME lies
 * below any time the player gives */
#define NO_TIME (-1)
#define NO_TIME_UNIT 1
/* server_frequency until the stage sets the player's */
#define NO_FREQUENCY (-1)
/* the samples of the header a seek puts on the joint: more than any stream
 * holds */
#define ENDLESS 0x7fffffff
/* the error a ring call records: the stage has no ring */
#define NO_RING ((Sint32)0xff000c03)
/* a player or joint SFADXT_Create can't make, and a handle created with no
 * ring or work */
#define NO_PLAYER ((Sint32)0xff000c04)
#define NO_STREAM ((Sint32)0xff000c05)
#define INVALID_PARAMS ((Sint32)0xff000c06)
/* the player's errors: any other than ADXT_ERR_SHRTBUF and ADXT_ERR_SNDBLK,
 * then those two */
#define PLAYER_ERROR ((Sint32)0xff000c07)
#define SHORT_INPUT ((Sint32)0xff000c08)
#define DECODER_STALLED ((Sint32)0xff000c09)
/* data in which sfadxt_SearchAlign finds no frame's start */
#define ALIGNMENT_ERROR ((Sint32)0xff000c0a)

/* the parameters SFD_SetAdxtPara set, which each handle's stage takes when it
 * is created */
static struct sfd_audio_params sfadxt_para;
/* the player's last error, and the starts sfadxt_GetVideoSample and
 * sfadxt_GetAudioSample last stored. each start is a time and a sample.
 * nothing reads them */
static Sint32 sfadxt_adxterr;
static Sint32 sfadxt_v_time;
static Sint32 sfadxt_v_sample;
static Sint32 sfadxt_as_pts;
static Sint32 sfadxt_a_sample;
/* a frame of zeros, which sfadxt_ExcludeSilence takes for silence. it is
 * defined after SFD_tr_ad_adxt, which the image holds first */
static const char sfadxt_silence[FRAME_BYTES];

static Sint32 sfadxt_SupplyData(struct sfd_handle *handle);
static void sfadxt_GetFreeChunk(SJ stream, Sint32 size, SJCK *chunk);
static void sfadxt_PutDataChunk(SJ stream, Sint32 size, SJCK *chunk);
static void sfadxt_CopyData(struct sfd_handle *handle, char *data, Sint32 size,
                            Sint32 *copied);
static void sfadxt_AdjustSync(struct sfd_handle *handle, char *data,
                              Sint32 size, Sint32 *copied);
static Sint32 sfadxt_GetAudioSample(const struct sfd_timer *timer, Sint32 rate);
static Sint32 sfadxt_GetVideoSample(const struct sfd_timer *timer, Sint32 rate,
                                    Bool *start_known);
static Sint32 sfadxt_AdjDiscard(char *data, Sint32 size, Bool *end);
static Sint32 sfadxt_InsertSilence(struct sfd_handle *handle, Sint32 channels,
                                   Sint32 samples);
static void sfadxt_ExcludeHdr(struct sfd_handle *handle, char *data,
                              Sint32 size, Sint32 *copied);
static char *sfadxt_SearchAlign(struct sfd_handle *handle, char *data,
                                Sint32 size);
static void sfadxt_ExcludeSilence(struct sfd_handle *handle, char *data,
                                  Sint32 size, Sint32 *copied);
static Sint32 sfadxt_GetNchSfreq(struct sfd_handle *handle, Sint32 *channels,
                                 Sint32 *rate);
static void sfadxt_ChkPrepFlg(struct sfd_handle *handle);
static Bool sfadxt_IsPrepEnd(struct sfd_handle *handle);
static void sfadxt_ChkTermFlg(struct sfd_handle *handle);
static Sint32 sfadxt_RingGetRead(struct sfd_handle *handle, char **data,
                                 Sint32 *size);
static Sint32 sfadxt_CopyFlap(struct sfd_handle *handle,
                              const struct sfd_ring_info *ring, Sint32 margin,
                              Sint32 *copied, char **end);
static Sint32 sfadxt_RingAddRead(struct sfd_handle *handle, Sint32 size);
static Bool sfadxt_GetTermSrc(struct sfd_handle *handle);
static Bool sfadxt_GetTermDst(struct sfd_handle *handle);
static void sfadxt_SetTermDst(struct sfd_handle *handle, Bool ended);
static void sfadxt_SetAdxtHd(struct sfd_handle *handle);
static Bool sfadxt_IsEndDecinfo(ADXT player);
static void sfadxt_SetSvrFreq(struct sfd_handle *handle);
static Sint32 sfadxt_InitInf(struct sfd_handle *handle,
                             struct sfd_audio_work *audio);
static ADXT sfadxt_CreateEx(struct sfd_audio_params *params);
static void sfadxt_SetTimeFn(struct sfd_handle *handle);
static Sint32 sfadxt_GetTime(struct sfd_handle *handle, Sint32 *time,
                             Sint32 *unit);
static Sint32 sfadxt_DestroySub(ADXT player);
static void sfadxt_WaitStart(struct sfd_handle *handle, Bool wait);
static void sfadxt_DoPause(struct sfd_handle *handle, Bool pause);
static void sfadxt_WtPause(ADXT player, Bool pause);
static void sfadxt_GetTimeOneFrmAudio(struct sfd_handle *handle,
                                      Sint32 *samples, Sint32 rate);
static struct sfd_audio_seek *sfadxt_GetHd(struct sfd_handle *handle);

void SFD_SetAdxtPara(const struct sfd_audio_params *params)
{
    sfadxt_para.ring_size = params->ring_size;
    sfadxt_para.margin = params->margin;
    sfadxt_para.ring = (char *)(((Uint32)params->ring + (BUFFER_ALIGN - 1)) &
                                ~(Uint32)(BUFFER_ALIGN - 1));
    sfadxt_para.channels = params->channels;
    sfadxt_para.sample_rate = params->sample_rate;
    sfadxt_para.work_size = params->work_size;
    sfadxt_para.work = (char *)(((Uint32)params->work + (BUFFER_ALIGN - 1)) &
                                ~(Uint32)(BUFFER_ALIGN - 1));
}
static Sint32 SFADXT_Init(struct sfd_handle *handle, Sint32 first,
                          Sint32 second, Sint32 third)
{
    UTY_MemsetDword((Uint32 *)&sfadxt_para, 0, UTY_DWORDS(sfadxt_para));
    return 0;
}
/* clang-format off */
static SFD_STAGE_NOOP(SFADXT_Finish)

static Sint32 SFADXT_ExecServer(struct sfd_handle *handle, Sint32 first,
                                Sint32 second, Sint32 third)
/* clang-format on */
{
    Sint32 result;
    if (!SFSET_GetCond(handle, SFD_COND_AUDIO) ||
        sfadxt_GetTermDst(handle) == TRUE) {
        return 0;
    }
    result = sfadxt_SupplyData(handle);
    sfadxt_ChkPrepFlg(handle);
    sfadxt_ChkTermFlg(handle);
    sfadxt_SetAdxtHd(handle);
    sfadxt_SetSvrFreq(handle);
    return result;
}
static Sint32 sfadxt_SupplyData(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    char *data;
    Sint32 size;
    Sint32 copied;
    Sint32 result;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    if ((result = sfadxt_RingGetRead(handle, &data, &size)) != 0) {
        return result;
    }
    audio->copy(handle, data, size, &copied);
    /* @hack spelling: the assignment inside the test, and result returned on
     * both of its sides, where returning the call would do */
    if ((result = sfadxt_RingAddRead(handle, copied)) != 0) {
        return result;
    }
    return result;
}
static void sfadxt_GetFreeChunk(SJ stream, Sint32 size, SJCK *chunk)
{
    SJ_GetChunk(stream, SJ_LIN_FREE, size, chunk);
}
static void sfadxt_PutDataChunk(SJ stream, Sint32 size, SJCK *chunk)
{
    SJCK rest;
    if (size == 0) {
        SJ_UngetChunk(stream, SJ_LIN_FREE, chunk);
    } else {
        SJ_SplitChunk(chunk, size, chunk, &rest);
        SJ_PutChunk(stream, SJ_LIN_DATA, chunk);
        SJ_UngetChunk(stream, SJ_LIN_FREE, &rest);
    }
}
/* audio->copy names one of four copies: sfadxt_ExcludeSilence,
 * sfadxt_ExcludeHdr, sfadxt_AdjustSync and this one, in turn. each takes from
 * the size bytes at data, and *copied gets the bytes it took. this one puts
 * as much of data on the joint as its free line holds, up to COPY_LIMIT */
static void sfadxt_CopyData(struct sfd_handle *handle, char *data, Sint32 size,
                            Sint32 *copied)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    struct sfd_audio_params *params;
    SJ stream;
    SJCK chunk;
    Sint32 available;
    char *destination;
    Sint32 bytes;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    params = &audio->params;
    stream = audio->stream;
    sfadxt_GetFreeChunk(stream, params->ring_size, &chunk);
    available = chunk.len;
    destination = chunk.data;
    if (size < available) {
        available = size;
    }
    bytes = available < COPY_LIMIT ? available : COPY_LIMIT;
    UTY_MemcpyByte((Uint8 *)destination, (const Uint8 *)data, bytes);
    sfadxt_PutDataChunk(stream, bytes, &chunk);
    *copied = bytes;
}
/* the copy after the header. it drops whole frames while the audio starts
 * ahead of the video. with timer->start known, it inserts silence while the
 * audio starts behind. *copied gets the bytes it drops. an end code among
 * them, or at the audio's start, turns the audio off. the handle's time
 * starts with the video, or with the audio where there is no video. once the
 * two starts lie within a frame, with timer->start known, the copy
 * becomes sfadxt_CopyData. it becomes sfadxt_CopyData at once where the
 * channels and rate are unknown or there is no video */
static void sfadxt_AdjustSync(struct sfd_handle *handle, char *data,
                              Sint32 size, Sint32 *copied)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    struct sfd_timer *timer;
    Sint32 channels;
    Sint32 rate;
    Bool end;
    Bool start_known;
    Sint32 end_size;
    Sint32 audio_sample;
    Sint32 video_sample;
    Sint32 difference;
    Sint32 length;
    Sint32 whole;
    Sint32 bytes;
    Sint32 samples;
    Sint32 inserted;
    *copied = 0;
    timer = &handle->timer;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    if (sfadxt_GetNchSfreq(handle, &channels, &rate) != 0) {
        audio->copy = sfadxt_CopyData;
        return;
    }
    audio_sample = sfadxt_GetAudioSample(timer, rate);
    if (!SFSET_GetCond(handle, SFD_COND_VIDEO)) {
        SFTIM_SetStartTime(timer, audio_sample, rate);
        audio->copy = sfadxt_CopyData;
        return;
    }
    video_sample = sfadxt_GetVideoSample(timer, rate, &start_known);
    if (video_sample < 0) {
        return;
    }
    SFTIM_SetStartTime(timer, video_sample, rate);
    difference = video_sample - audio_sample - audio->adjusted;
    bytes = 0;
    if (difference >= 0) {
        end = FALSE;
        length = difference / FRAME_SAMPLES * channels * FRAME_BYTES;
        if (length <= 0) {
            if (start_known) {
                audio->copy = sfadxt_CopyData;
                end = ADXT_IsEndcode(data, size, &end_size);
            }
        } else {
            whole = size / (channels * FRAME_BYTES) * channels * FRAME_BYTES;
            if (whole >= length) {
                whole = length;
            }
            bytes = sfadxt_AdjDiscard(data, whole, &end);
        }
        audio->adjusted += bytes / (channels * FRAME_BYTES) * FRAME_SAMPLES;
        if (end) {
            SFSET_SetCond(handle, SFD_COND_AUDIO, 0);
        }
    } else if (start_known) {
        difference = -difference;
        samples = difference / FRAME_SAMPLES * FRAME_SAMPLES;
        if (samples <= 0) {
            inserted = 0;
            audio->copy = sfadxt_CopyData;
        } else {
            inserted = sfadxt_InsertSilence(handle, channels, samples);
        }
        audio->adjusted -= inserted;
    }
    *copied = bytes;
}
static Sint32 sfadxt_GetAudioSample(const struct sfd_timer *timer, Sint32 rate)
{
    Sint32 sample;
    sample = UTY_MulAbDivC(timer->concat_offset, rate, PTS_UNIT);
    sfadxt_as_pts = timer->concat_offset;
    sfadxt_a_sample = sample;
    return sample;
}
/* the video's start as a sample at rate. that is timer->start's time where
 * it is known, or else the last skipped picture's. *start_known gets whether
 * timer->start is known. it returns -1 where neither is known */
static Sint32 sfadxt_GetVideoSample(const struct sfd_timer *timer, Sint32 rate,
                                    Bool *start_known)
{
    const struct sfd_time *stamp;
    const struct sfd_time *start;
    Sint32 time;
    Sint32 unit;
    Sint32 sample;
    stamp = &timer->skipped_picture;
    start = &timer->start;
    *start_known = start->valid;
    if (*start_known) {
        time = start->time;
        unit = start->unit;
    } else if (stamp->time >= 0) {
        time = stamp->time;
        unit = stamp->unit;
    } else {
        return -1;
    }
    sample = UTY_MulAbDivC(time, rate, unit);
    sfadxt_v_time = time;
    sfadxt_v_sample = sample;
    return sample;
}
/* the bytes of the whole frames in data's size bytes, up to the first end
 * code. *end is TRUE where there is one */
static Sint32 sfadxt_AdjDiscard(char *data, Sint32 size, Bool *end)
{
    Sint32 bytes;
    Sint32 end_size;
    *end = FALSE;
    for (bytes = 0; bytes < size; bytes += FRAME_BYTES) {
        if (ADXT_IsEndcode(data, FRAME_BYTES, &end_size)) {
            *end = TRUE;
            break;
        }
        data += FRAME_BYTES;
    }
    return bytes;
}
static Sint32 sfadxt_InsertSilence(struct sfd_handle *handle, Sint32 channels,
                                   Sint32 samples)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    return ADXT_InsertSilence(player, channels, samples);
}
/* the copy after any leading silence. it drops the stream's ADX header where
 * one opens the data. otherwise, from a stream before ALIGNED_MUX_VERSION, it
 * drops the bytes before the first whole frame. it waits for MIN_HEADER_INPUT
 * bytes. the next copy is sfadxt_AdjustSync */
static void sfadxt_ExcludeHdr(struct sfd_handle *handle, char *data,
                              Sint32 size, Sint32 *copied)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    Sint32 header_size;
    Sint32 bytes;
    *copied = 0;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    if (size < MIN_HEADER_INPUT) {
        return;
    }
    if (ADXT_IsHeader(data, size, &header_size)) {
        bytes = header_size;
    } else if (SFHDS_GetMuxVerNum(handle) >= ALIGNED_MUX_VERSION) {
        bytes = 0;
    } else {
        bytes = sfadxt_SearchAlign(handle, data, size) - data;
    }
    audio->copy = sfadxt_AdjustSync;
    *copied = bytes;
}

/* whether frame lies past last, the end code taken so far */
#ifdef NONMATCHING
/* this side's search starts last at NULL, which lies before every frame. the
 * shipped code's leaves last unset */
#define LATER_END(last, frame) ((last) == NULL || (last) < (frame))
#else
#define LATER_END(last, frame) ((last) < (frame))
#endif

/* the start of whole frames in data's size bytes. a marker here is a byte
 * whose top bit is set. a header and an end code each open with one. the
 * offsets it tries lie below 2 * FRAME_BYTES, SEARCH_STEP apart. the start is
 * the first whose frames open with no marker. where every offset meets one, it
 * is the offset whose first marker is the latest end code. where none is an end
 * code, it records ALIGNMENT_ERROR and returns data */
static char *sfadxt_SearchAlign(struct sfd_handle *handle, char *data,
                                Sint32 size)
{
    char *start;
    char *frame;
    char *end;
    char *last;
    char *found;
    Bool hit;
    Sint32 end_size;
    /* @bug uninitialized: the first end code is compared with last, which
     * no path has set, at 0x8c545bbe */
#ifdef NONMATCHING
    /* this side repairs the comparison: last starts at NULL, which LATER_END
     * takes as before every frame */
    last = NULL;
#endif
    found = NULL;
    end = data + size;
    for (start = data; start < data + (2 * FRAME_BYTES); start += SEARCH_STEP) {
        hit = FALSE;
        for (frame = start; frame < end; frame += FRAME_BYTES) {
            if (*(Sint8 *)frame < 0) {
                hit = TRUE;
                if (ADXT_IsEndcode(frame, FRAME_BYTES, &end_size) &&
                    LATER_END(last, frame)) {
                    last = frame;
                    found = start;
                }
                break;
            }
        }
        if (!hit) {
            break;
        }
    }
    if (hit) {
        if (found == NULL) {
            SFLIB_SetErr(handle, ALIGNMENT_ERROR);
            return data;
        }
        start = found;
    }
    return start;
}

/* the first copy after a seek. it drops the frames of silence that open a
 * stream before ALIGNED_MUX_VERSION, and audio->adjusted counts their samples.
 * it keeps the last frame it is given. the copy is sfadxt_ExcludeHdr from the
 * first frame that isn't silence */
static void sfadxt_ExcludeSilence(struct sfd_handle *handle, char *data,
                                  Sint32 size, Sint32 *copied)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    char *frame;
    Sint32 channels;
    Sint32 rate;
    Sint32 bytes;
    *copied = 0;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    if (SFHDS_GetMuxVerNum(handle) >= ALIGNED_MUX_VERSION) {
        audio->copy = sfadxt_ExcludeHdr;
        return;
    }
    frame = data;
    /* @hack spelling: bytes = bytes + FRAME_BYTES, where bytes +=
     * FRAME_BYTES would do */
    for (bytes = 0; bytes < size - FRAME_BYTES; bytes = bytes + FRAME_BYTES) {
        if (memcmp(frame, sfadxt_silence, FRAME_BYTES) != 0) {
            audio->copy = sfadxt_ExcludeHdr;
            break;
        }
        frame += FRAME_BYTES;
    }
    *copied = bytes;
    if (sfadxt_GetNchSfreq(handle, &channels, &rate) == 0) {
        audio->adjusted += bytes / (channels * FRAME_BYTES) * FRAME_SAMPLES;
    }
}
/* *channels and *rate from the seek information's audio header. it returns 0,
 * or -1 where there is none */
static Sint32 sfadxt_GetNchSfreq(struct sfd_handle *handle, Sint32 *channels,
                                 Sint32 *rate)
{
    struct sfd_audio_seek *header;
    if ((header = sfadxt_GetHd(handle)) == NULL) {
        return -1;
    }
    *channels = header->channels;
    *rate = header->sample_rate;
    return 0;
}
static void sfadxt_ChkPrepFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 input;
    Sint32 output;
    stage = &handle->stages[SFD_AUDIO];
    input = stage->input;
    output = stage->outputs[SFD_OUTPUT];
    if (SFBUF_GetPrepFlg(handle, output) == TRUE) {
        return;
    }
    if (SFBUF_GetPrepFlg(handle, input) != TRUE) {
        return;
    }
    if (!sfadxt_IsPrepEnd(handle)) {
        return;
    }
    SFBUF_SetPrepFlg(handle, output, TRUE);
}
static Bool sfadxt_IsPrepEnd(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    /* @hack spelling: TRUE and FALSE returned from an if, where returning the
     * test would do */
    if (ADXT_GetStat(player) == ADXT_STAT_PLAYING) {
        return TRUE;
    }
    return FALSE;
}
static void sfadxt_ChkTermFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    Sint32 state;
    Sint32 error;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    state = ADXT_GetStat(player);
    error = ADXT_GetErrCode(player);
    if (error != 0) {
        sfadxt_adxterr = error;
    }
    if (!SFSET_GetCond(handle, SFD_COND_AUDIO_ERRORS)) {
        error = 0;
    }
    switch (error) {
    case 0:
        break;
    case ADXT_ERR_SHRTBUF:
        SFLIB_SetErr(handle, SHORT_INPUT);
        break;
    case ADXT_ERR_SNDBLK:
        SFLIB_SetErr(handle, DECODER_STALLED);
        break;
    default:
        SFLIB_SetErr(handle, PLAYER_ERROR);
        break;
    }
    if (state == ADXT_STAT_PLAYEND || error != 0) {
        sfadxt_SetTermDst(handle, TRUE);
    }
}
/* the run of input to read, in *data and *size. it runs to the ring's end.
 * once sfadxt_CopyFlap has set an end, it runs on to that end */
static Sint32 sfadxt_RingGetRead(struct sfd_handle *handle, char **data,
                                 Sint32 *size)
{
    struct sfd_transfer *stage;
    Sint32 input;
    char *end;
    Sint32 copied;
    char *delimiter;
    char *scan;
    Sint32 margin;
    struct sfd_ring_info ring;
    char *head;
    Sint32 length;
    Sint32 result;
    stage = &handle->stages[SFD_AUDIO];
    input = stage->input;
    result = SFBUF_RingGetRead(handle, input, &ring);
    if (result != 0) {
        return result;
    }
    SFBUF_RingGetDlm(handle, input, &delimiter, &scan, &copied, &end, &margin);
    result = sfadxt_CopyFlap(handle, &ring, margin, &copied, &end);
    if (result != 0) {
        return SFLIB_SetErr(handle, result);
    }
    head = ring.head.data;
    length = ring.head.size;
    if (end != NULL) {
        *size = end - head;
    } else {
        *size = length;
    }
    *data = head;
    SFBUF_RingSetDlm(handle, input, delimiter, scan, copied, end);
    return 0;
}
/* copy the data that wraps to the ring's start into the margin past its end.
 * one run then holds the data. *copied counts the bytes earlier calls copied,
 * up to margin in all. *end gets the copy's end once margin bytes are copied,
 * or the input has ended. nothing is copied once *end is set */
static Sint32 sfadxt_CopyFlap(struct sfd_handle *handle,
                              const struct sfd_ring_info *ring, Sint32 margin,
                              Sint32 *copied, char **end)
{
    char *head;
    Sint32 head_size;
    char *wrap;
    Sint32 wrap_size;
    Sint32 bytes;
    char *source;
    char *destination;
    if (*end != NULL) {
        return 0;
    }
    head = ring->head.data;
    head_size = ring->head.size;
    wrap = ring->wrap.data;
    wrap_size = ring->wrap.size;
    if (wrap_size == 0) {
        return 0;
    }
    bytes = wrap_size - *copied < margin - *copied ? wrap_size - *copied
                                                   : margin - *copied;
    source = wrap + *copied;
    destination = head + (head_size + *copied);
    UTY_MemcpyByte((Uint8 *)destination, (const Uint8 *)source, bytes);
    *copied += bytes;
    if (*copied == margin || sfadxt_GetTermSrc(handle) == TRUE) {
        *end = destination + bytes;
    }
    return 0;
}
static Sint32 sfadxt_RingAddRead(struct sfd_handle *handle, Sint32 size)
{
    struct sfd_transfer *stage;
    Sint32 input;
    Sint32 result;
    stage = &handle->stages[SFD_AUDIO];
    input = stage->input;
    /* @hack spelling: the assignment inside the test, where returning the call
     * would do */
    if ((result = SFBUF_RingAddRead(handle, input, size)) != 0) {
        return result;
    }
    return 0;
}
static Bool sfadxt_GetTermSrc(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 input;
    stage = &handle->stages[SFD_AUDIO];
    input = stage->input;
    return SFBUF_GetTermFlg(handle, input);
}
static Bool sfadxt_GetTermDst(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 output;
    stage = &handle->stages[SFD_AUDIO];
    output = stage->outputs[SFD_OUTPUT];
    return SFBUF_GetTermFlg(handle, output);
}
static void sfadxt_SetTermDst(struct sfd_handle *handle, Bool ended)
{
    struct sfd_transfer *stage;
    Sint32 output;
    stage = &handle->stages[SFD_AUDIO];
    output = stage->outputs[SFD_OUTPUT];
    SFBUF_SetTermFlg(handle, output, ended);
}
/* fill the seek information's audio header from the player, once it has read
 * the stream's header */
static void sfadxt_SetAdxtHd(struct sfd_handle *handle)
{
    struct sfd_audio_seek *header;
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    if ((header = sfadxt_GetHd(handle)) == NULL || header->valid) {
        return;
    }
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    if (!sfadxt_IsEndDecinfo(player)) {
        return;
    }
    header->sample_rate = ADXT_GetSfreq(player);
    header->samples = ADXT_GetNumSmpl(player);
    header->channels = ADXT_GetNumChan(player);
    /* a frame's bytes for its samples, each halved */
    header->byte_rate = header->sample_rate * header->channels *
                        (FRAME_BYTES / 2) / (FRAME_SAMPLES / 2);
    header->constant = TRUE;
    header->valid = TRUE;
}
static Bool sfadxt_IsEndDecinfo(ADXT player)
{
    Sint32 status;
    status = ADXT_GetStat(player);
    /* @hack spelling: FALSE and TRUE returned from an if, where returning the
     * test would do */
    if (status == ADXT_STAT_STOP || status == ADXT_STAT_DECINFO) {
        return FALSE;
    }
    return TRUE;
}
static void sfadxt_SetSvrFreq(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    Sint32 frequency;
    Sint32 speed;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    frequency = SFSET_GetCond(handle, SFD_COND_AUDIO_SERVER_FREQUENCY);
    speed = SFTIM_GetSpeed(handle);
    if (speed > SFD_NORMAL_SPEED) {
        frequency = frequency * SFD_NORMAL_SPEED / speed;
    }
    if (audio->server_frequency != frequency) {
        audio->server_frequency = frequency;
        ADXT_SetSvrFreq(player, frequency);
    }
}
static Sint32 SFADXT_Create(struct sfd_handle *handle, Sint32 first,
                            Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    struct sfd_transfer *output;
    struct sfd_audio_output *sound;
    struct sfd_audio_work *audio;
    struct sfd_audio_params *params;
    ADXT player;
    SJ stream;
    Sint32 result;
    if (!SFSET_GetCond(handle, SFD_COND_AUDIO)) {
        return 0;
    }
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    params = &audio->params;
    result = sfadxt_InitInf(handle, audio);
    if (result != 0) {
        return result;
    }
    player = sfadxt_CreateEx(params);
    if (player == NULL) {
        return SFLIB_SetErr(handle, NO_PLAYER);
    }
    stream = SJRBF_Create(params->ring, params->ring_size, params->margin);
    if (stream == NULL) {
        /* @bug memory: a joint SJRBF_Create can't make, at 0x8c546198,
         * leaves the player made at 0x8c546176 undestroyed */
#ifdef NONMATCHING
        /* this side repairs the leak: it destroys the player before it
         * reports the failure */
        sfadxt_DestroySub(player);
#endif
        return SFLIB_SetErr(handle, NO_STREAM);
    }
    audio->player = player;
    audio->stream = stream;
    output = &handle->stages[SFD_AUDIO_OUT];
    sound = &output->work.audio_output;
    sound->converter = player->rna;
    ADXT_StartSj(player, stream);
    sfadxt_WaitStart(handle, TRUE);
    sfadxt_SetTimeFn(handle);
    return 0;
}
static Sint32 sfadxt_InitInf(struct sfd_handle *handle,
                             struct sfd_audio_work *audio)
{
    if (sfadxt_para.ring == NULL || sfadxt_para.work == NULL) {
        return SFLIB_SetErr(handle, INVALID_PARAMS);
    }
    audio->params = sfadxt_para;
    audio->player = NULL;
    audio->stream = NULL;
    audio->time = NO_TIME;
    audio->unit = NO_TIME_UNIT;
    audio->awaits_start = FALSE;
    audio->discard = 0;
    audio->seeked = FALSE;
    audio->adjusted = 0;
    audio->copy = sfadxt_CopyData;
    audio->no_header = 0;
    audio->server_frequency = NO_FREQUENCY;
    return 0;
}
static ADXT sfadxt_CreateEx(struct sfd_audio_params *params)
{
    ADXT player;
    player = ADXT_Create(params->channels, params->work, params->work_size);
    if (player == NULL) {
        return NULL;
    }
    ADXT_SetAutoRcvr(player, ADXT_RMODE_NOACT);
    return player;
}
static void sfadxt_SetTimeFn(struct sfd_handle *handle)
{
    Sint32 mask;
    SFLIB_LockCs(&mask);
    SFSET_SetCond(handle, SFD_COND_CLOCK, SFD_CLOCK_AUDIO);
    SFTIM_SetTimeFn(handle, sfadxt_GetTime, SFD_CLOCK_AUDIO);
    SFLIB_UnlockCs(&mask);
}
/* the handle's clock. while the handle plays, it is the player's time, which
 * never runs back. once the handle has finished, it is the last time the
 * player gave */
static Sint32 sfadxt_GetTime(struct sfd_handle *handle, Sint32 *time,
                             Sint32 *unit)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    Sint32 samples;
    Sint32 rate;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    if (SFTIM_ChkRegularTime(handle, time, unit)) {
        if (handle->status == SFD_PLAYING) {
            ADXT_GetTime(player, &samples, &rate);
            if (audio->time < samples) {
                audio->time = samples;
                audio->unit = rate;
            }
        }
        *time = audio->time;
        *unit = audio->unit;
    }
    return 0;
}
static Sint32 SFADXT_Destroy(struct sfd_handle *handle, Sint32 first,
                             Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    SJ stream;
    Sint32 result;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    stream = audio->stream;
    if (player == NULL) {
        return 0;
    }
    SJ_Destroy(stream);
    /* @hack spelling: sfadxt_DestroySub's result returned through result,
     * where returning the call would do */
    result = sfadxt_DestroySub(player);
    return result;
}
static Sint32 sfadxt_DestroySub(ADXT player)
{
    ADXT_Destroy(player);
    return 0;
}
/* clang-format off */
static SFD_STAGE_NOOP(SFADXT_Standby)

static Sint32 SFADXT_Start(struct sfd_handle *handle, Sint32 first,
                           Sint32 second, Sint32 third)
/* clang-format on */
{
    sfadxt_WaitStart(handle, FALSE);
    return 0;
}
static Sint32 SFADXT_Stop(struct sfd_handle *handle, Sint32 first,
                          Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    ADXT_Stop(player);
    return 0;
}
static Sint32 SFADXT_Pause(struct sfd_handle *handle, Sint32 mode,
                           Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    Sint32 samples;
    Sint32 rate;
    Sint32 total;
    Sint32 discarded;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    switch (mode) {
    case SFD_RESUME:
        audio->discard = 0;
        sfadxt_DoPause(handle, FALSE);
        break;
    case SFD_PAUSE:
        sfadxt_DoPause(handle, TRUE);
        break;
    case SFD_STEP:
        rate = audio->unit;
        sfadxt_GetTimeOneFrmAudio(handle, &samples, rate);
        total = samples + audio->discard;
        discarded = ADXT_DiscardSmpl(player, total);
        audio->discard = total - discarded;
        break;
    }
    return 0;
}
/* the player runs only while the stage isn't waiting for its start and the
 * handle isn't paused */
static void sfadxt_WaitStart(struct sfd_handle *handle, Bool wait)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    audio->awaits_start = wait;
    if (!wait && handle->pause == TRUE) {
        return;
    }
    sfadxt_WtPause(player, wait);
}
static void sfadxt_DoPause(struct sfd_handle *handle, Bool pause)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    if (!pause && audio->awaits_start == TRUE) {
        return;
    }
    sfadxt_WtPause(player, pause);
}
static void sfadxt_WtPause(ADXT player, Bool pause)
{
    ADXT_Pause(player, pause);
}
static void sfadxt_GetTimeOneFrmAudio(struct sfd_handle *handle,
                                      Sint32 *samples, Sint32 rate)
{
    Sint32 time;
    Sint32 unit;
    SFTIM_GetTimeOneFrmVideo(handle, &time, &unit);
    *samples = UTY_MulAbDivC(rate, time, unit);
}
SFD_STAGE_REJECT(SFADXT_GetWrite, NO_RING)
SFD_STAGE_REJECT(SFADXT_AddWrite, NO_RING)
SFD_STAGE_REJECT(SFADXT_GetRead, NO_RING)
SFD_STAGE_REJECT(SFADXT_AddRead, NO_RING)

/* after a seek, put a header with the stream's channels and rate on the joint.
 * the copies then start over from sfadxt_ExcludeSilence. it does nothing
 * without a valid header, or once it has done so */
static Sint32 SFADXT_Seek(struct sfd_handle *handle, Sint32 first,
                          Sint32 second, Sint32 third)
{
    struct sfd_audio_seek *header;
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    ADXT player;
    if ((header = sfadxt_GetHd(handle)) == NULL) {
        return 0;
    }
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    player = audio->player;
    if (audio->seeked) {
        return 0;
    }
    if (!header->valid) {
        return 0;
    }
    ADXT_InsertHdrSfa(player, header->channels, header->sample_rate, ENDLESS);
    ADXT_SetTimeOfst(player, 0);
    audio->seeked = TRUE;
    audio->copy = sfadxt_ExcludeSilence;
    return 0;
}
/* the seek information's audio header, or NULL where the handle has none or
 * no_header is set */
static struct sfd_audio_seek *sfadxt_GetHd(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    struct sfd_audio_work *audio;
    stage = &handle->stages[SFD_AUDIO];
    audio = &stage->work.audio;
    if (handle->seek.info == NULL) {
        return NULL;
    }
    if (audio->no_header > 0) {
        return NULL;
    }
    return &handle->seek.info->head.audio;
}

const struct sfd_stage_ops SFD_tr_ad_adxt = {
    {SFADXT_Init, SFADXT_Finish, SFADXT_ExecServer, SFADXT_Create,
     SFADXT_Destroy, SFADXT_Standby, SFADXT_Start, SFADXT_Stop, SFADXT_Pause,
     SFADXT_GetWrite, SFADXT_AddWrite, SFADXT_GetRead, SFADXT_AddRead,
     SFADXT_Seek}};
static const char sfadxt_silence[FRAME_BYTES] = {0};
