/* Sand Ocean, stg21: the stage's init, exit and step, and its background */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/black_smoke.h"
#include "shared/blast.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/sand_ocean_sounds.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg21/background_art.h"
#include "stg21/quicksand.h"
#include "stg21/sand_ocean.h"
#include "stg21/stacked_platform.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* @hack type: camera 0's land_block_mask reached through dat_8c0db20c, where
 * game_state's own field would do */
#define OCEAN_LAND_MASK STAGE_LAND_MASK(0)

/* task_create's levels for BgClip and BgExec */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 2048
#define SKY_COLOR 0x00070714
#define CLIP_NEAR (-3.0F)
#define CLIP_FAR (-50000.0F)
/* the sand's sound, from the stage's sound list. each of BgExec's steps
 * plays it for SAND_DURATION + 1 sound updates */
#define SAND_SOUND 8
#define SAND_PRIORITY 0
#define SAND_DURATION 15
#define FIRST_PYRAMID_BLOCKS                                                   \
    (OCEAN_BLOCK_1 | OCEAN_BLOCK_2 | OCEAN_BLOCK_3 | OCEAN_BLOCK_4 |           \
     OCEAN_BLOCK_5)
#define SECOND_PYRAMID_BLOCKS (OCEAN_BLOCK_6 | OCEAN_BLOCK_7)
/* the moon's place in the world. the draw puts the moon there in view
 * space, under none of the camera's turn. the moon then faces the screen */
#define MOON_X (-3500.0F)
#define MOON_Y 4500.0F
#define MOON_Z (-20000.0F)
/* the bounds of the land blocks that BgClip tests the current camera's pos
 * against. the first test that holds, in the order they run, gives the
 * camera's block */
#define BLOCK_1_MIN_Z (-1250.0F)
#define BLOCK_2_MIN_Z (-1925.0F)
#define BLOCK_3_MIN_Z (-2275.0F)
#define BLOCK_8_MAX_Z (-5930.0F)
#define BLOCK_8_MAX_X 3060.0F
#define BLOCK_4_MAX_X 2510.0F
#define BLOCKS_5_6_MIN_Z (-4600.0F)
#define BLOCK_5_MAX_X 2860.0F

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);
static void background_clip(struct task *task);

static struct kill_area ocean_kill_areas[5] = {
#include "stg21/sand_ocean/ocean_kill_areas.inc"
};
static struct light stage_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F}};
static struct set_head *set_file = NULL;
static Sint32 sand_sound_volume = 25;
static struct fog_setting stage_fog;

void sand_ocean_init(void)
{
    struct task *task;

    njSetBackColor(SKY_COLOR, SKY_COLOR, SKY_COLOR);
    task_create(TASK_WORK, CLIP_LEVEL, background_clip, "BgClip");
    world_land_start(&sand_ocean_land);
    texture_load_shared(sand_ocean_pvms, sand_ocean_texture_groups);
    texture_await_shared(sand_ocean_pvms, sand_ocean_texture_groups);
    texture_load_shared(ocean_background_pvms, ocean_background_groups);
    texture_await_shared(ocean_background_pvms, ocean_background_groups);
    set_file = set_read_stage(NULL, SET_ENTRIES);
    set_start(&sand_ocean_objects, set_file);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &stage_lights[0]);
    light_set(1, &stage_lights[1]);
    light_set(2, &stage_lights[0]);
    light_set(3, &stage_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(2);
    lights_load("stg21_light.bin");
    world_build_land(&sand_ocean_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    music_fade_in(0);
    if (game_state.demo == 0) {
        music_play_or_defer("S_OCEAN.ADX");
    } else {
        music_play_or_defer("t9_eggma.ADX");
    }
    path_create_manager(sand_ocean_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(ocean_kill_areas);
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
    smoke_task_create(0, NULL);
    if (game_state.two_players == 1) {
        smoke_task_create(1, NULL);
    }
    fog_start_file("stg21_fog.bin", &stage_fog);
    njFogDisable();
    object_light_create();
    messenger_messages_load("mh0021");
    broken_down_smoke_create(0, NULL);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    njClipZ(CLIP_NEAR, CLIP_FAR);
    sound_load_stage("se_ac_so.mlt", &sand_ocean_sound_list);
    game_state.block_grid_loaded = TRUE;
    stacked_platform_tint_parts();
    blast_load_texture();
}

void sand_ocean_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
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
    texture_release_shared(sand_ocean_pvms, sand_ocean_texture_groups);
    texture_release_shared(ocean_background_pvms, ocean_background_groups);
    set_end();
    if (set_file != NULL) {
        loader_free(set_file);
        set_file = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void sand_ocean_step(struct task *task)
{
    /* @hack spelling: work, a copy of task->work, where task->work would do */
    struct task_work *work = task->work;
    /* @hack redundant: a switch on the mode, whose one case does nothing */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
    sound_play_task_for(SAND_SOUND, task, SAND_PRIORITY, sand_sound_volume,
                        SAND_DURATION);
}

static void background_destroy(struct task *task)
{
}

static void background_draw(struct task *task)
{
    NJS_VECTOR pos;

    njFogDisable();
    njSetTexture(&ocean_first_stars_texlist);
    njCnkEasyDrawModel(&ocean_first_stars);
    njSetTexture(&ocean_second_stars_texlist);
    njCnkEasyDrawModel(&ocean_second_stars);
    if (OCEAN_LAND_MASK & FIRST_PYRAMID_BLOCKS) {
        njSetTexture(&ocean_first_pyramid_texlist);
        njCnkEasyDrawModel(&ocean_first_pyramid);
    }
    if (OCEAN_LAND_MASK & SECOND_PYRAMID_BLOCKS) {
        njSetTexture(&ocean_second_pyramid_texlist);
        njCnkEasyDrawModel(&ocean_second_pyramid);
    }

    njPushMatrixEx();
    njTranslate(NULL, MOON_X, MOON_Y, MOON_Z);
    njGetTranslation(NULL, &pos);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njRotateX(NULL, HALF_TURN);
    njSetTexture(&ocean_moon_texlist);
    njCnkEasyDrawModel(&ocean_moon);
    njPopMatrixEx();
    njPopMatrixEx();
    njFogEnable();
    ocean_quicksand_draw();
}

static void background_clip(struct task *task)
{
    NJS_POINT3 pos;

    pos = camera_views[camera_current_index]->pos;

    if (pos.z > BLOCK_1_MIN_Z) {
        OCEAN_LAND_MASK = OCEAN_BLOCK_1;
    } else if (pos.z > BLOCK_2_MIN_Z) {
        OCEAN_LAND_MASK = OCEAN_BLOCK_2;
    } else if (pos.z > BLOCK_3_MIN_Z) {
        OCEAN_LAND_MASK = OCEAN_BLOCK_3;
    } else if (pos.z < BLOCK_8_MAX_Z && pos.x < BLOCK_8_MAX_X) {
        OCEAN_LAND_MASK = OCEAN_BLOCK_8;
    } else if (pos.x < BLOCK_4_MAX_X) {
        OCEAN_LAND_MASK = OCEAN_BLOCK_4;
    } else if (pos.z > BLOCKS_5_6_MIN_Z && pos.x < BLOCK_5_MAX_X) {
        OCEAN_LAND_MASK = OCEAN_BLOCK_5;
    } else if (pos.z > BLOCKS_5_6_MIN_Z) {
        OCEAN_LAND_MASK = OCEAN_BLOCK_6;
    } else {
        OCEAN_LAND_MASK = OCEAN_BLOCK_7;
    }
}
