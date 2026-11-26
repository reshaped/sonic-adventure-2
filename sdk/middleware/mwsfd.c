/* the Sofdec player (sfdfx_mw.h), behind the middleware's interface
 * (sg_mwply.h). mwSfdExecDecServer copies each player's stream into its
 * decoder. mwPlyExecTexSvr loads each decoded picture into the player's
 * renderer. a player made for a masked composition also has alpha_renderer.
 * mwPlyExecTexSvr loads it with a mask made from the picture, or with the
 * caller's matte. under MWD_PLY_COMPO_OPEQ or MWD_PLY_COMPO_TRNSP, it clears
 * the mask instead */
#include <adx_stmc.h>
#include <cri_lsc.h>
#include <machine.h>
#include <mw_mvf.h>
#include <mw_rna.h>
#include <mwsfd.h>
#include <ninja.h>
#include <sfd.h>
#include <sfdfx_mw.h>
#include <sg_mw.h>
#include <sg_mwgsc.h>
#include <sg_mwply.h>
#include <sg_mwrv2.h>
#include <sg_syfbr.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <sofdec.h>
#include <sofdec_yuv.h>
#include <string.h>

#define PLAYER(handle) ((struct mwsfd_player *)(handle))
/* @hack type: the player cast to MWPLY, where &(player)->base would do */
#define HANDLE(player) ((MWPLY)(player))
#define PALETTE_COLORS 256
/* the alpha renderer's palette: PALETTE_COLORS entries from
 * ALPHA_PALETTE_START, entry n alpha n over black. an ARGB8888 entry holds
 * its alpha from bit ALPHA_SHIFT. mwRnv2SetVertexContextPlt8 takes the
 * palette's start in banks of BANK_COLORS entries */
#define ALPHA_PALETTE_START 768
#define BANK_COLORS 16
#define ALPHA_SHIFT 24
#define ALPHA_DEPTH_BIAS 0.0001F
/* a new player's brightness and brightness offset, as the levels
 * mwRnv2SetWndBright and mwRnv2SetWndBrightOfst take */
#define DEFAULT_BRIGHTNESS 224
#define DEFAULT_BRIGHTNESS_OFFSET 6
/* a macroblock's width and height in pixels */
#define MACROBLOCK_SIDE 16
/* a step of mwSfdExecDecServer counts as slow from these microseconds on.
 * SLOW_SFD_SERVER is for Sofdec's server, and SLOW_PLAYERS for the players'
 * pass */
#define SLOW_SFD_SERVER 1000
#define SLOW_PLAYERS 50
/* mwg_disp_dura_sum's entries. its last counts every duration of
 * DURATIONS - 1 or more */
#define DURATIONS 16
/* the bufsize and rdsize mwSfdStartFname's ADXSTM_SetBufSize means to set */
#define STREAM_BUFSIZE 0x32000
#define STREAM_RDSIZE 0x7fffffff
/* the player's times are in tenths of a millisecond */
#define TENTHS_A_SECOND 10000.0F
#define MICROSECONDS_A_TENTH 100
/* the pictures mwSfdExecTexSvrHndl loads in one pass, at most */
#define LOADS_A_PASS 1
/* the bytes of pixels in the decoder's macroblocks, 1.5 a pixel */
#define MACROBLOCK_BYTES(pixels) ((pixels) / 2 * 3)

static void mwSfdStopDec(struct mwsfd_player *player);
static void mwSfdStop(MWPLY handle);
static void mwSfdStartFname(MWPLY handle, Sint8 *name);
static void mwSfdStartMem(MWPLY handle, void *data, Sint32 bytes);
static void mwSfdStartSj(MWPLY handle, SJ input);
static MWE_PLY_STAT mwSfdGetStat(MWPLY handle);
static void mwSfdGetTime(MWPLY handle, Sint32 *time, Sint32 *unit);
static void mwSfdPause(MWPLY handle, Sint32 paused);
static void mwSfdSetOutVol(MWPLY handle, Sint32 volume);
static Sint32 mwSfdGetOutVol(MWPLY handle);
static void mwSfdSetOutPan(MWPLY handle, Sint32 channel, Sint32 pan);
static Sint32 mwSfdGetOutPan(MWPLY handle, Sint32 channel);
static Sint32 mwl_ply_draw_alph_cbfn(void *user, Sint32 time, Sint32 unit,
                                     Sint32 frame);
static Sint32 mwl_ply_draw_cbfn(void *user, Sint32 time, Sint32 unit,
                                Sint32 frame);
static void mwl_ply_disp_cbfn(void *user, Sint32 time, Sint32 unit,
                              Sint32 frame);
static void mwlSfdSleepDecSvr(void);

/* the interface's vertical blank and server slots take a player. mwSfdVsync
 * and mwSfdExecServer take the manager's void *, which neither reads */
#ifdef NONMATCHING
/* this side calls each through a function of the slot's type: a call through
 * another function type is undefined in C */
static void player_vsync(MWPLY player)
{
    mwSfdVsync(player);
}
static void player_server(MWPLY player)
{
    mwSfdExecServer(player);
}
#define PLAYER_VSYNC player_vsync
#define PLAYER_SERVER player_server
#else
#define PLAYER_VSYNC ((void (*)(MWPLY))mwSfdVsync)
#define PLAYER_SERVER ((void (*)(MWPLY))mwSfdExecServer)
#endif

/* each message is an array of its own, in the image's order */
/* @hack type: each array bytes long, its text rounded up to a word, where
 * the text's own size would do */
#define MESSAGE(name, bytes, text) static const char name[bytes] = text
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
const char mwg_sfd_build[] = "\nmwSfd Ver.2.28 Build:Feb 25 2000 10:52:13\n";
MESSAGE(no_handles, 44, "W2000 mwPlyCreateSofdec:can't create handle");
MESSAGE(work_too_small, 48, "E9082601 mwPlyCreateSofdec:illegal work size");
MESSAGE(no_color_renderer, 48, "E2010 mwPlyCreate:can't create RNV for movie");
MESSAGE(no_alpha_renderer, 48, "E2011 mwPlyCreate:can't create RNV for alpha");
MESSAGE(no_decoder, 36, "E2012 mwPlyCreate:can't create SFD");
MESSAGE(no_stream_joint, 36, "E2013 mwPlyCreate:can't create SJ");
MESSAGE(no_memory_joint, 36, "E2020 mwPlyCreate:can't create SJ");
MESSAGE(start_link_failed, 44, "E99072103 mwPlyStartXX: can't link stream");
MESSAGE(open_failed, 40, "W2001 mwPlyStartFname:can't open '%s'");
MESSAGE(read_limit_failed, 44, "W2005 mwPlyStartFname: gsc parameter miss");
MESSAGE(transfer_limit_failed, 44, "W2006 mwPlyStartFname: gsc prameter miss");
MESSAGE(stop_failed, 32, "E2003 mwSfdStop:can't stop SFD");
MESSAGE(null_handle, 36, "W2004 mwSfdGetStat: handle is NULL");
MESSAGE(link_failed, 44, "E99072101 mwPlyLinkStm: can't link stream");
MESSAGE(end_failed, 48, "E99072102 mwlSfdExecDecSvrPlaying: can't term");
MESSAGE(no_mask_to_clear, 64,
        "E99072601 mwSfdExecTexSvrHndl: not exist composition work area");
MESSAGE(no_mask_to_load, 64,
        "E99072603 mwSfdExecTexSvrHndl: not exist composition work area");
MESSAGE(time_failed, 36, "E2006 mwSfdGetTime; can't get time");
MESSAGE(pause_failed, 36, "E2007 mwSfdPause; can't pause (%s)");
MESSAGE(pause_on, 4, "ON");
MESSAGE(pause_off, 4, "OFF");
MESSAGE(no_matte, 52, "E99072604 mwPlySetMat: not exist composition buffer");
MESSAGE(mode_unavailable, 56,
        "E99071201: mwPlySetCompoMode; can't set compo mode (%d)");

/* the member exports these names, the statics aside. mwsfd.h declares
 * those the player's other members use, and no header of the SDK the rest */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* with mwg_sfd_mthrd FALSE, mwPlyExecTexSvr and mwPlyExecDrawSvr each run the
 * manager's idle functions themselves, mwSfdExecDecServer among them.
 * mwg_thread_exec_flag is TRUE while they run */
Bool mwg_sfd_mthrd = TRUE;
/* clang-format off */
static MWD_IF mwsfd_if = {
    NULL, NULL, NULL, PLAYER_VSYNC, PLAYER_SERVER, mwSfdDestroy,
    mwSfdStartFname, mwSfdStop, mwSfdGetStat, mwSfdGetTime, mwSfdPause,
    mwSfdSetOutVol, mwSfdGetOutVol, mwSfdSetOutPan, mwSfdGetOutPan,
    mwSfdStartSj, mwSfdStartMem};
/* clang-format on */
/* the display mwPlySetDispMode last set, its four arguments in order.
 * mwg_nvsync_flip is the vertical blanks to a frame, doubled for an
 * interlaced mode. the rates are in millihertz */
Sint32 mwg_sys_mode = NJD_DSP_NTSC | NJD_DSP_INTER | NJD_DSP_640x480;
Sint32 mwg_sys_frame = NJD_FRAMEBUFFER_MODE_ARGB8888;
Sint32 mwg_sys_count = 1;
Sint32 mwg_sys_latency = 0;
Sint32 mwg_vsync_mhz = MWSFD_NTSC_MILLIHZ;
Sint32 mwg_nvsync_flip = 2;
Sint32 mwg_flip_rate = MWSFD_NTSC_MILLIHZ / 2;
Bool mwg_sfd_init_flag = FALSE;
Bool mwg_sleep_enable_flag = TRUE;
Bool mwg_thread_exec_flag = FALSE;
/* no code reads or sets mwg_thread_sleep_flag */
Bool mwg_thread_sleep_flag = FALSE;
/* the vertical blanks since mwSfdStartFrame */
Sint32 mwg_field_no = 0;
/* mwl_sfd_dmy's last decoder, which no code reads */
struct sfd_handle *mwg_sfd_hn = NULL;
/* the calls of mwSfdExecServer */
Sint32 mwg_sfd_exec_svr_cnt = 0;
/* a slow step of mwSfdExecDecServer keeps the timer's counts at its start
 * and end. it adds its microseconds to a total. Sofdec's server keeps them
 * in mw_t1a, mw_t1b and mw_dectime. the players' pass keeps them in mw_t2a,
 * mw_t2b and mw_splttime */
Uint32 mw_t1a = 0, mw_t1b = 0;
Sint32 mw_dectime = 0;
Uint32 mw_t2a = 0, mw_t2b = 0;
Sint32 mw_splttime = 0;
/* nothing writes mw_t3a to mw_t5b, mw_gdfstime, mw_gsctime or mw_gdctime */
Uint32 mw_t3a = 0, mw_t3b = 0;
Sint32 mw_gdfstime = 0;
Uint32 mw_t4a = 0, mw_t4b = 0;
Sint32 mw_gsctime = 0;
Uint32 mw_t5a = 0, mw_t5b = 0;
Sint32 mw_gdctime = 0;
/* mwl_sfd_wait_vsync's timer counts. mw_t6b is the latest wait's entry, and
 * mw_t6a the one before. mw_t6c is when the fiber switches back */
Uint32 mw_t6a = 0, mw_t6b = 0, mw_t6c = 0;
/* mwSfdVsync keeps the timer's count at a vertical blank in mw_t7c and sets
 * vsync_flag. the first wait after the blank copies that count to mw_t7a. it
 * puts its own in mw_t7b and clears the flag */
Uint32 mw_t7a = 0, mw_t7b = 0, mw_t7c = 0;
/* the timer's counts at the start and end of mwSfdExecDecServer's last pass.
 * mw_t8c holds the start while a pass runs */
Uint32 mw_t8a = 0, mw_t8b = 0, mw_t8c = 0;
Bool vsync_flag = FALSE;
/* times in tenths of a millisecond. mw_start_time is the timer's time when
 * the display callback hears of picture 0. mw_dif_av_time2 is mw_atime_ms2
 * less mw_vtime_ms2, mw_dif_rv_time2 is mw_rtime_ms2 less mw_vtime_ms2, and
 * mw_dif_ra_time2 is mw_rtime_ms2 less mw_atime_ms2 */
Sint32 mw_start_time = 0;
Sint32 mw_dif_av_time2 = 0, mw_dif_rv_time2 = 0, mw_dif_ra_time2 = 0;
/* mwg_last_draw_dfno is the last picture mwl_ply_draw_cbfn was asked of.
 * mwg_last_disp_dfno is the last the display callback heard of */
Sint32 mwg_last_draw_dfno = 0, mwg_last_disp_dfno = 0;
/* with mwg_sfd_dstalph TRUE, a masked composition blends the picture by the
 * alpha of what it is drawn over */
Bool mwg_sfd_dstalph = FALSE;
/* the last picture the display callback heard of. discontinuities counts
 * each picture after picture 0 whose number isn't one past the last's */
static Sint32 previous_display_frame = 0;
static Sint32 discontinuities = 0;
struct mwsfd_library mwg_ply_work;
volatile Sint32 mwg_vcnt;
volatile Bool *mwg_eor_flag_ptr;
/* a fiber table, two fibers and a stack, which no code of the tree sets up.
 * mwl_sfd_wait_vsync, which no code calls, switches to g_fbrSub */
SYS_FBR_INFO g_fbrWork[5];
SYFBR g_fbrSub;
SYFBR g_fbrMain;
Uint32 g_subStack[4096];
/* times at the last display callback, in tenths of a millisecond.
 * mw_atime_ms2 is the player's as mwSfdGetTime gives it, and mw_vtime_ms2 the
 * picture's. mw_rtime_ms2 is the real time since mw_start_time */
Sint32 mw_atime_ms2, mw_vtime_ms2, mw_rtime_ms2;
/* mwg_vcnt at the last display callback, and the time of the picture it
 * heard of. a picture with a new time sets mwg_disp_dura to the vertical
 * blanks since the callback before. mwg_disp_dura_sum[d] counts the
 * durations of d */
Sint32 mwg_disp_vcnt, mwg_disp_vtime, mwg_disp_dura;
Sint32 mwg_disp_dura_sum[DURATIONS];
static Uint32 mwsfd_plttbl[PALETTE_COLORS];
static Sint8 alpha_lut[PALETTE_COLORS];
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 mwPlyLock(void)
{
    Sint32 mask = get_imask();
    set_imask(IMASK_ALL);
    return mask;
}

void mwPlyUnlock(Sint32 mask)
{
    set_imask(mask);
}

void mwPlySetVertexBuffer(void *buffer)
{
    if (buffer != NULL) {
        mwRnv2SetVertexBuffer(buffer);
    }
}

Sint32 mwPlyCalcCompoBufSize(Sint32 width, Sint32 height)
{
    Sint32 largest = width > height ? width : height;
    /* @hack spelling: height and width reused for the halved side and its
     * power of two, where locals of their own would do */
    height = largest;
    if (height == 0) {
        return 0;
    }
    width = 1;
    while (height > 1) {
        height >>= 1;
        width <<= 1;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: a side that isn't a power of two
     * takes the power above it */
    if (width < largest) {
        width <<= 1;
    }
#else
    /* @bug logic: sizes a side that isn't a power of two by the power of two
     * below it, not the one above. the test at 0x8c54288e and 0x8c542890 asks
     * whether the power exceeds the side. it never does */
    if (width - largest > 0) {
        width <<= 1;
    }
#endif
    return (width * width) + MWSFD_CACHE_LINE;
}

void mwPlySetDispMode(Sint32 mode, Sint32 frame, Sint32 count, Sint32 latency)
{
    mwg_sys_mode = mode;
    mwg_sys_frame = frame;
    mwg_sys_count = count;
    mwg_sys_latency = latency;
    if (mode & NJD_DSP_INTER) {
        mwg_nvsync_flip = count << 1;
    } else {
        mwg_nvsync_flip = count;
    }
    if (mode & NJD_DSP_PAL) {
        mwg_vsync_mhz = MWSFD_PAL_MILLIHZ;
    } else {
        mwg_vsync_mhz = MWSFD_NTSC_MILLIHZ;
    }
    mwg_flip_rate = mwg_vsync_mhz / mwg_nvsync_flip;
    if (latency == 2) {
        mwRnv2SetLatency(2);
    } else if (latency == 3) {
        mwRnv2SetLatency(3);
    }
    if (latency == 1) {
        mwRnv2SetLatency(3);
    }
}

#define CREATE_RENDERER(function, texture_format)                              \
    static MWRNV2 function(Sint32 width, Sint32 height, char *work,            \
                           Sint32 bytes)                                       \
    {                                                                          \
        MWS_RNV2_CPRM params;                                                  \
        params.format = texture_format;                                        \
        params.width = width;                                                  \
        params.height = height;                                                \
        if (mwg_sys_latency == 1) {                                            \
            params.frame_count = 3;                                            \
        } else {                                                               \
            params.frame_count = 2;                                            \
        }                                                                      \
        params.picture_count = 0;                                              \
        params.work_size = bytes;                                              \
        params.work = work;                                                    \
        return mwRnv2Create(NULL, &params);                                    \
    }
CREATE_RENDERER(mw_sfd_create_rnvmv, MVF_FMT_YUV420)
CREATE_RENDERER(mw_sfd_create_rnvalph, MVF_FMT_PLT8)

static void mwsfd_load_plttbl(void)
{
    Sint32 i;
    Uint32 alpha;
    for (i = 0; i < PALETTE_COLORS; ++i) {
        alpha = i;
        mwsfd_plttbl[i] = alpha << ALPHA_SHIFT;
    }
    syCacheOCBP(mwsfd_plttbl, sizeof(mwsfd_plttbl));
    mwRnv2LoadPltARGB8888(NULL, (Uint8 *)mwsfd_plttbl, ALPHA_PALETTE_START,
                          PALETTE_COLORS);
}
static void mwsfd_load_plttbl_clear_msk(struct mwsfd_player *player)
{
    Sint32 i;
    for (i = 0; i < player->composition_size; ++i) {
        player->composition_buffer[i] = 0;
    }
    mwsfd_load_plttbl();
}

/* alpha holds 256 levels from 0 to 1, which become the converter's table
 * under LUMI. under ALPH3, bank chooses the part of the table they fill. bank
 * 0 is entries 0 to 7, bank 1 entries 8 to 135, and bank 2 the rest */
void mwPlySetAlphTbl(MWPLY handle, Sint32 bank, const Float alpha[256])
{
    struct mwsfd_player *player = PLAYER(handle);
    Sint32 i;
    Sint32 level;
    if (player->composition == MWD_PLY_COMPO_LUMI) {
        for (i = 0; i < PALETTE_COLORS; ++i) {
            level = (Sint32)(alpha[i] * 255.0F);
            alpha_lut[i] = level;
        }
        mwYuvSetPltTbl(alpha_lut, YUV_LUMINANCE);
    } else if (player->composition == MWD_PLY_COMPO_ALPH3) {
        mwYuvGetPltTbl(alpha_lut, YUV_ALPHA3);
        if (bank == 2) {
            level = (Sint32)(alpha[255] * 255.0F);
            for (i = 136; i < 144; ++i) {
                alpha_lut[i] = level;
            }
            for (i = 144; i < 256; ++i) {
                level = (Sint32)(alpha[253 - ((i - 144) * 256 / 112)] * 255.0F);
                alpha_lut[i] = level;
            }
        } else if (bank == 1) {
            level = (Sint32)(alpha[0] * 255.0F);
            for (i = 8; i < 16; ++i) {
                alpha_lut[i] = level;
            }
            for (i = 16; i < 128; ++i) {
                level = (Sint32)(alpha[(i - 16) * 256 / 112] * 255.0F);
                alpha_lut[i] = level;
            }
            for (i = 128; i < 136; ++i) {
                level = (Sint32)(alpha[255] * 255.0F);
                alpha_lut[i] = level;
            }
        } else if (bank == 0) {
            for (i = 0; i < 8; ++i) {
                level = (Sint32)(alpha[i * 256 / 8] * 255.0F);
                alpha_lut[i] = level;
            }
        }
        mwYuvSetPltTbl(alpha_lut, YUV_ALPHA3);
    }
}

MWPLY mwPlyCreateSofdec(MWS_PLY_CPRM_SFD *parameters)
{
    Sint32 i;
    struct mwsfd_player *player;
    Sint32 width;
    Sint32 decoder_size;
    Sint32 stream_size;
    Sint32 height;
    Sint8 *work;
    Sint32 system;
    Sint32 picture;
    Sint32 sound;
    Sint32 adx_ring;
    Sint32 adx_work;
    Sint32 mask_size;
    Uint8 *mask;
    Sint32 decoder_height;
    struct sfd_handle *decoder;
    Sint32 status;

    for (i = 0; i < MWSFD_MAX_PLAYERS; ++i) {
        player = &mwg_ply_work.players[i];
        if (!player->used) {
            break;
        }
    }
    if (i == MWSFD_MAX_PLAYERS) {
        mwSfdSetErrCode(MWSFD_ERR_NO_HANDLE);
        mwMngError(no_handles);
        return NULL;
    }
    decoder_size = mwPlyCalcWorkSfd(parameters);
    work = parameters->work;
    mask_size = mwPlyCalcWorkCompo(parameters);
    /* the mask starts at the first cache line past the decoder's work. its
     * room holds a line more for that */
    if (mask_size > 0) {
        mask = (Uint8 *)(((Sint32)work + decoder_size + MWSFD_CACHE_LINE - 1) &
                         ~(MWSFD_CACHE_LINE - 1));
    } else {
        mask = NULL;
    }
    if (decoder_size + mask_size > parameters->wksize) {
        mwMngError(work_too_small);
        return NULL;
    }
    parameters->compo_mode &= 255;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it clears the whole player */
    memset(player, 0, sizeof(*player));
#else
    /* the cast and the parenthesized 0 keep clang's warning on swapped
     * arguments quiet */
    /* @bug logic: clears none of the player. the fill's value and length are
     * swapped at 0x8c542c38, 0x8c542c3a and 0x8c542c42 */
    /* NOLINTNEXTLINE(bugprone-suspicious-memset-usage) */
    memset(player, (int)sizeof(*player), (0));
#endif
    width = parameters->max_width;
    height = parameters->max_height;
    player->renderer = mw_sfd_create_rnvmv(width, height, player->render_work,
                                           sizeof(player->render_work));
    if (player->renderer == NULL) {
        mwMngError(no_color_renderer);
        mwSfdDestroy(HANDLE(player));
        return NULL;
    }
    mwRnv2EntryDrawCallbackFn(player->renderer, mwl_ply_draw_cbfn, player);
    mwRnv2EntryDispCallbackFn(player->renderer, mwl_ply_disp_cbfn, player);
    if (parameters->compo_mode >= MWD_PLY_COMPO_LUMI) {
        player->alpha_renderer = mw_sfd_create_rnvalph(
            width, height, player->alpha_work, sizeof(player->alpha_work));
        if (player->alpha_renderer == NULL) {
            mwMngError(no_alpha_renderer);
            mwSfdDestroy(HANDLE(player));
            return NULL;
        }
        mwRnv2SetVertexContextPlt8(player->alpha_renderer,
                                   ALPHA_PALETTE_START / BANK_COLORS);
        mwRnv2EntryDrawCallbackFn(player->alpha_renderer,
                                  mwl_ply_draw_alph_cbfn, player);
    } else {
        player->alpha_renderer = NULL;
    }
    /* @hack type: compo_mode cast to Uint8, where compo_mode would do: it is
     * masked to a byte above */
    if ((Uint8)parameters->compo_mode >= MWD_PLY_COMPO_ALPH256) {
        decoder_height = parameters->max_height * 2;
    } else {
        decoder_height = parameters->max_height;
    }
    /* @hack spelling: decoder and player->decoder set in one assignment,
     * where two would do */
    player->decoder = decoder = mwPlySfdCreate(
        player, parameters->ftype, parameters->max_bps, parameters->max_width,
        decoder_height, parameters->nfrm_pool_wk, work, decoder_size);
    if (player->decoder == NULL) {
        mwMngError(no_decoder);
        mwSfdDestroy(HANDLE(player));
        return NULL;
    }
    mwPlySfdCalcWorkStmBuf(parameters->ftype, parameters->max_bps, &stream_size,
                           &system, &picture, &sound, &adx_ring, &adx_work);
    mwPlySfdSetCond(player, parameters, mwg_nvsync_flip, mwg_sys_latency);
    player->input = mwPlySfdCreateSj();
    if (player->input == NULL) {
        mwMngError(no_stream_joint);
        mwSfdDestroy(HANDLE(player));
        return NULL;
    }
    player->memory = SJMEM_Create(NULL, 0);
    if (player->memory == NULL) {
        mwMngError(no_memory_joint);
        mwSfdDestroy(HANDLE(player));
        return NULL;
    }
    player->base.vtbl = &mwsfd_if;
    player->stream_size = stream_size;
    player->parameters = *parameters;
    player->created = TRUE;
    player->display_type = parameters->dtype;
    player->decoding = FALSE;
    player->frame_loaded = FALSE;
    player->brightness = DEFAULT_BRIGHTNESS;
    player->brightness_offset = DEFAULT_BRIGHTNESS_OFFSET;
    player->display_z = RNV2_DEFAULT_DEPTH;
    player->state = MWE_PLY_STAT_STOP;
    player->composition = parameters->compo_mode;
    /* @hack spelling: each pair of fields set in one assignment, where two
     * would do */
    player->draw_frame = player->load_frame = -1;
    player->display_x = player->display_y = 0.0F;
    player->display_width = player->display_height = -1.0F;
    status = SFD_GetTrHn(decoder, SFD_AUDIO_OUT, &player->audio_transfer);
    if (status != 0) {
        player->audio_transfer = NULL;
    }
    status = SFD_GetTrHn(decoder, SFD_AUDIO, &player->audio_decoder);
    if (status != 0) {
        player->audio_decoder = NULL;
    }
    player->video_enabled = TRUE;
    if (player->composition == MWD_PLY_COMPO_LUMI) {
        mwsfd_load_plttbl();
    } else if (player->composition == MWD_PLY_COMPO_ALPH3) {
        mwsfd_load_plttbl();
    } else if (player->composition == MWD_PLY_COMPO_ALPH5) {
        mwsfd_load_plttbl();
    } else if (player->composition == MWD_PLY_COMPO_ALPH256) {
        mwsfd_load_plttbl();
    }
    player->effect = NULL;
    player->effect_user = NULL;
    player->linked = FALSE;
    player->link_ended = FALSE;
    player->paused = FALSE;
    player->reserved_flag = 0;
    player->composition_buffer = mask;
    player->composition_size = mask_size - MWSFD_CACHE_LINE;
    player->link = LSC_Create(player->input);
    player->link_count = 0;
    player->link_time = -1;
    player->used = TRUE;
    mwg_disp_vcnt = 0;
    mwg_disp_vtime = 0;
    mwg_disp_dura = 0;
    for (i = 0; i < DURATIONS; ++i) {
        mwg_disp_dura_sum[i] = 0;
    }
    return HANDLE(player);
}

void mwSfdDestroy(MWPLY handle)
{
    struct mwsfd_player *player = PLAYER(handle);
    if (player == NULL) {
        return;
    }
    mwSfdStopDec(player);
    player->used = FALSE;
    if (player->link != NULL) {
        LSC_Destroy(player->link);
    }
    if (player->file != NULL) {
        mwGscClose(player->file);
    }
    if (player->input != NULL) {
        SJ_Destroy(player->input);
    }
    if (player->memory != NULL) {
        SJ_Destroy(player->memory);
    }
    if (player->decoder != NULL) {
        mwPlySfdDestroy(player->decoder);
    }
    if (player->renderer != NULL) {
        mwRnv2Destroy(player->renderer);
        player->renderer = NULL;
    }
    if (player->alpha_renderer != NULL) {
        mwRnv2Destroy(player->alpha_renderer);
        player->alpha_renderer = NULL;
    }
    memset(player, 0, sizeof(*player));
}
static void mw_sfd_start_ex(struct mwsfd_player *player)
{
    MWS_PLY_CPRM_SFD *params = &player->parameters;
    Sint32 height;
    Sint32 status;
    if (player->decoder != NULL) {
        /* @hack type: compo_mode cast to Uint8, where compo_mode would do:
         * mwPlyCreateSofdec has masked it to a byte */
        if ((Uint8)params->compo_mode >= MWD_PLY_COMPO_ALPH256) {
            height = params->max_height * 2;
        } else {
            height = params->max_height;
        }
        mwPlySfdDestroy(player->decoder);
        player->decoder = mwPlySfdCreate(
            player, params->ftype, params->max_bps, params->max_width, height,
            params->nfrm_pool_wk, params->work, params->wksize);
        mwPlySfdSetCond(player, params, mwg_nvsync_flip, mwg_sys_latency);
    }
    mwPlySfdStandby(player);
    mwPlySfdStart(player);
    mwSfdPause(HANDLE(player), player->paused);
    player->frame_loaded = FALSE;
    /* @hack spelling: draw_frame and load_frame set in one assignment, where
     * two would do */
    player->draw_frame = player->load_frame = -1;
    if (player->linked == TRUE) {
        status = SFD_SetConcatPlay(player->decoder);
        if (status != 0) {
            mwMngError(start_link_failed);
        }
    }
    player->link_ended = FALSE;
    player->link_time = -1;
    player->state = MWE_PLY_STAT_PREP;
}
static void mwSfdStartFname(MWPLY handle, Sint8 *name)
{
    Sint32 mask;
    SJ input;
    Sint32 status;

    mask = mwPlyLock();
    /* @hack spelling: the player as PLAYER(handle) at each use, where a local
     * taken at entry would do */
    /* @hack spelling: input and the player's stream set in one assignment,
     * where two would do */
    input = PLAYER(handle)->stream = PLAYER(handle)->input;
    mwSfdStopDec(PLAYER(handle));
    mw_sfd_start_ex(PLAYER(handle));
    PLAYER(handle)->file = mwGscOpenFname((char *)name, input);
    if (PLAYER(handle)->file == NULL) {
        mwSfdSetErrCode(MWSFD_ERR_GSC_OPEN);
        mwMngError(open_failed, name);
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: it unmasks the interrupts,
         * and leaves the player in MWE_PLY_STAT_ERROR */
        PLAYER(handle)->state = MWE_PLY_STAT_ERROR;
        mwPlyUnlock(mask);
#endif
        /* @bug logic: returns with every interrupt still masked. the branch
         * at 0x8c543108 passes over the unlock at 0x8c5431be */
        return;
    }
    if (mwGscSetRdSct(PLAYER(handle)->file,
                      PLAYER(handle)->stream_size / GDD_FS_SCTSIZE) != 0) {
        mwSfdSetErrCode(MWSFD_ERR_PARAMETER);
        mwMngError(read_limit_failed);
    }
    if (mwGscSetTrSct(PLAYER(handle)->file,
                      PLAYER(handle)->stream_size / GDD_FS_SCTSIZE) != 0) {
        mwSfdSetErrCode(MWSFD_ERR_PARAMETER);
        mwMngError(transfer_limit_failed);
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: no call overwrites the stream's
     * sizes, which keep what mwGscSetRdSct and mwGscSetTrSct set */
#else
    /* @bug mismatched-call: overwrites the GSC stream's stm_size and rd_sct.
     * ADXSTM_SetBufSize takes an ADX reader, and its writes at 0x8c156db0 and
     * 0x8c156db2 land on those fields. the call is at 0x8c54319a */
    ADXSTM_SetBufSize((ADXSTM)PLAYER(handle)->file, STREAM_BUFSIZE,
                      STREAM_RDSIZE);
#endif
    status = mwGscStart(PLAYER(handle)->file);
    if (status != 0) {
        mwSfdSetErrCode(MWSFD_ERR_GSC_START);
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: it unmasks the interrupts,
         * and leaves the player in MWE_PLY_STAT_ERROR */
        PLAYER(handle)->state = MWE_PLY_STAT_ERROR;
        mwPlyUnlock(mask);
#endif
        /* @bug logic: returns with every interrupt still masked. the return
         * after mwSfdSetErrCode at 0x8c5431b2 passes over the unlock at
         * 0x8c5431be */
        return;
    }
    mwPlyUnlock(mask);
}
static void mwSfdStartMem(MWPLY handle, void *data, Sint32 bytes)
{
    struct mwsfd_player *player = PLAYER(handle);
    /* @hack redundant: unused, a chunk that only sizes the frame */
    SJCK unused;
    (void)sizeof(unused);
    mwSfdStopDec(player);
    SJ_Destroy(player->memory);
    player->memory = SJMEM_Create(data, bytes);
    player->stream = player->memory;
    mw_sfd_start_ex(player);
}
static void mwSfdStartSj(MWPLY handle, SJ input)
{
    struct mwsfd_player *player = PLAYER(handle);
    /* @hack redundant: unused, a chunk that only sizes the frame */
    SJCK unused;
    (void)sizeof(unused);
    mwSfdStopDec(player);
    player->stream = input;
    mw_sfd_start_ex(player);
}
static void mwSfdStopDec(struct mwsfd_player *player)
{
    struct sfd_handle *decoder = player->decoder;
    Sint32 mask;
    Sint32 status;
    if (decoder == NULL) {
        return;
    }
    mask = mwPlyLock();
    mwlSfdSleepDecSvr();
    player->state = MWE_PLY_STAT_STOP;
    mwPlyUnlock(mask);
    status = SFD_Stop(decoder);
    if (status != 0) {
        mwSfdSetErrCode(MWSFD_ERR_STOP);
        mwMngError(stop_failed);
    }
    if (player->file != NULL) {
        mwGscClose(player->file);
        player->file = NULL;
    }
    if (player->stream != NULL) {
        SJ_Reset(player->stream);
    }
}
static void mwSfdStop(MWPLY handle)
{
    struct mwsfd_player *player = PLAYER(handle);
    mwSfdStopDec(player);
    mwPlyLinkStm(HANDLE(player), FALSE);
    player->link_count = 0;
    LSC_Stop(player->link);
    if (player->renderer != NULL) {
        mwRnv2Stop(player->renderer);
    }
    if (player->alpha_renderer != NULL) {
        mwRnv2Stop(player->alpha_renderer);
    }
}
static MWE_PLY_STAT mwSfdGetStat(MWPLY handle)
{
    struct mwsfd_player *player = PLAYER(handle);
    Sint32 state;
    if (player == NULL) {
        mwSfdSetErrCode(MWSFD_ERR_NULL_HANDLE);
        mwMngError(null_handle);
        return MWE_PLY_STAT_STOP;
    }
    if (player->state == MWE_PLY_STAT_PLAYING) {
        state = SFD_GetHnStat(player->decoder);
        if (state == SFD_PLAYING || state == SFD_FINISHED) {
            return MWE_PLY_STAT_PLAYING;
        }
        return MWE_PLY_STAT_PREP;
    }
    return player->state;
}
Sint32 mwSfdSetErrCode(Sint32 code)
{
    struct mwsfd_error *error = &mwg_ply_work.error;
    void (*report)(void *, Sint32);
    error->code = code;
    if (code == 0) {
        return 0;
    }
    report = error->report;
    if (report != NULL) {
        report(error->user, code);
    }
    return code;
}
Sint32 mwPlyGetMvFrm(MWPLY handle, MWS_PLY_MVFRM *frame)
{
    struct mwsfd_player *player = PLAYER(handle);
    memset(frame, 0, sizeof(*frame));
    if (player->state < MWE_PLY_STAT_PLAYING) {
        return FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((frame->srf = mwRnv2GetCurSrf(player->renderer)) == NULL) {
        return FALSE;
    }
    mwRnv2GetImgSize(player->renderer, &frame->width, &frame->height);
    return TRUE;
}
Sint32 mwPlyGetMskFrm(MWPLY handle, MWS_PLY_MVFRM *frame)
{
    struct mwsfd_player *player = PLAYER(handle);
    memset(frame, 0, sizeof(*frame));
    if (player->state < MWE_PLY_STAT_PLAYING) {
        return FALSE;
    }
    if (player->alpha_renderer == NULL) {
        return FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((frame->srf = mwRnv2GetCurSrf(player->alpha_renderer)) == NULL) {
        return FALSE;
    }
    mwRnv2GetImgSize(player->alpha_renderer, &frame->width, &frame->height);
    return TRUE;
}
void mwPlySetAsync(MWPLY handle, Bool enabled)
{
    struct mwsfd_player *player = PLAYER(handle);
    struct sfd_handle *decoder = player->decoder;
    if (!enabled) {
        SFD_SetCond(decoder, SFD_COND_CLOCK, SFD_CLOCK_NONE);
    } else if (player->parameters.ftype == MWD_PLY_FTYPE_SFD) {
        SFD_SetCond(decoder, SFD_COND_CLOCK, SFD_CLOCK_AUDIO);
    } else {
        SFD_SetCond(decoder, SFD_COND_CLOCK, SFD_CLOCK_VSYNC);
    }
}
SJ mwPlyGetInputSj(MWPLY handle)
{
    return PLAYER(handle)->input;
}
void mwPlyEntryFxCb(MWPLY handle, mwsfd_effect_fn effect, void *user)
{
    struct mwsfd_player *player = PLAYER(handle);
    player->effect = effect;
    player->effect_user = user;
}
void mwPlyLinkStm(MWPLY handle, Bool enabled)
{
    struct mwsfd_player *player = PLAYER(handle);
    struct sfd_handle *decoder = player->decoder;
    Sint32 status;
    if (player->linked == TRUE && enabled == FALSE) {
        player->link_ended = TRUE;
    }
    if (player->linked == FALSE && enabled == TRUE) {
        status = SFD_SetConcatPlay(decoder);
        if (status != 0) {
            mwMngError(link_failed);
        }
    }
    player->linked = enabled;
}
Sint32 mwPlyGetNumDecPool(MWPLY handle)
{
    struct sfd_handle *decoder = PLAYER(handle)->decoder;
    return SFMPVF_GetNumFrm(decoder);
}
#define PLAY_COUNT(function, field)                                            \
    Sint32 function(MWPLY handle)                                              \
    {                                                                          \
        struct sfd_handle *decoder = PLAYER(handle)->decoder;                  \
        struct sfd_play_info counts;                                           \
        SFD_GetPlyInf(decoder, &counts);                                       \
        return counts.field;                                                   \
    }
PLAY_COUNT(mwPlyGetNumTotalDec, decoded)
PLAY_COUNT(mwPlyGetNumTotalSkip, skipped)

Sint32 mwPlyGetNumIbufRoom(MWPLY handle)
{
    struct sfd_handle *decoder = PLAYER(handle)->decoder;
    struct sfd_ring_info buffer;
    SFD_GetWritePtr(decoder, &buffer);
    return buffer.head.size + buffer.wrap.size;
}
#define RENDER_COUNT(function, result)                                         \
    Sint32 function(MWPLY handle)                                              \
    {                                                                          \
        struct mwsfd_player *player = PLAYER(handle);                          \
        Sint32 ready;                                                          \
        Sint32 shown;                                                          \
        mwRnv2GetNumTxfrm(player->renderer, &ready, &shown);                   \
        return result;                                                         \
    }
RENDER_COUNT(mwPlyGetNumDispWait, ready)
RENDER_COUNT(mwPlyGetNumRend, shown)
void *mwPlyGetRnvImg(MWPLY handle)
{
    return PLAYER(handle)->renderer;
}
void *mwPlyGetRnvMsk(MWPLY handle)
{
    return PLAYER(handle)->alpha_renderer;
}
Sint32 mwPlyGetNumLoadFrm(MWPLY handle)
{
    return PLAYER(handle)->load_frame + 1;
}
static Sint32 mwl_AdjustDrawcb(Sint32 draw, Sint32 frame)
{
    if (frame == 0 && draw == RNV2_DRAW_HOLD) {
        draw = RNV2_DRAW_SHOW;
    }
    return draw;
}
/* the renderers' draw callbacks (rnv2_draw_fn). each gives RNV2_DRAW_SHOW
 * for picture 0 and any picture whose time is due, and RNV2_DRAW_HOLD for
 * the rest. while the movie's rate is mwg_flip_rate or more, RNV2_DRAW_SKIP
 * replaces RNV2_DRAW_SHOW. mwRnv2Draw then shows the last picture it skips */
#define DRAW_CALLBACK(function, remember_frame)                                \
    static Sint32 function(void *user, Sint32 time, Sint32 unit, Sint32 frame) \
    {                                                                          \
        struct mwsfd_player *player = user;                                    \
        struct sfd_handle *decoder = player->decoder;                          \
        Bool due;                                                              \
        Sint32 draw;                                                           \
        Sint32 rate;                                                           \
        remember_frame;                                                        \
        SFD_IsDrawTime(decoder, time, unit, &due);                             \
        if (due) {                                                             \
            player->draw_frame = frame;                                        \
            draw = RNV2_DRAW_SHOW;                                             \
        } else {                                                               \
            draw = RNV2_DRAW_HOLD;                                             \
        }                                                                      \
        draw = mwl_AdjustDrawcb(draw, frame);                                  \
        SFD_GetPlayFps(decoder, &rate);                                        \
        if (mwg_flip_rate <= rate && draw == RNV2_DRAW_SHOW) {                 \
            draw = RNV2_DRAW_SKIP;                                             \
        }                                                                      \
        return draw;                                                           \
    }
DRAW_CALLBACK(mwl_ply_draw_alph_cbfn, (void)0)
DRAW_CALLBACK(mwl_ply_draw_cbfn, mwg_last_draw_dfno = frame)

static void mwl_ChkDfno(Sint32 frame)
{
    Sint32 *previous = &previous_display_frame;
    if (frame != 0 && frame - *previous != 1) {
        discontinuities++;
    }
    *previous = frame;
}
static void mwl_ChkDispTime(struct mwsfd_player *player, Sint32 time,
                            Sint32 unit, Sint32 frame)
{
    Sint32 count;
    Sint32 scale;
    mwSfdGetTime(HANDLE(player), &count, &scale);
    mw_atime_ms2 = (Sint32)((Float)count * TENTHS_A_SECOND / (Float)scale);
    mw_vtime_ms2 = (Sint32)((Float)time * TENTHS_A_SECOND / (Float)unit);
    mw_rtime_ms2 = (syTmrCountToMicro(syTmrGetCount()) / MICROSECONDS_A_TENTH) -
                   mw_start_time;
    mw_dif_av_time2 = mw_atime_ms2 - mw_vtime_ms2;
    mw_dif_rv_time2 = mw_rtime_ms2 - mw_vtime_ms2;
    mw_dif_ra_time2 = mw_rtime_ms2 - mw_atime_ms2;
    if (frame == 0) {
        mw_start_time =
            syTmrCountToMicro(syTmrGetCount()) / MICROSECONDS_A_TENTH;
    }
}
static void mwl_DebugDispCb(struct mwsfd_player *player, Sint32 time,
                            Sint32 unit, Sint32 frame)
{
    Sint32 vsync;
    mwg_last_disp_dfno = frame;
    mwl_ChkDfno(frame);
    mwl_ChkDispTime(player, time, unit, frame);
    vsync = mwg_vcnt;
    if (mwg_disp_vtime != time) {
        mwg_disp_vtime = time;
        mwg_disp_dura = vsync - mwg_disp_vcnt;
        mwg_disp_dura_sum[mwg_disp_dura < DURATIONS - 1 ? mwg_disp_dura
                                                        : DURATIONS - 1]++;
    }
    mwg_disp_vcnt = vsync;
}
static void mwl_ply_disp_cbfn(void *user, Sint32 time, Sint32 unit,
                              Sint32 frame)
{
    struct mwsfd_player *player = user;
    struct sfd_handle *decoder = player->decoder;
    mwl_DebugDispCb(player, time, unit, frame);
    SFD_OutDispSync(decoder, time, unit);
}

/* @hack redundant: player declared register, where a plain parameter would
 * do */
static void mwlSfdExecDecSvrPrep(register struct mwsfd_player *player)
{
    struct sfd_movie_info movie;
    struct sfd_handle *decoder = player->decoder;
    Sint32 width;
    Sint32 height;
    Sint32 state;
    Sint32 mask_width;
    /* @hack redundant: mask_height declared register, where a plain local would
     * do */
    register Sint32 mask_height;
    SFD_GetMvInf(decoder, &movie);
    if (movie.width > 0) {
        width = movie.mb_width * MACROBLOCK_SIDE;
        height = movie.mb_height * MACROBLOCK_SIDE;
        if (player->composition == MWD_PLY_COMPO_ALPH256) {
            height /= 2;
        }
        mwRnv2SetImgSize(player->renderer, width, height);
        if (player->display_type == MWD_PLY_DTYPE_AUTO) {
            mwRnv2SetWndDefConf(player->renderer, RNV2_WND_BY_SIZE);
            mwRnv2SetWndBright(player->renderer, player->brightness);
            mwRnv2SetWndBrightOfst(player->renderer, player->brightness_offset);
            mwRnv2SetWndZ(player->renderer, player->display_z);
            if (player->display_x != 0.0F || player->display_y != 0.0F) {
                mwRnv2SetWndPos(player->renderer, player->display_x,
                                player->display_y);
            }
        } else if (player->display_type == MWD_PLY_DTYPE_FULL ||
                   player->display_type == MWD_PLY_DTYPE_WND) {
            mwRnv2SetWndImgSize(player->renderer, (Float)width, (Float)height);
        }
        /* @hack spelling: mask_width and mask_height, copies of width and
         * height, where those would do */
        mask_width = width;
        mask_height = height;
        mwPlySetVideoSw(HANDLE(player), player->video_enabled);
        if (player->composition != MWD_PLY_COMPO_OPEQ) {
            if (player->composition == MWD_PLY_COMPO_TRNSP) {
                mwRnv2SetCompoMode(player->renderer, MWD_RNV2_COMPO_ALPH);
            } else if (player->composition != MWD_PLY_COMPO_ADD &&
                       mwg_sfd_dstalph == TRUE) {
                mwRnv2SetCompoMode(player->renderer, MWD_RNV2_COMPO_DSTALPH);
            } else {
                mwRnv2SetCompoMode(player->renderer, MWD_RNV2_COMPO_ADD);
            }
        }
        if (mwRnv2GetStat(player->renderer) == MWE_RNV2_STAT_STOP) {
            mwRnv2Start(player->renderer);
        }
        if (player->composition >= MWD_PLY_COMPO_LUMI) {
            mwRnv2SetImgSize(player->alpha_renderer, width, mask_height);
            if (player->display_type == MWD_PLY_DTYPE_AUTO) {
                mwRnv2SetWndDefConf(player->alpha_renderer, RNV2_WND_BY_SIZE);
                mwRnv2SetWndBright(player->alpha_renderer, player->brightness);
                mwRnv2SetWndBrightOfst(player->alpha_renderer,
                                       player->brightness_offset);
                mwRnv2SetWndZ(player->alpha_renderer,
                              player->display_z + ALPHA_DEPTH_BIAS);
                if (player->display_x != 0.0F || player->display_y != 0.0F) {
                    mwRnv2SetWndPos(player->alpha_renderer, player->display_x,
                                    player->display_y);
                }
            } else if (player->display_type == MWD_PLY_DTYPE_FULL ||
                       player->display_type == MWD_PLY_DTYPE_WND) {
                mwRnv2SetWndImgSize(player->alpha_renderer, (Float)mask_width,
                                    (Float)mask_height);
            }
            if (mwRnv2GetStat(player->alpha_renderer) == MWE_RNV2_STAT_STOP) {
                mwRnv2Start(player->alpha_renderer);
            }
        } else if (player->alpha_renderer != NULL) {
            mwRnv2Stop(player->alpha_renderer);
        }
        player->state = MWE_PLY_STAT_PLAYING;
    }
    state = SFD_GetHnStat(decoder);
    if (state == SFD_PLAYING || state == SFD_FINISHED) {
        player->state = MWE_PLY_STAT_PLAYING;
    }
    mwPlySfdSjCopyToSfd(player);
}
/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mwl_sfd_dmy(struct sfd_handle *decoder);
void mwl_sfd_dmy(struct sfd_handle *decoder)
{
    mwg_sfd_hn = decoder;
}
/* NOLINTEND(misc-use-internal-linkage) */
static void mwlSfdExecDecSvrPlaying(struct mwsfd_player *player)
{
    struct sfd_handle *decoder = player->decoder;
    SJ input;
    Sint32 status;
    mwPlySfdSjCopyToSfd(player);
    if (player->link_ended == TRUE) {
        input = player->stream;
        if (SJ_GetNumData(input, SJ_LIN_DATA) == 0) {
            status = SFD_TermSupply(decoder);
            if (status != 0) {
                mwMngError(end_failed);
            }
            player->link_ended = FALSE;
        }
    }
    status = SFD_GetHnStat(decoder);
    if (status == SFD_FINISHED) {
        player->state = MWE_PLY_STAT_PLAYEND;
    }
}
static Sint32 mwSfdExecDecSvrHndl(struct mwsfd_player *player)
{
    Sint32 mask;
    struct gsc_stream *file;
    switch (player->state) {
    case MWE_PLY_STAT_STOP:
        break;
    case MWE_PLY_STAT_PREP:
        mwlSfdExecDecSvrPrep(player);
        break;
    case MWE_PLY_STAT_PLAYING:
        mwlSfdExecDecSvrPlaying(player);
        /* @hack spelling: PLAYING falls through to PLAYEND's break, where a
         * break of its own would do */
        /* fall through */
    case MWE_PLY_STAT_PLAYEND:
        break;
    case MWE_PLY_STAT_ERROR:
        break;
    default:
        break;
    }
    if (player->file != NULL) {
        if (mwGscIsFsStatErr(player->file)) {
            file = player->file;
            mask = mwPlyLock();
            player->state = MWE_PLY_STAT_ERROR;
            player->file = NULL;
            mwPlyUnlock(mask);
            if (file != NULL) {
                mwGscClose(file);
            }
        }
    }
    return 0;
}
void mwSfdExecDecServer(void)
{
    Uint32 decode_start;
    Uint32 start;
    Uint32 decode_end;
    Uint32 split_start;
    Uint32 split_end;
    Sint32 elapsed;
    Sint32 i;
    struct mwsfd_player *player;

    mw_t8c = syTmrGetCount();
    decode_start = syTmrGetCount();
    if (mwg_sfd_init_flag == TRUE) {
        SFD_ExecServer();
    }
    decode_end = syTmrGetCount();
    /* @hack spelling: start, a copy of decode_start, where decode_start would
     * do */
    start = decode_start;
    elapsed = syTmrCountToMicro(syTmrDiffCount(start, decode_end));
    if (elapsed >= SLOW_SFD_SERVER) {
        mw_t1a = decode_start;
        mw_t1b = decode_end;
        mw_dectime += elapsed;
    }
    split_start = syTmrGetCount();
    for (i = 0; i < MWSFD_MAX_PLAYERS; i++) {
        player = &mwg_ply_work.players[i];
        if (player->used != FALSE) {
            if (player->state == MWE_PLY_STAT_STOP) {
                player->decoding = FALSE;
            } else {
                player->decoding = TRUE;
                mwSfdExecDecSvrHndl(player);
            }
        }
    }
    split_end = syTmrGetCount();
    elapsed = syTmrCountToMicro(syTmrDiffCount(split_start, split_end));
    if (elapsed >= SLOW_PLAYERS) {
        mw_t2a = split_start;
        mw_t2b = split_end;
        mw_splttime += elapsed;
    }
    mw_t8a = mw_t8c;
    mw_t8b = syTmrGetCount();
}
/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mwl_sfd_wait_vsync(void);
void mwl_sfd_wait_vsync(void)
{
    mw_t6a = mw_t6b;
    mw_t6b = syTmrGetCount();
    if (vsync_flag == TRUE) {
        mw_t7a = mw_t7c;
        mw_t7b = syTmrGetCount();
        vsync_flag = FALSE;
    }
    syFbrSwitchToFiber(g_fbrSub);
    mw_t6c = syTmrGetCount();
}
/* NOLINTEND(misc-use-internal-linkage) */
static void mwlSfdSleepDecSvr(void)
{
    if (mwg_sleep_enable_flag) {
        mwMngSleepIdle();
    }
}
static void mwsfd_fx_callback(struct mwsfd_player *player,
                              struct sfd_frame_info *frame)
{
    if (player->effect != NULL) {
        player->effect(player->effect_user, player->load_frame, frame->time,
                       frame->time_unit);
    }
    if (player->link_count > 0 && frame->time < player->link_time) {
        player->link_count--;
        if (player->link_count == 0 && player->linked == FALSE) {
            player->link_ended = TRUE;
        }
    }
    player->link_time = frame->time;
}
static void mwSfdExecTexSvrHndl(struct mwsfd_player *player)
{
    struct sfd_frame_info *frame;
    Sint32 width;
    Sint32 height;
    Sint32 i;
    struct sfd_handle *decoder;
    Sint32 room;
    Sint32 bytes;
    Uint32 *word;
    Uint32 *end;

    decoder = player->decoder;
    if (player->renderer == NULL) {
        return;
    }
    player->frame_loaded = FALSE;
    room = mwRnv2GetNumRoomTex(player->renderer);
    room = room > LOADS_A_PASS ? LOADS_A_PASS : room;
    for (i = 0; i < room; ++i) {
        SFD_GetFrm(decoder, &frame);
        if (frame == NULL) {
            break;
        }
        player->load_frame++;
        mwsfd_fx_callback(player, frame);
        mwRnv2GetImgSize(player->renderer, &width, &height);
        /* bytes is the size of the mask's texture, a byte a texel. the
         * texture is the smallest square of 128 or 256 texels a side that
         * holds the picture. any larger picture gets 512 */
        if (frame->width <= 128 && frame->height <= 128) {
            bytes = 128 * 128;
        } else if (frame->width <= 256 && frame->height <= 256) {
            bytes = 256 * 256;
        } else {
            bytes = 512 * 512;
        }
        if ((player->composition == MWD_PLY_COMPO_OPEQ ||
             player->composition == MWD_PLY_COMPO_TRNSP) &&
            player->alpha_renderer != NULL) {
            if (player->composition_buffer == NULL) {
                mwMngError(no_mask_to_clear);
            } else {
                word = (Uint32 *)player->composition_buffer;
                end = (Uint32 *)(player->composition_buffer + bytes);
                for (; word < end; ++word) {
                    *word = 0;
                }
            }
        }
        if (player->composition == MWD_PLY_COMPO_LUMI) {
            mwYuvConvMblkToTwdlRctngl((Sint8 *)player->composition_buffer,
                                      (Sint8 *)frame->image, width, 0, 0, width,
                                      height, YUV_LUMINANCE);
        } else if (player->composition == MWD_PLY_COMPO_ALPH256) {
            /* the decoder's picture is twice the image's height here, and
             * its lower half is the mask */
            mwYuvConvMblkToTwdlRctngl(
                (Sint8 *)player->composition_buffer,
                (Sint8 *)(frame->image + MACROBLOCK_BYTES(width * height)),
                width, 0, 0, width, height, YUV_ALPHA256);
        } else if (player->composition == MWD_PLY_COMPO_ALPH3) {
            mwYuvConvMblkToTwdlRctngl((Sint8 *)player->composition_buffer,
                                      (Sint8 *)frame->image, width, 0, 0, width,
                                      height, YUV_ALPHA3);
            syCacheOCBP(frame->image, MACROBLOCK_BYTES(bytes));
        } else if (player->composition == MWD_PLY_COMPO_ALPH5) {
            mwYuvConvMblkToTwdlRctngl((Sint8 *)player->composition_buffer,
                                      (Sint8 *)frame->image, width, 0, 0, width,
                                      height, YUV_ALPHA5);
            syCacheOCBP(frame->image, MACROBLOCK_BYTES(bytes));
        } else if (player->composition == MWD_PLY_COMPO_MAT) {
            /* @hack redundant: a test for MWD_PLY_COMPO_MAT whose branch is
             * empty */
        }
        if (player->alpha_renderer != NULL) {
            if (player->composition_buffer == NULL) {
                mwMngError(no_mask_to_load);
            } else {
                syCacheOCBP(player->composition_buffer, bytes);
                mwRnv2Load(player->alpha_renderer, player->composition_buffer,
                           frame->time, frame->time_unit);
            }
        }
        mwRnv2Load(player->renderer, frame->image, frame->time,
                   frame->time_unit);
        SFD_RelFrm(decoder, frame);
        player->frame_loaded = TRUE;
    }
}
void mwPlyExecTexSvr(void)
{
    Sint32 i;
    struct mwsfd_player *player;
    if (!mwg_sfd_init_flag) {
        return;
    }
    mwGscExecServer();
    if (!mwg_sfd_mthrd) {
        mwg_thread_exec_flag = TRUE;
        mwMngIdle();
        mwg_thread_exec_flag = FALSE;
    }
    for (i = 0; i < MWSFD_MAX_PLAYERS; ++i) {
        player = &mwg_ply_work.players[i];
        if (player->used != FALSE) {
            mwSfdExecTexSvrHndl(player);
        }
    }
}
void mwPlyExecDrawSvr(void)
{
    if (!mwg_sfd_init_flag) {
        return;
    }
    mwRnv2ExecServer();
    if (!mwg_sfd_mthrd) {
        mwg_thread_exec_flag = TRUE;
        mwMngIdle();
        mwg_thread_exec_flag = FALSE;
    }
}
void mwSfdExecServer(void *user)
{
    if (mwg_eor_flag_ptr != NULL) {
        *mwg_eor_flag_ptr = FALSE;
    }
    mwg_sfd_exec_svr_cnt++;
    mwPlyExecTexSvr();
    mwPlyExecDrawSvr();
}
void mwSfdVsync(void *user)
{
    Sint32 mask = get_imask();
    set_imask(IMASK_ALL);
    mw_t7c = syTmrGetCount();
    vsync_flag = TRUE;
    mwg_field_no++;
    mwg_vcnt++;
    if (mwg_sfd_init_flag == TRUE) {
        SFD_VbIn();
    }
    set_imask(mask);
}
void mwSfdStartFrame(void *user)
{
    mwg_field_no = 0;
}
static void mwSfdGetTime(MWPLY handle, Sint32 *time, Sint32 *unit)
{
    struct mwsfd_player *player = PLAYER(handle);
    struct sfd_handle *decoder = player->decoder;
    Sint32 status = SFD_GetTime(decoder, time, unit);
    if (status != 0) {
        mwSfdSetErrCode(MWSFD_ERR_TIME);
        mwMngError(time_failed);
        *time = 0;
        *unit = 1;
    }
    if (*time < 0) {
        *time = 0;
        *unit = 1;
    }
}
static void mwSfdPause(MWPLY handle, Sint32 paused)
{
    struct mwsfd_player *player = PLAYER(handle);
    struct sfd_handle *decoder = player->decoder;
    Sint32 audio;
    Sint32 status;
    if (player->parameters.ftype == MWD_PLY_FTYPE_SFD) {
        if (SFD_GetCond(decoder, SFD_COND_AUDIO, &audio) != 0 ||
            audio == TRUE) {
            mwlSfdSleepDecSvr();
        }
    }
    status = SFD_Pause(decoder, paused);
    if (status != 0) {
        mwSfdSetErrCode(MWSFD_ERR_PAUSE);
        mwMngError(pause_failed, paused == TRUE ? pause_on : pause_off);
    }
    player->paused = paused;
}
void mwPlySetFastHalfpel(MWPLY handle, Bool enabled)
{
    struct sfd_handle *decoder = PLAYER(handle)->decoder;
    SFD_SetMpvCond(decoder, MPV_COND_HALF_PEL_2, enabled);
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}
void mwPlySetAdjustY16(MWPLY handle, Bool enabled)
{
    struct sfd_handle *decoder = PLAYER(handle)->decoder;
    SFD_SetMpvCond(decoder, MPV_COND_LUMA_OFFSET, enabled);
}
void mwPlySetMat(MWPLY handle, const Uint8 *mat)
{
    struct mwsfd_player *player;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: player is the handle's */
    player = PLAYER(handle);
#endif
    /* @bug uninitialized: reads the matte's buffer and size through player,
     * which nothing sets. the reads are at 0x8c544208, 0x8c54421e and
     * 0x8c544222 */
    /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
    if (player->composition_buffer == NULL) {
        mwMngError(no_matte);
    } else {
        memcpy(player->composition_buffer, mat, player->composition_size);
    }
}
void mwPlySetDispPos(MWPLY handle, Float x, Float y)
{
    struct mwsfd_player *player = PLAYER(handle);
    player->display_x = x;
    player->display_y = y;
    if (player->renderer != NULL) {
        mwRnv2SetWndPos(player->renderer, x, y);
    }
    if (player->alpha_renderer != NULL) {
        mwRnv2SetWndPos(player->alpha_renderer, x, y);
    }
}
void mwPlySetDispSize(MWPLY handle, Float width, Float height)
{
    struct mwsfd_player *player = PLAYER(handle);
    player->display_width = width;
    player->display_height = height;
    if (player->renderer != NULL) {
        mwRnv2SetWndSize(player->renderer, width, height);
    }
    if (player->alpha_renderer != NULL) {
        mwRnv2SetWndSize(player->alpha_renderer, width, height);
    }
}
#define BRIGHTNESS_SETTER(function, field, apply)                              \
    void function(MWPLY handle, Sint32 value)                                  \
    {                                                                          \
        struct mwsfd_player *player = PLAYER(handle);                          \
        player->field = value;                                                 \
        if (player->renderer != NULL) {                                        \
            apply(player->renderer, value);                                    \
        }                                                                      \
        if (player->alpha_renderer != NULL) {                                  \
            apply(player->alpha_renderer, value);                              \
        }                                                                      \
    }
BRIGHTNESS_SETTER(mwPlySetBright, brightness, mwRnv2SetWndBright)
BRIGHTNESS_SETTER(mwPlySetBrightOfst, brightness_offset, mwRnv2SetWndBrightOfst)
Sint32 mwPlyGetBright(MWPLY handle)
{
    return PLAYER(handle)->brightness;
}
Sint32 mwPlyGetBrightOfst(MWPLY handle)
{
    return PLAYER(handle)->brightness_offset;
}
Float mwPlyGetDispZ(MWPLY handle)
{
    return PLAYER(handle)->display_z;
}
void mwPlySetDispZ(MWPLY handle, Float z)
{
    /* @hack spelling: the player as PLAYER(handle) at each use, where a local
     * taken at entry would do */
    PLAYER(handle)->display_z = z;
    if (PLAYER(handle)->renderer != NULL) {
        mwRnv2SetWndZ(PLAYER(handle)->renderer, PLAYER(handle)->display_z);
    }
    if (PLAYER(handle)->alpha_renderer != NULL) {
        mwRnv2SetWndZ(PLAYER(handle)->alpha_renderer,
                      PLAYER(handle)->display_z + ALPHA_DEPTH_BIAS);
    }
}
static void mwSfdSetOutVol(MWPLY handle, Sint32 volume)
{
    /* @hack spelling: the player as PLAYER(handle) at each use, where a local
     * taken at entry would do */
    if (PLAYER(handle)->audio_transfer != NULL) {
        mwRnaSetOutVol(PLAYER(handle)->audio_transfer, volume);
    }
}
static Sint32 mwSfdGetOutVol(MWPLY handle)
{
    Sint32 volume = 0;
    /* @hack spelling: the player as PLAYER(handle) at each use, where a local
     * taken at entry would do */
    if (PLAYER(handle)->audio_transfer != NULL) {
        volume = mwRnaGetOutVol(PLAYER(handle)->audio_transfer);
    }
    return volume;
}
static void mwSfdSetOutPan(MWPLY handle, Sint32 channel, Sint32 pan)
{
    /* @hack spelling: the player as PLAYER(handle) at each use, where a local
     * taken at entry would do */
    if (PLAYER(handle)->audio_transfer != NULL) {
        mwRnaSetOutPan(PLAYER(handle)->audio_transfer, channel, pan);
    }
}
static Sint32 mwSfdGetOutPan(MWPLY handle, Sint32 channel)
{
    Sint32 pan = 0;
    /* @hack spelling: the player as PLAYER(handle) at each use, where a local
     * taken at entry would do */
    if (PLAYER(handle)->audio_transfer != NULL) {
        pan = mwRnaGetOutPan(PLAYER(handle)->audio_transfer, channel);
    }
    return pan;
}
void mwPlySetIpicOnly(MWPLY handle, Bool enabled)
{
    struct sfd_handle *decoder = PLAYER(handle)->decoder;
    if (enabled == TRUE) {
        SFD_SetMpvCond(decoder, MPV_COND_TEXTURE_ONLY, 1);
    } else {
        SFD_SetMpvCond(decoder, MPV_COND_TEXTURE_ONLY, 0);
    }
}
void mwPlySetNeverSkip(MWPLY handle, Bool enabled)
{
    struct sfd_handle *decoder = PLAYER(handle)->decoder;
    if (enabled == TRUE) {
        SFD_SetCond(decoder, SFD_COND_DECODE_ALL, 1);
    } else {
        SFD_SetCond(decoder, SFD_COND_DECODE_ALL, 0);
    }
}
#ifdef NONMATCHING
/* this side repairs the bug tagged at each use below: for a null player it
 * gives a NULL decoder and reads nothing */
#define PLAYER_DECODER(handle)                                                 \
    ((handle) != NULL ? PLAYER(handle)->decoder : NULL)
#else
#define PLAYER_DECODER(handle) (PLAYER(handle)->decoder)
#endif
void mwPlyEntryDecExecCb(MWPLY handle, mwsfd_picture_fn decoded)
{
    struct mwsfd_player *player = PLAYER(handle);
    /* @bug memory: reads a null player's decoder, in the delay slot of the
     * test for null at 0x8c544498 */
    struct sfd_handle *decoder = PLAYER_DECODER(player);
    if (player != NULL) {
        SFD_SetCond(decoder, SFD_COND_DECODE_CALLBACK, (Sint32)decoded);
    }
}
void mwPlyEntryDecSkipCb(MWPLY handle, mwsfd_picture_fn skipped)
{
    struct mwsfd_player *player = PLAYER(handle);
    /* @bug memory: reads a null player's decoder, in the delay slot of the
     * test for null at 0x8c5444b2 */
    struct sfd_handle *decoder = PLAYER_DECODER(player);
    if (player != NULL) {
        SFD_SetCond(decoder, SFD_COND_SKIP_CALLBACK, (Sint32)skipped);
    }
}
Sint32 mwPlyGetNumDropFrm(MWPLY handle)
{
    return mwg_rnv_ndrop;
}
void mwPlySetVideoSw(MWPLY handle, Sint32 enabled)
{
    struct mwsfd_player *player = PLAYER(handle);
    player->video_enabled = enabled;
    if (player->renderer != NULL) {
        mwRnv2SetDispSw(player->renderer, enabled);
    }
    if (player->alpha_renderer != NULL) {
        mwRnv2SetDispSw(player->alpha_renderer, enabled);
    }
}
Bool mwPlyGetVideoSw(MWPLY handle)
{
    return PLAYER(handle)->video_enabled;
}
void mwPlySetCompoMode(MWPLY handle, Sint32 mode)
{
    struct mwsfd_player *player = PLAYER(handle);
    if (mode >= MWD_PLY_COMPO_LUMI && player->alpha_renderer == NULL) {
        mwMngError(mode_unavailable, mode);
        return;
    }
    if (mode == MWD_PLY_COMPO_OPEQ) {
        mwRnv2SetCompoMode(player->renderer, MWD_RNV2_COMPO_OPEQ);
    } else if (mode == MWD_PLY_COMPO_TRNSP) {
        mwRnv2SetCompoMode(player->renderer, MWD_RNV2_COMPO_ALPH);
    } else {
        mwRnv2SetCompoMode(player->renderer, MWD_RNV2_COMPO_ADD);
    }
    if (player->alpha_renderer != NULL) {
        if (mode == MWD_PLY_COMPO_OPEQ) {
            mwsfd_load_plttbl_clear_msk(player);
        } else if (mode == MWD_PLY_COMPO_TRNSP) {
            mwsfd_load_plttbl_clear_msk(player);
        } else if (mode == MWD_PLY_COMPO_ADD) {
            mwsfd_load_plttbl_clear_msk(player);
        } else if (mode == MWD_PLY_COMPO_LUMI) {
            mwsfd_load_plttbl();
        } else if (mode == MWD_PLY_COMPO_ALPH3) {
            mwsfd_load_plttbl();
        } else if (mode == MWD_PLY_COMPO_ALPH5) {
            mwsfd_load_plttbl();
        } else if (mode == MWD_PLY_COMPO_ALPH256) {
            mwsfd_load_plttbl();
        }
    }
    player->composition = mode;
}
