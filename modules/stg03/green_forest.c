/* Green Forest, the stage of STG03: its start and end, and its tasks' steps.
 * the background task, BgExec, draws the background and the water about the
 * current camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg03/green_forest/text.inc"

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/common_object.h"
#include "shared/green_forest_sounds.h"
#include "shared/hint_box.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/splash.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg03/green_forest.h"
#include "stg03/stars.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* the set file's block has room for at least SET_ENTRIES entries */
#define SET_ENTRIES 2048
#define BACKGROUND_LEVEL 1
/* the points of each player's stars: STAR_POINTS in a one-player game, and
 * STAR_POINTS_TWO_PLAYERS in a two-player game */
#define STAR_POINTS 1800
#define STAR_POINTS_TWO_PLAYERS 900

/* the water's animation runs through WATER_FRAMES frames, a frame a step
 * while time moves. a water model's plist holds its texture's index in the
 * halfword at WATER_TID_AT. that halfword is the second of the model's
 * NJD_CT_TID chunk */
#define WATER_FRAMES 14
#define WATER_TID_AT 5
/* the water's frame, which runs only while time moves */
#define WATER_FRAME(work) ((work)->timer)

/* the background task's mode holds BG_LOWERED and BG_NO_WATER for each of
 * cameras 0 and 1. BG_NO_WATER's bits lie past BG_LOWERED's. a camera whose x
 * is over LOWERED_X and whose z is over LOWERED_Z takes BG_LOWERED. the
 * background then draws LOWERED_DROP along y for that camera. BG_NO_WATER
 * leaves out the water model drawn with the background */
#define BG_LOWERED(camera) (1 << (camera))
#define BG_NO_WATER(camera) ((1 << BG_CAMERAS) << (camera))
#define BG_CAMERAS 2
#define LOWERED_X 4420.394F
#define LOWERED_Z 4940.249F
#define LOWERED_DROP (-1341.91F)
/* a camera takes BG_NO_WATER within NO_WATER_RANGE of the line through
 * no_water_start and no_water_end. it takes it too within NO_WATER_RANGE of
 * no_water_center. that second test needs its y below NO_WATER_BELOW_Y and
 * its x below NO_WATER_BELOW_X */
#define NO_WATER_RANGE 50.0F
#define NO_WATER_BELOW_Y (-950.0F)
#define NO_WATER_BELOW_X 1017.0F
/* a piece of the water draws while its z in the camera's space lies between
 * WATER_NEAR and WATER_FAR */
#define WATER_NEAR (-300.0F)
#define WATER_FAR 1000.0F

#include "stg03/paths.h"
#include "stg03/background.h"
#include "stg03/kill_meshes.h"

static struct kill_area green_forest_kill_areas[9] = {
#include "stg03/green_forest/green_forest_kill_areas.inc"
};
static Bool background_compiled = FALSE;
#define BACKGROUND_BUILDS 1
static struct model_build background_builds[BACKGROUND_BUILDS] = {
#include "stg03/green_forest/background_builds.inc"
};
static struct light green_forest_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.3F, 0.4F, 0.6F, 0.5F},
};
/* the set file's block, or null */
static struct set_head *green_forest_set = NULL;
/* the stars of players 0 and 1, or null. each task sets its pointer back to
 * null as it ends */
static struct task *stars_task_0 = NULL;
static struct task *stars_task_1 = NULL;
/* texture_load_shared fills green_forest_background_texlist from bgtex03.
 * it gives the textures by name to jungle_background_model_texlist */
static NJS_TEXLIST *background_group[2] = {
#include "stg03/green_forest/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg03/green_forest/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg03/green_forest/background_pvms.inc"
};
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color green_forest_block_colors[] = {
    {0xff000000, 0xffffffff},
    {0xffffff00,      0x406},
    {0xffff0000,     0x18c0},
    {0xffff00ff,     0xc104},
    {0xff00ffff,     0x430c},
    {0xffff7d00,      0x208},
    {0xff0000ff,      0x614},
    {0xff007d00,      0xce8},
    {0xff7d007d,      0x860},
    {0xff7d0000,     0x1040},
    {0xff007d7d,     0x3080},
    {0xff00ff00,     0x2020},
    {0xff00007d,     0x61b0},
};
/* clang-format on */
static NJS_POINT3 green_forest_block_origin = {
#include "stg03/green_forest/green_forest_block_origin.inc"
};
static NJS_POINT3 green_forest_block_cell = {
#include "stg03/green_forest/green_forest_block_cell.inc"
};

static void background_step(struct task *task);
static void background_draw(struct task *task);

/* compile the background at buf, with the fog off, and return where the
 * build ends. where model_builds_compile builds nothing, background_draw
 * draws the model uncompiled */
static Uint8 *background_build(Uint8 *buf)
{
    njFogDisable();
    if (model_builds_compile(&buf, background_builds, BACKGROUND_BUILDS) ==
        BACKGROUND_BUILDS) {
        background_compiled = TRUE;
    }
    njFogEnable();
    return buf;
}

void green_forest_init(void)
{
    struct task *task;
    Uint8 *buf;

    world_land_start(&green_forest_land);
    world_load_block_grid("GREENFOREST.BMP", green_forest_block_colors,
                          &green_forest_block_origin, &green_forest_block_cell);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    texture_load_shared(green_forest_object_pvms,
                        green_forest_object_texture_groups);
    texture_await_shared(green_forest_object_pvms,
                         green_forest_object_texture_groups);
    green_forest_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&green_forest_objects, green_forest_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &green_forest_lights[0]);
    light_set(1, &green_forest_lights[1]);
    light_set(2, &green_forest_lights[0]);
    light_set(3, &green_forest_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg03_light.bin");
    buf = (Uint8 *)world_build_land(&green_forest_land,
                                    (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = background_build(buf);
    object_builds_run(buf);
    music_fade_in(0);
    music_play_or_defer("g_fores.adx");
    path_create_manager(green_forest_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        task->draw = background_draw;
    }
    kill_areas_start(green_forest_kill_areas);
    module_motions_register();
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.splash) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    messenger_messages_load("mh0003");
    hint_box_hints_load("bh0003");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    splash_task_create(0, NULL);
    if (game_state.two_players) {
        splash_task_create(1, NULL);
    }
    if (!stars_task_0 && !stars_task_1) {
        if (game_state.two_players) {
            if (player_work[0]) {
                stars_create(0, STAR_POINTS_TWO_PLAYERS, &stars_task_0);
            }
            if (player_work[1]) {
                stars_create(1, STAR_POINTS_TWO_PLAYERS, &stars_task_1);
            }
        } else {
            if (player_work[0]) {
                stars_create(0, STAR_POINTS, &stars_task_0);
            }
            if (player_work[1]) {
                stars_create(1, STAR_POINTS, &stars_task_1);
            }
        }
    }
    sound_load_stage("se_ac_gf.mlt", &green_forest_sound_list);
}

void green_forest_exit(void)
{
    if (stars_task_0) {
        task_destroy(stars_task_0);
        stars_task_0 = NULL;
    }
    if (stars_task_1) {
        task_destroy(stars_task_1);
        stars_task_1 = NULL;
    }
    hint_box_hints_free();
    messenger_messages_free();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    particle_core_stop();
    texture_release_shared(green_forest_object_pvms,
                           green_forest_object_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (green_forest_set) {
        loader_free(green_forest_set);
        green_forest_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void green_forest_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read
     * in place would do */
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

/* @hack spelling: the points defined here, after the stage's calls, where
 * the top of the source would do */
static const NJS_POINT3 no_water_start = {
#include "stg03/green_forest/no_water_start.inc"
};
static const NJS_POINT3 no_water_end = {
#include "stg03/green_forest/no_water_end.inc"
};
static const NJS_POINT3 no_water_center = {
#include "stg03/green_forest/no_water_center.inc"
};

static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_POINT3 start;
    NJS_POINT3 end;
    NJS_POINT3 center;
    NJS_LINE line;
    NJS_POINT3 nearest;
    NJS_CNK_OBJECT *object;
    NJS_CNK_MODEL *model;
    Sint16 *texture;
    Sint16 tid;
    Float distance;
    int i;

    for (i = 0; i < BG_CAMERAS; i++) {
        if (!camera_views[i]) {
            continue;
        }
        pos = camera_views[i]->pos;
        if (pos.x > LOWERED_X && pos.z > LOWERED_Z) {
            work->mode |= BG_LOWERED(i);
        } else {
            work->mode &= ~BG_LOWERED(i);
        }
        start = no_water_start;
        end = no_water_end;
        center = no_water_center;
        line.px = start.x;
        line.py = start.y;
        line.pz = start.z;
        line.vx = start.x - end.x;
        line.vy = start.y - end.y;
        line.vz = start.z - end.z;
        distance = njDistanceP2L(&pos, &line, &nearest);
        if (distance < NO_WATER_RANGE ||
            (njDistanceP2P(&center, &pos) < NO_WATER_RANGE &&
             pos.y < NO_WATER_BELOW_Y && pos.x < NO_WATER_BELOW_X)) {
            work->mode |= BG_NO_WATER(i);
        } else {
            work->mode &= ~BG_NO_WATER(i);
        }
    }
    if (!game_state.time_stopped) {
        WATER_FRAME(work)++;
        if (WATER_FRAME(work) >= WATER_FRAMES) {
            WATER_FRAME(work) = 0;
        }
    }
    /* @hack spelling: model, texture and tid, locals that build the halfword's
     * value in two statements, where one assignment would do */
    model = jungle_background_water_model;
    texture = model->plist;
    tid = texture[WATER_TID_AT] & NJD_FFM_MASK;
    tid |= WATER_FRAME(work);
    texture[WATER_TID_AT] = tid;
    /* @hack spelling: object set to the water's object, then to its child,
     * where the child alone would do */
    object = &green_forest_water_object;
    for (object = object->child; object; object = object->sibling) {
        /* @hack spelling: model, texture and tid, locals that build the
         * halfword's value, where one assignment would do */
        model = object->model;
        texture = &model->plist[WATER_TID_AT];
        tid = (*texture & NJD_FFM_MASK) | WATER_FRAME(work);
        *texture = tid;
    }
}

/* draw the background and its water model at the current camera's x and z,
 * and at y 0. then draw the water's pieces that lie between WATER_NEAR and
 * WATER_FAR */
static void background_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_CNK_OBJECT *object;

    njSetTexture(&green_forest_background_texlist);
    njPushMatrixEx();
    pos.x = camera_views[camera_current_index]->pos.x;
    pos.y = 0.0F;
    pos.z = camera_views[camera_current_index]->pos.z;
    njTranslateV(NULL, &pos);
    if (work->mode & BG_LOWERED(camera_current_index)) {
        njTranslate(NULL, 0.0F, LOWERED_DROP, 0.0F);
    }
    if (background_compiled) {
        njDirectDrawObject(background_builds[0].built);
    } else {
        njCnkSimpleDrawModel(&jungle_background_model);
    }
    if (!(work->mode & BG_NO_WATER(camera_current_index))) {
        njSetTexture(&green_forest_water_texlist);
        njCnkSimpleDrawModel(jungle_background_water_model);
    }
    matrix_load_parent();
    /* @hack spelling: object set to the water's object and then its child,
     * where both read in place would do */
    object = &green_forest_water_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njPushMatrixEx();
    object = object->child;
    njSetTexture(&green_forest_water_texlist);
    while (object) {
        njCalcPoint(NULL, (NJS_POINT3 *)object->pos, &pos);
        if (pos.z < WATER_FAR && pos.z > WATER_NEAR) {
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            njCnkSimpleDrawModel(object->model);
            matrix_load_parent();
        }
        object = object->sibling;
    }
    njPopMatrix(2);
}
