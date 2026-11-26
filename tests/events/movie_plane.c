#include <assert.h>
#include <string.h>
#include <ninja.h>
#include <sfdfx_mw.h>
#include <sg_sd.h>
#include <shinobi.h>
#include "system/areas.h"
#include "system/frame.h"
#include "sound/sound.h"

/* The fixture has no display, instruction cache, or audio hardware. */
#undef syCacheICI
#define syCacheICI() ((void)0)
#define njWaitVSync() ((void)0)
#define njSetVSyncFunction(callback) ((void)0)
#define njInitVertexBuffer(op, om, tp, tm, pt) ((void)0)
#define njReleaseTextureAll() ((void)0)
#define njExitPrint() ((void)0)
#define njInitPrint(p, n, a) ((void)0)
#define njPrintSize(n) ((void)0)
#define njPrintC(at, message) ((void)0)
#define njPrint(at, format, value) ((void)0)
#define sdMemBlkSetTransferMode(mode) ((void)0)
#define sound_fx_off() ((void)0)
#define sound_fx_on() ((void)0)
#define frame_pacing_init(interval, steps) ((void)0)
#define mwPlySetVideoSw(handle, on) ((void)0)
#define mwPlySetAudioSw(handle, on) ((void)0)
#define mwPlySetBright(handle, value) ((void)0)
#define mwPlyStartFrame() ((void)0)
#define mwPlyCalcWorkSofdec(type, bps, width, height, frames) 0

static union {
    double alignment;
    Uint8 bytes[2048];
} demo;
static Uint8 work[128];

#undef DEMO_AREA
#undef DEMO_AREA_BYTES
#undef WORK_AREA
#undef WORK_AREA_END
#define DEMO_AREA demo.bytes
#define DEMO_AREA_BYTES sizeof(demo.bytes)
#define WORK_AREA work
#define WORK_AREA_END (work + sizeof(work))
#include "../../modules/event/fullscreen.c"

struct plane_file {
    struct tails_plane header;
    NJS_TEXLIST list;
    NJS_TEXNAME slots[2];
    NJS_CNK_OBJECT object;
    NJS_MOTION flight, camera;
};

static Sint32 model_bytes, texture_bytes;
static Bool upload_ok;
static int texture_reads, uploads, light_calls, draws, releases, finishes;
static NJS_TEXLIST *owner;

Sint32 loader_read_prs_checked(const char *name, void *dst, Uint32 capacity)
{
    assert(strcmp(name, "tailsPlain.prs") == 0);
    assert(dst == DEMO_AREA && capacity == DEMO_AREA_BYTES);
    assert(owner == NULL);
    return model_bytes;
}

Sint32 loader_read_file_checked(const char *name, void *dst, Uint32 capacity)
{
    assert(strcmp(name, "tailsPlain.pvm") == 0);
    assert(dst == WORK_AREA && capacity == sizeof(work));
    ++texture_reads;
    return texture_bytes;
}

Bool texture_load_pvm_memory(const void *archive, Uint32 size,
                             NJS_TEXLIST *target)
{
    struct plane_file *file = (struct plane_file *)DEMO_AREA;
    assert(archive == work && size == (Uint32)texture_bytes);
    assert(target == &file->list && owner == NULL);
    ++uploads;
    if (upload_ok) {
        owner = target;
    }
    return upload_ok;
}

void texture_discard_pvm(NJS_TEXLIST *target)
{
    assert(owner != NULL && target == owner);
    owner = NULL;
    ++releases;
}

void simple_light_set_direction(Float x, Float y, Float z)
{
    assert(owner && x == 1.0F && y == 0.0F && z == 0.0F);
    ++light_calls;
}

void njCnkSetSimpleLightIntensity(Float intensity, Float ambient)
{
    assert(owner && intensity == 1.0F && ambient == 0.2F);
    ++light_calls;
}

void njCnkSetSimpleLightColor(Float r, Float g, Float b)
{
    assert(owner && r == 1.0F && g == 1.0F && b == 1.0F);
    ++light_calls;
}

Sint32 njSetTexture(NJS_TEXLIST *target)
{
    assert(owner && target == owner);
    return 1;
}

void njCameraMotion(NJS_CAMERA *camera, NJS_MOTION *motion, Float frame)
{
    struct plane_file *file = (struct plane_file *)DEMO_AREA;
    assert(owner && camera == &plane_camera && motion == &file->camera);
    assert(frame == draws * 0.5F);
}

void njCnkSimpleDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                           Float frame)
{
    struct plane_file *file = (struct plane_file *)DEMO_AREA;
    assert(owner && object == &file->object && motion == &file->flight);
    assert(frame == draws * 0.5F);
    ++draws;
}

void mwPlyFinishSofdec(void)
{
    ++finishes;
}

static void check_plane(void)
{
    struct plane_file *file = (struct plane_file *)DEMO_AREA;
    file->header.textures = &file->list;
    file->header.plane = &file->object;
    file->header.flight = &file->flight;
    file->header.camera = &file->camera;
    file->list.textures = file->slots;
    file->list.nbTexture = 2;
    texture_bytes = 13;
    upload_ok = TRUE;
    /* The failed read leaves the previous address words in the buffer. */
    model_bytes = -1;
    load_plane();
    draw_plane();
    release_plane();
    model_bytes = sizeof(struct tails_plane) - 1;
    load_plane();
    model_bytes = sizeof(*file) - 1;
    load_plane();
    model_bytes = sizeof(*file);
    file->list.textures = (NJS_TEXNAME *)(DEMO_AREA + model_bytes);
    load_plane();
    file->list.textures = file->slots;
    draw_plane();
    assert(!plane && !owner && !texture_reads && !uploads && !light_calls &&
           !draws);

    texture_bytes = -1;
    load_plane();
    assert(texture_reads == 1 && !uploads);
    texture_bytes = 13;
    upload_ok = FALSE;
    load_plane();
    draw_plane();
    finish_movie();
    assert(!plane && !owner && uploads == 1 && !releases && !light_calls &&
           !draws);
    assert(finishes == 1);

    upload_ok = TRUE;
    load_plane();
    draw_plane();
    draw_plane();
    assert(plane && owner && light_calls == 3 && draws == 2);
    /* A replacement must release its owner before a read can overwrite it. */
    model_bytes = -1;
    load_plane();
    draw_plane();
    finish_movie();
    assert(!plane && !owner && releases == 1 && finishes == 2 && draws == 2);
    model_bytes = sizeof(*file);
    load_plane();
    assert(plane_frame == 0.0F && light_calls == 6);
    event_movie_draw_plane = FALSE;
    finish_movie();
    release_plane();
    assert(!plane && !owner && releases == 2 && finishes == 3);
}

MWPLY event_movie_player;
MWS_PLY_CPRM_SFD event_movie_parameters;
struct sfd_handle *sfd_hn_last;
struct game_state game_state;
Bool event_debug_print, event_reset_pressed;
static PDS_PERIPHERAL pad;
const PDS_PERIPHERAL *player_pads[PAD_PORTS] = {&pad};

static MWD_IF vtable;
static MW_PLY_OBJ player = {&vtable};
static Sint32 movie_bytes;
static Bool create_ok, decoder_error;
static int display_changes, starts, stopped, destroyed, checks;
static MWE_PLY_STAT first, second;
static void (*report_error)(void *, Sint32);

Sint32 loader_file_size(const char *name)
{
    assert(strcmp(name, "movie.sfd") == 0);
    return movie_bytes;
}

void njChangeSystem(Int mode, Int frame, Int count)
{
    ++display_changes;
}

void mwPlyInitSofdec(MWS_PLY_INIT_SFD *display)
{
    assert(display->count == 1);
}

MWPLY mwPlyCreateSofdec(MWS_PLY_CPRM_SFD *params)
{
    assert(params == &event_movie_parameters);
    return create_ok ? &player : NULL;
}

void mwPlyEntryErrFunc(MW_PLY_ERRFN report, void *user)
{
    assert(report == event_movie_record_error && user == &event_movie_player);
}

void event_movie_record_error(void *user, char *message)
{
}

static void start(MWPLY handle, Sint8 *name)
{
    assert(handle == &player && strcmp((char *)name, "movie.sfd") == 0);
    ++starts;
    checks = 0;
}

static void stop(MWPLY handle)
{
    assert(handle == &player && stopped == destroyed);
    ++stopped;
}

static void destroy(MWPLY handle)
{
    assert(handle == &player && stopped == destroyed + 1);
    ++destroyed;
}

static MWE_PLY_STAT status(MWPLY handle)
{
    assert(handle == &player && checks < 3);
    return ++checks == 1 ? first : second;
}

static void pause_movie(MWPLY handle, Sint32 paused)
{
    assert(handle == &player);
}

Sint32 SFD_SetErrFn(struct sfd_handle *handle, void (*report)(void *, Sint32),
                    void *user)
{
    assert(handle == sfd_hn_last);
    report_error = report;
    return 0;
}

Sint32 SFD_SetAudioCh(struct sfd_handle *handle, Sint32 channel)
{
    return 0;
}

Sint32 SFD_SetVideoCh(struct sfd_handle *handle, Sint32 channel)
{
    return 0;
}

void pad_scan(void)
{
}

Bool pad_reset_pressed(int unused)
{
    return FALSE;
}

void mwPlyExecServer(void)
{
    if (decoder_error) {
        report_error(NULL, -1);
    }
}

static void check_movie(void)
{
    struct event_movie_request request = {"movie.sfd", 1, 255};
    vtable.StartFname = start;
    vtable.Stop = stop;
    vtable.Destroy = destroy;
    vtable.GetStat = status;
    vtable.Pause = pause_movie;
    sfd_hn_last = (struct sfd_handle *)&player;
    event_movie_draw_plane = TRUE;
    /* Stale optional plane bytes must not prevent a valid movie. */
    model_bytes = -1;
    assert(!event_movie_play(&request) && !display_changes && !starts);
    movie_bytes = 1;
    assert(!event_movie_play(&request) && display_changes == 2 && !starts);
    create_ok = TRUE;
    first = MWE_PLY_STAT_ERROR;
    assert(!event_movie_play(&request) && checks == 1);
    first = MWE_PLY_STAT_PLAYING;
    second = MWE_PLY_STAT_ERROR;
    assert(!event_movie_play(&request) && checks == 2);
    first = MWE_PLY_STAT_PREP;
    decoder_error = TRUE;
    assert(!event_movie_play(&request) && checks == 2);
    first = MWE_PLY_STAT_PLAYING;
    second = MWE_PLY_STAT_PLAYING;
    assert(!event_movie_play(&request) && checks == 2);
    decoder_error = FALSE;
    second = MWE_PLY_STAT_PLAYEND;
    assert(event_movie_play(&request) && checks == 2);
    assert(starts == 5 && stopped == 5 && destroyed == 5);
    assert(!event_movie_player && !stream && !plane && !owner);
    assert(display_changes == 12);
}

int main(void)
{
    check_plane();
    check_movie();
    return 0;
}
