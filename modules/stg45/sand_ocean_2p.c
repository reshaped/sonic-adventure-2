/* Sand Ocean's two-player stage, stg45: the stage's init, exit and step,
 * and its background. the background task, BgExec, draws the quicksand, the
 * sky and the moon */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/black_smoke.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/fog.h"
#include "shared/item_icon.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/sand_ocean_sounds.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "stg45/background_art.h"
#include "stg45/sand_ocean_2p.h"
#include "stg45/sand_ocean_2p/text.inc"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

static struct fog_setting sand_ocean_2p_fog;

/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 1024
#define BACKGROUND_LEVEL 1
#define CLIP_NEAR (-3.0F)
#define CLIP_FAR (-50000.0F)

/* the quicksand's texture is set by halfword QUICKSAND_TEXTURE_WORD of its
 * polygon list, the second of its NJD_CT_TID chunk. the draw keeps that
 * halfword's filter mode, its NJD_FFM_MASK bits, and replaces the rest with a
 * texture index. the index is the frame, game_state.steps >>
 * QUICKSAND_FRAME_SHIFT, modulo QUICKSAND_TEXTURES. QUICKSAND_TEXTURES counts
 * ocean_2p_quicksand_texlist's textures */
#define QUICKSAND_FRAME_SHIFT 1
#define QUICKSAND_TEXTURE_WORD 7
#define QUICKSAND_TEXTURES 32
#define QUICKSAND_LIFT 0.1F
/* the quicksand's strips are NJD_CS_UVN, whose v counts 256ths of a
 * texture. quicksand_anim scrolls v by QUICKSAND_SCROLL at each frame, and u
 * not at all */
#define QUICKSAND_SCROLL 1
/* the moon's place in the world. the draw puts the moon there in view
 * space, under none of the camera's turn. the moon then faces the screen */
#define MOON_X (-3500.0F)
#define MOON_Y 4500.0F
#define MOON_Z (-11000.0F)

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

static struct texture_anim quicksand_anim = {
#include "stg45/sand_ocean_2p/quicksand_anim.inc"
};
static Sint32 dat_8c52a9e0 = 0;
static Sint32 dat_8c52a9e4 = 0;
/* the background's own texture groups and PVMs, which are empty. the first
 * of sand_ocean_2p_texture_groups holds the background's three texture
 * lists */
static NJS_TEXLIST **background_texture_groups[1] = {
#include "stg45/sand_ocean_2p/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[1] = {
#include "stg45/sand_ocean_2p/background_pvms.inc"
};

/* The BASIC strip beneath the two-player stage is registered as sinking sand.
 */
static NJS_MATERIAL kill_area_material = {
#include "stg45/sand_ocean_2p/kill_area_material.inc"
};
static Sint16 kill_area_mesh[5] = {
#include "stg45/sand_ocean_2p/kill_area_mesh.inc"
};
static NJS_MESHSET kill_area_meshset = {
#include "stg45/sand_ocean_2p/kill_area_meshset.inc"
};
static NJS_POINT3 kill_area_points[4] = {
#include "stg45/sand_ocean_2p/kill_area_points.inc"
};
static NJS_VECTOR kill_area_normals[4] = {
#include "stg45/sand_ocean_2p/kill_area_normals.inc"
};
static NJS_MODEL kill_area_model = {
#include "stg45/sand_ocean_2p/kill_area_model.inc"
};
static NJS_OBJECT kill_area_object = {
#include "stg45/sand_ocean_2p/kill_area_object.inc"
};
static struct kill_area sand_ocean_2p_kill_areas[2] = {
#include "stg45/sand_ocean_2p/sand_ocean_2p_kill_areas.inc"
};
/* the set file's block, or null */
static struct set_head *sand_ocean_2p_set = NULL;

void sand_ocean_2p_init(void)
{
    struct task *task;

    world_land_start(&sand_ocean_2p_land);
    texture_load_shared(sand_ocean_2p_pvms, sand_ocean_2p_texture_groups);
    texture_await_shared(sand_ocean_2p_pvms, sand_ocean_2p_texture_groups);
    sand_ocean_2p_set = set_read("set0045_s.bin", "set0045_u.bin", SET_ENTRIES);
    set_start(&sand_ocean_2p_objects, sand_ocean_2p_set);
    loader_read_camera_files();
    particle_core_start();
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(2);
    lights_load(SAND_OCEAN_LIGHT_FILE);
    object_builds_run((Uint8 *)world_build_land(
        &sand_ocean_2p_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    music_fade_in(0);
    music_play_or_defer("S_OCEAN.ADX");
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(sand_ocean_2p_kill_areas);
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
    fog_start_file(SAND_OCEAN_FOG_FILE, &sand_ocean_2p_fog);
    njFogDisable();
    object_light_create();
    sound_load_stage("se_ac_so.mlt", &sand_ocean_sound_list);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    smoke_task_create(0, NULL);
    if (game_state.two_players == TWO_PLAYERS_3_POINTS) {
        smoke_task_create(1, NULL);
    }
    njClipZ(CLIP_NEAR, CLIP_FAR);
    broken_down_smoke_create(0, NULL);
    if (player_tasks[1] != NULL) {
        broken_down_smoke_create(1, NULL);
    }
}

void sand_ocean_2p_exit(void)
{
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
    texture_release_shared(sand_ocean_2p_pvms, sand_ocean_2p_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (sand_ocean_2p_set != NULL) {
        loader_free(sand_ocean_2p_set);
        sand_ocean_2p_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void sand_ocean_2p_step(struct task *task)
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

static void background_draw(struct task *task)
{
    /* @hack spelling: object, a local for each object the draw draws, where
     * the object read in place would do */
    NJS_CNK_OBJECT *object;
    NJS_POINT3 pos;
    Sint16 texture;

    object = &ocean_2p_quicksand_object;
    /* @hack spelling: texture, a copy of the polygon list's texture word,
     * where the word changed in place would do */
    texture = object->model->plist[QUICKSAND_TEXTURE_WORD];
    texture = (texture & NJD_FFM_MASK) |
              ((game_state.steps >> QUICKSAND_FRAME_SHIFT) &
               (QUICKSAND_TEXTURES - 1));
    object->model->plist[QUICKSAND_TEXTURE_WORD] = texture;
    njSetTexture(&ocean_2p_quicksand_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, QUICKSAND_LIFT, 0.0F);
    texture_anim_apply(object->model, &quicksand_anim,
                       game_state.steps >> QUICKSAND_FRAME_SHIFT);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    njFogDisable();
    object = &ocean_2p_sky_object;
    njSetTexture(&ocean_2p_sky_texlist);
    njCnkEasyDrawModel(object->model);
    object = &ocean_2p_moon_object;
    njSetTexture(&ocean_2p_moon_texlist);
    njPushMatrixEx();
    njTranslate(NULL, MOON_X, MOON_Y, MOON_Z);
    njGetTranslation(NULL, &pos);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njRotateX(NULL, HALF_TURN);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    njPopMatrixEx();
    njFogEnable();
}
