/* Radical Highway, stg14: the stage's init, exit and step, and its
 * background. the background task, BgExec, draws the sky about the current
 * camera. it plays the stage's ambient sound near the players */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
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
#include "stg14/chase_light.h"
#include "stg14/gust_sound.h"
#include "stg14/pulsing_light.h"
#include "stg14/radical_highway.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* clang-format off */
#include "stg14/radical_highway/text.inc"
#include "stg14/paths.h"
#include "stg14/sky.h"
/* clang-format on */

/* the init reads the fog's setting from stg14_fog.bin */
static struct fog_setting radical_highway_fog;

/* texture_load_shared fills background_pvm_texlist from bgtex14. it gives the
 * textures by name to sky_texlist */
static NJS_TEXNAME background_pvm_texnames[3] = {
#include "stg14/radical_highway/background_pvm_texnames.inc"
};
static NJS_TEXLIST background_pvm_texlist = {
#include "stg14/radical_highway/background_pvm_texlist.inc"
};
static NJS_TEXLIST *background_group[2] = {
#include "stg14/radical_highway/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg14/radical_highway/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg14/radical_highway/background_pvms.inc"
};

/* the models of the stage's sixteen kill areas. SHC refuses braces around a
 * material's colors, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL kill_area_0_material = {
#include "stg14/radical_highway/kill_area_0_material.inc"
};
static Uint16 kill_area_0_meshes[12] = {
#include "stg14/radical_highway/kill_area_0_meshes.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg14/radical_highway/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[8] = {
#include "stg14/radical_highway/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[8] = {
#include "stg14/radical_highway/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg14/radical_highway/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0 = {
#include "stg14/radical_highway/kill_area_0.inc"
};
static NJS_MATERIAL kill_area_1_material = {
#include "stg14/radical_highway/kill_area_1_material.inc"
};
static Uint16 kill_area_1_meshes[5] = {
#include "stg14/radical_highway/kill_area_1_meshes.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg14/radical_highway/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[4] = {
#include "stg14/radical_highway/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[4] = {
#include "stg14/radical_highway/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg14/radical_highway/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1 = {
#include "stg14/radical_highway/kill_area_1.inc"
};
static NJS_MATERIAL kill_area_2_material = {
#include "stg14/radical_highway/kill_area_2_material.inc"
};
static Uint16 kill_area_2_meshes[9] = {
#include "stg14/radical_highway/kill_area_2_meshes.inc"
};
static NJS_MESHSET kill_area_2_meshset = {
#include "stg14/radical_highway/kill_area_2_meshset.inc"
};
static NJS_POINT3 kill_area_2_points[8] = {
#include "stg14/radical_highway/kill_area_2_points.inc"
};
static NJS_VECTOR kill_area_2_normals[8] = {
#include "stg14/radical_highway/kill_area_2_normals.inc"
};
static NJS_MODEL kill_area_2_model = {
#include "stg14/radical_highway/kill_area_2_model.inc"
};
static NJS_OBJECT kill_area_2 = {
#include "stg14/radical_highway/kill_area_2.inc"
};
static NJS_MATERIAL kill_area_3_material = {
#include "stg14/radical_highway/kill_area_3_material.inc"
};
static Uint16 kill_area_3_meshes[12] = {
#include "stg14/radical_highway/kill_area_3_meshes.inc"
};
static NJS_MESHSET kill_area_3_meshset = {
#include "stg14/radical_highway/kill_area_3_meshset.inc"
};
static NJS_POINT3 kill_area_3_points[8] = {
#include "stg14/radical_highway/kill_area_3_points.inc"
};
static NJS_VECTOR kill_area_3_normals[8] = {
#include "stg14/radical_highway/kill_area_3_normals.inc"
};
static NJS_MODEL kill_area_3_model = {
#include "stg14/radical_highway/kill_area_3_model.inc"
};
static NJS_OBJECT kill_area_3 = {
#include "stg14/radical_highway/kill_area_3.inc"
};
static NJS_MATERIAL kill_area_4_material = {
#include "stg14/radical_highway/kill_area_4_material.inc"
};
static Uint16 kill_area_4_meshes[5] = {
#include "stg14/radical_highway/kill_area_4_meshes.inc"
};
static NJS_MESHSET kill_area_4_meshset = {
#include "stg14/radical_highway/kill_area_4_meshset.inc"
};
static NJS_POINT3 kill_area_4_points[4] = {
#include "stg14/radical_highway/kill_area_4_points.inc"
};
static NJS_VECTOR kill_area_4_normals[4] = {
#include "stg14/radical_highway/kill_area_4_normals.inc"
};
static NJS_MODEL kill_area_4_model = {
#include "stg14/radical_highway/kill_area_4_model.inc"
};
static NJS_OBJECT kill_area_4 = {
#include "stg14/radical_highway/kill_area_4.inc"
};
static NJS_MATERIAL kill_area_5_material = {
#include "stg14/radical_highway/kill_area_5_material.inc"
};
static Uint16 kill_area_5_meshes[5] = {
#include "stg14/radical_highway/kill_area_5_meshes.inc"
};
static NJS_MESHSET kill_area_5_meshset = {
#include "stg14/radical_highway/kill_area_5_meshset.inc"
};
static NJS_POINT3 kill_area_5_points[4] = {
#include "stg14/radical_highway/kill_area_5_points.inc"
};
static NJS_VECTOR kill_area_5_normals[4] = {
#include "stg14/radical_highway/kill_area_5_normals.inc"
};
static NJS_MODEL kill_area_5_model = {
#include "stg14/radical_highway/kill_area_5_model.inc"
};
static NJS_OBJECT kill_area_5 = {
#include "stg14/radical_highway/kill_area_5.inc"
};
static NJS_MATERIAL kill_area_6_material = {
#include "stg14/radical_highway/kill_area_6_material.inc"
};
static Uint16 kill_area_6_meshes[5] = {
#include "stg14/radical_highway/kill_area_6_meshes.inc"
};
static NJS_MESHSET kill_area_6_meshset = {
#include "stg14/radical_highway/kill_area_6_meshset.inc"
};
static NJS_POINT3 kill_area_6_points[4] = {
#include "stg14/radical_highway/kill_area_6_points.inc"
};
static NJS_VECTOR kill_area_6_normals[4] = {
#include "stg14/radical_highway/kill_area_6_normals.inc"
};
static NJS_MODEL kill_area_6_model = {
#include "stg14/radical_highway/kill_area_6_model.inc"
};
static NJS_OBJECT kill_area_6 = {
#include "stg14/radical_highway/kill_area_6.inc"
};
static NJS_MATERIAL kill_area_7_material = {
#include "stg14/radical_highway/kill_area_7_material.inc"
};
static Uint16 kill_area_7_meshes[12] = {
#include "stg14/radical_highway/kill_area_7_meshes.inc"
};
static NJS_MESHSET kill_area_7_meshset = {
#include "stg14/radical_highway/kill_area_7_meshset.inc"
};
static NJS_POINT3 kill_area_7_points[8] = {
#include "stg14/radical_highway/kill_area_7_points.inc"
};
static NJS_VECTOR kill_area_7_normals[8] = {
#include "stg14/radical_highway/kill_area_7_normals.inc"
};
static NJS_MODEL kill_area_7_model = {
#include "stg14/radical_highway/kill_area_7_model.inc"
};
static NJS_OBJECT kill_area_7 = {
#include "stg14/radical_highway/kill_area_7.inc"
};
static NJS_MATERIAL kill_area_8_material = {
#include "stg14/radical_highway/kill_area_8_material.inc"
};
static Uint16 kill_area_8_meshes[5] = {
#include "stg14/radical_highway/kill_area_8_meshes.inc"
};
static NJS_MESHSET kill_area_8_meshset = {
#include "stg14/radical_highway/kill_area_8_meshset.inc"
};
static NJS_POINT3 kill_area_8_points[4] = {
#include "stg14/radical_highway/kill_area_8_points.inc"
};
static NJS_VECTOR kill_area_8_normals[4] = {
#include "stg14/radical_highway/kill_area_8_normals.inc"
};
static NJS_MODEL kill_area_8_model = {
#include "stg14/radical_highway/kill_area_8_model.inc"
};
static NJS_OBJECT kill_area_8 = {
#include "stg14/radical_highway/kill_area_8.inc"
};
static NJS_MATERIAL kill_area_9_material = {
#include "stg14/radical_highway/kill_area_9_material.inc"
};
static Uint16 kill_area_9_meshes[7] = {
#include "stg14/radical_highway/kill_area_9_meshes.inc"
};
static NJS_MESHSET kill_area_9_meshset = {
#include "stg14/radical_highway/kill_area_9_meshset.inc"
};
static NJS_POINT3 kill_area_9_points[6] = {
#include "stg14/radical_highway/kill_area_9_points.inc"
};
static NJS_VECTOR kill_area_9_normals[6] = {
#include "stg14/radical_highway/kill_area_9_normals.inc"
};
static NJS_MODEL kill_area_9_model = {
#include "stg14/radical_highway/kill_area_9_model.inc"
};
static NJS_OBJECT kill_area_9 = {
#include "stg14/radical_highway/kill_area_9.inc"
};
static NJS_MATERIAL kill_area_10_material = {
#include "stg14/radical_highway/kill_area_10_material.inc"
};
static Uint16 kill_area_10_meshes[5] = {
#include "stg14/radical_highway/kill_area_10_meshes.inc"
};
static NJS_MESHSET kill_area_10_meshset = {
#include "stg14/radical_highway/kill_area_10_meshset.inc"
};
static NJS_POINT3 kill_area_10_points[4] = {
#include "stg14/radical_highway/kill_area_10_points.inc"
};
static NJS_VECTOR kill_area_10_normals[4] = {
#include "stg14/radical_highway/kill_area_10_normals.inc"
};
static NJS_MODEL kill_area_10_model = {
#include "stg14/radical_highway/kill_area_10_model.inc"
};
static NJS_OBJECT kill_area_10 = {
#include "stg14/radical_highway/kill_area_10.inc"
};
static NJS_MATERIAL kill_area_11_material = {
#include "stg14/radical_highway/kill_area_11_material.inc"
};
static Uint16 kill_area_11_meshes[5] = {
#include "stg14/radical_highway/kill_area_11_meshes.inc"
};
static NJS_MESHSET kill_area_11_meshset = {
#include "stg14/radical_highway/kill_area_11_meshset.inc"
};
static NJS_POINT3 kill_area_11_points[4] = {
#include "stg14/radical_highway/kill_area_11_points.inc"
};
static NJS_VECTOR kill_area_11_normals[4] = {
#include "stg14/radical_highway/kill_area_11_normals.inc"
};
static NJS_MODEL kill_area_11_model = {
#include "stg14/radical_highway/kill_area_11_model.inc"
};
static NJS_OBJECT kill_area_11 = {
#include "stg14/radical_highway/kill_area_11.inc"
};
static NJS_MATERIAL kill_area_12_material = {
#include "stg14/radical_highway/kill_area_12_material.inc"
};
static Uint16 kill_area_12_meshes[7] = {
#include "stg14/radical_highway/kill_area_12_meshes.inc"
};
static NJS_MESHSET kill_area_12_meshset = {
#include "stg14/radical_highway/kill_area_12_meshset.inc"
};
static NJS_POINT3 kill_area_12_points[6] = {
#include "stg14/radical_highway/kill_area_12_points.inc"
};
static NJS_VECTOR kill_area_12_normals[6] = {
#include "stg14/radical_highway/kill_area_12_normals.inc"
};
static NJS_MODEL kill_area_12_model = {
#include "stg14/radical_highway/kill_area_12_model.inc"
};
static NJS_OBJECT kill_area_12 = {
#include "stg14/radical_highway/kill_area_12.inc"
};
static NJS_MATERIAL kill_area_13_material = {
#include "stg14/radical_highway/kill_area_13_material.inc"
};
static Uint16 kill_area_13_meshes[5] = {
#include "stg14/radical_highway/kill_area_13_meshes.inc"
};
static NJS_MESHSET kill_area_13_meshset = {
#include "stg14/radical_highway/kill_area_13_meshset.inc"
};
static NJS_POINT3 kill_area_13_points[4] = {
#include "stg14/radical_highway/kill_area_13_points.inc"
};
static NJS_VECTOR kill_area_13_normals[4] = {
#include "stg14/radical_highway/kill_area_13_normals.inc"
};
static NJS_MODEL kill_area_13_model = {
#include "stg14/radical_highway/kill_area_13_model.inc"
};
static NJS_OBJECT kill_area_13 = {
#include "stg14/radical_highway/kill_area_13.inc"
};
static NJS_MATERIAL kill_area_14_material = {
#include "stg14/radical_highway/kill_area_14_material.inc"
};
static Uint16 kill_area_14_meshes[5] = {
#include "stg14/radical_highway/kill_area_14_meshes.inc"
};
static NJS_MESHSET kill_area_14_meshset = {
#include "stg14/radical_highway/kill_area_14_meshset.inc"
};
static NJS_POINT3 kill_area_14_points[4] = {
#include "stg14/radical_highway/kill_area_14_points.inc"
};
static NJS_VECTOR kill_area_14_normals[4] = {
#include "stg14/radical_highway/kill_area_14_normals.inc"
};
static NJS_MODEL kill_area_14_model = {
#include "stg14/radical_highway/kill_area_14_model.inc"
};
static NJS_OBJECT kill_area_14 = {
#include "stg14/radical_highway/kill_area_14.inc"
};
static NJS_MATERIAL kill_area_15_material = {
#include "stg14/radical_highway/kill_area_15_material.inc"
};
static Uint16 kill_area_15_meshes[5] = {
#include "stg14/radical_highway/kill_area_15_meshes.inc"
};
static NJS_MESHSET kill_area_15_meshset = {
#include "stg14/radical_highway/kill_area_15_meshset.inc"
};
static NJS_POINT3 kill_area_15_points[4] = {
#include "stg14/radical_highway/kill_area_15_points.inc"
};
static NJS_VECTOR kill_area_15_normals[4] = {
#include "stg14/radical_highway/kill_area_15_normals.inc"
};
static NJS_MODEL kill_area_15_model = {
#include "stg14/radical_highway/kill_area_15_model.inc"
};
static NJS_OBJECT kill_area_15 = {
#include "stg14/radical_highway/kill_area_15.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static struct kill_area radical_highway_kill_areas[17] = {
#include "stg14/radical_highway/radical_highway_kill_areas.inc"
};

static struct light radical_highway_light = {
    {0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F};
static struct light radical_highway_light_1 = {
    {0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F};
/* the set file's block, from radical_highway_init until radical_highway_exit
 * frees it, or null */
static struct set_head *radical_highway_set = NULL;
/* the background task, or null while there is none */
static struct task *background_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color radical_highway_block_colors[] = {
    {0xff000000, 0xffffffff},
    {0xffff0000,      0x222},
    {0xffffff00,      0x666},
    {0xff00ffff,       0x44},
    {0xff0000ff,       0xcc},
    {0xffff00ff,       0x88},
    {0xff00ff00,      0x198},
    {0xffff6400,      0x110},
    {0xff006400,      0x330},
};
/* clang-format on */
static NJS_POINT3 radical_highway_block_origin = {
#include "stg14/radical_highway/radical_highway_block_origin.inc"
};
static NJS_POINT3 radical_highway_block_cell = {
#include "stg14/radical_highway/radical_highway_block_cell.inc"
};
/* the sky's build, or null where background_build made none. the sky then
 * draws as a chunk model */
static NJS_DIRECT_HEAD *background_built = NULL;
/* the ambient sound's volume, which nothing changes from 0 */
static Sint32 ambience_volume = 0;

/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 2048
/* the background task's level */
#define BACKGROUND_LEVEL 1
/* each step of the background plays the ambient sound with a chance of
 * AMBIENCE_CHANCE. it plays at a player's pos, moved by up to half
 * AMBIENCE_SPREAD each way along each axis */
#define AMBIENCE_SOUND 7
#define AMBIENCE_PRIORITY 0
#define AMBIENCE_CHANCE 0.05F
#define AMBIENCE_SPREAD 530.0F

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);
static Uint8 *background_build(Uint8 *buf);

void radical_highway_init(void)
{
    struct task *task;
    Uint8 *buf;
    /* @hack spelling: set and light, locals for light_set and
     * &radical_highway_light, where those by their names would do */
    void (*set)(int index, const struct light *light);
    struct light *light;

    world_land_start(&radical_highway_land);
    world_load_block_grid("HIGHWAY.BMP", radical_highway_block_colors,
                          &radical_highway_block_origin,
                          &radical_highway_block_cell);
    texture_load_shared(radical_highway_pvms, radical_highway_texture_groups);
    texture_await_shared(radical_highway_pvms, radical_highway_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    radical_highway_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&radical_highway_objects, radical_highway_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    set = light_set;
    light = &radical_highway_light;
    set(0, light);
    set(1, &radical_highway_light_1);
    set(2, light);
    set(3, light);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg14_light.bin");
    buf = (Uint8 *)world_build_land(&radical_highway_land,
                                    (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = chase_light_build(buf);
    buf = highway_pulsing_light_build(buf);
    buf = background_build(buf);
    object_builds_run(buf);
    music_fade_in(0);
    if (game_state.demo == 0) {
        music_play_or_defer("r_hwy.adx");
    } else {
        music_play_or_defer("t9_shado.adx");
    }
    path_create_manager(radical_highway_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(radical_highway_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: the test and the set made again, though the first
     * set drop_rings */
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    fog_start_file("stg14_fog.bin", &radical_highway_fog);
    njFogEnable();
    messenger_messages_load("mh0014");
    hint_box_hints_load("bh0014");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_rh.mlt", &radical_highway_sound_list);
}

void radical_highway_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
    fog_stop();
    njFogDisable();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(radical_highway_pvms,
                           radical_highway_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (radical_highway_set != NULL) {
        loader_free(radical_highway_set);
        radical_highway_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void radical_highway_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

static Uint8 *background_build(Uint8 *buf)
{
    njSetTexture(&sky_texlist);
    background_built = object_tree_build(&buf, &sky_object);
    return buf;
}

/* play the ambient sound now and then near player 0. while player 1 is in
 * the game too, it plays near player 1 on an odd step instead */
static void background_step(struct task *task)
{
    NJS_POINT3 pos;

    if (player_work[0] != NULL && rand() * RAND_SCALE < AMBIENCE_CHANCE) {
        /* @hack redundant: player 0's work tested again, though the test
         * above has passed it */
        if (player_work[1] != NULL && player_work[0] != NULL) {
            pos = player_work[game_state.steps & 1]->pos;
        } else {
            pos = player_work[0]->pos;
        }
        pos.x += ((rand() * RAND_SCALE) - 0.5F) * AMBIENCE_SPREAD;
        pos.y += ((rand() * RAND_SCALE) - 0.5F) * AMBIENCE_SPREAD;
        pos.z += ((rand() * RAND_SCALE) - 0.5F) * AMBIENCE_SPREAD;
        sound_play_at(AMBIENCE_SOUND, NULL, AMBIENCE_PRIORITY, ambience_volume,
                      &pos);
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

/* draw the sky about the current camera with the fog off. it leaves the fog
 * on */
static void background_draw(struct task *task)
{
    NJS_POINT3 camera;

    /* @hack spelling: the camera's pos copied a field at a time, where a copy
     * of the whole point would do */
    camera.x = camera_views[camera_current_index]->pos.x;
    camera.y = camera_views[camera_current_index]->pos.y;
    camera.z = camera_views[camera_current_index]->pos.z;
    njFogDisable();
    njPushMatrixEx();
    njTranslateV(NULL, &camera);
    njSetTexture(&sky_texlist);
    if (background_built != NULL) {
        njDirectDrawObject(background_built);
    } else {
        njCnkEasyDrawObject(&sky_object);
    }
    njPopMatrix(1);
    njFogEnable();
}
