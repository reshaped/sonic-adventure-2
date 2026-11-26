/* Aquatic Mine, the stage of STG07: its start and end, and its tasks' steps.
 * the background task is BgExec. while the camera is under the water, its
 * draw sets the water's fog and shows the drifting debris. while the camera is
 * above, the draw sets the air's fog. only then does the step play
 * SURFACE_SOUND at random about player 0 */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/black_smoke.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/emerald.h"
#include "shared/emerald_hunt_data.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/splash.h"
#include "shared/switch.h"
#include "shared/water_ripple.h"
#include "shared/water_specks.h"
#include "sound/adx.h"
#include "stg07/sounds.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg07/aquatic_mine.h"
#include "stg07/object_archives.h"
#include "stg07/water.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the fogs under the water and above it, which aquatic_mine_init reads from
 * stg07_fog.bin and stg07_fogB.bin */
static struct fog_setting aquatic_mine_water_fog;
static struct fog_setting aquatic_mine_air_fog;

#define BACKGROUND_LEVEL 1
/* the set file's block is at least SET_ENTRIES entries long, its head
 * counted as one */
#define SET_ENTRIES 2048
#define BLOCK_COLORS 5
#define DEBRIS_SPECKS 1270

/* the background's step plays SURFACE_SOUND of the stage's list. on one step
 * in SOUND_STEPS, it plays with a chance of SOUND_CHANCE. it plays at random
 * in a box SOUND_SPREAD_X by SOUND_SPREAD_Y by SOUND_SPREAD_Z, centered on
 * player 0's pos */
#define SURFACE_SOUND 6
#define SOUND_STEPS 3
#define SOUND_CHANCE 0.3F
#define SOUND_SPREAD_X 300.0F
#define SOUND_SPREAD_Y 200.0F
#define SOUND_SPREAD_Z 300.0F

/* in the background's work's submode: whether its last draw found the camera
 * under the water */
#define UNDER_WATER(work) ((work)->submode)

/* the height the background's draw tests for no water at the camera.
 * water_height gives WATER_NONE instead, as the draw's bug tag says */
#define NO_WATER (-10000.0F)

static void background_step(struct task *task);
static void background_draw(struct task *task);

#include "shared/emerald_hunt_data.h"

/* Moving Emerald shard routes used by Aquatic Mine, Meteor Herd and Mad Space.
 */
/* clang-format off */
static NJS_POINT3 emerald_route_1_points[121] = {
#include "stg07/aquatic_mine/emerald_route_1_points.inc"
};

static struct path emerald_route_1 = {
#include "stg07/aquatic_mine/emerald_route_1.inc"
};

static NJS_POINT3 emerald_route_2_points[51] = {
#include "stg07/aquatic_mine/emerald_route_2_points.inc"
};

static struct path emerald_route_2 = {
#include "stg07/aquatic_mine/emerald_route_2.inc"
};

static NJS_POINT3 emerald_route_3_points[41] = {
#include "stg07/aquatic_mine/emerald_route_3_points.inc"
};

static struct path emerald_route_3 = {
#include "stg07/aquatic_mine/emerald_route_3.inc"
};

static NJS_POINT3 emerald_route_4_points[161] = {
#include "stg07/aquatic_mine/emerald_route_4_points.inc"
};

static struct path emerald_route_4 = {
#include "stg07/aquatic_mine/emerald_route_4.inc"
};

static NJS_POINT3 emerald_route_5_points[161] = {
#include "stg07/aquatic_mine/emerald_route_5_points.inc"
};

static struct path emerald_route_5 = {
#include "stg07/aquatic_mine/emerald_route_5.inc"
};

static NJS_POINT3 emerald_route_6_points[41] = {
#include "stg07/aquatic_mine/emerald_route_6_points.inc"
};

static struct path emerald_route_6 = {
#include "stg07/aquatic_mine/emerald_route_6.inc"
};

static NJS_POINT3 emerald_route_7_points[51] = {
#include "stg07/aquatic_mine/emerald_route_7_points.inc"
};

static struct path emerald_route_7 = {
#include "stg07/aquatic_mine/emerald_route_7.inc"
};

static NJS_POINT3 emerald_route_8_points[81] = {
#include "stg07/aquatic_mine/emerald_route_8_points.inc"
};

static struct path emerald_route_8 = {
#include "stg07/aquatic_mine/emerald_route_8.inc"
};

/* clang-format on */

struct path *hunt_piece_routes[HUNT_PIECE_ROUTES] = {
#include "stg07/aquatic_mine/hunt_piece_routes.inc"
};
/* the hints a search box gives for each place. each is the index of a
 * message in the hint file */
struct hunt_hint hunt_location_hints[149] = {
#include "stg07/aquatic_mine/hunt_location_hints.inc"
};
/* a table of hints that no code reads */
static struct hunt_hint unused_path_hints[4] = {
    {EMERALD_KIND_A_PATHMOVE, 1, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
    {HUNT_HINT_END, 0, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
};
/* Each stage loads its own messages and releases them on exit. */
void *hunt_stage_messages = NULL;

static Sint32 dat_8c547020 = 0;
static struct light aquatic_mine_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, from aquatic_mine_init until aquatic_mine_exit frees
 * it, or null */
static struct set_head *aquatic_mine_set = NULL;
static Sint32 dat_8c547068 = 8;
/* the drifting debris's task, while one lives */
static struct task *debris_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color aquatic_mine_block_colors[BLOCK_COLORS] = {
    {0xff000000, 0xffffffff},
    {0xffff0000,       0x66},
    {0xff06ff00,       0xc0},
    {0xffff9c00,       0x20},
    {0xff2400ff,       0x3e},
};
/* clang-format on */
static NJS_POINT3 aquatic_mine_block_origin = {-800.0F, 0.0F, 500.0F};
static NJS_POINT3 aquatic_mine_block_cell = {5.390625F, 0.0F, 5.390625F};

void aquatic_mine_init(void)
{
    struct task *task;

    world_land_start(&aquatic_mine_land);
    world_load_block_grid("WATER.BMP", aquatic_mine_block_colors,
                          &aquatic_mine_block_origin, &aquatic_mine_block_cell);
    texture_load_shared(aquatic_mine_pvms, aquatic_mine_texture_groups);
    texture_await_shared(aquatic_mine_pvms, aquatic_mine_texture_groups);
    aquatic_mine_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&aquatic_mine_objects, aquatic_mine_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &aquatic_mine_lights[0]);
    light_set(1, &aquatic_mine_lights[1]);
    light_set(2, &aquatic_mine_lights[0]);
    light_set(3, &aquatic_mine_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg07_light.bin");
    object_builds_run((Uint8 *)world_build_land(
        &aquatic_mine_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    music_fade_in(0);
    music_play_or_defer("A_MINE.ADX");
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
    }
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
    fog_read("stg07_fog.bin", &aquatic_mine_water_fog);
    fog_read("stg07_fogB.bin", &aquatic_mine_air_fog);
    object_light_create();
    muddy_create(0, NULL);
    water_start();
    if (debris_task == NULL) {
        water_specks_create(0, DEBRIS_SPECKS, &debris_task);
    }
    messenger_messages_load("mh0007");
    hint_box_hints_load("bh0007");
    hunt_set_hints(hunt_location_hints);
    hunt_stage_messages = hunt_load_hint_file("eh0007");
    emerald_set_paths(hunt_piece_routes, HUNT_PIECE_ROUTES);
    smoke_task_create(0, NULL);
    splash_task_create(0, NULL);
    if (game_state.two_players != 0) {
        smoke_task_create(1, NULL);
        splash_task_create(1, NULL);
    }
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    switch_clear_all();
    sound_load_stage("se_ac_am.mlt", &aquatic_mine_sound_list);
}

void aquatic_mine_exit(void)
{
    syFree(hunt_stage_messages);
    hunt_stage_messages = NULL;
    hint_box_hints_free();
    messenger_messages_free();
    if (debris_task != NULL) {
        task_destroy(debris_task);
    }
    emerald_clear_paths();
    water_stop();
    world_free_block_grid();
    object_light_destroy();
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
    texture_release_shared(aquatic_mine_pvms, aquatic_mine_texture_groups);
    set_end();
    if (aquatic_mine_set != NULL) {
        loader_free(aquatic_mine_set);
        aquatic_mine_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void aquatic_mine_step(struct task *task)
{
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
    NJS_POINT3 pos;

    if (player_work[0] != NULL && game_state.steps % SOUND_STEPS == 0 &&
        rand() * RAND_SCALE < SOUND_CHANCE) {
        if (!UNDER_WATER(task->work)) {
            pos.x = player_work[0]->pos.x +
                    (((rand() * RAND_SCALE) - 0.5F) * SOUND_SPREAD_X);
            pos.y = player_work[0]->pos.y +
                    (((rand() * RAND_SCALE) - 0.5F) * SOUND_SPREAD_Y);
            pos.z = player_work[0]->pos.z +
                    (((rand() * RAND_SCALE) - 0.5F) * SOUND_SPREAD_Z);
            sound_play_at(SURFACE_SOUND, NULL, 0, 0, &pos);
        }
    }
}

/* the camera is under the water where the surface over its x and z lies above
 * it */
static void background_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float height;
    /* @hack redundant: pos, a point that nothing uses, which only sizes the
     * frame */
    NJS_POINT3 pos;

    height = water_height(&camera_views[camera_current_index]->pos);
    /* @bug logic: the test for no water at the camera compares the height
     * with NO_WATER (0x8c50060e), where water_height gives WATER_NONE for
     * none (0x8c50194c). so the test never holds, and the fallback never runs
     * (0x8c500624). that fallback tests for water, WORLD_FLAT_THIN, above
     * player 0's ahead probe. a camera with no water at its x and z always
     * counts as above the water */
    if ((height == NO_WATER && player_work[0] != NULL &&
         (player_records[0]->ahead.above_flags & WORLD_FLAT_THIN)) ||
        (height != NO_WATER &&
         height > camera_views[camera_current_index]->pos.y)) {
        fog_set(&aquatic_mine_water_fog);
        if (debris_task != NULL) {
            debris_task->work->mode = MODE_SPECKS_SHOWN;
        }
        UNDER_WATER(work) = TRUE;
    } else {
        fog_set(&aquatic_mine_air_fog);
        if (debris_task != NULL) {
            debris_task->work->mode = MODE_SPECKS_HIDDEN;
        }
        UNDER_WATER(work) = FALSE;
    }
}
