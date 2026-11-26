/* seeking. a handle analyzes its stream's headers into the struct
 * sfd_seek_info SFD_EntrySeek gave it, as it runs. the info keeps the
 * analysis for later plays of the movie. the byte rate found converts between
 * a time and a byte position. a table SFD_SetSeekPosTbl sets takes the rate's
 * place, but a conversion through a table gives 0 */
#include <sfd.h>
#include <sfh.h>
#include <sg_xpt.h>
#include <sofdec.h>
#include <sofdec_uty.h>

#define MILLISECONDS 1000
/* the tool version from which a pack header's mux rate counts each pack's
 * PACK_SIZE bytes. an older stream's counts only the PACK_PAYLOAD bytes past
 * a pack's headers, and sfsee_ExecHeadAnaly scales it up */
#define WHOLE_PACK_VERSION SFH_VERSION(1, 8)
#define PACK_SIZE 2048
#define PACK_PAYLOAD 2018
#define INVALID_ATTACH ((Sint32)0xff000151)
#define INVALID_TABLE ((Sint32)0xff000152)
#define INVALID_ANALYSIS ((Sint32)0xff000153)
#define INVALID_ANALYSIS_QUERY ((Sint32)0xff000154)
#define INVALID_SEEK_QUERY ((Sint32)0xff000155)
#define INVALID_TIME ((Sint32)0xff000156)
#define INVALID_POSITION ((Sint32)0xff000157)
#define INVALID_SEEK ((Sint32)0xff000158)

static void sfsee_InitSeekInf(struct sfd_seek_info *info);
static void sfsee_InitHeadInf(struct sfd_analysis *head);
static void sfsee_IsHeadAnalyEnd(const struct sfd_seek_info *info, Bool *ended);
static void sfsee_ExecHeadAnaly(struct sfd_handle *handle);
static Bool sfsee_IsAudioAnalyzing(struct sfd_handle *handle,
                                   struct sfd_audio_seek *stream,
                                   Bool *present);
static Bool sfsee_IsVideoAnalyzing(struct sfd_handle *handle,
                                   struct sfd_video_seek *stream,
                                   Bool *present);
static Bool sfsee_IsMpsStream(struct sfd_handle *handle);
static void sfsee_IsSeekAble(const struct sfd_seek_info *info, Bool *able);
static void sfsee_CnvTimeToPos(const struct sfd_seek_info *info, Sint32 time,
                               Sint32 unit, Sint32 *position);
static void sfsee_SearchTimeToPos(void *table, Sint32 time, Sint32 unit,
                                  Sint32 *position);
static void sfsee_CnvPosToTime(const struct sfd_seek_info *info,
                               Sint32 position, Sint32 *time, Sint32 *unit);
static void sfsee_SearchPosToTime(void *table, Sint32 position, Sint32 *time,
                                  Sint32 *unit);

void SFSEE_InitHn(struct sfd_seek_state *state)
{
    struct sfd_seek_time *seek;
    state->info = NULL;
    seek = &state->position;
    seek->reserved = 0;
    seek->time = SFD_NO_SEEK;
    seek->unit = SFD_NO_UNIT;
}

void SFD_InitSeek(struct sfd_seek_info *info, Sint32 count)
{
    Sint32 i;
    for (i = 0; i < count; i++) {
        sfsee_InitSeekInf(&info[i]);
    }
}

static void sfsee_InitSeekInf(struct sfd_seek_info *info)
{
    UTY_MemsetDword((Uint32 *)info, 0, sizeof(*info) / sizeof(Uint32));
    sfsee_InitHeadInf(&info->head);
    info->positions = NULL;
}

static void sfsee_InitHeadInf(struct sfd_analysis *head)
{
    struct sfd_file_header *header;
    struct sfd_system_seek *system;
    struct sfd_video_seek *video;
    struct sfd_audio_seek *audio;

    header = &head->header;
    system = &head->system;
    video = &head->video;
    audio = &head->audio;
    head->analyzed = FALSE;
    head->byte_rate = 0;
    head->fixed = FALSE;
    SFHDS_InitFhd(header);
    system->saved = FALSE;
    system->byte_rate = 0;
    system->fixed = FALSE;
    system->video_count = 0;
    system->audio_count = 0;
    system->video_pts = 0;
    system->audio_pts = 0;
    video->valid = FALSE;
    video->byte_rate = 0;
    video->constant = FALSE;
    SFTIM_InitTtu(&video->first, SFD_NO_TIME);
    audio->valid = FALSE;
    audio->byte_rate = 0;
    audio->channels = 0;
    audio->sample_rate = 0;
    audio->samples = 0;
    audio->constant = TRUE;
}

Sint32 SFD_EntrySeek(struct sfd_handle *handle, struct sfd_seek_info *info)
{
    struct sfd_seek_state *state;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_ATTACH);
    }
    state = &handle->seek;
    state->info = info;
    return 0;
}

Sint32 SFD_SetSeekPosTbl(struct sfd_handle *handle, void *table)
{
    struct sfd_seek_state *state;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_TABLE);
    }
    state = &handle->seek;
    state->info->positions = table;
    return 0;
}

Sint32 SFD_StartHeadAnaly(struct sfd_handle *handle)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_ANALYSIS);
    }
    SFSET_SetCond(handle, SFD_COND_PLAY_MODE, SFD_PLAY_HEADERS);
    return SFPL2_Standby(handle);
}

Sint32 SFD_IsHeadAnalyEnd(struct sfd_handle *handle, Bool *ended)
{
    struct sfd_seek_state *state;
    *ended = FALSE;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_ANALYSIS_QUERY);
    }
    state = &handle->seek;
    sfsee_IsHeadAnalyEnd(state->info, ended);
    return 0;
}

static void sfsee_IsHeadAnalyEnd(const struct sfd_seek_info *info, Bool *ended)
{
    /* @hack redundant: a return at the end of each arm */
    if (info->head.analyzed == TRUE) {
        *ended = TRUE;
        return;
    } else {
        *ended = FALSE;
        return;
    }
}

void SFSEE_ExecServer(struct sfd_handle *handle)
{
    struct sfd_seek_state *state;
    struct sfd_seek_info *info;
    state = &handle->seek;
    info = state->info;
    if (info == NULL) {
        return;
    }
    sfsee_ExecHeadAnaly(handle);
}

static void sfsee_ExecHeadAnaly(struct sfd_handle *handle)
{
    struct sfd_seek_state *state;
    struct sfd_seek_info *info;
    struct sfd_analysis *head;
    struct sfd_file_header *header;
    struct sfd_system_seek *system;
    struct sfd_video_seek *video;
    struct sfd_audio_seek *audio;
    Bool has_video;
    Bool has_audio;
    Sint32 rate;

    state = &handle->seek;
    info = state->info;
    head = &info->head;
    header = &head->header;
    system = &head->system;
    video = &head->video;
    audio = &head->audio;
    if (head->analyzed) {
        return;
    }
    if (sfsee_IsAudioAnalyzing(handle, audio, &has_audio)) {
        return;
    }
    if (sfsee_IsVideoAnalyzing(handle, video, &has_video)) {
        return;
    }
    if (sfsee_IsMpsStream(handle)) {
        system->saved = TRUE;
        if (header->valid && header->byte_rate > 0) {
            rate = header->byte_rate;
        } else if (SFHDS_GetMuxVerNum(handle) >= WHOLE_PACK_VERSION) {
            rate = system->byte_rate;
        } else {
            rate = system->byte_rate * PACK_SIZE / PACK_PAYLOAD;
        }
        head->byte_rate = rate;
        head->fixed = system->fixed;
    } else if (has_video) {
        head->byte_rate = video->byte_rate;
        head->fixed = video->constant;
    } else if (has_audio) {
        head->byte_rate = audio->byte_rate;
        head->fixed = audio->constant;
    } else {
        return;
    }
    head->analyzed = TRUE;
}

/* define name, which sets *present to whether the handle plays the stream
 * that stage decodes. stream is that stream's seek record. name returns
 * whether the handle plays the stream and stream isn't valid yet */
#define STREAM_ANALYZING(name, record, stage, condition)                       \
    static Bool name(struct sfd_handle *handle, struct record *stream,         \
                     Bool *present)                                            \
    {                                                                          \
        Bool waiting;                                                          \
        if (SFTRN_IsSetup(handle, stage) != 0 &&                               \
            SFSET_GetCond(handle, condition) == TRUE) {                        \
            *present = TRUE;                                                   \
            /* @hack spelling: waiting set by a test of valid, where           \
             * !stream->valid would do */                                      \
            if (!stream->valid) {                                              \
                waiting = TRUE;                                                \
            } else {                                                           \
                waiting = FALSE;                                               \
            }                                                                  \
        } else {                                                               \
            *present = FALSE;                                                  \
            waiting = FALSE;                                                   \
        }                                                                      \
        return waiting;                                                        \
    }
STREAM_ANALYZING(sfsee_IsAudioAnalyzing, sfd_audio_seek, SFD_AUDIO,
                 SFD_COND_AUDIO)
STREAM_ANALYZING(sfsee_IsVideoAnalyzing, sfd_video_seek, SFD_VIDEO,
                 SFD_COND_VIDEO)

static Bool sfsee_IsMpsStream(struct sfd_handle *handle)
{
    /* @hack spelling: a test of SFTRN_IsSetup that returns FALSE or TRUE,
     * where returning its result would do */
    if (SFTRN_IsSetup(handle, SFD_SYSTEM) == 0) {
        return FALSE;
    }
    return TRUE;
}

Sint32 SFD_IsSeekAble(struct sfd_handle *handle, Bool *able)
{
    struct sfd_seek_state *state;
    *able = FALSE;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SEEK_QUERY);
    }
    state = &handle->seek;
    sfsee_IsSeekAble(state->info, able);
    return 0;
}

static void sfsee_IsSeekAble(const struct sfd_seek_info *info, Bool *able)
{
    const struct sfd_analysis *head;
    Bool ended;

    *able = FALSE;
    if (info == NULL) {
        return;
    }
    head = &info->head;
    sfsee_IsHeadAnalyEnd(info, &ended);
    if (!ended) {
        return;
    }
    if (info->positions != NULL || head->byte_rate > 0) {
        *able = TRUE;
    }
}

Sint32 SFD_CnvTimeToPos(struct sfd_handle *handle, Sint32 time, Sint32 unit,
                        Sint32 *position)
{
    struct sfd_seek_state *state;
    Bool able;

    *position = 0;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_TIME);
    }
    state = &handle->seek;
    sfsee_IsSeekAble(state->info, &able);
    if (able) {
        sfsee_CnvTimeToPos(state->info, time, unit, position);
    }
    return 0;
}
static void sfsee_CnvTimeToPos(const struct sfd_seek_info *info, Sint32 time,
                               Sint32 unit, Sint32 *position)
{
    const struct sfd_analysis *head;
    void *table;
    head = &info->head;
    table = info->positions;
    if (table != NULL) {
        sfsee_SearchTimeToPos(table, time, unit, position);
    } else {
        *position = UTY_MulAbDivC(head->byte_rate, time, unit);
    }
}
static void sfsee_SearchTimeToPos(void *table, Sint32 time, Sint32 unit,
                                  Sint32 *position)
{
    *position = 0;
}

Sint32 SFD_CnvPosToTime(struct sfd_handle *handle, Sint32 position,
                        Sint32 *time, Sint32 *unit)
{
    struct sfd_seek_state *state;
    Bool able;
    *time = 0;
    *unit = SFD_NO_UNIT;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_POSITION);
    }
    state = &handle->seek;
    sfsee_IsSeekAble(state->info, &able);
    if (able) {
        sfsee_CnvPosToTime(state->info, position, time, unit);
    }
    return 0;
}
static void sfsee_CnvPosToTime(const struct sfd_seek_info *info,
                               Sint32 position, Sint32 *time, Sint32 *unit)
{
    const struct sfd_analysis *head;
    void *table;
    Sint32 rate;
    head = &info->head;
    table = info->positions;
    if (table != NULL) {
        sfsee_SearchPosToTime(table, position, time, unit);
    } else {
        rate = head->byte_rate;
        if (rate == 0) {
            *time = 0;
        } else {
            *time = UTY_MulAbDivC(position, MILLISECONDS, rate);
        }
        *unit = MILLISECONDS;
    }
}
static void sfsee_SearchPosToTime(void *table, Sint32 position, Sint32 *time,
                                  Sint32 *unit)
{
    *time = 0;
    *unit = MILLISECONDS;
}

Sint32 SFD_Seek(struct sfd_handle *handle, struct sfd_seek_time *position)
{
    struct sfd_seek_state *state;
    Sint32 result;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SEEK);
    }
    result = SFPLY_Stop(handle);
    if (result != 0) {
        return result;
    }
    SFSET_SetCond(handle, SFD_COND_PLAY_MODE, SFD_PLAY_NORMAL);
    state = &handle->seek;
    state->position = *position;
    /* @hack spelling: the call's result held in result, where the return
     * could take it directly */
    result = SFTRN_CallTrSetup(handle, SFD_OP_SEEK);
    return result;
}
