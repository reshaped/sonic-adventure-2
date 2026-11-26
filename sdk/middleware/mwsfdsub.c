/* the player's Sofdec decoders (mwsfd.h): the layout of their work in the
 * caller's, and their conditions. mwPlySfdSjCopyToSfd copies a player's
 * stream into its decoder */
#include <mwsfd.h>
#include <sfd.h>
#include <sfd_player.h>
#include <sfdfx_mw.h>
#ifdef NONMATCHING
/* this side takes sg_mw.h's variadic prototype of mwMngError, by which a call
 * below repairs the bug tagged there */
#include <sg_mw.h>
#endif
#include <sg_mwply.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <sofdec.h>
#include <string.h>

/* the decoders SFD_Init makes room for */
#define DECODERS 4
#define ERRORS 16
#define BYTE_BITS 8
/* a picture's planes of chroma, and the pictures a decoder predicts from */
#define CHROMA_PLANES 2
#define REFERENCES 2
/* a chroma plane's width and height, for a picture's. a chroma row is whole
 * cache lines */
#define CHROMA_WIDTH(width)                                                    \
    ((((width) / 2) + MWSFD_CACHE_LINE - 1) / MWSFD_CACHE_LINE *               \
     MWSFD_CACHE_LINE)
#define CHROMA_HEIGHT(height) ((height) / 2)
/* a system stream's own buffer, and its audio buffer, which its ADX ring
 * matches. its video buffer is the stream less SYSTEM_BYTES */
#define SYSTEM_BYTES 0x10000
#define AUDIO_BYTES 0x12000
/* ADX_WORK is the ADX work a decoder is given. ADX_WORK_BYTES, the room laid
 * out for it, is a cache line more */
#define ADX_WORK 0x80a0
#define ADX_WORK_BYTES (ADX_WORK + MWSFD_CACHE_LINE)
/* the channels and the rate in hertz of a decoder's ADX audio */
#define AUDIO_CHANNELS 2
#define AUDIO_RATE 44100
/* the picture and the video buffer the records below give a decoder.
 * mwPlySfdCreate sets the decoder's own over them */
#define DEFAULT_WIDTH 352
#define DEFAULT_HEIGHT 480
#define DEFAULT_FRAMES 4
#define DEFAULT_VIDEO_BYTES 0x32000
/* the pictures decoded before a decoder's output is prepared */
#define PREP_FRAMES 4
/* a vertical blank, in the units of the delay mwPlySfdSetCond sets */
#define VSYNC_TICKS 1000

static struct sfd_handle handles[DECODERS];
/* the stream's ring in the last work mwPlySfdCreate laid out, which
 * mwPlySfdCreateSj makes a joint of */
static char *stream_area;
/* the codes Sofdec's errors report, in order. each code after the first
 * ERRORS - 1 overwrites the last entry */
static Sint32 errors[ERRORS];
static Sint32 error_count;
/* the sizes of the parts of the last work mwPlySfdCreate laid out */
static Sint32 stream_size;
static Sint32 system_size;
static Sint32 video_size;
static Sint32 audio_size;
static Sint32 reference_size;
static Sint32 output_size;
static Sint32 adx_work_size;
static Sint32 adx_ring_size;
static Sint32 alignment_size;
static Sint32 buffer_size;

static const struct sfd_stage_list system_stages = {
    {&SFD_tr_in_mem, &SFD_tr_sd_mps, &SFD_tr_vd_mpv, &SFD_tr_vo_manu,
     &SFD_tr_ad_adxt, &SFD_tr_ao_auto_p}};
static const struct sfd_stage_list video_stages = {
    {&SFD_tr_in_mem, &SFD_tr_vd_mpv, &SFD_tr_vo_manu}};
static const struct sfd_init_params system_init = {
    &system_stages, handles, DECODERS, MWSFD_NTSC_MILLIHZ};
static const struct sfd_init_params video_init = {&video_stages, handles,
                                                  DECODERS, MWSFD_NTSC_MILLIHZ};
static const struct sfd_setup system_setup = {
    {&SFD_tr_in_mem, &SFD_tr_sd_mps, &SFD_tr_vd_mpv, &SFD_tr_ad_adxt, NULL,
     NULL, &SFD_tr_vo_manu, &SFD_tr_ao_auto_p, NULL}};
static const struct sfd_setup video_setup = {{&SFD_tr_in_mem, NULL,
                                              &SFD_tr_vd_mpv, NULL, NULL, NULL,
                                              &SFD_tr_vo_manu, NULL, NULL}};

/* the records mwPlySfdCreate completes for each decoder it creates. it sets
 * every field of video, and ring and work of audio. of the create records,
 * it keeps setup, read_unit and serial and the sizes past SFD_AUDIO_BYTES */
/* clang-format off */
#define CREATE_PARAMS(setup)                                                   \
    {setup, NULL, {SYSTEM_BYTES, DEFAULT_VIDEO_BYTES, AUDIO_BYTES},            \
     GDD_FS_SCTSIZE, DEFAULT_FRAMES, DEFAULT_WIDTH, DEFAULT_HEIGHT, 1}
/* clang-format on */
static struct sfd_create_params system_params = CREATE_PARAMS(&system_setup);
static struct sfd_create_params video_params = CREATE_PARAMS(&video_setup);
/* clang-format off */
static struct sfd_video_params video = {
    CHROMA_WIDTH(DEFAULT_WIDTH), CHROMA_HEIGHT(DEFAULT_HEIGHT),
    DEFAULT_WIDTH, DEFAULT_HEIGHT, NULL,
    DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_FRAMES, NULL};
/* clang-format on */
static struct sfd_audio_params audio = {
    AUDIO_BYTES, 0, NULL, AUDIO_CHANNELS, AUDIO_RATE, ADX_WORK, NULL};
static Sint32 mwply_work_size = 0;

static void mwPlySfdErrFunc(void *user, Sint32 code);

void mwPlySfdCalcWorkStmBuf(Sint32 type, Sint32 bps, Sint32 *stream,
                            Sint32 *system, Sint32 *picture, Sint32 *sound,
                            Sint32 *adx_ring, Sint32 *adx_work)
{
    if (type == MWD_PLY_FTYPE_MPV) {
        *stream = bps / BYTE_BITS;
        *system = 0;
        *picture = bps / BYTE_BITS;
        *sound = 0;
        *adx_ring = 0;
    } else {
        *stream = ((bps / BYTE_BITS) + GDD_FS_SCTSIZE) / GDD_FS_SCTSIZE *
                  GDD_FS_SCTSIZE;
        *system = SYSTEM_BYTES;
        *picture = *stream - SYSTEM_BYTES;
        *sound = AUDIO_BYTES;
        *adx_ring = AUDIO_BYTES;
    }
    *adx_work = ADX_WORK_BYTES;
}
static void mwPlySfdCalcFrmRes(Sint32 width, Sint32 height, Sint32 *luma_width,
                               Sint32 *luma_height, Sint32 *chroma_width,
                               Sint32 *chroma_height)
{
    *chroma_width = CHROMA_WIDTH(width);
    *chroma_height = CHROMA_HEIGHT(height);
    *luma_width = width;
    *luma_height = height;
}
static void mwPlySfdCalcWorkFrmBuf(Sint32 width, Sint32 height, Sint32 frames,
                                   Sint32 *references, Sint32 *pool)
{
    Sint32 luma_width;
    Sint32 luma_height;
    Sint32 chroma_width;
    Sint32 chroma_height;
    mwPlySfdCalcFrmRes(width, height, &luma_width, &luma_height, &chroma_width,
                       &chroma_height);
    *references = (((luma_width * luma_height) +
                    (chroma_width * chroma_height * CHROMA_PLANES)) *
                   REFERENCES) +
                  MWSFD_CACHE_LINE;
    *pool = (frames * (luma_width * luma_height * 3 / 2)) + MWSFD_CACHE_LINE;
}
Sint32 mwPlyCalcWorkSofdec(Sint32 type, Sint32 bps, Sint32 width, Sint32 height,
                           Sint32 frames)
{
    Sint32 stream;
    Sint32 system;
    Sint32 picture;
    Sint32 sound;
    Sint32 references;
    Sint32 pool;
    Sint32 adx_ring;
    Sint32 adx_work;
    mwPlySfdCalcWorkStmBuf(type, bps, &stream, &system, &picture, &sound,
                           &adx_ring, &adx_work);
    mwPlySfdCalcWorkFrmBuf(width, height, frames, &references, &pool);
    return stream + system + picture + sound + references + pool + adx_ring +
           adx_work + MWSFD_CACHE_LINE;
}
Sint32 mwPlyCalcWorkCompo(MWS_PLY_CPRM_SFD *params)
{
    Sint32 side;
    Sint32 shift;
    if (params->compo_mode >= MWD_PLY_COMPO_LUMI) {
        side = params->max_width > params->max_height ? params->max_width
                                                      : params->max_height;
        if (side > 0) {
            side--;
            for (shift = 0; side != 0; shift++) {
                side >>= 1;
            }
            side = 1 << shift;
        } else {
            side = 0;
        }
        return (side * side) + MWSFD_CACHE_LINE;
    }
    return 0;
}
Sint32 mwPlyCalcWorkSfd(MWS_PLY_CPRM_SFD *params)
{
    Sint32 height;
    if (params->compo_mode >= MWD_PLY_COMPO_ALPH256) {
        height = params->max_height * 2;
    } else {
        height = params->max_height;
    }
    return mwPlyCalcWorkSofdec(params->ftype, params->max_bps,
                               params->max_width, height, params->nfrm_pool_wk);
}
Sint32 mwPlyCalcWorkCprmSfd(MWS_PLY_CPRM_SFD *parameters)
{
    Sint32 size = mwPlyCalcWorkSfd(parameters);
    size += mwPlyCalcWorkCompo(parameters);
    return size;
}
Sint32 mwPlySfdInit(struct mwsfd_player *player, Sint32 type, Sint32 time_unit)
{
    struct sfd_init_params params;
    Sint32 error;
    switch (type) {
    case MWD_PLY_FTYPE_SFD:
        params = system_init;
        break;
    case MWD_PLY_FTYPE_MPV:
        params = video_init;
        break;
    default:
        return mwSfdSetErrCode(MWSFD_ERR_FILE_TYPE);
    }
    params.time_unit = time_unit;
    error = SFD_Init(&params);
    if (error != 0) {
        return mwSfdSetErrCode(MWSFD_ERR_INIT);
    }
    error = SFD_SetErrFn(NULL, mwPlySfdErrFunc, NULL);
    if (error != 0) {
        return mwSfdSetErrCode(MWSFD_ERR_ERROR_FN);
    }
    return 0;
}
Sint32 mwPlySfdFinish(struct mwsfd_player *player)
{
    SFD_Finish();
    return 0;
}

struct sfd_handle *mwPlySfdCreate(struct mwsfd_player *player, Sint32 type,
                                  Sint32 bps, Sint32 width, Sint32 height,
                                  Sint32 frames, Sint8 *work, Sint32 size)
{
    Sint32 luma_width;
    Sint32 luma_height;
    Sint32 chroma_width;
    Sint32 chroma_height;
    Sint8 *output;
    Sint8 *references;
    Sint8 *stream;
    Sint8 *adx_work;
    Sint8 *adx_ring;
    struct sfd_create_params params;
    Sint8 *base;
    Sint8 *next;
    Sint8 *unaligned;
    struct sfd_handle *handle;
    Sint32 error;
    mwPlySfdCalcWorkStmBuf(type, bps, &stream_size, &system_size, &video_size,
                           &audio_size, &adx_ring_size, &adx_work_size);
    mwPlySfdCalcWorkFrmBuf(width, height, frames, &reference_size,
                           &output_size);
    buffer_size = system_size + video_size + audio_size;
    /* the work starts at its first cache line, which mwPlyCalcWorkSofdec
     * makes room for with a line more. the references, the output and the
     * ADX work each take a line more than they hold too */
    /* @hack spelling: the assignment to unaligned inside the rounding, where
     * a statement of its own would do */
    next = (Sint8 *)(((Uint32)(unaligned = work) + MWSFD_CACHE_LINE - 1) /
                     MWSFD_CACHE_LINE * MWSFD_CACHE_LINE);
    alignment_size = next - unaligned;
    base = next;
    next += buffer_size;
    stream = next;
    next += stream_size;
    references = next;
    next += reference_size;
    output = next;
    next += output_size;
    adx_ring = next;
    next += adx_ring_size;
    adx_work = next;
    next += adx_work_size;
    mwply_work_size = next - base + alignment_size;
    if (mwply_work_size > size) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: mwMngError is sg_mw.h's
         * variadic one, which reads the deficit where the call puts it */
#else
        /* @bug mismatched-call: prints a wrong deficit. the call at
         * 0x8c54497e passes it in a register, where mwMngError, which is
         * variadic, reads it from the stack at 0x8c106938 */
        extern void mwMngError(const char *format, Sint32 deficit);
#endif
        mwMngError("E2011: not enough work (%d byte)", mwply_work_size - size);
        return NULL;
    }
    mwPlySfdCalcFrmRes(width, height, &luma_width, &luma_height, &chroma_width,
                       &chroma_height);
    video.chroma_width = chroma_width;
    video.chroma_height = chroma_height;
    video.luma_width = luma_width;
    video.luma_height = luma_height;
    video.references = (Uint8 *)references;
    video.output_width = luma_width;
    video.output_height = luma_height;
    video.frames = frames;
    video.output = (Uint8 *)output;
    audio.ring = (char *)adx_ring;
    audio.work = (char *)adx_work;
    stream_area = (char *)stream;
    switch (type) {
    case MWD_PLY_FTYPE_SFD:
        params = system_params;
        break;
    case MWD_PLY_FTYPE_MPV:
        params = video_params;
        break;
        /* @bug uninitialized: a type other than MWD_PLY_FTYPE_SFD or
         * MWD_PLY_FTYPE_MPV leaves params as the stack held it, but for the
         * fields set below. the branch at 0x8c5449e8 passes over both copies,
         * and SFD_Create reads params at 0x8c544a88 */
#ifdef NONMATCHING
    /* this side repairs the read: another type fails with MWSFD_ERR_FILE_TYPE
     * before SFD_Create reads params */
    default:
        mwSfdSetErrCode(MWSFD_ERR_FILE_TYPE);
        return NULL;
#endif
    }
    params.work = (char *)base;
    params.sizes[SFD_SYSTEM_BYTES] = system_size;
    params.sizes[SFD_VIDEO_BYTES] = video_size;
    params.sizes[SFD_AUDIO_BYTES] = audio_size;
    params.frames = frames;
    params.width = width;
    params.height = height;
    SFD_SetMpvPara(&video);
    SFD_SetAdxtPara(&audio);
    handle = SFD_Create(&params, NULL);
    if (handle == NULL) {
        mwSfdSetErrCode(MWSFD_ERR_CREATE);
        return NULL;
    }
    error = SFD_SetErrFn(handle, mwPlySfdErrFunc, NULL);
    if (error != 0) {
        mwSfdSetErrCode(MWSFD_ERR_ERROR_FN);
        return NULL;
    }
    return handle;
}
void mwPlySfdDestroy(struct sfd_handle *handle)
{
    Sint32 error = SFD_Destroy(handle);
    if (error != 0) {
        mwSfdSetErrCode(MWSFD_ERR_DESTROY);
    }
}
void mwPlySfdSetCond(struct mwsfd_player *player, MWS_PLY_CPRM_SFD *params,
                     Sint32 vsyncs, Sint32 latency)
{
    Sint32 display_vsyncs;
    Sint32 display_latency;
    struct sfd_handle *handle;
    Float time;
    Sint32 ticks;
    Sint32 delay;
    /* @hack spelling: display_vsyncs and display_latency, copies of vsyncs
     * and latency, where those would do */
    display_vsyncs = vsyncs;
    display_latency = latency;
    handle = player->decoder;
    SFD_SetCond(handle, SFD_COND_REFEED, 0);
    SFD_SetCond(handle, 1, 1);
    SFD_SetCond(handle, 0, 0);
    SFD_SetCond(handle, SFD_COND_PREP_FRAMES, PREP_FRAMES);
    time = (Float)(display_vsyncs * display_latency * VSYNC_TICKS) + 0.5F;
    ticks = (Sint32)time;
    delay = (Float)ticks > time ? ticks - 1 : ticks;
    SFD_SetCond(handle, SFD_COND_DRAW_DELAY, delay);
    SFD_SetCond(handle, SFD_COND_DISPLAY_DELAY, delay);
    SFD_SetCond(handle, SFD_COND_DECODE_TIME, delay);
    SFD_SetCond(handle, SFD_COND_DISPLAY_ALWAYS, 1);
    SFD_SetMpvCond(handle, MPV_COND_LUMA_OFFSET, 0);
}
#define PLAYER_CALL(name, operation, failure)                                  \
    Sint32 name(struct mwsfd_player *player)                                   \
    {                                                                          \
        struct sfd_handle *handle = player->decoder;                           \
        Sint32 error = operation(handle);                                      \
        if (error != 0) {                                                      \
            return mwSfdSetErrCode(failure);                                   \
        }                                                                      \
        return 0;                                                              \
    }
PLAYER_CALL(mwPlySfdStandby, SFD_Standby, MWSFD_ERR_STANDBY)
PLAYER_CALL(mwPlySfdStart, SFD_Start, MWSFD_ERR_START)
PLAYER_CALL(mwPlySfdStop, SFD_Stop, MWSFD_ERR_STOP)

void mwPlySfdGetWritePtr(struct mwsfd_player *player, char **data, Sint32 *size,
                         Sint32 *written)
{
    struct sfd_handle *handle = player->decoder;
    struct sfd_ring_info ring;
    SFD_GetWritePtr(handle, &ring);
    *data = ring.head.data;
    *size = ring.head.size;
    *written = ring.written;
}
void mwPlySfdAddWritePtr(struct mwsfd_player *player, Sint32 size,
                         Sint32 written)
{
    struct sfd_handle *handle = player->decoder;
    if (size >= 0) {
        SFD_AddWritePtr(handle, size, written);
    }
}
Bool mwPlySfdIsEndDec(struct mwsfd_player *player)
{
    Sint32 state = SFD_GetSvrStat();
    if (state != SFD_SERVER_BUSY) {
        return TRUE;
    }
    return FALSE;
}
static void mwPlySfdErrFunc(void *user, Sint32 code)
{
    if (code != 0) {
        errors[error_count] = code;
        if (error_count < ERRORS - 1) {
            error_count++;
        }
    }
}
SJ mwPlySfdCreateSj(void)
{
    return SJRBF_Create(stream_area, stream_size, 0);
}
void mwPlySfdSjCopyToSfd(struct mwsfd_player *player)
{
    struct sfd_handle *handle = player->decoder;
    SJ stream = player->stream;
    Sint32 size;
    char *data;
    Sint32 written;
    SJCK chunk;
    if (handle == NULL) {
        return;
    }
    mwPlySfdGetWritePtr(player, &data, &size, &written);
    size = size / MWSFD_CACHE_LINE * MWSFD_CACHE_LINE;
    if (size <= 0) {
        return;
    }
    SJ_GetChunk(stream, SJ_LIN_DATA, size, &chunk);
    mwPlySh4CpyMem(data, chunk.data, chunk.len);
    SJ_PutChunk(stream, SJ_LIN_FREE, &chunk);
    mwPlySfdAddWritePtr(player, chunk.len, written);
}
void mwPlySetAudioSw(MWPLY handle, Sint32 enabled)
{
    struct sfd_handle *decoder = ((struct mwsfd_player *)handle)->decoder;
    SFD_SetCond(decoder, SFD_COND_AUDIO, enabled);
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}
