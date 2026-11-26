/* the player: the server that steps each handle, and the calls that create,
 * start, stop and feed one. a step runs the handle's stages, then moves its
 * status on toward its request */
#include <sfd.h>
#include <sofdec.h>
#include <sofdec_uty.h>
#include <sg_xpt.h>

/* the word of SFPLY_cond_dfl that SFPLY_Init checks. it reports
 * INVALID_DEFAULTS for any value there but GUARD_VALUE. the word lies within
 * SFD_CONDITIONS, so each handle's condition GUARD_INDEX starts as
 * GUARD_VALUE too */
#define GUARD_INDEX 51
#define GUARD_VALUE 0x5a5a5a5a
#define INVALID_DEFAULTS ((Sint32)0xff000201)
#define RESET_FAILED ((Sint32)0xff000202)
#define INVALID_PARAMS ((Sint32)0xff000203)
#define INVALID_DESTROY ((Sint32)0xff000131)
#define INVALID_START ((Sint32)0xff000132)
#define INVALID_STOP ((Sint32)0xff000133)
#define INVALID_WRITE ((Sint32)0xff000134)
#define INVALID_SUPPLY ((Sint32)0xff000135)
#define INVALID_FRAME ((Sint32)0xff000136)
#define INVALID_RELEASE ((Sint32)0xff000137)

typedef void (*picture_callback)(struct sfd_handle *, Sint32,
                                 struct sfd_play_info *);
static void sfply_ChkCondDfl(void);
static void sfply_ExecOne(struct sfd_handle *handle);
static void sfply_ExecOneSub(struct sfd_handle *handle);
static Sint32 sfply_TrExecServer(struct sfd_handle *handle);
static Sint32 sfply_StatStop(struct sfd_handle *handle);
static Sint32 sfply_StatPrep(struct sfd_handle *handle);
static Bool sfply_IsPrepEnd(struct sfd_handle *handle);
static void sfply_AdjustPrepEnd(struct sfd_handle *handle);
static void sfply_AdjustSyncMode(struct sfd_handle *handle);
static void sfply_AdjustEtrg(struct sfd_handle *handle);
static Sint32 sfply_StatStby(struct sfd_handle *handle);
static Sint32 sfply_StatPlay(struct sfd_handle *handle);
static Sint32 sfply_StatFin(struct sfd_handle *handle);
static Bool sfply_IsStartSync(struct sfd_handle *handle);
static Sint32 sfply_ChkFin(struct sfd_handle *handle);
static Bool sfply_IsEtime(struct sfd_handle *handle);
static Bool sfply_IsEtrg(struct sfd_handle *handle);
static Sint32 sfply_Fin(struct sfd_handle *handle);
static Sint32 sfply_ChkCrePara(const struct sfd_create_params *params);
static struct sfd_handle *sfply_SearchFreeHn(void);
static struct sfd_handle *sfply_InitHn(struct sfd_handle *handle,
                                       struct sfd_create_params *params,
                                       void *work);
static void sfply_InitMvInf(struct sfd_movie_info *movie);
static void sfply_InitPlyInf(struct sfd_play_info *play);
static Sint32 sfply_TrCreate(struct sfd_handle *handle);
static Sint32 sfply_TrDestroy(struct sfd_handle *handle);
static Sint32 sfply_Start(struct sfd_handle *handle);
static Sint32 sfply_TrStart(struct sfd_handle *handle);
static Sint32 sfply_TrStop(struct sfd_handle *handle);

/* the conditions each new handle starts with, by SFD_COND_ index, ten to a
 * row. a handle decodes I, P and B pictures and plays video and audio from
 * any channel. it times its pictures by the vertical blank and has no end
 * time. the word at GUARD_INDEX holds GUARD_VALUE */
/* clang-format off */
const Sint32 SFPLY_cond_dfl[SFD_CONDITIONS + 1] = {
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, SFD_CLOCK_VSYNC, 0, 1, -3, 1,
    SFD_NO_END_TIME, 1, 0, 3, 4096, SFD_END_EITHER, 1, 60, 1, SFD_ANY_CHANNEL,
    SFD_ANY_CHANNEL, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    -30000, -14000, 8000, 59940, 4000, 4000, 41, SFD_PLAY_NORMAL, 0, 0,
    0, GUARD_VALUE};
/* clang-format on */

void SFPLY_Init(void)
{
    sfply_ChkCondDfl();
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void sfply_ChkCondDfl(void)
{
    if (SFPLY_cond_dfl[GUARD_INDEX] != GUARD_VALUE) {
        SFLIB_SetErr(NULL, INVALID_DEFAULTS);
    }
}

void SFD_VbIn(void)
{
    SFTIM_VbIn();
}
void SFD_VbOut(void)
{
}

Sint32 SFD_ExecServer(void)
{
    Sint32 i;
    Sint32 status;
    Sint32 count;
    struct sfd_handle *handles;
    count = SFLIB_libwork.handle_count;
    handles = SFLIB_libwork.handles;
    for (i = 0; i < count; i++) {
        sfply_ExecOne(&handles[i]);
    }
    /* @hack spelling: the call's result held in status, where the return
     * could take it directly */
    status = SFPLY_DecideSvrStat();
    return status;
}

static void sfply_ExecOne(struct sfd_handle *handle)
{
    Sint32 status;
    status = handle->status;
    if (status == SFD_STOPPED || status == SFD_PREPARING ||
        status == SFD_STANDBY || status == SFD_PLAYING) {
        if (status == SFD_PREPARING || status == SFD_STANDBY ||
            status == SFD_PLAYING) {
            sfply_ExecOneSub(handle);
        }
        status = handle->status;
        switch (status) {
        case SFD_STOPPED:
            status = sfply_StatStop(handle);
            break;
        case SFD_PREPARING:
            status = sfply_StatPrep(handle);
            break;
        case SFD_STANDBY:
            status = sfply_StatStby(handle);
            break;
        case SFD_PLAYING:
            status = sfply_StatPlay(handle);
            break;
        case SFD_FINISHED:
            status = sfply_StatFin(handle);
            break;
        case SFD_FREE:
            break;
        }
        handle->status = status;
    }
}

static void sfply_ExecOneSub(struct sfd_handle *handle)
{
    sfply_TrExecServer(handle);
    SFSEE_ExecServer(handle);
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}
static Sint32 sfply_TrExecServer(struct sfd_handle *handle)
{
    return SFTRN_CallTrSetup(handle, SFD_OP_EXEC);
}
static Sint32 sfply_StatStop(struct sfd_handle *handle)
{
    Sint32 status;
    Sint32 request;
    status = handle->status;
    request = handle->request;
    switch (request) {
    case SFD_PREPARING:
    case SFD_STANDBY:
    case SFD_PLAYING:
    case SFD_FINISHED:
        status = SFD_PREPARING;
        break;
    }
    return status;
}

static Sint32 sfply_StatPrep(struct sfd_handle *handle)
{
    Sint32 status;
    Sint32 request;
    status = handle->status;
    request = handle->request;
    if (sfply_IsPrepEnd(handle)) {
        sfply_AdjustPrepEnd(handle);
        switch (request) {
        case SFD_PREPARING:
            status = SFD_PREPARING;
            break;
        case SFD_STANDBY:
            status = SFD_STANDBY;
            break;
        case SFD_PLAYING:
        case SFD_FINISHED:
            if (sfply_IsStartSync(handle)) {
                sfply_TrStart(handle);
                status = SFD_PLAYING;
            } else {
                status = SFD_STANDBY;
            }
            break;
        }
    }
    return status;
}

static Bool sfply_IsPrepEnd(struct sfd_handle *handle)
{
    Bool video;
    Bool audio;
    if (!SFSET_GetCond(handle, SFD_COND_VIDEO)) {
        video = TRUE;
    } else {
        video = SFTRN_GetPrepFlg(handle, SFD_VIDEO_OUT);
    }
    if (!SFSET_GetCond(handle, SFD_COND_AUDIO)) {
        audio = TRUE;
    } else {
        audio = SFTRN_GetPrepFlg(handle, SFD_AUDIO_OUT);
    }
    /* @hack spelling: a test that returns FALSE or TRUE, where returning
     * whether both are set would do */
    if (!video || !audio) {
        return FALSE;
    }
    return TRUE;
}

static void sfply_AdjustPrepEnd(struct sfd_handle *handle)
{
    sfply_AdjustSyncMode(handle);
    sfply_AdjustEtrg(handle);
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}
static void sfply_AdjustSyncMode(struct sfd_handle *handle)
{
    Sint32 *conditions;
    Sint32 *clock;
    conditions = handle->conditions;
    /* @hack spelling: the clock condition read through clock, where indexing
     * conditions would do */
    clock = &conditions[SFD_COND_CLOCK];
    if (!conditions[SFD_COND_AUDIO] && *clock == SFD_CLOCK_AUDIO) {
        SFSET_SetCond(handle, SFD_COND_CLOCK, SFD_CLOCK_VSYNC);
    }
    if (!conditions[SFD_COND_VIDEO] && *clock == SFD_CLOCK_VSYNC) {
        SFSET_SetCond(handle, SFD_COND_CLOCK, SFD_CLOCK_AUDIO);
    }
}
static void sfply_AdjustEtrg(struct sfd_handle *handle)
{
    Sint32 *conditions;
    Sint32 streams;
    Sint32 trigger;
    conditions = handle->conditions;
    streams = 0;
    if (conditions[SFD_COND_AUDIO] == TRUE) {
        streams = SFD_END_AUDIO;
    }
    if (conditions[SFD_COND_VIDEO] == TRUE) {
        streams |= SFD_END_VIDEO;
    }
    switch (streams) {
    case SFD_END_AUDIO:
        trigger = SFD_END_AUDIO;
        break;
    case SFD_END_VIDEO:
        trigger = SFD_END_VIDEO;
        break;
    case SFD_END_AUDIO | SFD_END_VIDEO:
        trigger = SFSET_GetCond(handle, SFD_COND_END_TRIGGER);
        break;
    default:
        trigger = SFD_END_EITHER;
        break;
    }
    SFSET_SetCond(handle, SFD_COND_END_TRIGGER, trigger);
}

static Sint32 sfply_StatStby(struct sfd_handle *handle)
{
    Sint32 status;
    Sint32 request;
    status = handle->status;
    request = handle->request;
    switch (request) {
    case SFD_PREPARING:
        status = SFD_PREPARING;
        break;
    case SFD_STANDBY:
        status = SFD_STANDBY;
        break;
    case SFD_PLAYING:
    case SFD_FINISHED:
        if (sfply_IsStartSync(handle)) {
            sfply_TrStart(handle);
            status = SFD_PLAYING;
        }
        break;
    }
    return status;
}
static Sint32 sfply_StatPlay(struct sfd_handle *handle)
{
    Sint32 status;
    Sint32 request;
    if (sfply_ChkFin(handle) != 0) {
        return handle->status;
    }
    status = handle->status;
    request = handle->request;
    switch (request) {
    case SFD_PREPARING:
    case SFD_STANDBY:
    case SFD_PLAYING:
        break;
    case SFD_FINISHED:
        status = SFD_FINISHED;
        break;
    }
    return status;
}
static Sint32 sfply_StatFin(struct sfd_handle *handle)
{
    return handle->status;
}

/* whether a prepared handle may start playing. one that plays video under
 * SFD_COND_DISPLAY_ALWAYS waits until the application shows a picture. it
 * starts anyway SFD_COND_DRAW_DELAY after its first picture is asked about,
 * or once its end trigger holds */
static Bool sfply_IsStartSync(struct sfd_handle *handle)
{
    Sint32 *conditions;
    struct sfd_timer *timer;
    conditions = handle->conditions;
    timer = &handle->timer;
    if (!conditions[SFD_COND_DISPLAY_ALWAYS]) {
        return TRUE;
    }
    if (!conditions[SFD_COND_VIDEO]) {
        return TRUE;
    }
    if (timer->display_count != 0) {
        return TRUE;
    }
    if (timer->vblank_time >= conditions[SFD_COND_DRAW_DELAY]) {
        return TRUE;
    }
    if (sfply_IsEtrg(handle)) {
        return TRUE;
    }
    return FALSE;
}
static Sint32 sfply_ChkFin(struct sfd_handle *handle)
{
    if (sfply_IsEtime(handle) || sfply_IsEtrg(handle)) {
        return sfply_Fin(handle);
    }
    return 0;
}
static Bool sfply_IsEtime(struct sfd_handle *handle)
{
    Sint32 end_time;
    Sint32 end_unit;
    Sint32 time;
    Sint32 unit;
    end_time = handle->conditions[SFD_COND_END_TIME];
    end_unit = handle->conditions[SFD_COND_END_UNIT];
    if (end_time == SFD_NO_END_TIME) {
        return FALSE;
    }
    SFTIM_GetTime(handle, &time, &unit);
    if (time < 0) {
        return FALSE;
    }
    if (UTY_CmpTime(time, unit, end_time, end_unit)) {
        return FALSE;
    }
    return TRUE;
}
static Bool sfply_IsEtrg(struct sfd_handle *handle)
{
    Sint32 *conditions;
    Bool finished;
    Bool video;
    Bool audio;
    Sint32 trigger;
    conditions = handle->conditions;
    if (!conditions[SFD_COND_AUDIO] && !conditions[SFD_COND_VIDEO]) {
        return TRUE;
    }
    finished = FALSE;
    video = SFTRN_GetTermFlg(handle, SFD_VIDEO_OUT);
    audio = SFTRN_GetTermFlg(handle, SFD_AUDIO_OUT);
    trigger = SFSET_GetCond(handle, SFD_COND_END_TRIGGER);
    switch (trigger) {
    case SFD_END_AUDIO:
        finished = audio;
        break;
    case SFD_END_VIDEO:
        finished = video;
        break;
    case SFD_END_EITHER:
        finished = video | audio;
        break;
    case SFD_END_BOTH:
        finished = video & audio;
        break;
    }
    return finished;
}
static Sint32 sfply_Fin(struct sfd_handle *handle)
{
    Sint32 error;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((error = sfply_TrStop(handle)) != 0) {
        return error;
    }
    handle->request = SFD_FINISHED;
    return 0;
}

Sint32 SFPLY_DecideSvrStat(void)
{
    Sint32 mask;
    Sint32 totals[SFD_SERVER_FINISHED + 1];
    Sint32 i;
    Sint32 category;
    Sint32 result;
    Sint32 status;
    Sint32 count;
    struct sfd_handle *handles;
    SFLIB_LockCs(&mask);
    totals[SFD_SERVER_IDLE] = 0;
    totals[SFD_SERVER_BUSY] = 0;
    totals[SFD_SERVER_FINISHED] = 0;
    count = SFLIB_libwork.handle_count;
    handles = SFLIB_libwork.handles;
    for (i = 0; i < count; i++) {
        status = handles[i].status;
        /* @hack spelling: the category chosen by nested conditional
         * expressions, where an if chain would do */
        /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
        category = status == SFD_FREE ? SFD_SERVER_IDLE
                   : (status == SFD_FINISHED || status < 0)
                       ? SFD_SERVER_FINISHED
                       : SFD_SERVER_BUSY;
        /* NOLINTEND(readability-avoid-nested-conditional-operator) */
        totals[category]++;
    }
    if (totals[SFD_SERVER_BUSY] != 0) {
        result = SFD_SERVER_BUSY;
    } else if (totals[SFD_SERVER_FINISHED] != 0) {
        result = SFD_SERVER_FINISHED;
    } else {
        result = SFD_SERVER_IDLE;
    }
    SFLIB_libwork.server_status = result;
    SFLIB_UnlockCs(&mask);
    return result;
}

struct sfd_handle *SFD_Create(struct sfd_create_params *params, void *work)
{
    struct sfd_handle *handle;
    if (sfply_ChkCrePara(params) != 0) {
        SFLIB_SetErr(NULL, INVALID_PARAMS);
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((handle = sfply_SearchFreeHn()) == NULL) {
        return NULL;
    }
    handle = sfply_InitHn(handle, params, work);
    return handle;
}

static Sint32 sfply_ChkCrePara(const struct sfd_create_params *params)
{
    if (params->work == NULL) {
        return -1;
    }
    return 0;
}
static struct sfd_handle *sfply_SearchFreeHn(void)
{
    Sint32 i;
    struct sfd_handle *handle;
    Sint32 count;
    struct sfd_handle *handles;
    count = SFLIB_libwork.handle_count;
    handles = SFLIB_libwork.handles;
    for (i = 0; i < count; i++) {
        handle = &handles[i];
        if (handle->status == SFD_FREE) {
            return handle;
        }
    }
    return NULL;
}
static struct sfd_handle *sfply_InitHn(struct sfd_handle *handle,
                                       struct sfd_create_params *params,
                                       void *work)
{
    UTY_MemsetDword((Uint32 *)handle, 0, sizeof(*handle) / sizeof(Uint32));
    params->work = (char *)(((Sint32)params->work + SFD_ALIGNMENT - 1) &
                            ~(SFD_ALIGNMENT - 1));
    handle->params = *params;
    handle->work = work;
    handle->pause = 0;
    SFHDS_InitFhd(&handle->header);
    sfply_InitMvInf(&handle->movie);
    sfply_InitPlyInf(&handle->play);
    SFLIB_InitErrInf(&handle->error);
    UTY_MemcpyDword((Uint32 *)handle->conditions,
                    (const Uint32 *)SFLIB_libwork.conditions, SFD_CONDITIONS);
    SFTIM_InitHn(handle, &handle->timer);
    SFBUF_InitHn(handle, &handle->buffers, params);
    SFTRN_InitHn(handle, handle->stages, params, work);
    SFSEE_InitHn(&handle->seek);
    if (sfply_TrCreate(handle) != 0) {
        handle = NULL;
    }
    if (handle != NULL) {
        handle->request = SFD_STOPPED;
        handle->status = SFD_STOPPED;
    }
    return handle;
}
/* @hack redundant: the fields cleared again, through Uint32 lvalues, after
 * UTY_MemsetDword cleared the whole record */
#define CLEAR_WORD(field) (*(Uint32 *)&(field) = 0)

static void sfply_InitMvInf(struct sfd_movie_info *movie)
{
    UTY_MemsetDword((Uint32 *)movie, 0, sizeof(*movie) / sizeof(Uint32));
    CLEAR_WORD(movie->width);
    CLEAR_WORD(movie->height);
    CLEAR_WORD(movie->mb_width);
    CLEAR_WORD(movie->mb_height);
    CLEAR_WORD(movie->bit_rate);
    CLEAR_WORD(movie->picture_rate);
    CLEAR_WORD(movie->mux_rate);
    movie->fixed = TRUE;
}
static void sfply_InitPlyInf(struct sfd_play_info *play)
{
    UTY_MemsetDword((Uint32 *)play, 0, sizeof(*play) / sizeof(Uint32));
    CLEAR_WORD(play->decoded);
    CLEAR_WORD(play->skipped);
    CLEAR_WORD(play->dct_intra);
    CLEAR_WORD(play->dct_inter);
    CLEAR_WORD(play->empty_pictures);
}
/* define function, which adds count to counter, a field of the handle's
 * play information. it then passes coding_type and the play information to
 * the function condition holds, if any */
#define COUNT_PICTURES(function, counter, condition)                           \
    void function(struct sfd_handle *handle, Sint32 count, Sint32 coding_type) \
    {                                                                          \
        struct sfd_play_info *play;                                            \
        picture_callback notify;                                               \
        play = &handle->play;                                                  \
        play->counter += count;                                                \
        notify = (picture_callback)SFSET_GetCond(handle, condition);           \
        if (notify != NULL) {                                                  \
            notify(handle, coding_type, play);                                 \
        }                                                                      \
    }
COUNT_PICTURES(SFPLY_AddDecPic, decoded, SFD_COND_DECODE_CALLBACK)
COUNT_PICTURES(SFPLY_AddSkipPic, skipped, SFD_COND_SKIP_CALLBACK)

static Sint32 sfply_TrCreate(struct sfd_handle *handle)
{
    return SFTRN_CallTrSetup(handle, SFD_OP_CREATE);
}
Sint32 SFD_Destroy(struct sfd_handle *handle)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_DESTROY);
    }
    SFHDS_FinishFhd(&handle->header);
    return sfply_TrDestroy(handle);
}
static Sint32 sfply_TrDestroy(struct sfd_handle *handle)
{
    Sint32 error;
    error = SFTRN_CallTrSetup(handle, SFD_OP_DESTROY);
    handle->status = SFD_FREE;
    handle->request = SFD_FREE;
    return error;
}
Sint32 SFD_Start(struct sfd_handle *handle)
{
    Sint32 error;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_START);
    }
    /* @hack spelling: each call's result held in error, where the return
     * could take it directly */
    if (SFSET_GetCond(handle, SFD_COND_PLAY_MODE) == SFD_PLAY_HEADERS) {
        error = SFPL2_Standby(handle);
    } else {
        error = sfply_Start(handle);
    }
    return error;
}
static Sint32 sfply_Start(struct sfd_handle *handle)
{
    handle->request = SFD_PLAYING;
    return 0;
}
static Sint32 sfply_TrStart(struct sfd_handle *handle)
{
    Sint32 error;
    /* @hack spelling: a test of the call's result that returns it or 0,
     * where returning it would do */
    if ((error = SFTRN_CallTrtTrif(handle, SFD_AUDIO_OUT, SFD_OP_START, 0,
                                   0)) != 0) {
        return error;
    }
    return 0;
}
Sint32 SFD_Stop(struct sfd_handle *handle)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_STOP);
    }
    return SFPLY_Stop(handle);
}
Sint32 SFPLY_Stop(struct sfd_handle *handle)
{
    Sint32 error;
    if (handle->status != SFD_STOPPED) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((error = sfply_TrStop(handle)) != 0) {
            return error;
        }
    }
    return 0;
}
static Sint32 sfply_TrStop(struct sfd_handle *handle)
{
    Sint32 error;
    if (handle->status == SFD_PLAYING) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((error = SFTRN_CallTrtTrif(handle, SFD_AUDIO_OUT, SFD_OP_STOP, 0,
                                       0)) != 0) {
            return error;
        }
    }
    handle->status = SFD_STOPPED;
    handle->request = SFD_STOPPED;
    return 0;
}

/* reset the handle to stopped, as SFD_Create left it, keeping its
 * conditions */
Sint32 sfply_ResetHn(struct sfd_handle *handle)
{
    Sint32 length;
    struct sfd_create_params params;
    struct sfd_ring_info ring;
    Sint32 conditions[SFD_CONDITIONS];
    void *work;
    Bool refeed;
    Sint32 error;
#ifdef NONMATCHING
    /* GCC can't see that length and refeed are set wherever they are read.
     * the second test of work and refeed reads them only where the first set
     * them */
    length = 0;
    refeed = FALSE;
#endif
    params = handle->params;
    work = handle->work;
    if (work == NULL) {
        refeed = handle->conditions[SFD_COND_REFEED];
        if (refeed) {
            SFD_GetWritePtr(handle, &ring);
            length = ring.written;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((error = sfply_TrDestroy(handle)) != 0) {
        return error;
    }
    UTY_MemcpyDword((Uint32 *)conditions, (const Uint32 *)handle->conditions,
                    SFD_CONDITIONS);
    handle = sfply_InitHn(handle, &params, work);
    if (handle == NULL) {
        return SFLIB_SetErr(NULL, RESET_FAILED);
    }
    UTY_MemcpyDword((Uint32 *)handle->conditions, (const Uint32 *)conditions,
                    SFD_CONDITIONS);
    if (work == NULL && refeed) {
        error = SFD_GetWritePtr(handle, &ring);
        if (error != 0) {
            return error;
        }
        error = SFD_AddWritePtr(handle, length, ring.written);
        if (error != 0) {
            return error;
        }
        SFD_TermSupply(handle);
    }
    return 0;
}

Sint32 SFD_GetWritePtr(struct sfd_handle *handle, struct sfd_ring_info *out)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_WRITE);
    }
    return SFTRN_CallTrtTrif(handle, SFD_INPUT, SFD_OP_GET_WRITE, (Sint32)out,
                             0);
}
Sint32 SFD_AddWritePtr(struct sfd_handle *handle, Sint32 bytes, Sint32 written)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SUPPLY);
    }
    return SFTRN_CallTrtTrif(handle, SFD_INPUT, SFD_OP_ADD_WRITE, bytes,
                             written);
}
Sint32 SFD_TermSupply(struct sfd_handle *handle)
{
    Sint32 buffer;
    struct sfd_transfer *input;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SUPPLY);
    }
    input = &handle->stages[SFD_INPUT];
    buffer = input->outputs[SFD_OUTPUT];
    SFBUF_SetTermFlg(handle, buffer, TRUE);
    return 0;
}
Sint32 SFD_GetFrm(struct sfd_handle *handle, struct sfd_frame_info **out)
{
    *out = NULL;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_FRAME);
    }
    return SFTRN_CallTrtTrif(handle, SFD_VIDEO_OUT, SFD_OP_GET_READ,
                             (Sint32)out, 0);
}
Sint32 SFD_RelFrm(struct sfd_handle *handle, struct sfd_frame_info *frame)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_RELEASE);
    }
    return SFTRN_CallTrtTrif(handle, SFD_VIDEO_OUT, SFD_OP_ADD_READ,
                             (Sint32)frame, 0);
}
