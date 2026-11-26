/* the video stage, which decodes the MPEG video in its input ring into the
 * frames SFD_SetMpvPara sets. a server call decodes one picture at most. the
 * video output takes the decoded pictures one at a time, earliest first.
 * each passes through the first slot of the stage's frame queue */
#include <sfd.h>
#include <sfd_mpvd.h>
#include <sg_xpt.h>
#include <sofdec.h>
#include <sofdec_uty.h>
#include <string.h>
#include "../sofdec/sfd_stage.h"

#define VIDEO_STAGE(h) (&(h)->stages[SFD_VIDEO])
#define START_CODE_SIZE 4
/* a start code that crosses a boundary begins at most this many bytes
 * before it */
#define LOOKBEHIND (START_CODE_SIZE - 1)
/* masks of start codes. video->resume_codes holds the codes decoding resumes
 * at, and the stage skips the bytes before the next of them. PICTURE_CODES
 * are the codes that can follow a picture. they mark where the bytes handed
 * to the decoder can end. HEADER_CODES are the codes whose headers
 * sfmpv_DecodePicAtr reads */
#define SEQUENCE_CODES (MPV_DELIM_SEQUENCE | MPV_DELIM_END)
#define GROUP_CODES (SEQUENCE_CODES | MPV_DELIM_GROUP)
#define PICTURE_CODES (GROUP_CODES | MPV_DELIM_PICTURE)
#define HEADER_CODES (MPV_DELIM_SEQUENCE | MPV_DELIM_GROUP | MPV_DELIM_PICTURE)
/* the head run is the data to read, as far as the ring's end. once it holds
 * this many bytes, it decodes whole, without a start code to end it */
#define READY_BYTES 0x28000
/* timer->supply_time once a file ends while the handle plays audio. no
 * picture decodes while it is below 0 */
#define NO_FILE_START (-8)
#define VARIABLE_BIT_RATE 0x3ffff
/* the bytes a second one unit of a sequence header's bit rate counts */
#define BIT_RATE_BYTES 50
/* a chroma plane's width and height go LUMA_PER_CHROMA times into its luma's */
#define LUMA_PER_CHROMA 2
/* sfmpv_SetFrmPara's result where no frame is free */
#define NO_FRAME_FREE (-1)
/* video->gop before the first picture's group */
#define NO_GROUP (-1)
#define VIDEO_DECODERS 8
#define DECODER_PAGE 8192
/* the pages MPV_Init's tables and their alignment take beside the decoders' */
#define TABLE_PAGES 2
#define INVALID_MPV_CONDITION ((Sint32)0xff000181)
#define INVALID_FRAME_COUNT ((Sint32)0xff000182)
#define INIT_FAILED ((Sint32)0xff000f01)
#define MARGIN_FULL ((Sint32)0xff000f02)
#define HEADER_FAILED ((Sint32)0xff000f04)
#define PICTURE_INFO_FAILED ((Sint32)0xff000f05)
#define DECODE_FAILED ((Sint32)0xff000f06)
#define SKIP_FAILED ((Sint32)0xff000f07)
#define CREATE_FAILED ((Sint32)0xff000f0a)
#define INVALID_CALLBACK ((Sint32)0xff000f0b)
#define DESTROY_FAILED ((Sint32)0xff000f0c)
#define NO_WRITE ((Sint32)0xff000f0d)
#define NOT_HELD ((Sint32)0xff000f0e)
#define WRONG_FRAME ((Sint32)0xff000f0f)
#define CONDITION_FAILED ((Sint32)0xff000f12)
#define BAD_SECTION_ORDER ((Sint32)0xff000f13)
#define NO_PARAMS ((Sint32)0xff000f15)
#define BIT_RATE_FAILED ((Sint32)0xff000f16)
#define REFERENCES_TOO_SMALL ((Sint32)0xff000f17)
#define FRAMES_TOO_SMALL ((Sint32)0xff000f18)
#define NO_FRAMES ((Sint32)0xff000f19)
#define REPARSE_FAILED ((Sint32)0xff000f1b)

enum picture_type { I_PICTURE = 1, P_PICTURE, B_PICTURE };
/* video->defect's states. a picture skips where it needs a reference the
 * decoder lacks. a P or B picture skips under NEED_BOTH, and a B picture
 * under NEED_SECOND. a seek, a broken link or a skipped I or P picture sets
 * NEED_BOTH. a closed group sets REFERENCES_READY. each picture decoded
 * moves the state one on */
enum reference_state { NEED_BOTH = 2, NEED_SECOND = 3, REFERENCES_READY = 5 };

static Uint8 sfmpv_work[(VIDEO_DECODERS + TABLE_PAGES) * DECODER_PAGE];
/* SFD_SetMpvPara's parameters, which a video stage copies when its handle is
 * created. SFMPV_Init clears them */
static struct sfd_video_params sfmpv_para;

static Bool sfmpv_ChkFatal(void);
static void sfmpv_SetCondY16(struct sfd_handle *handle);
static Bool sfmpv_GetTermDst(struct sfd_handle *handle);
static void sfmpv_ChkPrepFlg(struct sfd_handle *handle);
static Bool sfmpv_IsPrepEnd(struct sfd_handle *handle);
static void sfmpv_FixedStartTtu(struct sfd_handle *handle);
static void sfmpv_ChkTermFlg(struct sfd_handle *handle);
static void sfmpv_SetTermDst(struct sfd_handle *handle, Bool ended);
static Sint32 sfmpv_DecodeSomePic(struct sfd_handle *handle);
static Sint32 sfmpv_RingGetRead(struct sfd_handle *handle, char **data,
                                Sint32 *size);
static Sint32 sfmpv_CopyFlap(const struct sfd_ring_info *ring, Sint32 margin,
                             Sint32 *copied, char **end);
static Sint32 sfmpv_SearchSafeDlm(struct sfd_handle *handle,
                                  const struct sfd_ring_info *ring,
                                  char **delimiter, char **scan, Sint32 copied);
static Sint32 sfmpv_RingAddRead(struct sfd_handle *handle, Sint32 size);
static Sint32 sfmpv_DecodeOneUnit(struct sfd_handle *handle, char *bytes,
                                  Sint32 size, Sint32 *consumed,
                                  Bool *progress);
static Sint32 sfmpv_GetCurDelim(char *bytes, Sint32 size, Sint32 mask);
static void sfmpv_Concat(struct sfd_handle *handle, struct mpv_input *input,
                         Sint32 *consumed);
static void sfmpv_ConcatSub(struct sfd_handle *handle);
static Sint32 sfmpv_CalcTailEndTime(struct sfd_handle *handle);
static void sfmpv_DiscardSec(struct mpv_input *input, Sint32 *consumed);
static Bool sfmpv_IsTerm(struct sfd_handle *handle, Sint32 size, Sint32 kind);
static Bool sfmpv_GetTermSrc(struct sfd_handle *handle);
static Sint32 sfmpv_DecodePicAtr(struct sfd_handle *handle,
                                 struct mpv_input *input, Sint32 *consumed);
static void sfmpv_SetHeadTtu(struct sfd_handle *handle,
                             const struct mpv_picture *picture);
static void sfmpv_SetDecTtu(struct sfd_handle *handle,
                            const struct mpv_picture *picture);
static void sfmpv_ReadTcode(const struct mpv_picture *picture,
                            struct sfd_timecode *code);
static Sint32 sfmpv_FirstPicAtr(struct sfd_handle *handle,
                                struct mpv_obj *decoder,
                                const struct mpv_picture *picture,
                                const struct mpv_input *input);
static void sfmpv_SetMpvHd(struct sfd_handle *handle, Sint32 rate,
                           const struct mpv_input *input);
static void sfmpv_SetMvInf(struct sfd_movie_info *movie, Sint32 rate,
                           const struct mpv_picture *picture);
static Sint32 sfmpv_ChkBufSiz(struct sfd_handle *handle,
                              const struct sfd_movie_info *movie);
static Bool sfmpv_IsConcatTimeFix(struct sfd_handle *handle);
static Bool sfmpv_IsSkip(struct sfd_handle *handle,
                         const struct mpv_input *input);
static Bool sfmpv_IsDefect(struct sfd_handle *handle, Sint32 kind);
static void sfmpv_UpdateDefect(struct sfd_handle *handle,
                               const struct mpv_picture *picture, Bool skipped);
static Bool sfmpv_IsSeekSkip(struct sfd_handle *handle);
static Bool sfmpv_IsPtypeSkip(struct sfd_handle *handle, Sint32 kind);
static Bool sfmpv_IsEmptyBpic(struct sfd_handle *handle, Sint32 kind,
                              const struct mpv_input *input);
static Bool sfmpv_IsLate(struct sfd_handle *handle, Sint32 kind);
static void sfmpv_GetDtime(struct sfd_handle *handle, Sint32 kind, Sint32 *time,
                           Sint32 *unit);
static Sint32 sfmpv_SkipFrm(struct sfd_handle *handle, struct mpv_input *input,
                            Sint32 *consumed);
static void sfmpv_SetSkipTtu(struct sfd_handle *handle);
static Sint32 sfmpv_DecodeFrm(struct sfd_handle *handle,
                              struct mpv_input *input, Sint32 *consumed);
static Sint32 sfmpv_SetFrmPara(struct sfd_handle *handle,
                               const struct mpv_picture *picture,
                               struct mpv_request *request,
                               struct sfd_output_frame **out);
static void sfmpv_SetStartTtu(struct sfd_handle *handle);
static void sfmpv_SetFrmTime(struct sfd_handle *handle,
                             struct sfd_output_frame *frame);
static void sfmpv_GoDdelim(struct mpv_input *input, Sint32 *consumed,
                           Sint32 mask);
static Uint8 *sfmpv_GoDdelimSub(struct mpv_input *input, Sint32 *consumed,
                                Sint32 mask);
static Sint32 sfmpv_InitInf(struct sfd_handle *handle,
                            struct sfd_video_work *video);
static void sfmpv_InitPicAtr(struct mpv_picture *picture);
static void sfmpv_ErrFn(void *handle, Sint32 code);
static Sint32 sfmpv_DestroySub(struct mpv_obj *decoder);
static void sfmpv_SetFrmInf(struct sfd_handle *handle,
                            struct sfd_output_frame *frame,
                            struct sfd_frame_info **out);
static Sint32 sfmpv_ReprocessShc(struct sfd_handle *handle,
                                 struct sfd_video_work *video, Bool *found);
static struct sfd_video_seek *sfmpv_GetHd(struct sfd_handle *handle);

void SFD_SetMpvPara(const struct sfd_video_params *params)
{
    sfmpv_para.chroma_width = params->chroma_width;
    sfmpv_para.chroma_height = params->chroma_height;
    sfmpv_para.luma_width = params->luma_width;
    sfmpv_para.luma_height = params->luma_height;
    sfmpv_para.references =
        (Uint8 *)(((Uint32)params->references + (SFD_ALIGNMENT - 1)) &
                  ~(SFD_ALIGNMENT - 1UL));
    sfmpv_para.output_width = params->output_width;
    sfmpv_para.output_height = params->output_height;
    sfmpv_para.frames = params->frames;
    sfmpv_para.output =
        (Uint8 *)(((Uint32)params->output + (SFD_ALIGNMENT - 1)) &
                  ~(SFD_ALIGNMENT - 1UL));
}

Sint32 SFD_SetMpvCond(struct sfd_handle *handle, Sint32 id, Sint32 value)
{
    struct mpv_obj *decoder;
    Sint32 error;
    if (handle == NULL) {
        decoder = NULL;
    } else {
        struct sfd_transfer *stage;
        struct sfd_video_work *video;
        if (SFLIB_CheckHn(handle) != 0) {
            return SFLIB_SetErr(NULL, INVALID_MPV_CONDITION);
        }
        stage = VIDEO_STAGE(handle);
        video = &stage->work.video;
        decoder = video->decoder;
    }
    error = MPV_SetCond(decoder, id, value);
    if (error != 0) {
        return SFLIB_SetErr(handle, CONDITION_FAILED);
    }
    return 0;
}

Sint32 SFD_GetNumFrm(struct sfd_handle *handle, Sint32 *count)
{
    *count = 0;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_FRAME_COUNT);
    }
    *count = SFMPVF_GetNumFrm(handle);
    return 0;
}
static Sint32 SFMPV_Init(struct sfd_handle *handle, Sint32 first, Sint32 second,
                         Sint32 third)
{
    Sint32 error;
    if (sfmpv_ChkFatal()) {
        for (;;) {
        }
    }
    error = MPV_Init(VIDEO_DECODERS, sfmpv_work);
    if (error != 0) {
        Sint32 code;
        if (error == MPV_ERR_SECTION_ORDER) {
            code = BAD_SECTION_ORDER;
        } else {
            code = INIT_FAILED;
        }
        return SFLIB_SetErr(NULL, code);
    }
    UTY_MemsetDword((Uint32 *)&sfmpv_para, 0,
                    sizeof(sfmpv_para) / sizeof(Uint32));
    return 0;
}

static Bool sfmpv_ChkFatal(void)
{
    return FALSE;
}

static Sint32 SFMPV_Finish(struct sfd_handle *handle, Sint32 first,
                           Sint32 second, Sint32 third)
{
    MPV_Finish();
    return 0;
}

static Sint32 SFMPV_ExecServer(struct sfd_handle *handle, Sint32 first,
                               Sint32 second, Sint32 third)
{
    Sint32 error;
    if (!SFSET_GetCond(handle, SFD_COND_VIDEO) ||
        sfmpv_GetTermDst(handle) == TRUE) {
        return 0;
    }
    sfmpv_SetCondY16(handle);
    error = sfmpv_DecodeSomePic(handle);
    sfmpv_ChkPrepFlg(handle);
    sfmpv_ChkTermFlg(handle);
    return error;
}

static void sfmpv_SetCondY16(struct sfd_handle *handle)
{
    Sint32 color;
    Sint32 offset;
    if (!SFSET_GetCond(handle, SFD_COND_Y16)) {
        return;
    }
    color = SFHDS_GetColType(handle);
    switch (color) {
        /* @bug uninitialized: offset is passed unset for a color type other
         * than SFD_NO_COLOR, 0 and 1, whose branch at 0x8c54893a skips both
         * assignments */
#ifdef NONMATCHING
    /* this side repairs the unset offset: another color type sets no
     * condition, as SFD_NO_COLOR does */
    default:
        return;
#endif
    case SFD_NO_COLOR:
        return;
    case 0:
        offset = TRUE;
        break;
    case 1:
        offset = FALSE;
        break;
    }
    SFD_SetMpvCond(handle, MPV_COND_LUMA_OFFSET, offset);
}
static Bool sfmpv_GetTermDst(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    Sint32 buffer = stage->outputs[SFD_OUTPUT];
    return SFBUF_GetTermFlg(handle, buffer);
}
static void sfmpv_ChkPrepFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    Sint32 input = stage->input;
    Sint32 output = stage->outputs[SFD_OUTPUT];
    if (SFBUF_GetPrepFlg(handle, output) == TRUE ||
        SFBUF_GetPrepFlg(handle, input) != TRUE || !sfmpv_IsPrepEnd(handle)) {
        return;
    }
    SFBUF_SetPrepFlg(handle, output, TRUE);
    sfmpv_FixedStartTtu(handle);
}
static Bool sfmpv_IsPrepEnd(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    /* @hack redundant: video, formed and never read */
    struct sfd_video_work *video = &stage->work.video;
    struct sfd_create_params *params = &handle->params;
    Sint32 *conditions = handle->conditions;
    Sint32 frames = params->frames;
    Sint32 wanted = conditions[SFD_COND_PREP_FRAMES];
    Sint32 needed;
    Sint32 ready;
    (void)video;
    if (wanted == SFD_ALL_FRAMES) {
        wanted = frames;
    }
    needed = frames < wanted ? frames : wanted;
    ready = SFMPVF_GetNumFrm(handle);
    if (ready >= needed) {
        return TRUE;
    }
    if (SFMPVF_IsTermDec(handle)) {
        return TRUE;
    }
    return FALSE;
}
static void sfmpv_FixedStartTtu(struct sfd_handle *handle)
{
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *start = &timer->start;
    if (start->time != SFD_NO_TIME) {
        start->valid = TRUE;
    }
}
static void sfmpv_ChkTermFlg(struct sfd_handle *handle)
{
    struct sfd_play_info *play = &handle->play;
    Sint32 count = SFMPVF_GetNumFrm(handle);
    if (count != SFMPVF_ENDED) {
        return;
    }
    sfmpv_SetTermDst(handle, TRUE);
    if (play->decoded) {
        return;
    }
    SFSET_SetCond(handle, SFD_COND_VIDEO, 0);
}
static void sfmpv_SetTermDst(struct sfd_handle *handle, Bool ended)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    Sint32 buffer = stage->outputs[SFD_OUTPUT];
    SFBUF_SetTermFlg(handle, buffer, ended);
}

static Sint32 sfmpv_DecodeSomePic(struct sfd_handle *handle)
{
    char *bytes;
    Sint32 size;
    Sint32 consumed;
    Sint32 error;
    Bool progress;
#ifdef NONMATCHING
    /* GCC can't see that sfmpv_RingGetRead sets bytes and size wherever it
     * returns 0 */
    bytes = NULL;
    size = 0;
#endif
    for (;;) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((error = sfmpv_RingGetRead(handle, &bytes, &size)) != 0) {
            break;
        }
        error = sfmpv_DecodeOneUnit(handle, bytes, size, &consumed, &progress);
        if (error != 0) {
            break;
        }
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((error = sfmpv_RingAddRead(handle, consumed)) != 0) {
            break;
        }
        if (!progress) {
            break;
        }
    }
    return error;
}

/* set *data to the head run's start and *size to the bytes to decode from
 * there. once the head run holds READY_BYTES, they are all of it. short of
 * that, they run up to the end sfmpv_CopyFlap finds past the ring's end.
 * otherwise they run up to the delimiter sfmpv_SearchSafeDlm finds. *size is
 * 0 until one is found */
static Sint32 sfmpv_RingGetRead(struct sfd_handle *handle, char **data,
                                Sint32 *size)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_ring_info ring;
    char *delimiter;
    char *end;
    char *scan;
    char *head;
    Sint32 copied;
    Sint32 margin;
    Sint32 available;
    Sint32 error;
    Sint32 buffer = stage->input;
    error = SFBUF_RingGetRead(handle, buffer, &ring);
    if (error != 0) {
        return error;
    }
    SFBUF_RingGetDlm(handle, buffer, &delimiter, &scan, &copied, &end, &margin);
    error = sfmpv_CopyFlap(&ring, margin, &copied, &end);
    if (error != 0) {
        return SFLIB_SetErr(handle, error);
    }
    head = ring.head.data;
    available = ring.head.size;
    if (available >= READY_BYTES) {
        *size = available;
    } else if (end != NULL) {
        *size = end - head;
    } else {
        error = sfmpv_SearchSafeDlm(handle, &ring, &delimiter, &scan, copied);
        if (error != 0) {
            return error;
        }
        if (delimiter != NULL) {
            *size = delimiter - head;
        } else {
            *size = 0;
        }
    }
    *data = head;
    SFBUF_RingSetDlm(handle, buffer, delimiter, scan, copied, end);
    return 0;
}

/* copy the ring's wrap run into the margin past its end. the wrap run is the
 * bytes that go on from the ring's start. a unit across the end then reads
 * in one piece.
 * *copied counts the bytes copied so far, at most margin. each call copies
 * on to past the next code of PICTURE_CODES, within the wrap run and the
 * margin. below, a code is one of PICTURE_CODES. *end is set past that code
 * where it is a sequence header or end code. a group or picture start sets
 * *end too, where the code before it is a picture start. returns MARGIN_FULL
 * where the margin fills with no code found, and otherwise 0 */
static Sint32 sfmpv_CopyFlap(const struct sfd_ring_info *ring, Sint32 margin,
                             Sint32 *copied, char **end)
{
    Sint32 window;
    Sint32 head_size;
    Sint32 wrap_size;
    Sint32 amount;
    Sint32 moved;
    Sint32 kind;
    Sint32 overlap;
    Sint32 missing;
    char *head;
    char *wrap;
    char *dest;
    char *source;
    char *stop;
    char *previous;
    if (*end != NULL) {
        return 0;
    }
    head = ring->head.data;
    head_size = ring->head.size;
    wrap = ring->wrap.data;
    wrap_size = ring->wrap.size;
    if (wrap_size <= *copied) {
        return 0;
    }
    overlap = *copied > LOOKBEHIND ? LOOKBEHIND : *copied;
    missing = LOOKBEHIND - *copied < 0 ? 0 : LOOKBEHIND - *copied;
    amount = wrap_size - *copied < margin - *copied ? wrap_size - *copied
                                                    : margin - *copied;
    source = wrap + *copied;
    dest = head_size + *copied + head;
    stop = (char *)MPV_CopyStm((Uint8 *)(dest - overlap),
                               (Uint8 *)(source - overlap), amount + overlap,
                               PICTURE_CODES);
    moved = stop - source;
    *copied += moved;
    kind = MPV_CheckDelim((Uint8 *)(stop - START_CODE_SIZE));
    switch (kind) {
    case MPV_DELIM_SEQUENCE:
    case MPV_DELIM_END:
        *end = moved + dest;
        break;
    case MPV_DELIM_PICTURE:
    case MPV_DELIM_GROUP:
        /* the search back starts where this copy does, or LOOKBEHIND bytes
         * past the ring's end if later. so it takes in a code that runs
         * across the end, and not the one just found */
        window = dest - head;
        previous = (char *)MPV_BsearchDelim((Uint8 *)(missing + dest),
                                            missing + window, PICTURE_CODES);
        if (previous != NULL &&
            MPV_CheckDelim((Uint8 *)previous) == MPV_DELIM_PICTURE) {
            *end = moved + dest;
        }
        break;
    default:
        if (*copied >= margin) {
            return MARGIN_FULL;
        }
    }
    return 0;
}
/* set *delimiter past the last start code in the head run that
 * sfmpv_CopyFlap would end at. a code here is one of PICTURE_CODES. that is
 * a sequence header or end code. a group or picture start counts too, where
 * the code before it is a picture start. short of one, *delimiter goes past a
 * group or picture start that begins the run. a *delimiter more than
 * START_CODE_SIZE bytes into the run stays as it is. *scan is where the last
 * search started: the head run's end, plus up to LOOKBEHIND bytes of margin.
 * no search runs again until that moves */
static Sint32 sfmpv_SearchSafeDlm(struct sfd_handle *handle,
                                  const struct sfd_ring_info *ring,
                                  char **delimiter, char **scan, Sint32 copied)
{
    char *head = ring->head.data;
    Sint32 size = ring->head.size;
    char *stop;
    char *cursor;
    char *found;
    char *last;
    Sint32 left;
    Sint32 kind;
    Sint32 advance;
    if (*delimiter != NULL && *delimiter - head > START_CODE_SIZE) {
        return 0;
    }
    stop = (copied > LOOKBEHIND ? LOOKBEHIND : copied) + size + head;
    if (stop == *scan) {
        return 0;
    }
    *scan = stop;
    last = NULL;
    left = stop - head;
    cursor = stop;
    while (left >= START_CODE_SIZE) {
        found = (char *)MPV_BsearchDelim((Uint8 *)cursor, left, PICTURE_CODES);
        if (found == NULL) {
            break;
        }
        kind = MPV_CheckDelim((Uint8 *)found);
        switch (kind) {
        case MPV_DELIM_SEQUENCE:
        case MPV_DELIM_END:
            *delimiter = found + START_CODE_SIZE;
            return 0;
        case MPV_DELIM_GROUP:
            last = found + START_CODE_SIZE;
            break;
        case MPV_DELIM_PICTURE:
            if (last != NULL) {
                goto accept;
            }
            last = found + START_CODE_SIZE;
            break;
        }
        /* @bug logic: left grows by the distance each search steps back,
         * where it should shrink by it. a later search then reaches before
         * the head run's start (0x8c548dd0) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: left shrinks by the distance
         * each search steps back */
        advance = found - cursor;
        left += advance;
#else
        /* @hack spelling: the assignment inside the expression, where a
         * statement of its own would do */
        left -= (advance = found - cursor);
#endif
        cursor += advance;
    }
    if (last == head + START_CODE_SIZE) {
    accept:
        *delimiter = last;
    }
    return 0;
}
static Sint32 sfmpv_RingAddRead(struct sfd_handle *handle, Sint32 size)
{
    Sint32 error;
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    Sint32 buffer = stage->input;
    /* @hack spelling: a test of the call's result that returns it or 0,
     * where returning it would do */
    if ((error = SFBUF_RingAddRead(handle, buffer, size)) != 0) {
        return error;
    }
    return 0;
}

/* decode the unit at the start of bytes: headers, a picture, or bytes to
 * skip. the bytes it skips run up to the next of video->resume_codes.
 * *consumed gets the bytes it takes. *progress is TRUE where the next unit can
 * follow at once, as after headers or a skipped picture. it is FALSE after a
 * decoded picture, and where the unit has to wait */
static Sint32 sfmpv_DecodeOneUnit(struct sfd_handle *handle, char *bytes,
                                  Sint32 size, Sint32 *consumed, Bool *progress)
{
    struct sfd_transfer *stage;
    struct sfd_video_work *video;
    struct mpv_input input;
    Sint32 error = 0;
    Sint32 kind;
    *consumed = 0;
    *progress = FALSE;
    stage = VIDEO_STAGE(handle);
    video = &stage->work.video;
    input.data = (Uint8 *)bytes;
    input.size = size;
    kind = sfmpv_GetCurDelim(bytes, size, video->resume_codes);
    if (kind & GROUP_CODES) {
        SFMPVF_SetGopStat(handle, TRUE);
    }
    if (kind == MPV_DELIM_END && SFCON_IsEndcodeSkip(handle)) {
        sfmpv_Concat(handle, &input, consumed);
        *progress = TRUE;
    } else if (sfmpv_IsTerm(handle, size, kind)) {
        SFMPVF_TermDec(handle);
    } else if (size <= START_CODE_SIZE) {
        /* the bytes hold no more than a start code, so wait for those after
         * it */
    } else if (kind & HEADER_CODES) {
        error = sfmpv_DecodePicAtr(handle, &input, consumed);
        if (!error) {
            video->resume_codes = PICTURE_CODES;
            *progress = TRUE;
        }
    } else if (kind & MPV_DELIM_FIRST_SLICE) {
        if (SFSET_GetCond(handle, SFD_COND_PLAY_MODE) == SFD_PLAY_HEADERS) {
            return 0;
        }
        if (!sfmpv_IsConcatTimeFix(handle)) {
            return 0;
        }
        if (sfmpv_IsSkip(handle, &input)) {
            error = sfmpv_SkipFrm(handle, &input, consumed);
            if (!error) {
                *progress = TRUE;
            }
        } else {
            error = sfmpv_DecodeFrm(handle, &input, consumed);
        }
    } else if (!kind) {
        sfmpv_GoDdelim(&input, consumed, video->resume_codes);
        *progress = TRUE;
    }
    return error;
}
static Sint32 sfmpv_GetCurDelim(char *bytes, Sint32 size, Sint32 mask)
{
    Sint32 kind;
    if (size < START_CODE_SIZE) {
        kind = 0;
    } else {
        kind = MPV_CheckDelim((Uint8 *)bytes);
        if (mask != PICTURE_CODES) {
            kind &= mask;
        }
    }
    return kind;
}
static void sfmpv_Concat(struct sfd_handle *handle, struct mpv_input *input,
                         Sint32 *consumed)
{
    sfmpv_ConcatSub(handle);
    sfmpv_DiscardSec(input, consumed);
}
static void sfmpv_ConcatSub(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct sfd_timer *timer = &handle->timer;
    Sint32 duration;
    if (!SFSET_GetCond(handle, SFD_COND_AUDIO)) {
        duration = sfmpv_CalcTailEndTime(handle);
        if (duration > 0) {
            SFCON_UpdateConcatTime(handle, duration);
            video->sequence_ends++;
        }
    } else {
        timer->supply_time = NO_FILE_START;
        video->sequence_ends++;
    }
    SFTIM_InitTtu(&timer->first_picture, SFD_NO_TIME);
    SFTIM_InitTtu(&timer->latest_picture, 0);
    video->resume_codes = SEQUENCE_CODES;
}
static Sint32 sfmpv_CalcTailEndTime(struct sfd_handle *handle)
{
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *last = &timer->latest_picture;
    struct sfd_timecode code;
    Sint32 time;
    Sint32 unit;
    if (!last->valid) {
        return 0;
    }
    code = last->code;
    code.frame_adjust++;
    SFTIM_Tc2Time(&code, &time, &unit);
    return time;
}
static void sfmpv_DiscardSec(struct mpv_input *input, Sint32 *consumed)
{
    Uint8 *bytes = input->data;
    Sint32 remaining = input->size;
    while (MPV_CheckDelim(bytes) == MPV_DELIM_END) {
        remaining -= START_CODE_SIZE;
        *consumed += START_CODE_SIZE;
        bytes += START_CODE_SIZE;
        if (remaining < START_CODE_SIZE) {
            break;
        }
    }
    input->data = bytes;
    input->size = remaining;
}
static Bool sfmpv_IsTerm(struct sfd_handle *handle, Sint32 size, Sint32 kind)
{
    if (kind == MPV_DELIM_END) {
        return TRUE;
    }
    if (size <= START_CODE_SIZE) {
        if (sfmpv_GetTermSrc(handle) == TRUE) {
            return TRUE;
        }
    }
    return FALSE;
}
static Bool sfmpv_GetTermSrc(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    Sint32 buffer = stage->input;
    return SFBUF_GetTermFlg(handle, buffer);
}
static Sint32 sfmpv_DecodePicAtr(struct sfd_handle *handle,
                                 struct mpv_input *input, Sint32 *consumed)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct mpv_obj *decoder = video->decoder;
    struct mpv_picture *picture = &video->picture;
    if (MPV_DecodePicAtr(decoder, input, consumed) != 0) {
        return SFLIB_SetErr(handle, HEADER_FAILED);
    }
    if (MPV_GetPicAtr(decoder, picture) != 0) {
        return SFLIB_SetErr(handle, PICTURE_INFO_FAILED);
    }
    sfmpv_SetHeadTtu(handle, picture);
    sfmpv_SetDecTtu(handle, picture);
    return sfmpv_FirstPicAtr(handle, decoder, picture, input);
}
static void sfmpv_SetHeadTtu(struct sfd_handle *handle,
                             const struct mpv_picture *picture)
{
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *first = &timer->first_picture;
    struct sfd_timecode code;
    Sint32 time;
    Sint32 unit;
    if (first->valid) {
        return;
    }
    sfmpv_ReadTcode(picture, &code);
    code.frame_adjust = 0;
    SFTIM_Tc2Time(&code, &time, &unit);
    first->code = code;
    first->time = time;
    first->unit = unit;
    first->valid = TRUE;
}
static void sfmpv_SetDecTtu(struct sfd_handle *handle,
                            const struct mpv_picture *picture)
{
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *first = &timer->first_picture;
    struct sfd_time *latest = &timer->latest_picture;
    struct sfd_time *current = &timer->current_picture;
    struct sfd_timecode code;
    Sint32 time;
    Sint32 unit;
    sfmpv_ReadTcode(picture, &code);
    SFTIM_Tc2Time(&code, &time, &unit);
    current->code = code;
    current->time = time - first->time;
    current->unit = unit;
    current->valid = TRUE;
    if (latest->time <= current->time) {
        *latest = *current;
    }
}
static void sfmpv_ReadTcode(const struct mpv_picture *picture,
                            struct sfd_timecode *code)
{
    code->rate = picture->frame_rate;
    code->drop = picture->drop_frame;
    code->hour = picture->hours;
    code->minute = picture->minutes;
    code->second = picture->seconds;
    code->frame = picture->pictures;
    code->frame_adjust = picture->temporal_ref;
}

static Sint32 sfmpv_FirstPicAtr(struct sfd_handle *handle,
                                struct mpv_obj *decoder,
                                const struct mpv_picture *picture,
                                const struct mpv_input *input)
{
    struct sfd_movie_info *movie = &handle->movie;
    Sint32 rate;
    if (movie->bit_rate != 0) {
        return 0;
    }
    if (MPV_GetBitRate(decoder, &rate) != 0) {
        return SFLIB_SetErr(handle, BIT_RATE_FAILED);
    }
    sfmpv_SetMpvHd(handle, rate, input);
    sfmpv_SetMvInf(movie, rate, picture);
    return sfmpv_ChkBufSiz(handle, movie);
}
/* keep the first sequence header, its bit rate and the first picture's time
 * in the seek record. sfmpv_ReprocessShc restores the header and the time
 * after a seek */
static void sfmpv_SetMpvHd(struct sfd_handle *handle, Sint32 rate,
                           const struct mpv_input *input)
{
    struct sfd_video_seek *seek;
    struct sfd_timer *timer;
    struct sfd_sequence_header *saved;
    Uint32 size;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((seek = sfmpv_GetHd(handle)) == NULL) {
        return;
    }
    if (seek->valid) {
        return;
    }
    timer = &handle->timer;
    saved = &seek->sequence;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    saved->size = (size = input->size) < sizeof(saved->bytes)
                      ? size
                      : sizeof(saved->bytes);
    UTY_MemcpyByte(saved->bytes, input->data, saved->size);
    if (rate == VARIABLE_BIT_RATE) {
        seek->constant = FALSE;
    } else {
        seek->constant = TRUE;
    }
    seek->byte_rate = rate * BIT_RATE_BYTES;
    seek->first = timer->first_picture;
    seek->valid = TRUE;
}
static void sfmpv_SetMvInf(struct sfd_movie_info *movie, Sint32 rate,
                           const struct mpv_picture *picture)
{
    movie->width = picture->width;
    movie->height = picture->height;
    movie->mb_width = picture->mb_width;
    movie->mb_height = picture->mb_height;
    movie->picture_rate = picture->frame_rate;
    movie->bit_rate = rate;
}
static Sint32 sfmpv_ChkBufSiz(struct sfd_handle *handle,
                              const struct sfd_movie_info *movie)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    const struct sfd_video_params *params = &video->params;
    Sint32 width = movie->width;
    Sint32 height = movie->height;
    if (width > params->luma_width ||
        width > params->chroma_width * LUMA_PER_CHROMA ||
        height > params->luma_height ||
        height > params->chroma_height * LUMA_PER_CHROMA) {
        return SFLIB_SetErr(handle, REFERENCES_TOO_SMALL);
    }
    if (SFD_PICTURE_BYTES(width, height) >
        SFD_PICTURE_BYTES(params->output_width, params->output_height)) {
        return SFLIB_SetErr(handle, FRAMES_TOO_SMALL);
    }
    if (params->frames <= 0) {
        return SFLIB_SetErr(handle, NO_FRAMES);
    }
    return 0;
}
static Bool sfmpv_IsConcatTimeFix(struct sfd_handle *handle)
{
    struct sfd_timer *timer = &handle->timer;
    return timer->supply_time >= 0;
}

static Bool sfmpv_IsSkip(struct sfd_handle *handle,
                         const struct mpv_input *input)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct mpv_picture *picture = &video->picture;
    Sint32 kind;
    Bool skip;
    if (handle->conditions[SFD_COND_DECODE_ALL]) {
        return FALSE;
    }
    kind = picture->coding_type;
    if (sfmpv_IsSeekSkip(handle) || sfmpv_IsPtypeSkip(handle, kind) ||
        sfmpv_IsEmptyBpic(handle, kind, input) ||
        sfmpv_IsDefect(handle, kind) || sfmpv_IsLate(handle, kind)) {
        skip = TRUE;
    } else {
        skip = FALSE;
    }
    sfmpv_UpdateDefect(handle, picture, skip);
    return skip;
}
static Bool sfmpv_IsDefect(struct sfd_handle *handle, Sint32 kind)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    Sint32 state = video->defect;
    Bool missing = FALSE;
    switch (state) {
    case NEED_BOTH:
        if (kind == P_PICTURE || kind == B_PICTURE) {
            missing = TRUE;
        }
        break;
    case NEED_SECOND:
        if (kind == B_PICTURE) {
            missing = TRUE;
        }
        break;
    case REFERENCES_READY:
        break;
    }
    return missing;
}
static void sfmpv_UpdateDefect(struct sfd_handle *handle,
                               const struct mpv_picture *picture, Bool skipped)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct mpv_obj *decoder = video->decoder;
    Sint32 state = video->defect;
    Sint32 closed;
    Sint32 broken;
    Sint32 kind;
    if (video->gop != picture->gop) {
        video->gop = picture->gop;
        MPV_GetLinkFlg(decoder, &closed, &broken);
        if (!closed && broken == TRUE) {
            state = NEED_BOTH;
        }
        if (closed == TRUE) {
            state = REFERENCES_READY;
        }
    }
    if (skipped == TRUE) {
        kind = picture->coding_type;
        if (kind == I_PICTURE || kind == P_PICTURE) {
            state = NEED_BOTH;
        }
    } else {
        if (state == NEED_BOTH) {
            state = NEED_SECOND;
        } else if (state == NEED_SECOND) {
            state = REFERENCES_READY;
        }
    }
    video->defect = state;
}
static Bool sfmpv_IsSeekSkip(struct sfd_handle *handle)
{
    struct sfd_seek_state *seek = &handle->seek;
    struct sfd_seek_time *position = &seek->position;
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *current = &timer->current_picture;
    struct sfd_time *start = &timer->start;
    if (position->time == SFD_NO_SEEK) {
        return FALSE;
    }
    if (start->valid) {
        return FALSE;
    }
    if (UTY_CmpTime(position->time, position->unit, current->time,
                    current->unit)) {
        return FALSE;
    }
    return TRUE;
}
static Bool sfmpv_IsPtypeSkip(struct sfd_handle *handle, Sint32 kind)
{
    Sint32 *conditions = handle->conditions;
    Bool enabled;
    switch (kind) {
    case I_PICTURE:
        enabled = conditions[SFD_COND_DECODE_I];
        break;
    case P_PICTURE:
        enabled = conditions[SFD_COND_DECODE_P];
        break;
    case B_PICTURE:
        enabled = conditions[SFD_COND_DECODE_B];
        break;
    default:
        return TRUE;
    }
    if (!enabled) {
        return TRUE;
    }
    return FALSE;
}
static Bool sfmpv_IsEmptyBpic(struct sfd_handle *handle, Sint32 kind,
                              const struct mpv_input *input)
{
    struct sfd_play_info *play;
    struct sfd_movie_info *movie;
    Bool empty;
    if (kind != B_PICTURE) {
        return FALSE;
    }
    if (SFSET_GetCond(handle, SFD_COND_DECODE_EMPTY_B)) {
        return FALSE;
    }
    movie = &handle->movie;
    play = &handle->play;
    empty = MPV_IsEmptyBpic(input->data, input->size,
                            movie->mb_width * movie->mb_height);
    if (empty) {
        play->empty_pictures++;
    }
    return empty;
}
/* whether the picture is too late to decode, which it is once the handle's
 * time passes its deadline. the deadline is the picture's time less
 * SFD_COND_DECODE_TIME. an I or P picture's time is taken as when the next
 * I picture is due, by SFTIM_GetNextItime. at SFD_NORMAL_SPEED or slower, no
 * more than SFD_COND_LATE_SKIPS pictures in a row are late. a function set
 * by SFD_SetUsrIsSkipFn decides in its place */
static Bool sfmpv_IsLate(struct sfd_handle *handle, Sint32 kind)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *start = &timer->start;
    struct sfd_time *current = &timer->current_picture;
    Sint32 time;
    Sint32 unit;
    Sint32 now;
    Sint32 now_unit;
    Sint32 delay;
    Sint32 delay_unit;
    sfd_stage_fn skip;
    if (!start->valid) {
        time = 0;
    } else {
        time = current->time - start->time + timer->supply_time;
    }
    skip = timer->skip;
    unit = current->unit;
    if (skip != NULL) {
        return skip(handle, kind, time, unit);
    }
    if (kind == I_PICTURE) {
        SFTIM_UpdateItime(timer, time);
    }
    if (kind == I_PICTURE || kind == P_PICTURE) {
        time = SFTIM_GetNextItime(timer, time);
    }
    if (SFTIM_GetSpeed(handle) <= SFD_NORMAL_SPEED) {
        if (video->late >= handle->conditions[SFD_COND_LATE_SKIPS]) {
            return FALSE;
        }
    }
    SFTIM_GetTime(handle, &now, &now_unit);
    if (now < 0) {
        return FALSE;
    }
    sfmpv_GetDtime(handle, kind, &delay, &delay_unit);
    time -= unit * delay / delay_unit;
    if (UTY_CmpTime(now, now_unit, time, unit)) {
        return FALSE;
    }
    video->late++;
    return TRUE;
}
static void sfmpv_GetDtime(struct sfd_handle *handle, Sint32 kind, Sint32 *time,
                           Sint32 *unit)
{
    Sint32 *conditions = handle->conditions;
    Sint32 duration;
    *unit = conditions[SFD_COND_DECODE_UNIT];
    /* @hack spelling: the decode time held in duration, where *time could take
     * it directly */
    duration = conditions[SFD_COND_DECODE_TIME];
    *time = duration;
}

static Sint32 sfmpv_SkipFrm(struct sfd_handle *handle, struct mpv_input *input,
                            Sint32 *consumed)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct mpv_obj *decoder = video->decoder;
    struct mpv_picture *picture = &video->picture;
    Sint32 error;
    sfmpv_SetSkipTtu(handle);
    error = MPV_SkipFrm(decoder, input, consumed);
    if (error != 0) {
        return SFLIB_SetErr(handle, SKIP_FAILED);
    }
    SFPLY_AddSkipPic(handle, 1, picture->coding_type);
    return 0;
}
static void sfmpv_SetSkipTtu(struct sfd_handle *handle)
{
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *current = &timer->current_picture;
    struct sfd_time *skipped = &timer->skipped_picture;
    struct sfd_time *start = &timer->start;
    if (current->time < start->time) {
        *skipped = *current;
    }
}
static Sint32 sfmpv_DecodeFrm(struct sfd_handle *handle,
                              struct mpv_input *input, Sint32 *consumed)
{
    struct sfd_play_info *play = &handle->play;
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct mpv_obj *decoder = video->decoder;
    struct mpv_picture *picture = &video->picture;
    struct mpv_request request;
    struct sfd_output_frame *frame;
    Sint32 error;
    if (sfmpv_SetFrmPara(handle, picture, &request, &frame) != 0) {
        return 0;
    }
    sfmpv_SetStartTtu(handle);
    error = MPV_DecodeFrm(decoder, input, consumed, &request);
    if (error != 0) {
        SFMPVF_FreeFrm(frame);
        return SFLIB_SetErr(handle, DECODE_FAILED);
    }
    if (*consumed > 0) {
        SFMPVF_SetGopStat(handle, FALSE);
        sfmpv_SetFrmTime(handle, frame);
        frame->sequence = video->sequence_ends;
        SFMPVF_StbyFrm(frame);
        MPV_GetDctCnt(decoder, &play->dct_intra, &play->dct_inter);
        SFPLY_AddDecPic(handle, 1, picture->coding_type);
        video->late = 0;
    } else {
        SFMPVF_FreeFrm(frame);
    }
    return 0;
}
static Sint32 sfmpv_SetFrmPara(struct sfd_handle *handle,
                               const struct mpv_picture *picture,
                               struct mpv_request *request,
                               struct sfd_output_frame **out)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct mpv_frame *references;
    Sint32 kind;
    *out = SFMPVF_AllocFrm(handle);
    if (*out == NULL) {
        return NO_FRAME_FREE;
    }
    (*out)->picture = *picture;
    kind = picture->coding_type;
    if (kind == I_PICTURE || kind == P_PICTURE) {
        video->first_reference ^= 1;
        video->second_reference ^= 1;
    }
    references = video->references;
    request->first = references[video->first_reference];
    request->second = references[video->second_reference];
    request->output = (*out)->pixels;
    request->picture = &(*out)->picture;
    request->delimiter_misses = 0;
    request->other_errors = 0;
    return 0;
}
static void sfmpv_SetStartTtu(struct sfd_handle *handle)
{
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *first = &timer->first_picture;
    struct sfd_time *current = &timer->current_picture;
    struct sfd_time *start = &timer->start;
    struct sfd_timecode code;
    Sint32 time;
    Sint32 unit;
    if (start->valid) {
        return;
    }
    code = current->code;
    /* where B pictures decode, timer->start takes the group's time code,
     * without this picture's temporal reference */
    if (!sfmpv_IsDefect(handle, B_PICTURE) &&
        !sfmpv_IsPtypeSkip(handle, B_PICTURE)) {
        code.frame_adjust = 0;
    }
    SFTIM_Tc2Time(&code, &time, &unit);
    start->code = code;
    start->time = time - first->time;
    start->unit = unit;
    start->valid = TRUE;
}
static void sfmpv_SetFrmTime(struct sfd_handle *handle,
                             struct sfd_output_frame *frame)
{
    struct sfd_timer *timer = &handle->timer;
    struct sfd_time *current = &timer->current_picture;
    struct sfd_time *start = &timer->start;
    frame->time = current->time - start->time + timer->supply_time;
    frame->unit = current->unit;
}

static void sfmpv_GoDdelim(struct mpv_input *input, Sint32 *consumed,
                           Sint32 mask)
{
    Uint8 *found;
    while (input->size >= START_CODE_SIZE) {
        found = sfmpv_GoDdelimSub(input, consumed, mask);
        if (found == NULL) {
            break;
        }
        if (mask != SEQUENCE_CODES) {
            break;
        }
        if (MPV_CheckDelim(found) != MPV_DELIM_SEQUENCE) {
            break;
        }
        if (!SFMPVD_IsShc(input->data, input->size)) {
            *consumed += START_CODE_SIZE;
            input->data += START_CODE_SIZE;
            input->size -= START_CODE_SIZE;
        } else {
            break;
        }
    }
}
static Uint8 *sfmpv_GoDdelimSub(struct mpv_input *input, Sint32 *consumed,
                                Sint32 mask)
{
    Sint32 remaining = input->size;
    Uint8 *bytes = input->data;
    Uint8 *found = MPV_SearchDelim(bytes, remaining, mask);
    Sint32 skip;
    if (found != NULL) {
        skip = found - bytes;
    } else {
        skip = remaining - LOOKBEHIND;
    }
    bytes += skip;
    remaining -= skip;
    *consumed += skip;
    input->data = bytes;
    input->size = remaining;
    return found;
}

static Sint32 SFMPV_Create(struct sfd_handle *handle, Sint32 first,
                           Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    struct sfd_video_work *video;
    struct mpv_obj *decoder;
    Sint32 error;
    if (!SFSET_GetCond(handle, SFD_COND_VIDEO)) {
        return 0;
    }
    stage = VIDEO_STAGE(handle);
    video = &stage->work.video;
    error = sfmpv_InitInf(handle, video);
    if (error != 0) {
        return error;
    }
    decoder = MPV_Create();
    if (decoder == NULL) {
        return SFLIB_SetErr(handle, CREATE_FAILED);
    }
    error = MPV_SetErrFunc(decoder, sfmpv_ErrFn, handle);
    if (error != 0) {
        sfmpv_DestroySub(decoder);
        return SFLIB_SetErr(handle, INVALID_CALLBACK);
    }
    MPV_SetCond(decoder, MPV_COND_DECODE, 1);
    MPV_SetCond(decoder, MPV_COND_RECOVER, 0);
    MPV_SetCond(decoder, MPV_COND_HALF_PEL_2, 0);
    video->decoder = decoder;
    return 0;
}
static Sint32 sfmpv_InitInf(struct sfd_handle *handle,
                            struct sfd_video_work *video)
{
    const struct sfd_video_params *params = &sfmpv_para;
    if (params->references == NULL || params->output == NULL) {
        return SFLIB_SetErr(handle, NO_PARAMS);
    }
    video->params = *params;
    video->decoder = NULL;
    video->held = NULL;
    video->defect = REFERENCES_READY;
    video->resume_codes = SEQUENCE_CODES;
    SFMPVF_InitRfb(handle);
    video->late = 0;
    video->sequence_ends = 0;
    sfmpv_InitPicAtr(&video->picture);
    video->gop = NO_GROUP;
    return 0;
}
static void sfmpv_InitPicAtr(struct mpv_picture *picture)
{
    UTY_MemsetDword((Uint32 *)picture, ~0UL, sizeof(*picture) / sizeof(Uint32));
}
static void sfmpv_ErrFn(void *handle, Sint32 code)
{
    SFLIB_SetErr(handle, code);
}
static Sint32 SFMPV_Destroy(struct sfd_handle *handle, Sint32 first,
                            Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    struct mpv_obj *decoder = video->decoder;
    Sint32 error;
    if (decoder != NULL) {
        error = sfmpv_DestroySub(decoder);
        if (error != 0) {
            return SFLIB_SetErr(handle, DESTROY_FAILED);
        }
        video->decoder = NULL;
    }
    return 0;
}
static Sint32 sfmpv_DestroySub(struct mpv_obj *decoder)
{
    return MPV_Destroy(decoder);
}
/* clang-format off */
static SFD_STAGE_NOOP(SFMPV_Standby)
static SFD_STAGE_NOOP(SFMPV_Start)
static SFD_STAGE_NOOP(SFMPV_Stop)
static SFD_STAGE_NOOP(SFMPV_Pause)
SFD_STAGE_REJECT(SFMPV_GetWrite, NO_WRITE)
SFD_STAGE_REJECT(SFMPV_AddWrite, NO_WRITE)

static Sint32 SFMPV_GetRead(struct sfd_handle *handle, Sint32 destination,
                            Sint32 second, Sint32 third)
/* clang-format on */
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    /* @hack redundant: params, formed and never read */
    struct sfd_video_params *params = &video->params;
    struct sfd_frame_info **out = (struct sfd_frame_info **)destination;
    struct sfd_timer *timer;
    struct sfd_output_frame *frame;
    Bool last;
    Sint32 count;
    (void)params;
    count = SFMPVF_GetNumFrm(handle);
    if (count == SFMPVF_ENDED) {
        *out = NULL;
    } else if ((frame = SFMPVF_HoldFrm(handle, &last)) == NULL) {
        *out = NULL;
    } else {
        sfmpv_SetFrmInf(handle, frame, out);
        if (last) {
            timer = &handle->timer;
            timer->end_time = (*out)->time;
            timer->end_unit = (*out)->time_unit;
        }
    }
    return 0;
}
static void sfmpv_SetFrmInf(struct sfd_handle *handle,
                            struct sfd_output_frame *frame,
                            struct sfd_frame_info **out)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct mpv_picture *picture = &frame->picture;
    struct sfd_video_work *video = &stage->work.video;
    Sint32 buffer = stage->outputs[SFD_OUTPUT];
    struct sfd_buffer_table *buffers = &handle->buffers;
    struct sfd_buffer *record = &buffers->records[buffer];
    struct sfd_frame_queue *queue = &record->data.frames;
    struct sfd_frame_slot *slot = &queue->slots[0];
    *out = &slot->frame;
    slot->state = SFD_SLOT_HELD;
    video->held = frame;
    (*out)->width = picture->width;
    (*out)->height = picture->height;
    (*out)->mb_width = picture->mb_width;
    (*out)->mb_height = picture->mb_height;
    (*out)->coding_type = picture->coding_type;
    (*out)->time = frame->time;
    (*out)->time_unit = frame->unit;
    (*out)->serial = handle->params.serial;
    (*out)->image = frame->pixels;
    (*out)->flags = 0;
    (*out)->reserved = 0;
    (*out)->sequence = frame->sequence;
}
static Sint32 SFMPV_AddRead(struct sfd_handle *handle, Sint32 released,
                            Sint32 second, Sint32 third)
{
    Sint32 mask;
    struct sfd_video_work *video;
    struct sfd_video_params *params;
    Sint32 buffer;
    struct sfd_frame_slot *slot;
    struct sfd_buffer *record;
    SFLIB_LockCs(&mask);
    video = &VIDEO_STAGE(handle)->work.video;
    /* @hack redundant: params, formed and never read */
    params = &video->params;
    (void)params;
    buffer = VIDEO_STAGE(handle)->outputs[SFD_OUTPUT];
    /* @hack spelling: record stepped on to buffer, where indexing the records
     * by buffer would do */
    record = handle->buffers.records;
    record += buffer;
    slot = &record->data.frames.slots[0];
    if (slot->state != SFD_SLOT_HELD) {
        SFLIB_UnlockCs(&mask);
        return SFLIB_SetErr(handle, NOT_HELD);
    }
    if (&slot->frame != (struct sfd_frame_info *)released) {
        SFLIB_UnlockCs(&mask);
        return SFLIB_SetErr(handle, WRONG_FRAME);
    }
    slot->state = SFD_SLOT_FREE;
    SFMPVF_FreeFrm(video->held);
    SFLIB_UnlockCs(&mask);
    return 0;
}
static Sint32 SFMPV_Seek(struct sfd_handle *handle, Sint32 first, Sint32 second,
                         Sint32 third)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    Bool found;
    Sint32 error = sfmpv_ReprocessShc(handle, video, &found);
    if (error != 0) {
        return error;
    }
    video->defect = NEED_BOTH;
    if (!found || !SFSET_GetCond(handle, SFD_COND_SEEK_GROUP)) {
        video->resume_codes = SEQUENCE_CODES;
    } else {
        video->resume_codes = GROUP_CODES;
    }
    return 0;
}
static Sint32 sfmpv_ReprocessShc(struct sfd_handle *handle,
                                 struct sfd_video_work *video, Bool *found)
{
    struct sfd_video_seek *seek;
    struct sfd_timer *timer;
    struct sfd_sequence_header *saved;
    struct mpv_obj *decoder;
    struct mpv_input input;
    Sint32 consumed;
    *found = FALSE;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((seek = sfmpv_GetHd(handle)) == NULL) {
        return 0;
    }
    if (!seek->valid) {
        return 0;
    }
    timer = &handle->timer;
    saved = &seek->sequence;
    decoder = video->decoder;
    timer->first_picture = seek->first;
    input.data = saved->bytes;
    input.size = saved->size;
    if (MPV_DecodePicAtr(decoder, &input, &consumed) != 0) {
        return SFLIB_SetErr(handle, REPARSE_FAILED);
    }
    *found = TRUE;
    return 0;
}
/* the seek record's video entry, which holds the first sequence's header.
 * it is NULL without a seek record, and once a sequence has ended */
static struct sfd_video_seek *sfmpv_GetHd(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = VIDEO_STAGE(handle);
    struct sfd_video_work *video = &stage->work.video;
    if (handle->seek.info == NULL) {
        return NULL;
    }
    if (video->sequence_ends > 0) {
        return NULL;
    }
    return &handle->seek.info->head.video;
}

const struct sfd_stage_ops SFD_tr_vd_mpv = {
    {SFMPV_Init, SFMPV_Finish, SFMPV_ExecServer, SFMPV_Create, SFMPV_Destroy,
     SFMPV_Standby, SFMPV_Start, SFMPV_Stop, SFMPV_Pause, SFMPV_GetWrite,
     SFMPV_AddWrite, SFMPV_GetRead, SFMPV_AddRead, SFMPV_Seek}};
