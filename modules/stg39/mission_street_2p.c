/* Mission Street's two-player stage, stg39: the stage's init, exit and step,
 * and its background. the background task, BgExec, draws the background
 * around the current camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg39/mission_street_2p/text.inc"

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
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/parked_car.h"
#include "shared/ring.h"
#include "shared/street_prop.h"
#include "shared/street_sounds.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "stg39/mission_street_2p.h"
#include "stg39/object_archives.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#define BACKGROUND_LEVEL 1
/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 1024
/* how far above the current camera the draw puts the background object's
 * origin. the object's pos.y is -BACKGROUND_LIFT, so its model's origin falls
 * at the camera */
#define BACKGROUND_LIFT 886.5248F

/* camera 0's land_block_mask, by where the camera stands */
#define BLOCK_SPLIT 100.0F
#define LOW_Z_BLOCKS 0xe
#define LOW_X_BLOCKS 0x1c
#define HIGH_XZ_BLOCKS 0x38

static struct fog_setting mission_street_2p_fog;

#include "shared/mission_street/sky.h"

/* texture_load_shared gives each of street_sky_texlist's textures the one of
 * its name in this PVM */
static NJS_TEXNAME background_pvm_texnames[3] = {
#include "stg39/mission_street_2p/background_pvm_texnames.inc"
};
static NJS_TEXLIST background_pvm_texlist = {
#include "stg39/mission_street_2p/background_pvm_texlist.inc"
};
static NJS_TEXLIST *background_group[2] = {
#include "stg39/mission_street_2p/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg39/mission_street_2p/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg39/mission_street_2p/background_pvms.inc"
};

/* One collision strip covers the area beneath the two-player course. */
static NJS_MATERIAL kill_area_material = {
#include "stg39/mission_street_2p/kill_area_material.inc"
};
static Sint16 kill_area_meshes[5] = {
#include "stg39/mission_street_2p/kill_area_meshes.inc"
};
static NJS_MESHSET kill_area_meshset = {
#include "stg39/mission_street_2p/kill_area_meshset.inc"
};
static NJS_POINT3 kill_area_points[4] = {
#include "stg39/mission_street_2p/kill_area_points.inc"
};
static NJS_VECTOR kill_area_normals[4] = {
#include "stg39/mission_street_2p/kill_area_normals.inc"
};
static NJS_MODEL kill_area_model = {
#include "stg39/mission_street_2p/kill_area_model.inc"
};
static NJS_OBJECT kill_area_object = {
#include "stg39/mission_street_2p/kill_area_object.inc"
};
static struct kill_area mission_street_2p_kill_areas[2] = {
#include "stg39/mission_street_2p/mission_street_2p_kill_areas.inc"
};

/* the set file's block, or null */
static struct set_head *mission_street_2p_set = NULL;

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

void mission_street_2p_init(void)
{
    struct task *task;

    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    world_land_start(&mission_street_2p_land);
    texture_load_shared(mission_street_2p_pvms,
                        mission_street_2p_texture_groups);
    texture_await_shared(mission_street_2p_pvms,
                         mission_street_2p_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    mission_street_2p_set =
        set_read("set0039_s.bin", "set0039_u.bin", SET_ENTRIES);
    set_start(&mission_street_2p_objects, mission_street_2p_set);
    loader_read_camera_files();
    particle_core_start();
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg39_light.bin");
    parked_car_build(
        object_builds_run(street_prop_build((Uint8 *)world_build_land(
            &mission_street_2p_land, (Uint32)STAGE_MODEL_BUILD_AREA))));
    music_fade_in(0);
    music_play_or_defer("m_street.adx");
    kill_areas_start(mission_street_2p_kill_areas);
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.draw_blast_ring == NULL) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    fog_start_file("stg39_fog.bin", &mission_street_2p_fog);
    njFogDisable();
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    broken_down_smoke_create(0, NULL);
    if (player_tasks[1] != NULL) {
        broken_down_smoke_create(1, NULL);
    }
    sound_load_stage("se_ac_ms.mlt", &mission_street_sound_list);
}

void mission_street_2p_exit(void)
{
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
    texture_release_shared(mission_street_2p_pvms,
                           mission_street_2p_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (mission_street_2p_set != NULL) {
        loader_free(mission_street_2p_set);
        mission_street_2p_set = NULL;
    }
    world_land_stop();
}

void mission_street_2p_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
}

static void background_destroy(struct task *task)
{
}

/* the draw sets camera 0's land blocks too. camera 1 keeps every block, as
 * world_land_start leaves them */
static void background_draw(struct task *task)
{
    NJS_POINT3 camera;

    camera = camera_views[camera_current_index]->pos;
    njFogDisable();
    njPushMatrixEx();
    njTranslate(NULL, camera.x, camera.y + BACKGROUND_LIFT, camera.z);
    njSetTexture(&street_sky_texlist);
    njCnkEasyDrawObject(&street_sky_object);
    njPopMatrixEx();
    njFogEnable();
    if (camera_current_index == 0) {
        /* @hack spelling: split, a local for BLOCK_SPLIT, where BLOCK_SPLIT
         * in place would do */
        Float split = BLOCK_SPLIT;
        /* @hack spelling: state, a local for &game_state, where game_state
         * read in place would do */
        struct game_state *state = &game_state;

        if (camera.z < split) {
            state->land_block_mask[0] = LOW_Z_BLOCKS;
        } else if (camera.x < split) {
            state->land_block_mask[0] = LOW_X_BLOCKS;
        } else {
            state->land_block_mask[0] = HIGH_XZ_BLOCKS;
        }
    }
}
