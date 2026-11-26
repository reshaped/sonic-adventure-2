/* Mission Street, stg17: the stage's init, exit and step, and its
 * background. the background task, BgExec, draws the sky about the current
 * camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg17/mission_street/text.inc"
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
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
#include "shared/parked_car.h"
#include "shared/ring.h"
#include "shared/street_prop.h"
#include "shared/street_sounds.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg17/mission_street.h"
#include "stg17/object_archives.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

#define BACKGROUND_BUILDS 1
/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 2048
/* the background task's level */
#define BACKGROUND_LEVEL 1
#define FALL_PLAYERS 2
#define FALL_OUT_HEIGHT (-260.0F)
#define BOARD_PAD 1
#define BOARD_BUTTON PDD_DGT_TY

/* the init reads the fog's setting from stg17_fog.bin */
static struct fog_setting mission_street_fog;

/* the list holds only the null that ends it, so path_create_manager starts
 * no path */
static struct path *mission_street_paths[] = {NULL};

#include "shared/mission_street/sky.h"

/* texture_load_shared fills background_pvm_texlist from bgtex17. it gives the
 * textures by name to street_sky_texlist */
static NJS_TEXNAME background_pvm_texnames[3] = {
#include "stg17/mission_street/background_pvm_texnames.inc"
};
static NJS_TEXLIST background_pvm_texlist = {
#include "stg17/mission_street/background_pvm_texlist.inc"
};
static Bool background_built = FALSE;
static struct model_build background_builds[BACKGROUND_BUILDS] = {
#include "stg17/mission_street/background_builds.inc"
};
static NJS_TEXLIST *background_group[2] = {
#include "stg17/mission_street/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg17/mission_street/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg17/mission_street/background_pvms.inc"
};

/* The ten kill areas and their BASIC collision strips. */
static NJS_MATERIAL kill_area_0_material = {
#include "stg17/mission_street/kill_area_0_material.inc"
};
static Sint16 kill_area_0_meshes[5] = {
#include "stg17/mission_street/kill_area_0_meshes.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg17/mission_street/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[4] = {
#include "stg17/mission_street/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[4] = {
#include "stg17/mission_street/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg17/mission_street/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0_object = {
#include "stg17/mission_street/kill_area_0_object.inc"
};
static NJS_MATERIAL kill_area_1_material = {
#include "stg17/mission_street/kill_area_1_material.inc"
};
static Sint16 kill_area_1_meshes[12] = {
#include "stg17/mission_street/kill_area_1_meshes.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg17/mission_street/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[8] = {
#include "stg17/mission_street/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[8] = {
#include "stg17/mission_street/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg17/mission_street/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1_object = {
#include "stg17/mission_street/kill_area_1_object.inc"
};
static NJS_MATERIAL kill_area_2_material = {
#include "stg17/mission_street/kill_area_2_material.inc"
};
static Sint16 kill_area_2_meshes[12] = {
#include "stg17/mission_street/kill_area_2_meshes.inc"
};
static NJS_MESHSET kill_area_2_meshset = {
#include "stg17/mission_street/kill_area_2_meshset.inc"
};
static NJS_POINT3 kill_area_2_points[8] = {
#include "stg17/mission_street/kill_area_2_points.inc"
};
static NJS_VECTOR kill_area_2_normals[8] = {
#include "stg17/mission_street/kill_area_2_normals.inc"
};
static NJS_MODEL kill_area_2_model = {
#include "stg17/mission_street/kill_area_2_model.inc"
};
static NJS_OBJECT kill_area_2_object = {
#include "stg17/mission_street/kill_area_2_object.inc"
};
static NJS_MATERIAL kill_area_3_material = {
#include "stg17/mission_street/kill_area_3_material.inc"
};
static Sint16 kill_area_3_meshes[5] = {
#include "stg17/mission_street/kill_area_3_meshes.inc"
};
static NJS_MESHSET kill_area_3_meshset = {
#include "stg17/mission_street/kill_area_3_meshset.inc"
};
static NJS_POINT3 kill_area_3_points[4] = {
#include "stg17/mission_street/kill_area_3_points.inc"
};
static NJS_VECTOR kill_area_3_normals[4] = {
#include "stg17/mission_street/kill_area_3_normals.inc"
};
static NJS_MODEL kill_area_3_model = {
#include "stg17/mission_street/kill_area_3_model.inc"
};
static NJS_OBJECT kill_area_3_object = {
#include "stg17/mission_street/kill_area_3_object.inc"
};
static NJS_MATERIAL kill_area_4_material = {
#include "stg17/mission_street/kill_area_4_material.inc"
};
static Sint16 kill_area_4_meshes[5] = {
#include "stg17/mission_street/kill_area_4_meshes.inc"
};
static NJS_MESHSET kill_area_4_meshset = {
#include "stg17/mission_street/kill_area_4_meshset.inc"
};
static NJS_POINT3 kill_area_4_points[4] = {
#include "stg17/mission_street/kill_area_4_points.inc"
};
static NJS_VECTOR kill_area_4_normals[4] = {
#include "stg17/mission_street/kill_area_4_normals.inc"
};
static NJS_MODEL kill_area_4_model = {
#include "stg17/mission_street/kill_area_4_model.inc"
};
static NJS_OBJECT kill_area_4_object = {
#include "stg17/mission_street/kill_area_4_object.inc"
};
static NJS_MATERIAL kill_area_5_material = {
#include "stg17/mission_street/kill_area_5_material.inc"
};
static Sint16 kill_area_5_meshes[5] = {
#include "stg17/mission_street/kill_area_5_meshes.inc"
};
static NJS_MESHSET kill_area_5_meshset = {
#include "stg17/mission_street/kill_area_5_meshset.inc"
};
static NJS_POINT3 kill_area_5_points[4] = {
#include "stg17/mission_street/kill_area_5_points.inc"
};
static NJS_VECTOR kill_area_5_normals[4] = {
#include "stg17/mission_street/kill_area_5_normals.inc"
};
static NJS_MODEL kill_area_5_model = {
#include "stg17/mission_street/kill_area_5_model.inc"
};
static NJS_OBJECT kill_area_5_object = {
#include "stg17/mission_street/kill_area_5_object.inc"
};
static NJS_MATERIAL kill_area_6_material = {
#include "stg17/mission_street/kill_area_6_material.inc"
};
static Sint16 kill_area_6_meshes[5] = {
#include "stg17/mission_street/kill_area_6_meshes.inc"
};
static NJS_MESHSET kill_area_6_meshset = {
#include "stg17/mission_street/kill_area_6_meshset.inc"
};
static NJS_POINT3 kill_area_6_points[4] = {
#include "stg17/mission_street/kill_area_6_points.inc"
};
static NJS_VECTOR kill_area_6_normals[4] = {
#include "stg17/mission_street/kill_area_6_normals.inc"
};
static NJS_MODEL kill_area_6_model = {
#include "stg17/mission_street/kill_area_6_model.inc"
};
static NJS_OBJECT kill_area_6_object = {
#include "stg17/mission_street/kill_area_6_object.inc"
};
static NJS_MATERIAL kill_area_7_material = {
#include "stg17/mission_street/kill_area_7_material.inc"
};
static Sint16 kill_area_7_meshes[5] = {
#include "stg17/mission_street/kill_area_7_meshes.inc"
};
static NJS_MESHSET kill_area_7_meshset = {
#include "stg17/mission_street/kill_area_7_meshset.inc"
};
static NJS_POINT3 kill_area_7_points[4] = {
#include "stg17/mission_street/kill_area_7_points.inc"
};
static NJS_VECTOR kill_area_7_normals[4] = {
#include "stg17/mission_street/kill_area_7_normals.inc"
};
static NJS_MODEL kill_area_7_model = {
#include "stg17/mission_street/kill_area_7_model.inc"
};
static NJS_OBJECT kill_area_7_object = {
#include "stg17/mission_street/kill_area_7_object.inc"
};
static NJS_MATERIAL kill_area_8_material = {
#include "stg17/mission_street/kill_area_8_material.inc"
};
static Sint16 kill_area_8_meshes[5] = {
#include "stg17/mission_street/kill_area_8_meshes.inc"
};
static NJS_MESHSET kill_area_8_meshset = {
#include "stg17/mission_street/kill_area_8_meshset.inc"
};
static NJS_POINT3 kill_area_8_points[4] = {
#include "stg17/mission_street/kill_area_8_points.inc"
};
static NJS_VECTOR kill_area_8_normals[4] = {
#include "stg17/mission_street/kill_area_8_normals.inc"
};
static NJS_MODEL kill_area_8_model = {
#include "stg17/mission_street/kill_area_8_model.inc"
};
static NJS_OBJECT kill_area_8_object = {
#include "stg17/mission_street/kill_area_8_object.inc"
};
static NJS_MATERIAL kill_area_9_material = {
#include "stg17/mission_street/kill_area_9_material.inc"
};
static Sint16 kill_area_9_meshes[5] = {
#include "stg17/mission_street/kill_area_9_meshes.inc"
};
static NJS_MESHSET kill_area_9_meshset = {
#include "stg17/mission_street/kill_area_9_meshset.inc"
};
static NJS_POINT3 kill_area_9_points[4] = {
#include "stg17/mission_street/kill_area_9_points.inc"
};
static NJS_VECTOR kill_area_9_normals[4] = {
#include "stg17/mission_street/kill_area_9_normals.inc"
};
static NJS_MODEL kill_area_9_model = {
#include "stg17/mission_street/kill_area_9_model.inc"
};
static NJS_OBJECT kill_area_9_object = {
#include "stg17/mission_street/kill_area_9_object.inc"
};
static struct kill_area mission_street_kill_areas[11] = {
#include "stg17/mission_street/mission_street_kill_areas.inc"
};

static struct light mission_street_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *mission_street_set = NULL;
/* the background task, or null while there is none */
static struct task *background_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color mission_street_block_colors[] = {
    {0xff000000,  0x0},
    {0xffff0000, 0x22},
    {0xffffff00, 0x66},
    {0xff00ffff, 0x44},
    {0xff0000ff, 0xcc},
    {0xffff00ff,  0x8},
    {0xff00ff00, 0x18},
    {0xffff6400, 0x10},
    {0xff006400, 0x30},
};
/* clang-format on */
static NJS_POINT3 mission_street_block_origin = {
#include "stg17/mission_street/mission_street_block_origin.inc"
};
static NJS_POINT3 mission_street_block_cell = {
#include "stg17/mission_street/mission_street_block_cell.inc"
};
/* how far above the current camera the draw puts the sky's origin */
static Float background_lift = 886.5248F;

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

static Uint8 *background_build(Uint8 *buf)
{
    njFogDisable();
    if (model_builds_compile(&buf, background_builds, BACKGROUND_BUILDS) ==
        BACKGROUND_BUILDS) {
        background_built = TRUE;
    }
    njFogEnable();
    return buf;
}

void mission_street_init(void)
{
    struct task *task;
    Uint8 *buf;

    world_land_start(&mission_street_land);
    world_load_block_grid("MISSION.BMP", mission_street_block_colors,
                          &mission_street_block_origin,
                          &mission_street_block_cell);
    texture_load_shared(mission_street_pvms, mission_street_texture_groups);
    texture_await_shared(mission_street_pvms, mission_street_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    mission_street_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&mission_street_objects, mission_street_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &mission_street_lights[0]);
    light_set(1, &mission_street_lights[1]);
    light_set(2, &mission_street_lights[0]);
    light_set(3, &mission_street_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg17_light.bin");
    buf = (Uint8 *)world_build_land(&mission_street_land,
                                    (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = street_prop_build(buf);
    buf = object_builds_run(buf);
    buf = background_build(buf);
    parked_car_build(buf);
    music_fade_in(0);
    music_play_or_defer("m_street.adx");
    path_create_manager(mission_street_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(mission_street_kill_areas);
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
    fog_start_file("stg17_fog.bin", &mission_street_fog);
    njFogEnable();
    messenger_messages_load("mh0017");
    hint_box_hints_load("bh0017");
    broken_down_smoke_create(0, NULL);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    object_builds_init();
    sound_load_stage("se_ac_ms.mlt", &mission_street_sound_list);
}

void mission_street_exit(void)
{
    object_builds_exit();
    hint_box_hints_free();
    messenger_messages_free();
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
    texture_release_shared(mission_street_pvms, mission_street_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (mission_street_set != NULL) {
        loader_free(mission_street_set);
        mission_street_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void mission_street_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    if (kill_areas_task != NULL && kill_areas_task->child == NULL) {
        for (player = 0; player < FALL_PLAYERS; player++) {
            if (player_work[player] != NULL &&
                !(player_records[player]->flags & PLAYER_MISSED)) {
                /* @hack spelling: the height tested in an if of its own, where
                 * one test of all three would do */
                if (player_work[player]->pos.y < FALL_OUT_HEIGHT) {
                    player_fall_out(player);
                }
            }
        }
    }
    /* @hack spelling: a switch on the work's mode with one case, where an if
     * would do */
    switch (work->mode) {
    case 0:
        if (player_pad_records[BOARD_PAD].press & BOARD_BUTTON) {
            player_request(0, PLAYER_REQUEST_BOARD, 0);
        }
        break;
    }
}

static void background_step(struct task *task)
{
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

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
    njTranslate(NULL, camera.x, camera.y + background_lift, camera.z);
    njSetTexture(&street_sky_texlist);
    if (background_built) {
        njDirectDrawObject(background_builds[0].built);
    } else {
        njCnkEasyDrawObject(&street_sky_object);
    }
    njPopMatrix(1);
    njFogEnable();
}
