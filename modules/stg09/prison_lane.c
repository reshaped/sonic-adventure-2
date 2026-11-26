/* Prison Lane, the stage of STG09: its start and end, and its tasks' steps.
 * the background task, BgExec, does nothing */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg09/prison_lane.h"
#include "stg09/sound_point.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* the stage's fog, which prison_lane_init reads from stg09_fog.bin */
static struct fog_setting prison_lane_fog;

#define BACKGROUND_LEVEL 1
/* the set file's block is at least SET_ENTRIES entries long, its head
 * counted as one */
#define SET_ENTRIES 2048
#define KILL_AREAS 3
/* the kill areas' objects, one quad each */
#define KILL_AREA_POINTS 4
/* the lights prison_lane_init sets, until lights_load reads stg09_light.bin
 * over them */
#define STAGE_LIGHTS 2

/* no paths: the list holds only the null that ends it */
static struct path *prison_lane_paths[] = {NULL};
static NJS_MATERIAL kill_area_0_material = {
#include "stg09/prison_lane/kill_area_0_material.inc"
};
static Sint16 kill_area_0_meshes[5] = {
#include "stg09/prison_lane/kill_area_0_meshes.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg09/prison_lane/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[KILL_AREA_POINTS] = {
#include "stg09/prison_lane/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[KILL_AREA_POINTS] = {
#include "stg09/prison_lane/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg09/prison_lane/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0 = {
#include "stg09/prison_lane/kill_area_0.inc"
};
static NJS_MATERIAL kill_area_1_material = {
#include "stg09/prison_lane/kill_area_1_material.inc"
};
static Sint16 kill_area_1_meshes[5] = {
#include "stg09/prison_lane/kill_area_1_meshes.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg09/prison_lane/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[KILL_AREA_POINTS] = {
#include "stg09/prison_lane/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[KILL_AREA_POINTS] = {
#include "stg09/prison_lane/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg09/prison_lane/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1 = {
#include "stg09/prison_lane/kill_area_1.inc"
};
static NJS_MATERIAL kill_area_2_material = {
#include "stg09/prison_lane/kill_area_2_material.inc"
};
static Sint16 kill_area_2_meshes[5] = {
#include "stg09/prison_lane/kill_area_2_meshes.inc"
};
static NJS_MESHSET kill_area_2_meshset = {
#include "stg09/prison_lane/kill_area_2_meshset.inc"
};
static NJS_POINT3 kill_area_2_points[KILL_AREA_POINTS] = {
#include "stg09/prison_lane/kill_area_2_points.inc"
};
static NJS_VECTOR kill_area_2_normals[KILL_AREA_POINTS] = {
#include "stg09/prison_lane/kill_area_2_normals.inc"
};
static NJS_MODEL kill_area_2_model = {
#include "stg09/prison_lane/kill_area_2_model.inc"
};
static NJS_OBJECT kill_area_2 = {
#include "stg09/prison_lane/kill_area_2.inc"
};
static struct kill_area prison_lane_kill_areas[KILL_AREAS + 1] = {
#include "stg09/prison_lane/prison_lane_kill_areas.inc"
};
static struct light prison_lane_lights[STAGE_LIGHTS] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, from prison_lane_init until prison_lane_exit frees
 * it, or null */
static struct set_head *prison_lane_set = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color prison_lane_block_colors[] = {
    {0xff000000, 0xffffffff},
    {0xff00ff00,       0x22},
    {0xffffff00,       0x6e},
    {0xffffffff,       0x44},
    {0xffff0000,       0x8c},
    {0xff0000ff,       0x08},
    {0xff00ffff,       0x18},
    {0xff006700,       0x10},
    {0xff800000,       0x30},
    {0xffff00ff,       0xc0},
};
/* clang-format on */
static NJS_POINT3 prison_lane_block_origin = {
#include "stg09/prison_lane/prison_lane_block_origin.inc"
};
static NJS_POINT3 prison_lane_block_cell = {
#include "stg09/prison_lane/prison_lane_block_cell.inc"
};
/* the three colors prison_lane_step hands njSetBackColor at each step */
static Uint32 prison_lane_back_color_1 = 0xff250d00;
static Uint32 prison_lane_back_color_2 = 0xff250d00;
static Uint32 prison_lane_back_color_3 = 0xff250d00;

static void background_step(struct task *task);
static void background_draw(struct task *task);

void prison_lane_init(void)
{
    struct task *task;

    world_land_start(&prison_lane_land);
    world_load_block_grid("TAILS_PRISON.BMP", prison_lane_block_colors,
                          &prison_lane_block_origin, &prison_lane_block_cell);
    texture_load_shared(prison_lane_pvms, prison_lane_texture_groups);
    texture_await_shared(prison_lane_pvms, prison_lane_texture_groups);
    prison_lane_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&prison_lane_objects, prison_lane_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &prison_lane_lights[0]);
    light_set(1, &prison_lane_lights[1]);
    light_set(2, &prison_lane_lights[0]);
    light_set(3, &prison_lane_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg09_light.bin");
    object_builds_run((Uint8 *)world_build_land(
        &prison_lane_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    music_fade_in(0);
    if (game_state.demo == 0) {
        music_play_or_defer("p_lane.adx");
    } else {
        music_play_or_defer("t9_miles.adx");
    }
    path_create_manager(prison_lane_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
    }
    kill_areas_start(prison_lane_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.draw_blast_ring == NULL) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    fog_start_file("stg09_fog.bin", &prison_lane_fog);
    njFogDisable();
    messenger_messages_load("mh0009");
    hint_box_hints_load("bh0009");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_pl.mlt", &prison_lane_sounds);
}

void prison_lane_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
    world_free_block_grid();
    fog_stop();
    njFogDisable();
    if (player_hooks.draw_blast_ring == bomb_hat_draw_ring) {
        player_hooks.draw_blast_ring = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(prison_lane_pvms, prison_lane_texture_groups);
    set_end();
    if (prison_lane_set != NULL) {
        loader_free(prison_lane_set);
        prison_lane_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
    njSetBackColor(0, 0, 0);
}

void prison_lane_step(struct task *task)
{
    struct task_work *work = task->work;

    njSetBackColor(prison_lane_back_color_1, prison_lane_back_color_2,
                   prison_lane_back_color_3);
    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
}

static void background_draw(struct task *task)
{
}
