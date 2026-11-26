/* STAGE_VERSUS_SONIC_1, the first fight of Sonic and Shadow: its init and
 * exit, and its tasks' steps. the background task, BgExec, draws the
 * background and its water at the current camera's x and z */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg19/versus_sonic_1/text.inc"

#include "camera/bosspoint.h"
#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "player/sonic.h"
#include "shared/big_splash.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/green_forest_sounds.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "stg19/rival_shadow.h"
#include "stg19/versus_sonic_1.h"
#include "stg19/object_archives.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

#ifdef NONMATCHING
/* the declarations of jungle_prop_texlists and vine_preview_texlist, which this
 * side's texture load reads */
#include "shared/jungle_prop.h"
#include "shared/looping_vine.h"
#endif

static struct fog_setting versus_sonic_1_fog;

/* the set file's block has room for at least SET_ENTRIES entries */
#define SET_ENTRIES 512
#define BACKGROUND_LEVEL 1
#define BACKGROUND_HEIGHT 920.0F
#define BACK_COLOR 0xff001020
/* the light both players take */
#define PLAYER_LIGHT 0
/* the light the camera hands Ninja, which light_select_camera sets */
#define CAMERA_LIGHT 0

/* the water's animation runs through WATER_FRAMES frames, a frame a step.
 * the water model's plist holds its texture's index in the halfword at
 * WATER_TID_AT. that halfword is the second of the model's NJD_CT_TID
 * chunk */
#define WATER_FRAMES 14
#define WATER_TID_AT 5
#define WATER_FRAME(work) ((work)->timer)

#define CAMERA_DIST 50.0F
#define CAMERA_HEIGHT 50.0F
#define CAMERA_PART 0.5F
#define CAMERA_LIFT 20.0F
#define CAMERA_RISE 10.0F
enum versus_sonic_1_mode { MODE_START, MODE_FOLLOW };

/* the names the texture lists below point at. each starts at a multiple of
 * 4. the Uint32 aligns sky, and each array's size is a multiple of 4 */
struct arena_sky_names {
    union {
        char text[12];
        Uint32 alignment;
    } sky;
    char horizon[12];
    char water[16];
};
static const struct arena_sky_names sky_names;

static NJS_TEXNAME jungle_background_model_textures[2] = {
#include "stg19/versus_sonic_1/jungle_background_model_textures.inc"
};

static NJS_TEXLIST jungle_background_model_texlist = {
#include "stg19/versus_sonic_1/jungle_background_model_texlist.inc"
};

#include "shared/jungle_sky_shape.h"

static NJS_TEXNAME background_water_textures[1] = {
#include "stg19/versus_sonic_1/background_water_textures.inc"
};

static NJS_TEXLIST background_water_texlist = {
#include "stg19/versus_sonic_1/background_water_texlist.inc"
};

#include "shared/jungle_water_shape.h"

static NJS_TEXNAME arena_sky_textures[2] = {
#include "stg19/versus_sonic_1/arena_sky_textures.inc"
};
static NJS_TEXLIST arena_sky_texlist = {
#include "stg19/versus_sonic_1/arena_sky_texlist.inc"
};

#include "stg19/water_floor.h"

/* a player who meets the kill area's water misses */
static struct kill_area versus_sonic_1_kill_areas[2] = {
#include "stg19/versus_sonic_1/versus_sonic_1_kill_areas.inc"
};
static struct light versus_sonic_1_lights[2] = {
    {{0.0F, -0.707F, 0.707F}, 1.0F, 0.2F, 1.0F, 1.0F, 1.0F},
    {{0.0F, -0.707F, 0.707F}, 0.5F, 0.2F, 0.5F, 0.5F, 0.5F},
};
/* the set file's block, or null */
static struct set_head *versus_sonic_1_set = NULL;

static void background_step(struct task *task);
static void background_draw(struct task *task);

void versus_sonic_1_init(void)
{
    struct task *task;
#ifdef NONMATCHING
    /* for this side's texture load below: the slots it may leave empty, and
     * jungle prop 17's texture list */
    const NJS_TEXNAME *optional[2];
    const NJS_TEXLIST *prop;
#endif

    if (stage_character(0) == PLAYER_SONIC) {
        shadow_create(1);
    } else if (stage_character(0) == PLAYER_SHADOW) {
        sonic_create(1);
    }
    if (player_tasks[1]) {
        stage_place_player(1);
        player_work[1]->id = WORK_ID_RIVAL;
        pad_disable_player(1);
        rival_shadow_create();
    }
    world_land_start(&versus_sonic_1_land);
#ifdef NONMATCHING
    /* The duel's archives omit the vine editor preview and jungle prop 17.
     * Its two SET files place other props from the same group. */
    prop = jungle_prop_texlists[17];
    optional[0] = vine_preview_texlist.textures;
    optional[1] = prop != NULL ? prop->textures : NULL;
    texture_load_shared_with_optional(versus_sonic_1_object_pvms,
                                      versus_sonic_1_object_texture_groups,
                                      optional, 2);
#else
    texture_load_shared(versus_sonic_1_object_pvms,
                        versus_sonic_1_object_texture_groups);
#endif
    texture_load_pvm(JUNGLE_BACKGROUND_ARCHIVE, &arena_sky_texlist);
    versus_sonic_1_set =
        set_read("set0019_s.bin", "set0019_u.bin", SET_ENTRIES);
    set_start(&versus_sonic_1_objects, versus_sonic_1_set);
    loader_read_camera_files();
    particle_core_start();
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        task->draw = background_draw;
    }
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &versus_sonic_1_lights[0]);
    light_set(1, &versus_sonic_1_lights[1]);
    light_set(2, &versus_sonic_1_lights[0]);
    light_set(3, &versus_sonic_1_lights[0]);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(CAMERA_LIGHT);
    light_select_after_draw(LIGHT_LAND);
    fog_start_file("stg19_fog.bin", &versus_sonic_1_fog);
    njFogEnable();
    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    world_build_land(&versus_sonic_1_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    kill_areas_start(versus_sonic_1_kill_areas);
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (!player_hooks.splash) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    music_fade_in(0);
    music_play_or_defer("BOSS_04.ADX");
    messenger_messages_load("mh0019");
    sound_load_stage("se_ac_gf.mlt", &green_forest_sound_list);
}

static const struct arena_sky_names sky_names = {
#include "stg19/versus_sonic_1/sky_names.inc"
};

/* SHC places this byte-aligned record on a word boundary. Keep each
 * filename at the same alignment in the relocated land table. */
#ifdef NONMATCHING
/* this side aligns the record to 4 bytes, as SHC places it */
__attribute__((aligned(4)))
#endif
const struct duel_land_texture_names versus_sonic_1_land_names = {
#include "stg19/versus_sonic_1/versus_sonic_1_land_names.inc"
};

void versus_sonic_1_exit(void)
{
    messenger_messages_free();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    fog_stop();
    njFogDisable();
    particle_core_stop();
    njReleaseTexture(&arena_sky_texlist);
    texture_release_shared(versus_sonic_1_object_pvms,
                           versus_sonic_1_object_texture_groups);
    set_end();
    if (versus_sonic_1_set) {
        loader_free(versus_sonic_1_set);
        versus_sonic_1_set = NULL;
    }
    world_land_stop();
}

void versus_sonic_1_step(struct task *task)
{
    struct task_work *work = task->work;
    struct camera_bosspoint camera;

    switch (work->mode) {
    case MODE_START:
        camera.target = &work->pos;
        camera.dist = CAMERA_DIST;
        camera.height = CAMERA_HEIGHT;
        camera.part = CAMERA_PART;
        camera.adjust = CAMERA_ADJUST_RELATIVE3;
        camera.collides = 1;
        camera.trigger_push = 0;
        camera.lift = CAMERA_LIFT;
        camera.level = 0;
        camera_bosspoint_start(0, &camera);
        work->mode = MODE_FOLLOW;
        /* fall through */
    case MODE_FOLLOW:
        if (player_work[1]) {
            work->pos = player_work[1]->pos;
            work->pos.y += CAMERA_RISE;
        }
        break;
    }
}

static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    Sint16 *tid;
    Sint16 flags;
    Sint16 frame;

    /* @hack spelling: the frame's step as an assignment, where ++ would do */
    WATER_FRAME(work) = WATER_FRAME(work) + 1;
    if (WATER_FRAME(work) >= WATER_FRAMES) {
        WATER_FRAME(work) = 0;
    }
    tid = jungle_background_water_model->plist + WATER_TID_AT;
    /* @hack spelling: flags and frame, locals for the halfword's steps,
     * where one masked store would do */
    flags = *tid;
    flags = flags & NJD_FFM_MASK;
    frame = flags | WATER_FRAME(work);
    *tid = frame;
}

static void background_draw(struct task *task)
{
    NJS_VECTOR pos;

    njFogEnable();
    njSetTexture(&arena_sky_texlist);
    njPushMatrixEx();
    pos.x = camera_views[camera_current_index]->pos.x;
    pos.y = BACKGROUND_HEIGHT;
    pos.z = camera_views[camera_current_index]->pos.z;
    njTranslateEx(&pos);
    njCnkSimpleDrawModel(&jungle_background_model);
    njSetTexture(&versus_sonic_1_water_texlist);
    njCnkSimpleDrawModel(jungle_background_water_model);
    njPopMatrixEx();
}
