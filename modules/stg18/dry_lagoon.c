/* Dry Lagoon, stg18: the stage's init, exit and step, and its background.
 * BgClip picks the land blocks for the current camera. the background task,
 * BgExec, plays the sounds above the water, sets the fog and draws the
 * animated water */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/emerald.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/switch.h"
#include "shared/water_ripple.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg18/butterflies.h"
#include "stg18/chin.h"
#include "stg18/chin2.h"
#include "stg18/chin3.h"
#include "stg18/dry_lagoon.h"
#include "stg18/object_archives.h"
#include "stg18/flower.h"
#include "stg18/lagoon_util.h"
#include "stg18/leaf.h"
#include "stg18/palm.h"
#include "stg18/pool.h"
#include "stg18/pot.h"
#include "stg18/sasa.h"
#include "stg18/shelf.h"
#include "stg18/sounds.h"
#include "stg18/specks.h"
#include "stg18/stake.h"
#include "stg18/water.h"
#include "stg18/water_surface.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

static struct fog_setting under_water_fog;
static struct fog_setting above_water_fog;

/* task_create's levels for BgClip and BgExec */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 2048
#define EMERALD_PATHS 8
#define DEBRIS_SPECKS 1270

/* the background's sounds, from the stage's sound list. they play only while
 * the camera stands above the water. while player 0 lies within the BOX_
 * bounds in x and z, OCCASIONAL_SOUND may play every OCCASIONAL_PERIOD
 * steps. it plays with a chance of OCCASIONAL_CHANCE. while player 0 lies
 * outside them, each step plays STEADY_SOUND for STEADY_DURATION + 1 sound
 * updates. each starts at START_VOLUME, before sound_set_volume gives it its
 * own */
#define STEADY_SOUND 9
#define OCCASIONAL_SOUND 10
#define STEADY_PRIORITY 1
#define OCCASIONAL_PRIORITY 0
#define START_VOLUME 0
#define STEADY_DURATION 10
#define OCCASIONAL_PERIOD 90
#define OCCASIONAL_CHANCE 0.25F
#define BOX_WEST_X (-90.0F)
#define BOX_EAST_X 320.0F
#define BOX_SOUTH_Z 860.0F
#define BOX_NORTH_Z 1215.0F

/* each piece of animated water cycles through LAGOON_WATER_POSES models, each
 * drawn for two steps. its texture animation advances water_anim_speed frames a
 * step */
#define WATER_FRAME(steps) (((steps) >> 1) & (LAGOON_WATER_POSES - 1))
#define WATER_X 200.0F
#define SOUTH_WATER_Y (-72.5F)
#define SOUTH_WATER_Z (-70.0F)
#define MIDDLE_WATER_Z 1220.0F
#define NORTH_WATER_Z 1450.0F
/* the drifting debris shows only while the camera sees under the water. it
 * hides even then where player 0 lies within the HIDE_ bounds, or where
 * lagoon_camera_blocks gives SOUTH_WEST_BLOCKS */
#define HIDE_WEST_X 100.0F
#define HIDE_EAST_X 270.0F
#define HIDE_SOUTH_Z 900.0F
#define HIDE_NORTH_Z 1220.0F
#define HIDE_TOP_Y 100.0F
/* the center and the half sizes of the surface the background hands
 * lagoon_water_draw_around. SURFACE_Y is the water's height in the east */
#define SURFACE_X 1105.0F
#define SURFACE_Y 245.0F
#define SURFACE_Z 1475.0F
#define SURFACE_HALF_X 85.0F
#define SURFACE_HALF_Z 95.0F

static void land_clip_step(struct task *task);
static void background_step(struct task *task);
static void background_draw(struct task *task);
static void background_destroy(struct task *task);

/* The original route section precedes the water geometry. */
#pragma section ROUTES
/* The eight paths followed by moving Emerald shards. */
static NJS_POINT3 emerald_route_1_positions[67] = {
#include "stg18/dry_lagoon/emerald_route_1_positions.inc"
};
static struct path emerald_route_1 = {
#include "stg18/dry_lagoon/emerald_route_1.inc"
};

static NJS_POINT3 emerald_route_2_positions[121] = {
#include "stg18/dry_lagoon/emerald_route_2_positions.inc"
};
static struct path emerald_route_2 = {
#include "stg18/dry_lagoon/emerald_route_2.inc"
};

static NJS_POINT3 emerald_route_3_positions[51] = {
#include "stg18/dry_lagoon/emerald_route_3_positions.inc"
};
static struct path emerald_route_3 = {
#include "stg18/dry_lagoon/emerald_route_3.inc"
};

static NJS_POINT3 emerald_route_4_positions[41] = {
#include "stg18/dry_lagoon/emerald_route_4_positions.inc"
};
static struct path emerald_route_4 = {
#include "stg18/dry_lagoon/emerald_route_4.inc"
};

static NJS_POINT3 emerald_route_5_positions[121] = {
#include "stg18/dry_lagoon/emerald_route_5_positions.inc"
};
static struct path emerald_route_5 = {
#include "stg18/dry_lagoon/emerald_route_5.inc"
};

static NJS_POINT3 emerald_route_6_positions[41] = {
#include "stg18/dry_lagoon/emerald_route_6_positions.inc"
};
static struct path emerald_route_6 = {
#include "stg18/dry_lagoon/emerald_route_6.inc"
};

static NJS_POINT3 emerald_route_7_positions[181] = {
#include "stg18/dry_lagoon/emerald_route_7_positions.inc"
};
static struct path emerald_route_7 = {
#include "stg18/dry_lagoon/emerald_route_7.inc"
};

static NJS_POINT3 emerald_route_8_positions[201] = {
#include "stg18/dry_lagoon/emerald_route_8_positions.inc"
};
static struct path emerald_route_8 = {
#include "stg18/dry_lagoon/emerald_route_8.inc"
};

/* A search box gives three message numbers for each possible location. */
static struct hunt_hint dry_lagoon_hints[149] = {
#include "stg18/dry_lagoon/dry_lagoon_hints.inc"
};

/* a table of hints that no code reads */
static struct hunt_hint unused_path_hints[4] = {
    {EMERALD_KIND_A_PATHMOVE, 1, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
    {HUNT_HINT_END, 0, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
};

static Sint32 *dry_lagoon_hint_file = NULL;
static struct path *dry_lagoon_paths[] = {NULL};
static struct path *dry_lagoon_emerald_paths[EMERALD_PATHS + 1] = {
#include "stg18/dry_lagoon/dry_lagoon_emerald_paths.inc"
};
#pragma section

static NJS_TEXLIST *background_group[2] = {
#include "stg18/dry_lagoon/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg18/dry_lagoon/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg18/dry_lagoon/background_pvms.inc"
};

/* One BASIC collision strip forms the kill plane beneath the lagoon. */
static NJS_MATERIAL kill_area_material = {
#include "stg18/dry_lagoon/kill_area_material.inc"
};
static Sint16 kill_area_meshes[5] = {
#include "stg18/dry_lagoon/kill_area_meshes.inc"
};
static NJS_MESHSET kill_area_meshset = {
#include "stg18/dry_lagoon/kill_area_meshset.inc"
};
static NJS_POINT3 kill_area_points[4] = {
#include "stg18/dry_lagoon/kill_area_points.inc"
};
static NJS_VECTOR kill_area_normals[4] = {
#include "stg18/dry_lagoon/kill_area_normals.inc"
};
static NJS_MODEL kill_area_model = {
#include "stg18/dry_lagoon/kill_area_model.inc"
};
static NJS_OBJECT kill_area = {
#include "stg18/dry_lagoon/kill_area.inc"
};
static struct kill_area dry_lagoon_kill_areas[2] = {
#include "stg18/dry_lagoon/dry_lagoon_kill_areas.inc"
};

static struct light dry_lagoon_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the background task, or null while there is none */
static struct task *background_task = NULL;
/* the set file's block, or null */
static struct set_head *dry_lagoon_set = NULL;
/* the drifting debris's task, or null while there is none */
static struct task *debris_task = NULL;
/* the volumes of STEADY_SOUND and OCCASIONAL_SOUND, as sound_set_volume takes
 * them */
static Sint32 steady_volume = -20;
static Sint32 occasional_volume = -20;
/* the animated water's tint for model_draw_tinted. it scales the water's
 * alpha by 0x28 / 0x100, and its colors by 0xff / 0x100 */
static Uint32 water_tint = 0x28ffffff;
static Sint32 dat_8c571e1c = 0x80;
static Float water_anim_speed = 4.0F;

void dry_lagoon_init(void)
{
    struct task *task;
    Uint8 *buf;

    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&dry_lagoon_land);
    texture_load_shared(dry_lagoon_pvms, dry_lagoon_texture_groups);
    texture_await_shared(dry_lagoon_pvms, dry_lagoon_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    dry_lagoon_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&dry_lagoon_objects, dry_lagoon_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &dry_lagoon_lights[0]);
    light_set(1, &dry_lagoon_lights[1]);
    light_set(2, &dry_lagoon_lights[0]);
    light_set(3, &dry_lagoon_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg18_light.bin");
    buf = object_builds_run((Uint8 *)world_build_land(
        &dry_lagoon_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    buf = flower_build(buf);
    buf = sasa_build(buf);
    buf = leaf_build(buf);
    buf = stake_build(buf);
    buf = palm_build(buf);
    buf = pot_build(buf);
    buf = pool_build(buf);
    buf = chin_build(buf);
    buf = chin2_build(buf);
    buf = chin3_build(buf);
    buf = shelf_build(buf);
    butterflies_build(buf);
    music_fade_in(0);
    if (game_state.demo == 0) {
        music_play_or_defer("d_lagoon.adx");
    } else {
        music_play_or_defer("t9_rouge.adx");
    }
    path_create_manager(dry_lagoon_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        if (background_task == NULL) {
            background_task = task;
        }
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(dry_lagoon_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.unk_04 == NULL) {
        player_hooks.unk_04 = dirt_create;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.create_piece == NULL) {
        player_hooks.create_piece = emerald_create;
    }
    if (player_hooks.show_piece_sign == NULL) {
        player_hooks.show_piece_sign = emerald_sign_create;
    }
    if (player_hooks.ripple == NULL) {
        player_hooks.ripple = ripple_create;
    }
    if (player_hooks.splash == NULL) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    fog_read("stg18_fog.bin", &under_water_fog);
    fog_read("stg18_fogB.bin", &above_water_fog);
    njFogDisable();
    sand_task_create(0, NULL);
    if (debris_task == NULL) {
        specks_create(0, DEBRIS_SPECKS, &debris_task);
    }
    hunt_set_hints(dry_lagoon_hints);
    dry_lagoon_hint_file = hunt_load_hint_file("eh0018");
    emerald_set_paths(dry_lagoon_emerald_paths, EMERALD_PATHS);
    messenger_messages_load("mh0018");
    hint_box_hints_load("bh0018");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    switch_clear_all();
    sound_load_stage("se_ac_dl.mlt", &dry_lagoon_sound_list);
    game_state.block_grid_loaded = TRUE;
}

void dry_lagoon_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
    emerald_clear_paths();
    syFree(dry_lagoon_hint_file);
    dry_lagoon_hint_file = NULL;
    hunt_clear_hints();
    if (debris_task != NULL) {
        task_destroy(debris_task);
    }
    fog_stop();
    njFogDisable();
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    if (player_hooks.ripple == ripple_create) {
        player_hooks.ripple = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.unk_04 == dirt_create) {
        player_hooks.unk_04 = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    if (player_hooks.create_piece == emerald_create) {
        player_hooks.create_piece = NULL;
    }
    if (player_hooks.show_piece_sign == emerald_sign_create) {
        player_hooks.show_piece_sign = NULL;
    }
    particle_core_stop();
    texture_release_shared(dry_lagoon_pvms, dry_lagoon_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (dry_lagoon_set != NULL) {
        loader_free(dry_lagoon_set);
        dry_lagoon_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void dry_lagoon_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    fog_start();
    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
    Float level = lagoon_water_level();

    if (camera_views[camera_current_index]->pos.y > level) {
        if (player_work[0]->pos.x >= BOX_WEST_X &&
            player_work[0]->pos.x <= BOX_EAST_X &&
            BOX_SOUTH_Z <= player_work[0]->pos.z &&
            player_work[0]->pos.z <= BOX_NORTH_Z) {
            if (game_state.steps % OCCASIONAL_PERIOD == 0 &&
                (Float)rand() * RAND_SCALE < OCCASIONAL_CHANCE) {
                sound_play(OCCASIONAL_SOUND, task, OCCASIONAL_PRIORITY,
                           START_VOLUME);
                sound_set_volume(task, SOUND_ANY_ID, occasional_volume);
            }
        } else {
            sound_play_for(STEADY_SOUND, task, STEADY_PRIORITY, START_VOLUME,
                           STEADY_DURATION);
            sound_set_volume(task, SOUND_ANY_ID, steady_volume);
        }
    }
}

/* the fog and the drifting debris follow the side of the water the camera
 * sees. the animated water draws in the land blocks camera 0 shows */
static void background_draw(struct task *task)
{
    /* @hack redundant: work, a local for task->work that nothing reads */
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    Float level;
    NJS_POINT3 pos;
    Uint32 frame;

    level = lagoon_water_level();
    if (camera_views[camera_current_index]->pos.y <= level ||
        lagoon_view_dips_below(level)) {
        pos = player_work[0]->pos;
        fog_set(&under_water_fog);
        if (lagoon_camera_blocks() != SOUTH_WEST_BLOCKS &&
            !(HIDE_WEST_X < pos.x && pos.x < HIDE_EAST_X &&
              HIDE_SOUTH_Z < pos.z && pos.z < HIDE_NORTH_Z &&
              pos.y < HIDE_TOP_Y)) {
            if (debris_task != NULL) {
                debris_task->work->mode = MODE_LAGOON_SPECKS_SHOWN;
            }
        } else {
            /* @bug memory: writes MODE_LAGOON_SPECKS_HIDDEN through debris_task
             * untested, though the draw's other two writes test it for null.
             * specks_create leaves it null where player 0's slot holds no work
             * or the heap runs out: 0x8c5007c0, and the test at 0x8c5007ac */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it writes the mode only
             * through a task that specks_create made */
            if (debris_task != NULL) {
                debris_task->work->mode = MODE_LAGOON_SPECKS_HIDDEN;
            }
#else
            debris_task->work->mode = MODE_LAGOON_SPECKS_HIDDEN;
#endif
        }
        lagoon_water_draw_under();
    } else {
        fog_set(&above_water_fog);
        if (debris_task != NULL) {
            debris_task->work->mode = MODE_LAGOON_SPECKS_HIDDEN;
        }
        lagoon_water_draw();
        if (game_state.land_block_mask[0] & (EAST_BLOCKS | MIDDLE_BLOCKS)) {
            lagoon_water_draw_around(SURFACE_X, SURFACE_Y, SURFACE_Z,
                                     SURFACE_HALF_X, SURFACE_HALF_Z,
                                     &player_work[0]->pos);
        }
    }
    if (game_state.land_block_mask[0] & SOUTH_WEST_BLOCKS) {
        njFogDisable();
        njPushMatrixEx();
        njTranslate(NULL, WATER_X, SOUTH_WATER_Y, SOUTH_WATER_Z);
        njSetTexture(&dry_lagoon_water_surface_texlist);
        texture_anim_apply(
            dry_lagoon_south_water_models[WATER_FRAME(game_state.steps)],
            dry_lagoon_south_water_anims[WATER_FRAME(game_state.steps)],
            game_state.steps * water_anim_speed);
        model_draw_tinted(
            dry_lagoon_south_water_models[WATER_FRAME(game_state.steps)],
            water_tint);
        njPopMatrixEx();
        njFogEnable();
    }
    if (game_state.land_block_mask[0] & LOW_BLOCKS) {
        njFogDisable();
        njSetTexture(&dry_lagoon_water_surface_texlist);
        njPushMatrixEx();
        njTranslate(NULL, WATER_X, 0.0F, MIDDLE_WATER_Z);
        texture_anim_apply(
            dry_lagoon_middle_water_models[WATER_FRAME(game_state.steps)],
            dry_lagoon_middle_water_anims[WATER_FRAME(game_state.steps)],
            game_state.steps * water_anim_speed);
        model_draw_tinted(
            dry_lagoon_middle_water_models[WATER_FRAME(game_state.steps)],
            water_tint);
        texture_anim_apply(
            dry_lagoon_middle_water_2_models[WATER_FRAME(game_state.steps)],
            dry_lagoon_middle_water_2_anims[WATER_FRAME(game_state.steps)],
            game_state.steps * water_anim_speed);
        model_draw_tinted(
            dry_lagoon_middle_water_2_models[WATER_FRAME(game_state.steps)],
            water_tint);
        njPopMatrixEx();
        njFogEnable();
    }
    if (game_state.land_block_mask[0] & (LOW_BLOCKS | MIDDLE_BLOCKS)) {
        njFogDisable();
        njSetTexture(&dry_lagoon_water_surface_texlist);
        njPushMatrixEx();
        njTranslate(NULL, WATER_X, 0.0F, NORTH_WATER_Z);
        texture_anim_apply(
            dry_lagoon_north_water_models[WATER_FRAME(game_state.steps)],
            dry_lagoon_north_water_anims[WATER_FRAME(game_state.steps)],
            game_state.steps * water_anim_speed);
        model_draw_tinted(
            dry_lagoon_north_water_models[WATER_FRAME(game_state.steps)],
            water_tint);
        njPopMatrixEx();
        njFogEnable();
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

static void land_clip_step(struct task *task)
{
    lagoon_land_clip();
}
