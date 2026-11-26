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
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/sand_ocean_sounds.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "shared/wood_box_e.h"
#include "shared/iron_box_e.h"
#include "stg23/background.h"
#include "stg23/hidden_base.h"
#include "stg23/object_archives.h"
#include "stg21/stacked_platform.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#define IN_BLOCK(block) (game_state.land_block_mask[0] & (block))

static struct fog_setting hidden_base_fog;

#define SET_ENTRIES 2048
#define SINKS 7
#define SKY_COLOR 0x0096a4a4
#define CLIP_NEAR (-3.0F)
#define CLIP_FAR (-50000.0F)
/* task_create's levels for BgClip and BgExec */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
/* the sand's sound, from the stage's sound list. BgExec plays it for
 * SAND_DURATION + 1 sound updates at a time */
#define SAND_SOUND 8
#define SAND_PRIORITY 1
#define SAND_DURATION 10
/* the tops of the three bands of land blocks. the bands hold blocks 1 to 5,
 * 6 to 8 and 9 to 11. block 12 lies above them */
#define LOW_BAND_TOP 500.0F
#define MIDDLE_BAND_TOP 850.0F
#define HIGH_BAND_TOP 1230.0F
/* block 13 crosses the bands from this height up */
#define BLOCK_13_FLOOR 550.0F

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void base_background_draw(struct task *task);
static void base_background_clip(struct task *task);

/* a kill area for each of the stage's sinks, then one whose null object ends
 * the list */
static struct kill_area base_kill_areas[SINKS + 1] = {
#include "stg23/hidden_base/base_kill_areas.inc"
};
static struct light stage_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F}};
static struct set_head *set_file = NULL;
static Sint32 sand_sound_volume = 125;

void stg23_init(void)
{
    struct task *task;
    Uint8 *cursor;

    njSetBackColor(SKY_COLOR, SKY_COLOR, SKY_COLOR);
    task_create(TASK_WORK, CLIP_LEVEL, base_background_clip, "BgClip");
    world_land_start(&hidden_base_land);
    texture_load_shared(hidden_base_pvms, hidden_base_texture_groups);
    texture_await_shared(hidden_base_pvms, hidden_base_texture_groups);
    texture_load_shared(base_background_pvms, base_background_groups);
    texture_await_shared(base_background_pvms, base_background_groups);
    set_file = set_read_stage(NULL, SET_ENTRIES);
    set_start(&stg23_objects, set_file);
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
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg23_light.bin");
    cursor = (Uint8 *)world_build_land(&hidden_base_land,
                                       (Uint32)STAGE_MODEL_BUILD_AREA);
    cursor = object_builds_run(cursor);
    cursor = wood_box_e_build(cursor);
    iron_box_e_build(cursor);
    music_fade_in(0);
    music_play_or_defer("h_base.adx");
    path_create_manager(hidden_base_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = base_background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(base_kill_areas);
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
    fog_start_file("stg23_fog.bin", &hidden_base_fog);
    njFogDisable();
    messenger_messages_load("mh0023");
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
    sound_load_stage("se_ac_hb.mlt", &sand_ocean_sound_list);
    game_state.block_grid_loaded = TRUE;
    stacked_platform_tint_parts();
    blast_load_texture();
}

void stg23_exit(void)
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
    texture_release_shared(hidden_base_pvms, hidden_base_texture_groups);
    texture_release_shared(base_background_pvms, base_background_groups);
    set_end();
    if (set_file != NULL) {
        loader_free(set_file);
        set_file = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void stg23_step(struct task *task)
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
    struct task_work *player = player_work[0];

    if (LOW_BAND_TOP <= player->pos.y) {
        if (player->pos.x > -3940.0F || player->pos.z > -4600.0F) {
            if (-3770.0F > player->pos.x || -5105.0F > player->pos.z) {
                if (-3170.0F > player->pos.x || player->pos.z > -5750.0F) {
                    if (-3725.0F > player->pos.x || -5195.0F > player->pos.z) {
                        if (player->pos.z <= -3230.0F) {
                            sound_play_for(SAND_SOUND, task, SAND_PRIORITY, 0,
                                           SAND_DURATION);
                            sound_set_volume(task, -1, sand_sound_volume);
                        }
                    }
                }
            }
        }
    }
}

static void background_destroy(struct task *task)
{
}

static void base_background_draw(struct task *task)
{
    njFogDisable();
    if (IN_BLOCK(BASE_BLOCK_1)) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -4700.0F);
        njSetTexture(&base_far_pyramid_texlist);
        njCnkEasyDrawModel(&base_far_pyramid);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_1) || IN_BLOCK(BASE_BLOCK_2) ||
        IN_BLOCK(BASE_BLOCK_3) || IN_BLOCK(BASE_BLOCK_4) ||
        IN_BLOCK(BASE_BLOCK_5)) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -2250.0F);
        njSetTexture(&base_low_desert_texlist);
        njCnkEasyDrawModel(&base_low_desert);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_6) || IN_BLOCK(BASE_BLOCK_7) ||
        IN_BLOCK(BASE_BLOCK_8) || IN_BLOCK(BASE_BLOCK_9) ||
        IN_BLOCK(BASE_BLOCK_10) || IN_BLOCK(BASE_BLOCK_11) ||
        IN_BLOCK(BASE_BLOCK_12) || IN_BLOCK(BASE_BLOCK_13)) {
        njPushMatrixEx();
        njTranslate(NULL, -3000.0F, 0.0F, -5000.0F);
        njSetTexture(&base_high_desert_texlist);
        njCnkEasyDrawModel(&base_high_desert);
        njPopMatrixEx();
    }
    njFogEnable();
    base_quicksand_draw();
}

static void base_background_clip(struct task *task)
{
    /* camera 0's mask takes the one block that holds the current camera. the
     * camera's height picks the band, and its x and z the block in a band of
     * several */
    /* @hack spelling: state, a pointer to game_state, where game_state read
     * in place would do */
    struct game_state *state = &game_state;
    NJS_POINT3 pos;

    pos = camera_views[camera_current_index]->pos;
    if (pos.y < LOW_BAND_TOP) {
        if (pos.x > -1050.0F && pos.z > -2050.0F) {
            state->land_block_mask[0] = BASE_BLOCK_1;
        } else if (pos.x > -1050.0F && pos.z > -3400.0F) {
            state->land_block_mask[0] = BASE_BLOCK_2;
        } else if (pos.x > -1640.0F) {
            state->land_block_mask[0] = BASE_BLOCK_3;
        } else if (pos.x > -3940.0F) {
            state->land_block_mask[0] = BASE_BLOCK_4;
        } else {
            state->land_block_mask[0] = BASE_BLOCK_5;
        }
    } else if (pos.y < MIDDLE_BAND_TOP) {
        if (pos.z > -3820.0F) {
            state->land_block_mask[0] = BASE_BLOCK_6;
        } else if (pos.z > -4620.0F) {
            state->land_block_mask[0] = BASE_BLOCK_7;
        } else {
            state->land_block_mask[0] = BASE_BLOCK_8;
        }
    } else if (pos.y < HIGH_BAND_TOP) {
        if (pos.x < -3950.0F) {
            state->land_block_mask[0] = BASE_BLOCK_9;
        } else if (pos.x < -3180.0F) {
            state->land_block_mask[0] = BASE_BLOCK_10;
        } else {
            state->land_block_mask[0] = BASE_BLOCK_11;
        }
    } else {
        state->land_block_mask[0] = BASE_BLOCK_12;
    }
    /* a camera inside block 13 is in it, whatever its band */
    if (pos.y >= BLOCK_13_FLOOR) {
        if (pos.x >= -3770.0F && pos.x < -3180.0F && -5220.0F <= pos.z &&
            pos.z < -4450.0F && (pos.x >= -3710.0F || pos.z >= -5120.0F)) {
            state->land_block_mask[0] = BASE_BLOCK_13;
        }
    }
}
