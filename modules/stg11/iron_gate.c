/* Iron Gate, the stage of STG11. the background task, BgExec, sets the fog by
 * the shutters' flags, and plays the stage's opening voice */
#include <ninja.h>
#include <sg_xpt.h>
#include <string.h>

#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/black_smoke.h"
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
#include "stage/stage.h"
#include "stg11/iron_gate.h"
#include "stg11/shutter.h"
#include "stg11/sound_point.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* the stage task's mode, in its work's mode */
enum iron_gate_mode { MODE_RUN };

/* the stage's fog and its red fog, which iron_gate_init reads from
 * stg11_fog.bin and stg11_fogB.bin */
static struct fog_setting iron_gate_fog;
static struct fog_setting iron_gate_red_fog;

/* task_create's level for the background task */
#define BACKGROUND_LEVEL 1
/* the set file's block is at least SET_ENTRIES entries long, its head
 * counted as one */
#define SET_ENTRIES 2048
#define KILL_AREAS 3
/* the kill areas' objects, one quad each */
#define KILL_AREA_POINTS 4
/* the lights iron_gate_init sets. where stg11_light.bin reads, the file
 * replaces every light */
#define STAGE_LIGHTS 2

/* while the fog is red, the background plays iron_gate_sounds' RED_SOUND each
 * time game_state.steps reaches a multiple of RED_SOUND_STEPS.
 * RED_SOUND_UPDATES is sound_play_for's duration, in sound updates */
#define RED_SOUND 5
#define RED_SOUND_STEPS 55
#define RED_SOUND_PRIORITY 1
#define RED_SOUND_VOLUME 0
#define RED_SOUND_UPDATES 50
/* the background's first step that finds SHUTTER_FLAG_OPENING_VOICE clear
 * sets it. that step plays the opening voice where the stage's clock reads
 * under OPENING_FRAMES frames */
#define OPENING_FRAMES 10
#define OPENING_PRIORITY 0

static Sint32 dat_8c5480e4 = 0;
static NJS_MATERIAL kill_area_0_material = {
#include "stg11/iron_gate/kill_area_0_material.inc"
};
static Sint16 kill_area_0_meshes[5] = {
#include "stg11/iron_gate/kill_area_0_meshes.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg11/iron_gate/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[KILL_AREA_POINTS] = {
#include "stg11/iron_gate/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[KILL_AREA_POINTS] = {
#include "stg11/iron_gate/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg11/iron_gate/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0 = {
#include "stg11/iron_gate/kill_area_0.inc"
};
static NJS_MATERIAL kill_area_1_material = {
#include "stg11/iron_gate/kill_area_1_material.inc"
};
static Sint16 kill_area_1_meshes[5] = {
#include "stg11/iron_gate/kill_area_1_meshes.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg11/iron_gate/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[KILL_AREA_POINTS] = {
#include "stg11/iron_gate/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[KILL_AREA_POINTS] = {
#include "stg11/iron_gate/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg11/iron_gate/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1 = {
#include "stg11/iron_gate/kill_area_1.inc"
};
static NJS_MATERIAL kill_area_2_material = {
#include "stg11/iron_gate/kill_area_2_material.inc"
};
static Sint16 kill_area_2_meshes[5] = {
#include "stg11/iron_gate/kill_area_2_meshes.inc"
};
static NJS_MESHSET kill_area_2_meshset = {
#include "stg11/iron_gate/kill_area_2_meshset.inc"
};
static NJS_POINT3 kill_area_2_points[KILL_AREA_POINTS] = {
#include "stg11/iron_gate/kill_area_2_points.inc"
};
static NJS_VECTOR kill_area_2_normals[KILL_AREA_POINTS] = {
#include "stg11/iron_gate/kill_area_2_normals.inc"
};
static NJS_MODEL kill_area_2_model = {
#include "stg11/iron_gate/kill_area_2_model.inc"
};
static NJS_OBJECT kill_area_2 = {
#include "stg11/iron_gate/kill_area_2.inc"
};
static struct kill_area iron_gate_kill_areas[KILL_AREAS + 1] = {
#include "stg11/iron_gate/iron_gate_kill_areas.inc"
};
static struct light iron_gate_lights[STAGE_LIGHTS] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
struct task *iron_gate_background = NULL;
/* the set file's block, from iron_gate_init until iron_gate_exit frees it,
 * or null */
static struct set_head *iron_gate_set = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color iron_gate_block_colors[] = {
    {0xff000000, 0xffffffff},
    {0xff00ffff,      0x002},
    {0xffff0000,      0x006},
    {0xff0000ff,      0x004},
    {0xffff00ff,      0x00c},
    {0xffffff00,      0x008},
    {0xff00ff00,      0x018},
    {0xffff8a00,      0x010},
    {0xffff8aff,      0x030},
    {0xff590087,      0x060},
    {0xff871f00,      0x060},
    {0xff00a061,      0x0c0},
    {0xffe53d54,      0x180},
    {0xff2b2b62,      0x300},
    {0xff5788b0,      0x600},
    {0xffb40058,      0x400},
};
/* clang-format on */
static NJS_POINT3 iron_gate_block_origin = {
#include "stg11/iron_gate/iron_gate_block_origin.inc"
};
static NJS_POINT3 iron_gate_block_cell = {
#include "stg11/iron_gate/iron_gate_block_cell.inc"
};

static void background_step(struct task *task);
static void background_draw(struct task *task);
static void background_destroy(struct task *task);

void iron_gate_init(void)
{
    struct task *task;

    world_land_start(&iron_gate_land);
    world_load_block_grid("EGG_WEAPON.BMP", iron_gate_block_colors,
                          &iron_gate_block_origin, &iron_gate_block_cell);
    texture_load_shared(iron_gate_pvms, iron_gate_texture_groups);
    texture_await_shared(iron_gate_pvms, iron_gate_texture_groups);
    iron_gate_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&iron_gate_objects, iron_gate_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &iron_gate_lights[0]);
    light_set(1, &iron_gate_lights[1]);
    light_set(2, &iron_gate_lights[0]);
    light_set(3, &iron_gate_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
#ifdef NONMATCHING
    /* this side loads the lights with lights_load, which keeps the current
     * lights unless the file is exactly their size. the matching side copies
     * them from a file of any size */
    lights_load("stg11_light.bin");
#else
    if (loader_read_file("stg11_light.bin", WORK_AREA) == 0) {
        memcpy(lights, WORK_AREA, sizeof(lights));
    }
#endif
    object_builds_run((Uint8 *)world_build_land(
        &iron_gate_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    music_fade_in(0);
    music_play_or_defer("i_gate.adx");
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        if (iron_gate_background == NULL) {
            iron_gate_background = task;
        }
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(iron_gate_kill_areas);
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
    fog_read("stg11_fog.bin", &iron_gate_fog);
    fog_read("stg11_fogB.bin", &iron_gate_red_fog);
    object_light_create();
    messenger_messages_load("mh0011");
    hint_box_hints_load("bh0011");
    sound_load_stage("se_ac_ig.mlt", &iron_gate_sounds);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    object_builds_init();
    broken_down_smoke_create(0, NULL);
}

void iron_gate_exit(void)
{
    object_builds_exit();
    hint_box_hints_free();
    messenger_messages_free();
    world_free_block_grid();
    object_light_destroy();
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
    texture_release_shared(iron_gate_pvms, iron_gate_texture_groups);
    set_end();
    if (iron_gate_set != NULL) {
        loader_free(iron_gate_set);
        iron_gate_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void iron_gate_step(struct task *task)
{
    struct task_work *work = task->work;

    fog_start();
    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case MODE_RUN:
        break;
    }
}

static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 minute;
    Sint8 second;
    Sint8 frame;

    if ((!shutter_flag_test(SHUTTER_FLAG_CLEAR_A) &&
         shutter_flag_test(SHUTTER_FLAG_RED_A)) ||
        (!shutter_flag_test(SHUTTER_FLAG_CLEAR_B) &&
         shutter_flag_test(SHUTTER_FLAG_RED_B))) {
        if (game_state.steps % RED_SOUND_STEPS == 0) {
            sound_play_for(RED_SOUND, work, RED_SOUND_PRIORITY,
                           RED_SOUND_VOLUME, RED_SOUND_UPDATES);
        }
        fog_set(&iron_gate_red_fog);
    } else {
        fog_set(&iron_gate_fog);
    }
    if (!shutter_flag_test(SHUTTER_FLAG_OPENING_VOICE)) {
        shutter_flag_set(SHUTTER_FLAG_OPENING_VOICE);
        stage_clock_get(&minute, &second, &frame);
        if (minute == 0 && second == 0 && frame < OPENING_FRAMES) {
            voice_play(SHUTTER_OPENING_VOICE, OPENING_PRIORITY);
        }
    }
}

static void background_draw(struct task *task)
{
}

static void background_destroy(struct task *task)
{
    if (iron_gate_background == task) {
        iron_gate_background = NULL;
    }
}
