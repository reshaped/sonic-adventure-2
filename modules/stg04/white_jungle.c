/* White Jungle, the stage of STG04: its start and end, and its tasks' steps.
 * the background task, BgExec, draws the background about the current
 * camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg04/white_jungle/text.inc"

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
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg04/stars.h"
#include "stg04/white_jungle.h"
#include "stg04/white_jungle_sounds.h"
#include "system/areas.h"
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
/* the stage task's step keeps AMBIENT_SOUND of the stage's sound list
 * playing. each step gives it AMBIENT_DURATION anew. the list gives the sound a
 * priority of 2, which takes the place of AMBIENT_PRIORITY. so
 * AMBIENT_PRIORITY changes nothing */
#define AMBIENT_SOUND 5
#define AMBIENT_PRIORITY 1
#define AMBIENT_VOLUME 0
#define AMBIENT_DURATION 10

/* the background task's mode holds BG_LOWERED for each of cameras 0 and 1. a
 * camera whose x is over LOWERED_X and whose y is under LOWERED_Y takes
 * BG_LOWERED. the background then draws LOWERED_DROP along y for that camera */
#define BG_LOWERED(camera) (1 << (camera))
#define BG_CAMERAS 2
#define LOWERED_X 6530.394F
#define LOWERED_Y (-1827.0F)
#define LOWERED_DROP (-1317.0F)
/* the background task's timer counts from 0 to TIMER_CYCLE - 1, and then
 * starts again. nothing reads it */
#define TIMER_CYCLE 14

/* clang-format off */
#include "stg04/paths.h"
#include "stg04/background.h"
/* clang-format on */

static struct fog_setting white_jungle_fog;
static Bool background_compiled = FALSE;
#define BACKGROUND_BUILDS 1
static struct model_build background_builds[BACKGROUND_BUILDS] = {
#include "stg04/white_jungle/background_builds.inc"
};

/* the kill areas' objects, one quad each */
#define KILL_AREA_POINTS 4
static NJS_MATERIAL kill_area_0_material = {
#include "stg04/white_jungle/kill_area_0_material.inc"
};
static Sint16 kill_area_0_meshes[5] = {
#include "stg04/white_jungle/kill_area_0_meshes.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg04/white_jungle/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[KILL_AREA_POINTS] = {
#include "stg04/white_jungle/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[KILL_AREA_POINTS] = {
#include "stg04/white_jungle/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg04/white_jungle/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0 = {
#include "stg04/white_jungle/kill_area_0.inc"
};
static NJS_MATERIAL kill_area_1_material = {
#include "stg04/white_jungle/kill_area_1_material.inc"
};
static Sint16 kill_area_1_meshes[5] = {
#include "stg04/white_jungle/kill_area_1_meshes.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg04/white_jungle/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[KILL_AREA_POINTS] = {
#include "stg04/white_jungle/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[KILL_AREA_POINTS] = {
#include "stg04/white_jungle/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg04/white_jungle/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1 = {
#include "stg04/white_jungle/kill_area_1.inc"
};
static struct kill_area white_jungle_kill_areas[3] = {
#include "stg04/white_jungle/white_jungle_kill_areas.inc"
};

static struct light white_jungle_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *white_jungle_set = NULL;
/* the turning model's turn a step, in Angle units */
static Sint32 background_turn_rate = 8;
/* the stars of players 0 and 1, or null. each task sets its pointer back to
 * null as it ends */
static struct task *stars_task_0 = NULL;
static struct task *stars_task_1 = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color white_jungle_block_colors[] = {
    {0xff000000, 0xffffffff},
    {0xffffff00,       0xc2},
    {0xffff0000,      0x186},
    {0xffff00ff,      0x10c},
    {0xff00ffff,      0x300},
    {0xffff7d00,      0x208},
    {0xff0000ff,      0x618},
    {0xff007d00,      0xc10},
    {0xff7d007d,      0x860},
    {0xff7d0000,     0x1800},
    {0xff007d7d,     0x1080},
    {0xff00ff00,     0x3020},
    {0xff00007d,      0x430},
    {0xffffffff,        0xc},
};
/* clang-format on */
static NJS_POINT3 white_jungle_block_origin = {
#include "stg04/white_jungle/white_jungle_block_origin.inc"
};
static NJS_POINT3 white_jungle_block_cell = {
#include "stg04/white_jungle/white_jungle_block_cell.inc"
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

void white_jungle_init(void)
{
    struct task *task;
    Uint8 *buf;

    world_land_start(&white_jungle_land);
    world_load_block_grid("White_Jungle.Bmp", white_jungle_block_colors,
                          &white_jungle_block_origin, &white_jungle_block_cell);
    texture_load_shared(white_jungle_object_pvms,
                        white_jungle_object_texture_groups);
    texture_await_shared(white_jungle_object_pvms,
                         white_jungle_object_texture_groups);
    texture_load_pvm("bgtex04", &background_texlist);
    white_jungle_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&white_jungle_objects, white_jungle_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &white_jungle_lights[0]);
    light_set(1, &white_jungle_lights[1]);
    light_set(2, &white_jungle_lights[0]);
    light_set(3, &white_jungle_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg04_light.bin");
    buf = (Uint8 *)world_build_land(&white_jungle_land,
                                    (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = background_build(buf);
    object_builds_run(buf);
    music_fade_in(0);
    music_play_or_defer("w_jungl.adx");
    path_create_manager(white_jungle_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        task->draw = background_draw;
    }
    kill_areas_start(white_jungle_kill_areas);
    module_motions_register();
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.splash) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    fog_start_file("stg04_fog.bin", &white_jungle_fog);
    njFogDisable();
    object_light_create();
    messenger_messages_load("mh0004");
    hint_box_hints_load("bh0004");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    if (!stars_task_0 && !stars_task_1) {
        if (game_state.two_players) {
            if (player_work[0]) {
                white_jungle_stars_create(0, STAR_POINTS_TWO_PLAYERS,
                                          &stars_task_0);
            }
            if (player_work[1]) {
                white_jungle_stars_create(1, STAR_POINTS_TWO_PLAYERS,
                                          &stars_task_1);
            }
        } else {
            if (player_work[0]) {
                white_jungle_stars_create(0, STAR_POINTS, &stars_task_0);
            }
            if (player_work[1]) {
                white_jungle_stars_create(1, STAR_POINTS, &stars_task_1);
            }
        }
    }
    sound_load_stage("se_ac_wj.mlt", &white_jungle_sound_list);
}

void white_jungle_exit(void)
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
    object_light_destroy();
    fog_stop();
    njFogDisable();
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    particle_core_stop();
    njReleaseTexture(&background_texlist);
    texture_release_shared(white_jungle_object_pvms,
                           white_jungle_object_texture_groups);
    set_end();
    if (white_jungle_set) {
        loader_free(white_jungle_set);
        white_jungle_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void white_jungle_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode with two empty cases */
    switch (work->mode) {
    case 0:
        break;
    default:
        break;
    }
    sound_play_for(AMBIENT_SOUND, task, AMBIENT_PRIORITY, AMBIENT_VOLUME,
                   AMBIENT_DURATION);
}

/* keep the BG_LOWERED bits of cameras 0 and 1, and turn the background's
 * turning model by background_turn_rate. a camera at or past the first that
 * camera_views lacks keeps its bit as it was */
static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    int i;

    for (i = 0; i < BG_CAMERAS && camera_views[i]; i++) {
        pos = camera_views[i]->pos;
        if (pos.x > LOWERED_X && pos.y < LOWERED_Y) {
            work->mode |= BG_LOWERED(i);
        } else {
            work->mode &= ~BG_LOWERED(i);
        }
    }
    work->timer++;
    if (work->timer >= TIMER_CYCLE) {
        work->timer = 0;
    }
    work->ang.y += background_turn_rate;
}

static void background_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njFogDisable();
    njSetTexture(&background_texlist);
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
        njCnkEasyDrawModel(&background_model);
    }
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&turning_sky_model);
    njPopMatrix(1);
    njFogEnable();
}
