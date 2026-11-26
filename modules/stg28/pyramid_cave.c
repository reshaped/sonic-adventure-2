/* Pyramid Cave, stg28: the stage's init, exit and step, and its background.
 * the background picks the land blocks camera 0 draws, and draws a far wall
 * and eight lamps */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/iron_box_e.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/tint_pulse.h"
#include "shared/wood_box_e.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg28/pyramid_cave.h"
#include "stg28/cave_scenery.h"
#include "stg28/paths.h"
#include "stg28/object_archives.h"
#include "stg28/pyramid_cave_sounds.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"

/* the task levels of the land blocks' task and of the background's */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
/* the least the set file is read as, in entries */
#define SET_ENTRIES 2048

/* the land blocks of zone n, as game_state.land_block_mask holds them.
 * land_clip_step picks player 0's zone by its pos. west is toward -x. the
 * zones follow one another toward -z, and each ZONE_n_END_Z is where zone n
 * ends. zone 3 lies west of ZONE_3_EAST_X, beside zone 2, and ends with it.
 * zone 9 lies west of ZONE_9_EAST_X, and zone 10 starts east of it. zone 11
 * is all past ZONE_10_END_Z */
#define ZONE_BLOCKS(n) (1 << (n))
#define ZONE_1_END_Z (-4000.0F)
#define ZONE_2_END_Z (-8650.0F)
#define ZONE_3_EAST_X (-2150.0F)
#define ZONE_4_END_Z (-9600.0F)
#define ZONE_5_END_Z (-11600.0F)
#define ZONE_6_END_Z (-13600.0F)
#define ZONE_7_END_Z (-15900.0F)
#define ZONE_8_END_Z (-18550.0F)
#define ZONE_9_EAST_X (-850.0F)
#define ZONE_9_END_Z (-20000.0F)
#define ZONE_10_END_Z (-21790.0F)

/* where the far wall draws, while camera 0 draws zone 5 or zone 6 */
#define WALL_X (-2600.0F)
#define WALL_Y (-2480.0F)
#define WALL_Z (-11000.0F)
/* the lamps draw at lamp_points while camera 0 draws zone 10. the pulse of
 * shared/tint_pulse.h tints them */
#define LAMPS 8

static void background_step(struct task *task);
static void background_draw(struct task *task);
static void land_clip_step(struct task *task);

static struct kill_area pyramid_cave_kill_areas[9] = {
#include "stg28/pyramid_cave/pyramid_cave_kill_areas.inc"
};
static struct light pyramid_cave_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file pyramid_cave_init read, until pyramid_cave_exit frees it */
static struct set_head *pyramid_cave_set = NULL;
static Sint32 dat_8c54e718 = 0;
KEPT static NJS_POINT3 lamp_points[LAMPS] = {
#include "stg28/pyramid_cave/lamp_points.inc"
};

void pyramid_cave_init(void)
{
    struct task *task;

    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&pyramid_cave_land);
    texture_load_shared(pyramid_cave_pvms, pyramid_cave_texture_groups);
    texture_await_shared(pyramid_cave_pvms, pyramid_cave_texture_groups);
    texture_load_shared(pyramid_cave_background_pvms,
                        pyramid_cave_background_texture_groups);
    texture_await_shared(pyramid_cave_background_pvms,
                         pyramid_cave_background_texture_groups);
    pyramid_cave_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&pyramid_cave_objects, pyramid_cave_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &pyramid_cave_lights[0]);
    light_set(1, &pyramid_cave_lights[1]);
    light_set(2, &pyramid_cave_lights[0]);
    light_set(3, &pyramid_cave_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg28_light.bin");
    iron_box_e_build(
        wood_box_e_build(object_builds_run((Uint8 *)world_build_land(
            &pyramid_cave_land, (Uint32)STAGE_MODEL_BUILD_AREA))));
    music_fade_in(0);
    music_play_or_defer("p_cave.adx");
    path_create_manager(pyramid_cave_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
    }
    kill_areas_start(pyramid_cave_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: a second test of drop_rings, which the first
     * settles */
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    fog_start_file("stg28_fog.bin", &pyramid_cave_fog);
    njFogEnable();
    object_light_create();
    messenger_messages_load("mh0028");
    hint_box_hints_load("bh0028");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_pc.mlt", &pyramid_cave_sound_list);
    object_builds_init();
    game_state.block_grid_loaded = TRUE;
}

void pyramid_cave_exit(void)
{
    object_builds_exit();
    hint_box_hints_free();
    messenger_messages_free();
    object_light_destroy();
    fog_stop();
    njFogDisable();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(pyramid_cave_pvms, pyramid_cave_texture_groups);
    texture_release_shared(pyramid_cave_background_pvms,
                           pyramid_cave_background_texture_groups);
    set_end();
    if (pyramid_cave_set != NULL) {
        loader_free(pyramid_cave_set);
        pyramid_cave_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void pyramid_cave_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    /* @hack redundant: a switch of one empty case, where nothing would do */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
}

/* the far wall draws without the fog, and the lamps with it */
static void background_draw(struct task *task)
{
    Uint32 color;
    int step;
    int i;

    njFogDisable();
    if ((game_state.land_block_mask[0] & ZONE_BLOCKS(5)) ||
        (game_state.land_block_mask[0] & ZONE_BLOCKS(6))) {
        njPushMatrixEx();
        njTranslate(NULL, WALL_X, WALL_Y, WALL_Z);
        njSetTexture(&pyramid_cave_wall_texlist);
        njCnkEasyDrawModel(&pyramid_cave_wall_model);
        njPopMatrixEx();
    }
    njFogEnable();
    if (game_state.land_block_mask[0] & ZONE_BLOCKS(10)) {
        step = game_state.steps % TINT_PULSE_STEPS;
        if (step < TINT_PULSE_RISE) {
            color = TINT_PULSE_RISE_ALPHA(step) | TINT_PULSE_COLORS;
        } else {
            color = TINT_PULSE_FALL_ALPHA(step) | TINT_PULSE_COLORS;
        }
        njSetTexture(&pyramid_cave_lamp_texlist);
        for (i = 0; i < LAMPS; i++) {
            njPushMatrixEx();
            njTranslateEx(&lamp_points[i]);
            model_draw_tinted(pyramid_cave_lamp_model, color);
            njPopMatrixEx();
        }
        /* @hack redundant: a block no path reaches, of a draw of the lamp */
        if (0) {
            model_draw_tinted(pyramid_cave_lamp_model, 0);
        }
    }
}

/* give camera 0 the land blocks of player 0's zone */
static void land_clip_step(struct task *task)
{
    struct task_work *work;
    Sint32 *mask;

    game_state.land_block_mask[0] = 0;
    work = player_work[0];
    /* @hack spelling: the mask's address taken after the player's work is
     * read, where its address taken first would do */
    mask = &game_state.land_block_mask[0];
    if (work->pos.z >= ZONE_1_END_Z) {
        *mask = ZONE_BLOCKS(1);
    } else if (work->pos.z >= ZONE_2_END_Z) {
        if (work->pos.x >= ZONE_3_EAST_X) {
            *mask = ZONE_BLOCKS(2);
        } else {
            *mask = ZONE_BLOCKS(3);
        }
    } else if (work->pos.z >= ZONE_4_END_Z) {
        *mask = ZONE_BLOCKS(4);
    } else if (work->pos.z >= ZONE_5_END_Z) {
        *mask = ZONE_BLOCKS(5);
    } else if (work->pos.z >= ZONE_6_END_Z) {
        *mask = ZONE_BLOCKS(6);
    } else if (work->pos.z >= ZONE_7_END_Z) {
        *mask = ZONE_BLOCKS(7);
    } else if (work->pos.z >= ZONE_8_END_Z) {
        *mask = ZONE_BLOCKS(8);
    } else if (work->pos.x < ZONE_9_EAST_X && work->pos.z >= ZONE_9_END_Z) {
        *mask = ZONE_BLOCKS(9);
    } else if (work->pos.z >= ZONE_10_END_Z) {
        *mask = ZONE_BLOCKS(10);
    } else {
        *mask = ZONE_BLOCKS(11);
    }
}
