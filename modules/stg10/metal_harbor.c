/* Metal Harbor, the stage of STG10. the background task, BgExec, draws the sky
 * and the sea about the current camera */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "shared/sea.h"
#include "stg10/metal_harbor/text.inc"
#include "camera/camera.h"
#include "camera/stack.h"
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
#include "shared/footsteps.h"
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
#include "stg10/container_0.h"
#include "stg10/harbor_missile.h"
#include "stg10/metal_harbor.h"
#include "stg10/metal_harbor_sounds.h"
#include "stg10/missile_bridge.h"
#include "stg10/sea.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the stage task's mode, in its work's mode */
enum metal_harbor_mode { MODE_RUN };

/* the most a wave lifts the sea, sea_wave_height_max. it is 0 while the
 * missile waits or is gone, and WAVE_CAP_LOW from the missile's count on. it
 * is WAVE_CAP_HIGH once the missile is above the clouds */
#define WAVE_CAP_LOW 800.0F
#define WAVE_CAP_HIGH 2000.0F

/* task_create's level for the background task */
#define BACKGROUND_LEVEL 1
/* the background task's submode bits. the sea draws under the current camera,
 * at y SEA_Y. from the missile's count until it is above the clouds or gone,
 * the sea stays at metal_harbor_sea_pos instead. the step sets BG_FIX_NEXT as
 * the count starts. the next draw then sets metal_harbor_sea_pos under the
 * camera, and trades BG_FIX_NEXT for BG_FIXED. BG_HIGH says the missile is
 * above the clouds. the background then draws no sky, sets the high fog and
 * draws the sea opaque */
#define BG_HIGH 0x1
#define BG_FIXED 0x2
#define BG_FIX_NEXT 0x4
#define SEA_Y (-0.1F)
/* the background sets the music's name to m_harb1.adx while game_state.steps
 * is below MUSIC_NAME_STEPS */
#define MUSIC_NAME_STEPS 10

/* the sounds of player 0's two feet in metal_harbor_sound_list, which
 * footsteps_create takes as first_sound and second_sound */
#define FIRST_FOOT_SOUND 4
#define SECOND_FOOT_SOUND 3
/* the lights metal_harbor_init sets */
#define STAGE_LIGHTS 2

#include "stg10/paths.h"
/* The sea animates twenty frames from the background texture archive. */
static NJS_TEXNAME sea_names[20] = {
#include "stg10/metal_harbor/sea_names.inc"
};

NJS_TEXLIST sea_texlist = {
#include "stg10/metal_harbor/sea_texlist.inc"
};

#include "shared/sea_mesh.h"

NJS_CNK_OBJECT sea_object[1] = {
#include "stg10/metal_harbor/sea_object.inc"
};

static NJS_TEXNAME sky_names[2] = {
#include "stg10/metal_harbor/sky_names.inc"
};

static NJS_TEXLIST sky_texlist = {
#include "stg10/metal_harbor/sky_texlist.inc"
};

static Sint16 sky_polygons[831] = {
#include "stg10/metal_harbor/sky_polygons.inc"
};

static Sint32 sky_vertices[459] = {
#include "stg10/metal_harbor/sky_vertices.inc"
};

static NJS_CNK_MODEL sky_model = {
#include "stg10/metal_harbor/sky_model.inc"
};

static NJS_CNK_OBJECT sky_object = {
#include "stg10/metal_harbor/sky_object.inc"
};

static NJS_TEXNAME background_archive_names[22] = {
#include "stg10/metal_harbor/background_archive_names.inc"
};

static NJS_TEXLIST background_archive = {
#include "stg10/metal_harbor/background_archive.inc"
};

static NJS_TEXLIST *background_lists[3] = {
#include "stg10/metal_harbor/background_lists.inc"
};

typedef NJS_TEXLIST *harbor_texture_group;

static harbor_texture_group *background_groups[2] = {
#include "stg10/metal_harbor/background_groups.inc"
};

static struct texture_pvm background_pvms[2] = {
#include "stg10/metal_harbor/background_pvms.inc"
};
/* Sea-level collision plane for every playable character. */
static NJS_MATERIAL kill_material = {
#include "stg10/metal_harbor/kill_material.inc"
};

static Sint16 kill_strip[5] = {
#include "stg10/metal_harbor/kill_strip.inc"
};

static NJS_MESHSET kill_faces = {
#include "stg10/metal_harbor/kill_faces.inc"
};

static NJS_POINT3 kill_points[4] = {
#include "stg10/metal_harbor/kill_points.inc"
};

static NJS_VECTOR kill_normals[4] = {
#include "stg10/metal_harbor/kill_normals.inc"
};

static NJS_MODEL kill_model = {
#include "stg10/metal_harbor/kill_model.inc"
};

static NJS_OBJECT kill_plane = {
#include "stg10/metal_harbor/kill_plane.inc"
};

static struct fog_setting metal_harbor_fog;
static struct fog_setting metal_harbor_high_fog;
static NJS_POINT3 metal_harbor_sea_pos;

/* a player who falls into the sea splashes and misses */
static struct kill_area metal_harbor_kill_areas[2] = {
#include "stg10/metal_harbor/metal_harbor_kill_areas.inc"
};
static struct light metal_harbor_lights[STAGE_LIGHTS] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block is at least SET_ENTRIES entries long, its head
 * counted as one */
#define SET_ENTRIES 2048
/* the set file's block, or null */
static struct set_head *metal_harbor_set = NULL;
/* camera 0's angle of view heads for view_angle degrees, and up to view_widen
 * more with player 0's speed. it turns by at most view_turn a step */
static Float view_angle = 70.0F;
static Float view_widen = 6.0F;
static Angle view_turn = 0x200;

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

void metal_harbor_init(void)
{
    struct task *task;
    Uint8 *buf;

    world_land_start(&metal_harbor_land);
    texture_load_shared(metal_harbor_object_pvms,
                        metal_harbor_object_texture_groups);
    texture_await_shared(metal_harbor_object_pvms,
                         metal_harbor_object_texture_groups);
    texture_load_shared(background_pvms, background_groups);
    texture_await_shared(background_pvms, background_groups);
    metal_harbor_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&metal_harbor_objects, metal_harbor_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &metal_harbor_lights[0]);
    light_set(1, &metal_harbor_lights[1]);
    light_set(2, &metal_harbor_lights[0]);
    light_set(3, &metal_harbor_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg10_light.bin");
    buf = (Uint8 *)world_build_land(&metal_harbor_land,
                                    (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = object_builds_run(buf);
    buf = container_0_build(buf);
    missile_bridges_build(buf);
    music_fade_in(0);
    music_play_or_defer("m_harb1.adx");
    path_create_manager(metal_harbor_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
        task->destructor = background_destroy;
        footsteps_create(0, FIRST_FOOT_SOUND, SECOND_FOOT_SOUND, task);
    }
    kill_areas_start(metal_harbor_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.splash == NULL) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    fog_read("stg10_fog.bin", &metal_harbor_fog);
    fog_read("stg10_fogB.bin", &metal_harbor_high_fog);
    njFogDisable();
    messenger_messages_load("mh0010");
    hint_box_hints_load("bh0010");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_mh.mlt", &metal_harbor_sound_list);
}

void metal_harbor_exit(void)
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
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    particle_core_stop();
    texture_release_shared(metal_harbor_object_pvms,
                           metal_harbor_object_texture_groups);
    texture_release_shared(background_pvms, background_groups);
    set_end();
    if (metal_harbor_set != NULL) {
        loader_free(metal_harbor_set);
        metal_harbor_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

#define SPEED_FULL 6.9F
#define SPEED_LOW 1.38F
#define SPEED_SHARE 0.0067285695F

/* set camera 0's angle of view by player 0's spd.x. the angle is view_angle
 * degrees, plus view_widen times the root of a share. the share is 1 at
 * SPEED_FULL or more, and 0 at SPEED_LOW or less. between, it is
 * (spd.x - SPEED_LOW) * SPEED_SHARE, whose root stays under a fifth. nothing
 * happens while slot 0 holds no player, and task isn't read */
static void metal_harbor_set_view_angle(struct task *task)
{
    struct player *player = player_records[0];
    Float speed;
    Angle view;

    if (player == NULL) {
        return;
    }
    speed = player->spd.x;
    if (speed >= SPEED_FULL) {
        speed = 1.0F;
    } else if (speed > SPEED_LOW) {
        speed -= SPEED_LOW;
        speed *= SPEED_SHARE;
    } else {
        speed = 0.0F;
    }
    view = DEGREES(view_angle) + (Angle)(DEGREES(view_widen) * njSqrt(speed));
    camera_set_view_angle(0, view, view_turn);
}

void metal_harbor_step(struct task *task)
{
    struct task_work *work = task->work;

    fog_start();
    switch (work->mode) {
    case MODE_RUN:
        if (player_pad_records[1].press & PDD_DGT_TY) {
            player_request(0, PLAYER_REQUEST_BOARD, 0);
        }
        metal_harbor_set_view_angle(task);
        break;
    }
}

static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: mode a Sint16, where the work's Sint8 would do */
    Sint16 mode;

    sea_waves_step(0.0F, 0.0F);
    /* @hack spelling: the mode set inside the test, where a statement of its
     * own would do */
    if (harbor_missile_task != NULL &&
        (mode = harbor_missile_task->work->mode) != HARBOR_MISSILE_MODE_WAIT) {
        sea_wave_height_max = WAVE_CAP_LOW;
        if (!(work->submode & (BG_FIXED | BG_FIX_NEXT))) {
            work->submode |= BG_FIX_NEXT;
        }
        if ((mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
             mode == HARBOR_MISSILE_MODE_THROWN) &&
            harbor_missile_task->work->timer > HARBOR_MISSILE_CLOUD_STEPS) {
            sea_wave_height_max = WAVE_CAP_HIGH;
            work->submode &= ~(BG_FIXED | BG_FIX_NEXT);
            work->submode |= BG_HIGH;
        }
    } else {
        work->submode = 0;
        sea_wave_height_max = 0.0F;
    }
    if (game_state.steps < MUSIC_NAME_STEPS) {
        music_set_name("m_harb1.adx");
    }
}

static void background_destroy(struct task *task)
{
}

static void background_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    pos.x = camera_views[camera_current_index]->pos.x;
    pos.y = SEA_Y;
    pos.z = camera_views[camera_current_index]->pos.z;
    if (work->submode & BG_FIX_NEXT) {
        metal_harbor_sea_pos = pos;
        work->submode &= ~BG_FIX_NEXT;
        work->submode |= BG_FIXED;
    }
    if (!(work->submode & BG_HIGH)) {
        njFogDisable();
        njPushMatrixEx();
        njTranslateV(NULL, &pos);
        njSetTexture(&sky_texlist);
        njCnkSimpleDrawObject(&sky_object);
        njPopMatrix(1);
        fog_set(&metal_harbor_fog);
        njFogEnable();
    } else {
        fog_set(&metal_harbor_high_fog);
    }
    if (work->submode & BG_HIGH) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(~NJD_FST_UA, 0);
    }
    njPushMatrixEx();
    njTranslateV(NULL,
                 (work->submode & BG_FIXED) ? &metal_harbor_sea_pos : &pos);
    njTranslateV(NULL, (NJS_VECTOR *)sea_object->pos);
    sea_draw();
    njPopMatrix(1);
    if (work->submode & BG_HIGH) {
        draw_attr_restore();
        draw_control_restore();
    }
}
