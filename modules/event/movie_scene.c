#include <ninja.h>
#include <sfdfx_mw.h>
#include <sg_mwply.h>
#include <sg_xpt.h>
#include <string.h>

#include "event/movie.h"
#include "event/script.h"

#define LARGE_MOVIE_WORK ((Sint8 *)0x8c880000)
#define SMALL_MOVIE_WORK ((Sint8 *)0x8c9c0000)
#define MOVIE_ALPHAS 256
/* each frame lasts FRAME_BLANKS vertical blanks */
#define FRAME_BLANKS 1
/* the debug prints of a failed start, from (ERROR_X, ERROR_Y) */
#define ERROR_X 5
#define ERROR_Y 10

MWPLY event_movie_player = NULL;
Bool event_movie_scene_active;
MWS_PLY_CPRM_SFD event_movie_parameters;
static Sint32 largest_kind;
static char last_error[256];

static Sint32 largest_scene_kind(void)
{
    Sint32 i;
    Sint32 largest = 0;
    for (i = 0; i < EVENT_MOVIE_CUES; i++) {
        if (largest < event_tracks->movies[i].kind) {
            if (event_tracks->movies[i].frame != 0) {
                largest = event_tracks->movies[i].kind;
            }
        }
    }
    return largest;
}

void event_movie_start_scenes(void)
{
    MWS_PLY_INIT_SFD display;
    Float alpha[MOVIE_ALPHAS];
    Sint32 i;
    largest_kind = largest_scene_kind();
    if (largest_kind == 0) {
        return;
    }
    memset(&display, 0, sizeof(display));
    EVENT_SET_DISPLAY_MODE(display.mode, EVENT_MOVIE_PAL)
    display.frame = NJD_FRAMEBUFFER_MODE_RGB565;
    display.count = FRAME_BLANKS;
    display.latency = 2;
    mwPlyInitSofdec(&display);

    memset(&event_movie_parameters, 0, sizeof(event_movie_parameters));
    event_movie_parameters.ftype = MWD_PLY_FTYPE_MPV;
    event_movie_parameters.dtype = MWD_PLY_DTYPE_AUTO;
    event_movie_parameters.max_bps = 0x384000;
    event_movie_parameters.max_width = 320;
    event_movie_parameters.max_height = 480;
    if (largest_kind == MOVIE_MASKED) {
        event_movie_parameters.nfrm_pool_wk = 6;
        event_movie_parameters.compo_mode = MWD_PLY_COMPO_LUMI;
        event_movie_parameters.work = LARGE_MOVIE_WORK;
    } else {
        event_movie_parameters.nfrm_pool_wk = 3;
        event_movie_parameters.compo_mode = MWD_PLY_COMPO_ADD;
        event_movie_parameters.work = SMALL_MOVIE_WORK;
    }
    event_movie_parameters.wksize =
        mwPlyCalcWorkCprmSfd(&event_movie_parameters);
    if (event_movie_parameters.work == NULL) {
        for (;;) {
            if (event_debug_print) {
                njPrintC(NJM_LOCATION(ERROR_X, ERROR_Y),
                         "Akan! Malloc dekihen.");
            }
            njPrint(NJM_LOCATION(ERROR_X, ERROR_Y + 1), "size = %8x",
                    (unsigned)event_movie_parameters.wksize);
            njWaitVSync();
        }
    }
    event_movie_player = mwPlyCreateSofdec(&event_movie_parameters);
    if (event_movie_player == NULL) {
        for (;;) {
            if (event_debug_print) {
                njPrintC(NJM_LOCATION(ERROR_X, ERROR_Y),
                         "Sofdec no nazono era-.");
            }
            njWaitVSync();
        }
    }
    for (i = 0; i < MOVIE_ALPHAS; i++) {
        alpha[i] = i * (1.0F / MOVIE_ALPHAS);
    }
    mwPlySetAlphTbl(event_movie_player, 0, alpha);
    mwPlyEntryErrFunc(event_movie_record_error, &event_movie_player);
    event_movie_scene_active = FALSE;
    event_movie_texture_active = FALSE;
}

void event_movie_record_error(void *obj, char *message)
{
    strncpy(last_error, message, sizeof(last_error));
    /* @hack redundant: the return at the end of a function that returns
     * nothing */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void event_movie_stop_scenes(void)
{
    if (largest_kind == 0) {
        return;
    }
    event_movie_player->vtbl->Stop(event_movie_player);
    event_movie_player->vtbl->Destroy(event_movie_player);
    event_movie_scene_active = FALSE;
    event_movie_texture_active = FALSE;
    mwPlyFinishSofdec();
}

static void step_scene(void)
{
    MWE_PLY_STAT status;
    status = event_movie_player->vtbl->GetStat(event_movie_player);
    if (status == MWE_PLY_STAT_PLAYEND || status == MWE_PLY_STAT_ERROR) {
        event_movie_player->vtbl->Stop(event_movie_player);
        event_movie_scene_active = FALSE;
    }
    mwPlyExecTexSvr();
}

void event_movie_step(void)
{
    if (event_movie_scene_active) {
        step_scene();
    }
    if (event_movie_texture_active) {
        event_movie_step_texture();
    }
}

void event_movie_draw(void)
{
    if (event_movie_scene_active) {
        mwPlyExecDrawSvr();
    }
}

void event_movie_pause(Bool paused)
{
    /* @hack spelling: the flags joined by |, where || would do */
    if (event_movie_scene_active | event_movie_texture_active) {
        event_movie_player->vtbl->Pause(event_movie_player, paused);
    }
}

void event_movie_start_scene(struct event_movie_scene *request)
{
    /* @hack spelling: the flags joined by | and compared with 0, where ||
     * would do for flags that hold TRUE or FALSE */
    if ((event_movie_scene_active | event_movie_texture_active) > 0) {
        return;
    }
    mwPlySetVideoSw(event_movie_player, TRUE);
    switch (request->draw.screen.kind) {
    case MOVIE_MASKED:
        mwPlySetCompoMode(event_movie_player, MWD_PLY_COMPO_LUMI);
        break;
    case MOVIE_OPAQUE:
        mwPlySetCompoMode(event_movie_player, MWD_PLY_COMPO_OPEQ);
        break;
    default:
        mwPlySetCompoMode(event_movie_player, MWD_PLY_COMPO_ADD);
        break;
    }
    event_movie_scene_active = TRUE;
    mwPlySetDispPos(event_movie_player, request->position.x,
                    request->position.y);
    mwPlySetDispZ(event_movie_player, request->draw.screen.depth);
    event_movie_player->vtbl->StartFname(event_movie_player,
                                         (Sint8 *)request->name);
}
