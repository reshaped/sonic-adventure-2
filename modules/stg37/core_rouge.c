/* Rouge's route through Cannon's Core, stg37: the stage's init, exit and
 * step */
#include <ninja.h>
#include <sg_xpt.h>

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
#include "shared/dirt.h"
#include "shared/item_icon.h"
#include "shared/ring.h"
#include "shared/water_ripple.h"
#include "shared/common_object.h"
#include "shared/core_flags.h"
#include "shared/core_sound.h"
#include "shared/core_water_hooks.h"
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
#include "stg37/background.h"
#include "stg37/core_rouge.h"
#include "stg37/object_archives.h"
#include "stg37/routes.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#include "stg37/kill_planes.h"

/* the task level of the land blocks' task */
#define CLIP_LEVEL 0
/* set_read_stage reads the stage's two SET files into one block. the block
 * holds at least SET_ENTRIES 32-byte slots, the head's among them */
#define SET_ENTRIES 2048
/* the stage's task plays AMBIENT_SOUND under the task's address, and
 * PULSE_SOUND under SOUND_SECOND_KEY(task). the two keys keep each
 * sound_set_volume of SOUND_ANY_ID to one sound. each sound stops
 * SOUND_STEPS + 1 steps after the last step that plays it */
#define AMBIENT_SOUND 19
#define PULSE_SOUND 21
#define SOUND_PRIORITY 1
#define SOUND_STEPS 10
#define START_MESSAGE 21

static struct kill_area route_kill_areas[3] = {
#include "stg37/core_rouge/route_kill_areas.inc"
};
static struct light route_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1, 1, 1},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1, 1, 1}};
static Sint32 dat_8c54b6ec = 0;
/* camera 0's land_block_mask before the land blocks' task's latest step */
static Sint32 previous_mask = 0;
/* the block of the stage's SET files that core_rouge_init read, until
 * core_rouge_exit frees it */
static struct set_head *route_set = NULL;
/* no code sets it, so core_rouge_exit destroys no sand task. the one
 * core_rouge_init starts ends once player 0 has no work */
static struct task *sand_task = NULL;
/* AMBIENT_SOUND's volume */
static Sint32 ambient_volume = -20;
/* PULSE_SOUND's volume swings pulse_amplitude either side of pulse_center,
 * once every pulse_period steps */
static Sint32 pulse_center = 10;
static Sint32 pulse_amplitude = 20;
static Sint32 pulse_period = 480;
/* the fogs core_rouge_init reads. no code of the route hands either to
 * fog_set */
struct fog_setting core_rouge_fog;
struct fog_setting core_rouge_upper_fog;

static void ambience_step(struct task *task);
static void land_clip_step(struct task *task);

void core_rouge_init(void)
{
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&core_rouge_land);
    texture_load_shared(core_rouge_object_pvms, core_rouge_object_groups);
    texture_await_shared(core_rouge_object_pvms, core_rouge_object_groups);
    texture_load_shared(core_rouge_bg_pvms, core_rouge_bg_groups);
    texture_await_shared(core_rouge_bg_pvms, core_rouge_bg_groups);
    route_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&core_rouge_objects, route_set);
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
    lights_load("stg37_light.bin");
    world_build_land(&core_rouge_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    music_fade_in(0);
    music_play_or_defer("c_core_2.adx");
    path_create_manager(core_rouge_paths);
    core_rouge_background_init();
    kill_areas_start(route_kill_areas);
    module_motions_register();
    CORE_INSTALL_EFFECT(drop_rings, ring_scatter);
    CORE_INSTALL_EFFECT(unk_04, dirt_create);
    CORE_INSTALL_EFFECT(add_item_icon, item_icon_row_add);
    CORE_INSTALL_EFFECT(ripple, ripple_create);
    CORE_INSTALL_EFFECT(splash, big_splash_create);
    big_splash_load_texture();
    fog_read("stg37_fog.bin", &core_rouge_fog);
    fog_read("stg37_fogB.bin", &core_rouge_upper_fog);
    njFogDisable();
    sand_task_create(0, NULL);
    /* @hack redundant: an empty test of sand_task */
    if (sand_task) {
    }
    messenger_messages_load("mh0037");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    switch_clear_all();
    sound_load_stage("se_ac_cc_ru.mlt", &core_sound_list);
    game_state.block_grid_loaded = TRUE;
}

void core_rouge_exit(void)
{
    messenger_messages_free();
    if (sand_task) {
        task_destroy(sand_task);
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
    texture_release_shared(core_rouge_object_pvms, core_rouge_object_groups);
    texture_release_shared(core_rouge_bg_pvms, core_rouge_bg_groups);
    set_end();
    if (route_set) {
        loader_free(route_set);
        route_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

/* @hack redundant: register on task, where a plain parameter would do */
void core_rouge_step(register struct task *task)
{
    core_rouge_colliders_start();
    core_rouge_camera_start();
    core_rouge_cover_start();
    core_flags_clear();
    messenger_create(&task->work->pos, 0, 0, START_MESSAGE,
                     MESSENGER_TALK_WINDOW, NULL);
    task->step = ambience_step;
}

static void ambience_step(struct task *task)
{
    Float wave;
    int volume;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.land_block_mask[0] == LAND_BLOCK(1)) {
        sound_play_for(AMBIENT_SOUND, task, SOUND_PRIORITY, 0, SOUND_STEPS);
        sound_set_volume(task, SOUND_ANY_ID, ambient_volume);
    } else {
        wave = njSin(game_state.steps * ((Float)FULL_TURN / pulse_period));
        volume = pulse_center + (wave * pulse_amplitude);
        sound_play_for(PULSE_SOUND, SOUND_SECOND_KEY(task), SOUND_PRIORITY, 0,
                       SOUND_STEPS);
        sound_set_volume(SOUND_SECOND_KEY(task), SOUND_ANY_ID, volume);
    }
}

/* where land blocks 1 and 2 end toward -z. block 3 is all past
 * BLOCK_2_END_Z */
#define BLOCK_1_END_Z (-330.0F)
#define BLOCK_2_END_Z (-700.0F)

/* give camera 0 the land block where player 0 stands. while camera 0's top
 * mode is CAMERA_MODE_EASYSET, the current camera's place decides instead */
static void land_clip_step(struct task *task)
{
    NJS_POINT3 pos;
    if (camera_stacks[0].modes[camera_top(0)].number == CAMERA_MODE_EASYSET) {
        pos = camera_views[camera_current_index]->pos;
    } else {
        pos = player_work[0]->pos;
    }
    /* @hack type: camera 0's land_block_mask read and written through
     * STAGE_LAND_MASK, game_state under a second name, where
     * game_state.land_block_mask[0] would do */
    previous_mask = STAGE_LAND_MASK(0);
    /* @hack redundant: camera 0's land_block_mask cleared before it is set */
    STAGE_LAND_MASK(0) = 0;
    if (pos.z >= BLOCK_1_END_Z) {
        STAGE_LAND_MASK(0) = LAND_BLOCK(1);
    } else if (pos.z >= BLOCK_2_END_Z) {
        STAGE_LAND_MASK(0) = LAND_BLOCK(2);
    } else {
        STAGE_LAND_MASK(0) = LAND_BLOCK(3);
    }
}
