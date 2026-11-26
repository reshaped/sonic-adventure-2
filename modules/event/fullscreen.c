#include <ninja.h>
#include <sfd_player.h>
#include <sfdfx_mw.h>
#include <sg_bup.h>
#include <sg_mwply.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "draw/light.h"
#include "event/areas.h"
#include "event/playback.h"
#include "event/movie.h"
#include "event/scene.h"
#include "main.h"
#include "sound/sound.h"
#include "system/areas.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"

#ifdef NONMATCHING
/* the event's file checks and texture_discard_pvm, by which this side's plane
 * loads and releases */
#include "event/file.h"
#include "system/texture.h"
#endif

#define FULLSCREEN_PAL (NJD_DSP_PAL | NJD_DSP_PSINTER | NJD_DSP_640x480)
#define FULL_BRIGHTNESS 255
/* the frames a movie without a fade out holds at its end */
#define FADE_WAIT 60
/* each frame lasts FRAME_BLANKS vertical blanks and takes FRAME_STEPS steps */
#define FRAME_BLANKS 1
#define FRAME_STEPS 1
/* the motion frames Tails' plane moves on each vertical blank */
#define PLANE_FRAME_STEP 0.5F
/* the movies' audio channels: Japanese voices, and English */
#define JAPANESE_CHANNEL 0
#define ENGLISH_CHANNEL 1

/* the record tailsPlain.prs opens with */
struct tails_plane {
    NJS_TEXLIST *textures;
    NJS_CNK_OBJECT *plane;
    NJS_MOTION *flight, *camera;
};

Bool event_movie_draw_plane;
static Sint32 brightness;
/* nothing reads or writes it */
static Uint8 unk_08[256];
static Bool stream_failed;
static Sint32 stream_error;
static struct sfd_handle *stream;
static struct tails_plane *plane;
static NJS_CAMERA plane_camera;
static Float plane_frame;

static void load_plane(void);
static void release_plane(void);
static void draw_plane(void);

static void note_stream_error(void *obj, Sint32 code)
{
    stream_failed = TRUE;
    stream_error = code;
}

static void play_stream(struct event_movie_request *request)
{
    MWE_PLY_STAT status;
    Sint32 wait;
    event_movie_player->vtbl->Pause(event_movie_player, FALSE);
    /* @hack spelling: while (1), where for (;;) would do */
    while (1) {
        njWaitVSync();
        /* @hack spelling: stream_failed compared with TRUE, where the flag
         * alone would do */
        if (stream_failed == TRUE) {
            njPrint(NJM_LOCATION(0, 8), "Error%X", (unsigned)stream_error);
#ifdef NONMATCHING
            /* this side stops a stream that failed, where the matching side
             * waits on it for good */
            return;
#else
            continue;
#endif
        }
        mwPlyStartFrame();
        status = event_movie_player->vtbl->GetStat(event_movie_player);
#ifdef NONMATCHING
        /* this side stops at a player that failed or stopped, and marks the
         * stream failed */
        if (status == MWE_PLY_STAT_ERROR || status == MWE_PLY_STAT_STOP) {
            stream_failed = TRUE;
            return;
        }
#endif
        pad_scan();
        if (pad_reset_pressed(0)) {
            event_reset_pressed = TRUE;
        } else {
            event_reset_pressed = FALSE;
        }
        if (player_pads[EVENT_PLAYER_PAD]->press & PDD_DGT_ST) {
            game_state.trial_screen = TRIAL_SCREEN_TITLE;
            break;
        }
        if (status == MWE_PLY_STAT_PLAYEND || status == MWE_PLY_STAT_ERROR) {
            break;
        }
        if (request->fade_in > 0) {
            brightness += request->fade_in;
            if (brightness > FULL_BRIGHTNESS) {
                brightness = FULL_BRIGHTNESS;
            }
        }
        mwPlySetBright(event_movie_player, brightness);
        /* @hack spelling: event_movie_draw_plane tested by its low bit, where
         * the flag alone would do */
        if (event_movie_draw_plane & 1) {
            draw_plane();
        }
        mwPlyExecServer();
    }
    wait = FADE_WAIT;
    while (wait != 0 && brightness != 0) {
        if (request->fade_out > 0) {
            brightness -= request->fade_out;
            if (brightness < 0) {
                brightness = 0;
            }
        } else {
            wait--;
            if (wait < 0) {
                wait = 0;
            }
        }
        mwPlySetBright(event_movie_player, brightness);
        mwPlyExecServer();
        njWaitVSync();
    }
#ifdef NONMATCHING
    /* GCC closes the player in finish_movie, which runs even where open_stream
     * fails */
#else
    event_movie_player->vtbl->Stop(event_movie_player);
    event_movie_player->vtbl->Destroy(event_movie_player);
    mwPlyEntryErrFunc(event_movie_record_error, &event_movie_player);
#endif
}

static Sint32 open_stream(struct event_movie_request *request)
{
    MWS_PLY_INIT_SFD display;
    MWE_PLY_STAT status;

    (void)unk_08;
    stream_failed = FALSE;
    if (event_movie_draw_plane) {
        load_plane();
    }
    memset(&display, 0, sizeof(display));
    EVENT_SET_DISPLAY_MODE(display.mode, FULLSCREEN_PAL)
    display.frame = NJD_FRAMEBUFFER_MODE_ARGB8888;
    display.count = FRAME_BLANKS;
    display.latency = 2;
    mwPlyInitSofdec(&display);
    memset(&event_movie_parameters, 0, sizeof(event_movie_parameters));
    event_movie_parameters.ftype = MWD_PLY_FTYPE_SFD;
    event_movie_parameters.dtype = MWD_PLY_DTYPE_AUTO;
    event_movie_parameters.max_bps = 0x546000;
    event_movie_parameters.max_width = 480;
    event_movie_parameters.max_height = 480;
    event_movie_parameters.nfrm_pool_wk = 6;
    event_movie_parameters.wksize = mwPlyCalcWorkSofdec(
        event_movie_parameters.ftype, event_movie_parameters.max_bps,
        event_movie_parameters.max_width, event_movie_parameters.max_height,
        event_movie_parameters.nfrm_pool_wk);
    event_movie_parameters.compo_mode = MWD_PLY_COMPO_OPEQ;
    event_movie_parameters.work = (Sint8 *)EVENT_SCENE_AREA;
    event_movie_player = mwPlyCreateSofdec(&event_movie_parameters);
    if (event_movie_player == NULL) {
        if (event_debug_print) {
            njPrintC(NJM_LOCATION(3, 10), "Akan! Handol ga kurieito dekihen!.");
        }
        njWaitVSync();
        /* @bug memory: a failed create goes on to read the null player's vtbl
         * at 0x8c5087b6 */
#ifdef NONMATCHING
        /* this side repairs the tag above: it returns -1 where the player
         * can't be made, and event_movie_play then plays no movie */
        return -1;
#endif
    }
    mwPlySetVideoSw(event_movie_player, TRUE);
    mwPlyEntryErrFunc(event_movie_record_error, &event_movie_player);
    event_movie_player->vtbl->StartFname(event_movie_player,
                                         (Sint8 *)request->name);
    mwPlySetAudioSw(event_movie_player, TRUE);
    stream = sfd_hn_last;
#ifdef NONMATCHING
    /* this side returns -1 where the stream didn't open, which the matching
     * side sets through the null */
    if (stream == NULL) {
        return -1;
    }
#endif
    SFD_SetVideoCh(stream, 0);
    SFD_SetAudioCh(stream, game_state.voice_language > VOICE_JAPANESE
                               ? ENGLISH_CHANNEL
                               : JAPANESE_CHANNEL);
    SFD_SetErrFn(stream, note_stream_error, NULL);
    if (request->fade_in > 0) {
        brightness = 0;
    } else {
        brightness = FULL_BRIGHTNESS;
    }
    event_movie_player->vtbl->Pause(event_movie_player, FALSE);
    for (;;) {
        njWaitVSync();
        mwPlyStartFrame();
#ifdef NONMATCHING
        /* this side returns -1 for a stream that fails before it plays. the
         * matching side waits on such a stream for good */
        status = event_movie_player->vtbl->GetStat(event_movie_player);
        if (stream_failed ||
            (status != MWE_PLY_STAT_PREP && status != MWE_PLY_STAT_PLAYING)) {
            return -1;
        }
        if (status == MWE_PLY_STAT_PLAYING) {
            break;
        }
#else
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((status = event_movie_player->vtbl->GetStat(event_movie_player)) ==
            MWE_PLY_STAT_PLAYING) {
            break;
        }
        if (status == MWE_PLY_STAT_ERROR) {
            for (;;) {
                njWaitVSync();
            }
        }
#endif
        mwPlySetBright(event_movie_player, brightness);
        mwPlyExecServer();
    }
    event_movie_player->vtbl->Pause(event_movie_player, TRUE);
    return 0;
}

static void set_display(Bool movie_mode)
{
    int mode;
#ifdef NONMATCHING
    /* this side starts mode at VGA, so that it holds a resolution on each path
     * the compiler sees. no path keeps it: game_state.display holds only
     * displays that EVENT_SET_DISPLAY_MODE names */
    mode = NJD_RESOLUTION_VGA;
#endif
    njWaitVSync();
    njReleaseTextureAll();
    njExitPrint();
    /* @hack spelling: movie_mode compared with TRUE, where movie_mode alone
     * would do */
    if (movie_mode == TRUE) {
        EVENT_SET_DISPLAY_MODE(mode, FULLSCREEN_PAL)
        njWaitVSync();
        njWaitVSync();
        /* @bug uninitialized: a display EVENT_SET_DISPLAY_MODE doesn't name
         * leaves mode unset, and 0x8c50895a passes it */
        njChangeSystem(mode, NJD_FRAMEBUFFER_MODE_ARGB8888, FRAME_BLANKS);
        njInitVertexBuffer(OPAQUE_WORDS, OPAQUE_MODIFIER_WORDS,
                           TRANSLUCENT_WORDS, TRANSLUCENT_MODIFIER_WORDS,
                           PUNCH_THROUGH_WORDS);
    } else {
        EVENT_SET_DISPLAY_MODE(mode, EVENT_MOVIE_PAL)
        njWaitVSync();
        njWaitVSync();
        /* @bug uninitialized: the same unset mode reaches the change back at
         * 0x8c508994 */
        njChangeSystem(mode, NJD_FRAMEBUFFER_MODE_RGB565, FRAME_BLANKS);
        njInitVertexBuffer(OPAQUE_WORDS, OPAQUE_MODIFIER_WORDS,
                           TRANSLUCENT_WORDS, TRANSLUCENT_MODIFIER_WORDS,
                           PUNCH_THROUGH_WORDS);
    }
    njInitPrint(NULL, 0, 0);
    njPrintSize(PRINT_SIZE);
}

static void movie_vblank(void)
{
    sound_update();
    buExecServer();
    loader_poll_drive();
}

static void finish_movie(void)
{
#ifdef NONMATCHING
    /* this side stops and destroys the player, so that a later movie makes its
     * own */
    if (event_movie_player != NULL) {
        event_movie_player->vtbl->Stop(event_movie_player);
        event_movie_player->vtbl->Destroy(event_movie_player);
        event_movie_player = NULL;
        stream = NULL;
        mwPlyEntryErrFunc(event_movie_record_error, &event_movie_player);
    }
#endif
    mwPlyFinishSofdec();
#ifdef NONMATCHING
    /* this side's release_plane tests the plane itself */
    release_plane();
#else
    if (event_movie_draw_plane) {
        release_plane();
    }
#endif
}

#ifdef NONMATCHING
/* this side returns whether the movie played, as movie.h says */
Bool event_movie_play(struct event_movie_request *request)
#else
void event_movie_play(struct event_movie_request *request)
#endif
{
#ifdef NONMATCHING
    /* this side plays nothing for a request without a file */
    Bool played = FALSE;
    if (request == NULL || request->name == NULL ||
        loader_file_size(request->name) <= 0) {
        return FALSE;
    }
#endif
    syCacheICI();
    njWaitVSync();
    njSetVSyncFunction(movie_vblank);
    njWaitVSync();
    set_display(TRUE);
    sound_fx_off();
    sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE_DMA);
    if (open_stream(request) >= 0) {
        play_stream(request);
#ifdef NONMATCHING
        /* this side's movie played where the stream never failed */
        played = !stream_failed;
#endif
    }
    finish_movie();
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    set_display(FALSE);
    sound_fx_on();
#ifdef NONMATCHING
    /* this side returns whether the movie played */
    return played;
#endif
}

static void load_plane(void)
{
#ifdef NONMATCHING
    /* this side reads the plane bounded by DEMO_AREA's room, and checks it
     * before it takes it. it draws none where the read or a check fails */
    struct tails_plane *candidate = (struct tails_plane *)DEMO_AREA;
    Sint32 size;

    release_plane();
    size =
        loader_read_prs_checked("tailsPlain.prs", DEMO_AREA, DEMO_AREA_BYTES);
    if (size < (Sint32)sizeof(*candidate) ||
        !event_file_texlist(DEMO_AREA, size, candidate->textures) ||
        !event_file_records(DEMO_AREA, size, candidate->plane, 1,
                            sizeof(*candidate->plane)) ||
        !event_file_records(DEMO_AREA, size, candidate->flight, 1,
                            sizeof(*candidate->flight)) ||
        !event_file_records(DEMO_AREA, size, candidate->camera, 1,
                            sizeof(*candidate->camera))) {
        return;
    }
    size = loader_read_file_checked("tailsPlain.pvm", WORK_AREA,
                                    WORK_AREA_END - WORK_AREA);
    if (size <= 0 || !texture_load_pvm_memory(WORK_AREA, (Uint32)size,
                                              candidate->textures)) {
        return;
    }
    plane = candidate;
#else
    loader_read_prs("tailsPlain.prs", DEMO_AREA);
    plane = (struct tails_plane *)DEMO_AREA;
    njLoadTexturePvmFile("tailsPlain.pvm", plane->textures);
#endif
    plane_frame = 0.0F;
    simple_light_set_direction(1.0F, 0.0F, 0.0F);
    njCnkSetSimpleLightIntensity(1.0F, 0.2F);
    njCnkSetSimpleLightColor(1.0F, 1.0F, 1.0F);
}

static void release_plane(void)
{
#ifdef NONMATCHING
    /* this side releases a plane only where one loaded, and forgets it */
    if (plane != NULL) {
        texture_discard_pvm(plane->textures);
        plane = NULL;
    }
#else
    njReleaseTexture(plane->textures);
#endif
}

static void draw_plane(void)
{
#ifdef NONMATCHING
    /* this side draws no plane that didn't load */
    if (plane == NULL) {
        return;
    }
#endif
    njSetTexture(plane->textures);
    njCameraMotion(&plane_camera, plane->camera, plane_frame);
    njCnkSimpleDrawMotion(plane->plane, plane->flight, plane_frame);
    plane_frame += PLANE_FRAME_STEP;
}
