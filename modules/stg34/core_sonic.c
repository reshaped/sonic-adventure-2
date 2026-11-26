/* Sonic's route through Cannon's Core, stg34: the stage's init, exit and
 * step. the land blocks' task gives camera 0 the land block of the place
 * where the current camera stands. the stage's task plays the route's sounds.
 * it changes the music at the drop, where the route comes down to its lower
 * floor */
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
#include "shared/common_object.h"
#include "shared/core_flags.h"
#include "shared/core_sound.h"
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
#include "stg34/background.h"
#include "stg34/core_sonic.h"
#include "stg34/object_archives.h"
#include "stg34/routes.h"
#include "stg34/scenery.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

#include "stg34/kill_planes.h"

/* the task level of the land blocks' task */
#define CLIP_LEVEL 0
/* set_read_stage reads the stage's two SET files into one block. the block
 * holds at least SET_ENTRIES 32-byte slots, the head's among them */
#define SET_ENTRIES 2048
/* the stage's task plays AMBIENT_SOUND while camera 0's land_block_mask holds
 * one of blocks 1 to 7, and PULSE_SOUND otherwise. it plays neither while
 * game_state.time_stopped isn't 0. it plays AMBIENT_SOUND under the task's
 * address, and PULSE_SOUND under SOUND_SECOND_KEY(task). the two keys keep each
 * sound_set_volume of SOUND_ANY_ID to one sound. each sound stops
 * SOUND_STEPS + 1 steps after the last step that plays it */
#define AMBIENT_SOUND 19
#define PULSE_SOUND 21
#define SOUND_PRIORITY 1
#define SOUND_STEPS 10
/* START_MESSAGE shows as the route starts. DROP_MESSAGE shows as camera 0's
 * land block changes from BEFORE_DROP_BLOCK to DROP_BLOCK. it shows only
 * while the core flag DROP_MESSAGE_FLAG is clear, and showing it sets the
 * flag */
#define START_MESSAGE 21
#define DROP_MESSAGE 22
#define DROP_MESSAGE_FLAG 1
#define BEFORE_DROP_BLOCK 7
#define DROP_BLOCK 8

static struct kill_area core_sonic_kill_areas[4] = {
#include "stg34/core_sonic/core_sonic_kill_areas.inc"
};
static struct light route_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1, 1, 1},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1, 1, 1}};
/* camera 0's land_block_mask before the land blocks' task's latest step */
static Sint32 previous_mask = 0;
/* the block of the stage's SET files that core_sonic_init read, until
 * core_sonic_exit frees it */
static struct set_head *route_set = NULL;
/* a word of the stage's data that no code reads */
static Sint32 dat_8c554c14 = 0;
/* AMBIENT_SOUND's volume, and PULSE_SOUND's, which swings pulse_amplitude
 * either side of pulse_center over pulse_period steps */
static Sint32 ambient_volume = -20;
static Sint32 pulse_center = 10;
static Sint32 pulse_amplitude = 20;
static Sint32 pulse_period = 480;
/* the task that changes the music at the drop, or null while none lives.
 * once the new music plays it is MUSIC_CHANGED, so that the stage starts no
 * other */
#define MUSIC_CHANGED ((struct task *)-1)
static struct task *music_task = NULL;

static void route_step(struct task *task);
static void land_clip_step(struct task *task);

void core_sonic_init(void)
{
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&core_sonic_land);
    texture_load_shared(core_sonic_object_pvms, core_sonic_object_groups);
    texture_await_shared(core_sonic_object_pvms, core_sonic_object_groups);
    texture_load_shared(core_sonic_bg_pvms, core_sonic_bg_groups);
    texture_await_shared(core_sonic_bg_pvms, core_sonic_bg_groups);
    route_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&core_sonic_objects, route_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &route_lights[0]);
    light_set(1, &route_lights[1]);
    light_set(2, &route_lights[0]);
    light_set(3, &route_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg34_light.bin");
    world_build_land(&core_sonic_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    path_create_manager(core_sonic_paths);
    core_sonic_background_init();
    kill_areas_start(core_sonic_kill_areas);
    module_motions_register();
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: drop_rings tested and set a second time */
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    njFogEnable();
    object_light_create();
    messenger_messages_load("mh0034");
    hint_box_hints_load("bh0034");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    music_fade_in(0);
    sound_load_stage("se_ac_cc_so.mlt", &core_sound_list);
    object_builds_init();
    game_state.block_grid_loaded = TRUE;
}

void core_sonic_exit(void)
{
    object_builds_exit();
    hint_box_hints_free();
    messenger_messages_free();
    object_light_destroy();
    fog_stop();
    njFogDisable();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(core_sonic_object_pvms, core_sonic_object_groups);
    texture_release_shared(core_sonic_bg_pvms, core_sonic_bg_groups);
    set_end();
    if (route_set) {
        loader_free(route_set);
        route_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

/* @hack redundant: register on task, where a plain parameter would do */
void core_sonic_step(register struct task *task)
{
    core_sonic_background_start();
    music_play_or_defer("c_core_5.adx");
    music_task = NULL;
    core_flags_clear();
    messenger_create(&task->work->pos, 0, 0, START_MESSAGE,
                     MESSENGER_TALK_WINDOW, NULL);
    task->step = route_step;
}

/* the music's task lives while player 0 is past LEAVE_DROP_Z, toward -z, until
 * it starts the next music. once he is past ENTER_DROP_Z, it stops the music.
 * it starts the next where he is past ENTER_DROP_Z and below DROP_FLOOR_Y. he
 * must also be within GROUND_SLACK of the ground below him */
#define LEAVE_DROP_Z (-3400.0F)
#define ENTER_DROP_Z (-3500.0F)
#define DROP_FLOOR_Y (-785.0F)
#define GROUND_SLACK 10.0F
enum { MUSIC_PLAYING, MUSIC_STOPPED };
/* TRUE where player 0 rode a rail from beneath, faster than RAIL_SPEED toward
 * -z, as the task began. the task then keeps his pad off from the music's stop
 * until the next music starts */
#define ENTERED_ON_RAIL(task) ((task)->unk_3c.l)
#define RAIL_SPEED 2.0F

static void music_step(struct task *task)
{
    struct task_work *player = player_work[0];
    struct task_work *work = task->work;
    struct player *record = player_records[0];

    if (player->pos.z > LEAVE_DROP_Z) {
        task_destroy(task);
        music_task = NULL;
        return;
    }
    switch (work->submode) {
    case MUSIC_PLAYING:
        if (player->pos.z < ENTER_DROP_Z) {
            music_stop();
            ++work->submode;
        }
        break;
    case MUSIC_STOPPED:
        if (ENTERED_ON_RAIL(task)) {
            pad_disable_player(0);
        }
        if (player->pos.z < ENTER_DROP_Z && player->pos.y < DROP_FLOOR_Y &&
            njAbs(record->ground.below_y - player->pos.y) < GROUND_SLACK) {
            pad_enable_player(0);
            music_play_name("c_core_6.adx");
            task_destroy(task);
            music_task = MUSIC_CHANGED;
            return;
        }
        break;
    }
}

static void route_step(struct task *task)
{
    struct task_work *player = player_work[0];
    struct player_motion *motion = player_motion[0];
    int volume;
    Float wave;
    if (!music_task && player->pos.z < LEAVE_DROP_Z) {
        music_task = task_create_child(TASK_WORK, music_step, task);
        if (music_task) {
            ENTERED_ON_RAIL(music_task) =
                motion->spd.z < -RAIL_SPEED &&
                player->mode == PLAYER_MODE_SONIC_RAIL_BENEATH;
            music_task->work->submode = MUSIC_PLAYING;
        }
    }
    if (!game_state.time_stopped) {
        /* @hack spelling: a test of each of blocks 1 to 7, where one test of
         * LAND_BLOCKS(1, 7) would do */
        if ((game_state.land_block_mask[0] & LAND_BLOCK(1)) ||
            (game_state.land_block_mask[0] & LAND_BLOCK(2)) ||
            (game_state.land_block_mask[0] & LAND_BLOCK(3)) ||
            (game_state.land_block_mask[0] & LAND_BLOCK(4)) ||
            (game_state.land_block_mask[0] & LAND_BLOCK(5)) ||
            (game_state.land_block_mask[0] & LAND_BLOCK(6)) ||
            (game_state.land_block_mask[0] & LAND_BLOCK(7))) {
            sound_play_for(AMBIENT_SOUND, task, SOUND_PRIORITY, 0, SOUND_STEPS);
            sound_set_volume(task, SOUND_ANY_ID, ambient_volume);
        } else {
            wave = njSin(game_state.steps * ((Float)FULL_TURN / pulse_period));
            volume = (wave * pulse_amplitude) + pulse_center;
            sound_play_for(PULSE_SOUND, SOUND_SECOND_KEY(task), SOUND_PRIORITY,
                           0, SOUND_STEPS);
            sound_set_volume(SOUND_SECOND_KEY(task), SOUND_ANY_ID, volume);
        }
    }
    /* @hack type: camera 0's land_block_mask read as
     * game_state_land_block_mask_0, where game_state.land_block_mask[0] would
     * do */
    if (!core_flag_is_set(DROP_MESSAGE_FLAG) &&
        (previous_mask & LAND_BLOCK(BEFORE_DROP_BLOCK)) &&
        (game_state_land_block_mask_0 & LAND_BLOCK(DROP_BLOCK))) {
        messenger_create(&task->work->pos, 0, 0, DROP_MESSAGE,
                         MESSENGER_TALK_WINDOW, NULL);
        core_flag_set(DROP_MESSAGE_FLAG);
    }
}

/* where each land block ends toward -z. the blocks follow one another toward
 * -z, from block 1. block 4 lies west of BLOCK_4_EAST_X, beside block 3, and
 * ends with it. block 13 is all past BLOCK_12_END_Z. west is toward -x */
#define BLOCK_1_END_Z (-1250.0F)
#define BLOCK_2_END_Z (-1650.0F)
#define BLOCK_3_END_Z (-2150.0F)
#define BLOCK_4_EAST_X (-350.0F)
#define BLOCK_5_END_Z (-2450.0F)
#define BLOCK_6_END_Z (-2660.0F)
#define BLOCK_7_END_Z (-2940.0F)
#define BLOCK_8_END_Z (-3750.0F)
#define BLOCK_9_END_Z (-4800.0F)
#define BLOCK_10_END_Z (-5600.0F)
#define BLOCK_11_END_Z (-6700.0F)
#define BLOCK_12_END_Z (-8700.0F)

/* give camera 0 the land block of the place where the current camera
 * stands */
static void land_clip_step(struct task *task)
{
    NJS_POINT3 camera;
    Sint32 *mask;
    previous_mask = game_state.land_block_mask[0];
    /* @hack redundant: game_state.land_block_mask[0] cleared before it is
     * set */
    game_state.land_block_mask[0] = 0;
    camera = camera_views[camera_current_index]->pos;
    /* @hack spelling: camera 0's land_block_mask written through a pointer,
     * where assignments to game_state.land_block_mask[0] would do */
    mask = &game_state.land_block_mask[0];
    if (camera.z >= BLOCK_1_END_Z) {
        *mask = LAND_BLOCK(1);
    } else if (camera.z >= BLOCK_2_END_Z) {
        *mask = LAND_BLOCK(2);
    } else if (camera.z >= BLOCK_3_END_Z) {
        if (camera.x >= BLOCK_4_EAST_X) {
            *mask = LAND_BLOCK(3);
        } else {
            *mask = LAND_BLOCK(4);
        }
    } else if (camera.z >= BLOCK_5_END_Z) {
        *mask = LAND_BLOCK(5);
    } else if (camera.z >= BLOCK_6_END_Z) {
        *mask = LAND_BLOCK(6);
    } else if (camera.z >= BLOCK_7_END_Z) {
        *mask = LAND_BLOCK(7);
    } else if (camera.z >= BLOCK_8_END_Z) {
        *mask = LAND_BLOCK(8);
    } else if (camera.z >= BLOCK_9_END_Z) {
        *mask = LAND_BLOCK(9);
    } else if (camera.z >= BLOCK_10_END_Z) {
        *mask = LAND_BLOCK(10);
    } else if (camera.z >= BLOCK_11_END_Z) {
        *mask = LAND_BLOCK(11);
    } else if (camera.z >= BLOCK_12_END_Z) {
        *mask = LAND_BLOCK(12);
    } else {
        *mask = LAND_BLOCK(13);
    }
}
