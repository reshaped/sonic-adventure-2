/* Knuckles's route through Cannon's Core, stg38: the stage's init, exit and
 * step */
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
#include "shared/big_splash.h"
#include "shared/dirt.h"
#include "shared/item_icon.h"
#include "shared/ring.h"
#include "shared/water_ripple.h"
#include "shared/common_object.h"
#include "shared/core_flags.h"
#include "shared/core_sound.h"
#include "shared/core_water_hooks.h"
#include "shared/lagoon_specks.h"
#include "shared/dust.h"
#include "shared/fog.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_light.h"
#include "shared/switch.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg38/background.h"
#include "stg38/core_knuckles.h"
#include "stg38/object_archives.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#include "stg38/kill_planes.h"

#define BACK_COLOR 0xe6af32
/* the task level of the land blocks' task */
#define CLIP_LEVEL 0
/* set_read_stage reads the stage's two SET files into one block. the block
 * holds at least SET_ENTRIES 32-byte slots, the head's among them */
#define SET_ENTRIES 2048
/* the specks in the cloud core_knuckles_init creates */
#define SPECKS 1270
/* the stage's task plays AMBIENT_SOUND while camera 0's land block is 1, 21
 * or 22, and PULSE_SOUND otherwise. it plays neither while
 * game_state.time_stopped isn't 0. it plays AMBIENT_SOUND under the task's
 * address, and PULSE_SOUND under SOUND_SECOND_KEY(task). the two keys keep
 * each sound_set_volume of SOUND_ANY_ID to one sound. each sound stops
 * SOUND_STEPS + 1 steps after the last step that plays it */
#define AMBIENT_SOUND 19
#define PULSE_SOUND 21
#define SOUND_PRIORITY 1
#define SOUND_STEPS 10
/* START_MESSAGE shows as the route starts. BLOCK_11_MESSAGE shows as camera
 * 0's land block changes from 1 to 11. it shows only while the core flag
 * BLOCK_11_MESSAGE_FLAG is clear. showing it sets the flag */
#define START_MESSAGE 21
#define BLOCK_11_MESSAGE 22
#define BLOCK_11_MESSAGE_FLAG 1

static struct kill_area route_kill_areas[3] = {
#include "stg38/core_knuckles/route_kill_areas.inc"
};
static struct light route_lights[] = {
    {{0.2F, -1.0F, 0.3F}, 1.0F, 0.3F, 1, 1, 1},
    {{0.2F, -1.0F, 0.3F}, 0.5F, 0.3F, 1, 1, 1}};
/* camera 0's land_block_mask before the land blocks' task's latest step */
static Sint32 previous_mask = 0;
/* the block of the stage's SET files that core_knuckles_init read, until
 * core_knuckles_exit frees it */
static struct set_head *route_set = NULL;
/* AMBIENT_SOUND's volume */
static Sint32 ambient_volume = -20;
/* PULSE_SOUND's volume swings pulse_amplitude either side of pulse_center,
 * once every pulse_period steps */
static Sint32 pulse_center = 10;
static Sint32 pulse_amplitude = 20;
static Sint32 pulse_period = 480;
/* the fogs the background's step sets: core_knuckles_fog under the water
 * line, and core_knuckles_upper_fog above it */
struct fog_setting core_knuckles_fog;
struct fog_setting core_knuckles_upper_fog;

static void route_step(struct task *task);
static void land_clip_step(struct task *task);

void core_knuckles_init(void)
{
    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&core_knuckles_land);
    texture_load_shared(core_knuckles_object_pvms, core_knuckles_object_groups);
    texture_await_shared(core_knuckles_object_pvms,
                         core_knuckles_object_groups);
    texture_load_shared(core_knuckles_bg_pvms, core_knuckles_bg_groups);
    texture_await_shared(core_knuckles_bg_pvms, core_knuckles_bg_groups);
    route_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&core_knuckles_objects, route_set);
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
    lights_load("stg38_light.bin");
    world_build_land(&core_knuckles_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    music_fade_in(0);
    music_play_or_defer("c_core_2.adx");
    core_knuckles_background_init();
    kill_areas_start(route_kill_areas);
    module_motions_register();
    CORE_INSTALL_EFFECT(drop_rings, ring_scatter);
    CORE_INSTALL_EFFECT(unk_04, dirt_create);
    CORE_INSTALL_EFFECT(add_item_icon, item_icon_row_add);
    CORE_INSTALL_EFFECT(ripple, ripple_create);
    CORE_INSTALL_EFFECT(splash, big_splash_create);
    big_splash_load_texture();
    fog_read("stg38_fog.bin", &core_knuckles_fog);
    fog_read("stg38_fogB.bin", &core_knuckles_upper_fog);
    njFogDisable();
    if (!core_knuckles_specks) {
        specks_create(0, SPECKS, &core_knuckles_specks);
    }
    object_light_create();
    messenger_messages_load("mh0038");
    smoke_task_create(0, NULL);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    switch_clear_all();
    sound_load_stage("se_ac_cc_kn.mlt", &core_sound_list);
    game_state.block_grid_loaded = TRUE;
}

void core_knuckles_exit(void)
{
    messenger_messages_free();
    object_light_destroy();
    if (core_knuckles_specks) {
        task_destroy(core_knuckles_specks);
    }
    fog_stop();
    njFogDisable();
    CORE_REMOVE_EFFECT(splash, big_splash_create);
    big_splash_release_texture();
    CORE_REMOVE_EFFECT(ripple, ripple_create);
    CORE_REMOVE_EFFECT(drop_rings, ring_scatter);
    CORE_REMOVE_EFFECT(unk_04, dirt_create);
    CORE_REMOVE_EFFECT(add_item_icon, item_icon_row_add);
    particle_core_stop();
    texture_release_shared(core_knuckles_object_pvms,
                           core_knuckles_object_groups);
    texture_release_shared(core_knuckles_bg_pvms, core_knuckles_bg_groups);
    set_end();
    if (route_set) {
        loader_free(route_set);
        route_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

/* @hack redundant: register on task, where a plain parameter would do */
void core_knuckles_step(register struct task *task)
{
    core_knuckles_hazards_start();
    core_knuckles_first_fence_start();
    core_knuckles_second_fence_start();
    core_knuckles_water_side = WATER_SIDE_UNSET;
    core_flags_clear();
    messenger_create(&task->work->pos, 0, 0, START_MESSAGE,
                     MESSENGER_TALK_WINDOW, NULL);
    task->step = route_step;
}

static void route_step(struct task *task)
{
    Float wave;
    int volume;
    fog_start();
    if (!game_state.time_stopped) {
        if (game_state.land_block_mask[0] == LAND_BLOCK(1) ||
            game_state.land_block_mask[0] == LAND_BLOCK(21) ||
            game_state.land_block_mask[0] == LAND_BLOCK(22)) {
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
    if (!core_flag_is_set(BLOCK_11_MESSAGE_FLAG) &&
        (previous_mask & LAND_BLOCK(1)) &&
        (game_state_land_block_mask_0 & LAND_BLOCK(11))) {
        messenger_create(&task->work->pos, 0, 0, BLOCK_11_MESSAGE,
                         MESSENGER_TALK_WINDOW, NULL);
        core_flag_set(BLOCK_11_MESSAGE_FLAG);
    }
}

/* where land blocks start: toward -z from a _START_Z bound, and up from a
 * _START_Y one. land_clip_step tests the bounds in this order, and its first
 * test that holds gives the block. blocks 2 and 3 lie below LOWER_BLOCKS_Y,
 * and block 1 is where no test holds */
#define BLOCK_12_START_Z (-1100.0F)
#define LOWER_BLOCKS_Y 200.0F
#define BLOCK_3_START_Z (-700.0F)
#define BLOCK_2_START_Z (-350.0F)
#define BLOCK_11_START_Z (-450.0F)
#define BLOCK_21_START_Y 400.0F
#define BLOCK_22_START_Y 560.0F

/* give camera 0 the land block where the current camera stands */
static void land_clip_step(struct task *task)
{
    NJS_POINT3 camera;
    Sint32 *mask;

    camera = camera_views[camera_current_index]->pos;
    previous_mask = game_state.land_block_mask[0];
    /* @hack redundant: game_state.land_block_mask[0] cleared before it is
     * set */
    game_state.land_block_mask[0] = 0;
    /* @hack spelling: camera 0's land_block_mask written through a pointer,
     * where assignments to game_state.land_block_mask[0] would do */
    mask = &game_state.land_block_mask[0];
    if (camera.z < BLOCK_12_START_Z) {
        *mask = LAND_BLOCK(12);
    } else if (camera.y < LOWER_BLOCKS_Y && camera.z < BLOCK_3_START_Z) {
        *mask = LAND_BLOCK(3);
    } else if (camera.y < LOWER_BLOCKS_Y && camera.z < BLOCK_2_START_Z) {
        *mask = LAND_BLOCK(2);
    } else if (camera.z < BLOCK_11_START_Z) {
        *mask = LAND_BLOCK(11);
    } else if (camera.y < BLOCK_21_START_Y) {
        *mask = LAND_BLOCK(1);
    } else if (camera.y < BLOCK_22_START_Y) {
        *mask = LAND_BLOCK(21);
    } else {
        *mask = LAND_BLOCK(22);
    }
}

static struct light unused_route_light = {
    {0.3F, 0.1F, 1.0F}, 0.9F, 0.3F, 0.9F, 0.7F, 1.0F};
