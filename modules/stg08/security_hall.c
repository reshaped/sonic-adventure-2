/* Security Hall, the stage of STG08: its init, exit and step, and its
 * background. the background task, BgExec, sets the fog. it shows the land of
 * camera 0's floor, and makes it solid. while camera 0 is on the lower floor,
 * it draws the safes' fronts too */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/emerald.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/switch.h"
#include "shared/white_breath.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg08/background.h"
#include "stg08/cash_box.h"
#include "stg08/machines.h"
#include "stg08/paper.h"
#include "stg08/security_hall.h"
#include "stg08/security_props.h"
#include "stg08/solid_props.h"
#include "stg08/sounds.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* the stage task's mode, in its work's mode */
enum security_hall_mode { MODE_RUN };

#define BACKGROUND_LEVEL 1
/* the set file's block is at least SET_ENTRIES entries long, its head
 * counted as one */
#define SET_ENTRIES 2048
#define EMERALD_PATHS 8
#define PIECE_SPIN 0xc0
#define PIECE_BOB_HEIGHT 1.0F
#define PIECE_BOB_SPEED 0x180
/* the step plays these voices in the first mission, while no mini event
 * runs. OPENING_VOICE plays at 0:00 and OPENING_FRAME frames on the clock.
 * until the stage is cleared, ALERT_VOICE plays at 1:00. the next ids play at
 * 2:00, 3:00, 4:00 and 4:55 in turn */
#define OPENING_VOICE 2019
#define OPENING_FRAME 10
#define ALERT_VOICE 2020
#define VOICE_PRIORITY 0
#define FALL_PLAYERS 2
#define FALL_OUT_HEIGHT (-1100.0F)
/* through the stage's first START_STEPS steps, the background clears its
 * submode, and so takes security_fog */
#define START_STEPS 5

#include "stg08/paths.h"
/* the hunt's hints for each of the stage's places, as hunt_set_hints takes
 * them */
static struct hunt_hint security_hints[149] = {
#include "stg08/security_hall/security_hints.inc"
};

/* a table of four hints, apart from the one the hunt takes */
static struct hunt_hint unused_path_hints[4] = {
#include "stg08/security_hall/unused_path_hints.inc"
};
/* the hint file security_hall_init loaded, until security_hall_exit frees it */
static Sint32 *security_hint_file = NULL;
/* the stage's paths for path_create_manager: none, only the null that ends
 * the list */
static struct path *security_paths[] = {NULL};
#include "stg08/kill_plane.h"

struct fog_setting security_fog;
struct fog_setting security_alt_fog;

/* the stage's kill areas: the kill plane of stg08/kill_plane.h, then the
 * null that ends the list */
static struct kill_area stage_kill_areas[2] = {
#include "stg08/security_hall/stage_kill_areas.inc"
};
static struct light stage_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F}};
struct task *security_background = NULL;
/* the set file's block, from security_hall_init until security_hall_exit
 * frees it, or null */
static struct set_head *security_set = NULL;
Sint32 security_open_area = 0;
Float security_floor_height = -210.0F;
static Uint32 top_left = 0xffb9ffff;
static Uint32 top_right = 0xffb9ffff;
static Uint32 bottom_left = 0xffb9ffff;

void security_hall_init(void)
{
    struct task *task;
    Uint8 *buffer;

    world_land_start(&security_land);
    texture_load_shared(security_pvms, security_texture_groups);
    texture_await_shared(security_pvms, security_texture_groups);
    security_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&security_hall_objects, security_set);
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
    lights_load("stg08_light.bin");
    buffer = (Uint8 *)world_build_land(&security_land,
                                       (Uint32)STAGE_MODEL_BUILD_AREA);
    buffer = security_money_build(buffer);
    buffer = security_laser_build(buffer);
    buffer = object_builds_run(buffer);
    buffer = security_scenery_build(buffer);
    buffer = security_fan_build(buffer);
    security_cylinder_build(buffer);
    music_fade_in(0);
    music_play_or_defer("s_hall.adx");
    path_create_manager(security_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, security_background_step,
                       "BgExec");
    if (task != NULL) {
        if (security_background == NULL) {
            security_background = task;
        }
        task->draw = security_background_draw;
        task->destructor = security_background_destroy;
    }
    kill_areas_start(stage_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.create_piece == NULL) {
        player_hooks.create_piece = emerald_create;
    }
    if (player_hooks.show_piece_sign == NULL) {
        player_hooks.show_piece_sign = emerald_sign_create;
    }
    fog_read("stg08_fog.bin", &security_fog);
    fog_read("stg08_fogB.bin", &security_alt_fog);
    njFogDisable();
    messenger_messages_load("mh0008");
    hint_box_hints_load("bh0008");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    hunt_set_hints(security_hints);
    security_hint_file = hunt_load_hint_file("eh0008");
    emerald_set_paths(security_piece_paths, EMERALD_PATHS);
    emerald_look.flags = 0;
    emerald_look.bob_height = PIECE_BOB_HEIGHT;
    emerald_look.bob_speed = PIECE_BOB_SPEED;
    /* @hack spelling: the three assignments chained, where three statements
     * would do */
    emerald_look.models[0] = emerald_look.models[1] = emerald_look.models[2] =
        &security_piece_model;
    emerald_look.texlists[0] = &security_piece_1_texlist;
    emerald_look.texlists[1] = &security_piece_2_texlist;
    emerald_look.texlists[2] = &security_piece_3_texlist;
    emerald_look.spin = PIECE_SPIN;
    sound_load_stage("se_ac_sh.mlt", &security_sounds);
    object_builds_init();
    white_breath_create(0, NULL);
    if (game_state.two_players) {
        white_breath_create(1, NULL);
    }
}

void security_hall_exit(void)
{
    object_builds_exit();
    syFree(security_hint_file);
    security_hint_file = NULL;
    emerald_reset_look();
    hint_box_hints_free();
    messenger_messages_free();
    fog_stop();
    njFogDisable();
    /* the second stop finds no fog running */
    fog_stop();
    njFogDisable();
    emerald_clear_paths();
    /* the hint file again, which is null by now */
    syFree(security_hint_file);
    security_hint_file = NULL;
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    if (player_hooks.create_piece == emerald_create) {
        player_hooks.create_piece = NULL;
    }
    if (player_hooks.show_piece_sign == emerald_sign_create) {
        player_hooks.show_piece_sign = NULL;
    }
    particle_core_stop();
    texture_release_shared(security_pvms, security_texture_groups);
    set_end();
    if (security_set != NULL) {
        loader_free(security_set);
        security_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void security_hall_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int selected;
    Sint8 minute;
    Sint8 second;
    Sint8 frame;

    fog_start();
    switch (work->mode) {
    case MODE_RUN:
        selected = -1;
        for (i = 0; i < SAFE_AREAS; i++) {
            if (security_open_area != i && switch_is_on(SAFE_AREA_SWITCH(i))) {
                selected = i;
            }
        }
        /* @hack redundant: the test that selected isn't the open area, which
         * the loop makes sure of */
        if (selected >= 0 && security_open_area != selected) {
            security_open_area = selected;
        }
        for (i = 0; i < SAFE_AREAS; i++) {
            /* @hack spelling: the test's result picked as 1 or 0, where the
             * test passed in place would do */
            switch_set(SAFE_AREA_SWITCH(i), security_open_area == i ? 1 : 0);
        }
    }
    stage_clock_get(&minute, &second, &frame);
    if (game_state.mission == MISSION_FIRST && !game_state.mini_event_running) {
        if (minute == 0 && second == 0 && frame == OPENING_FRAME) {
            voice_play(OPENING_VOICE, VOICE_PRIORITY);
        }
        if (game_state.cleared == 0) {
            if (minute == 1 && second == 0 && frame == 0) {
                voice_play(ALERT_VOICE, VOICE_PRIORITY);
            }
            if (minute == 2 && second == 0 && frame == 0) {
                voice_play(ALERT_VOICE + 1, VOICE_PRIORITY);
            }
            if (minute == 3 && second == 0 && frame == 0) {
                voice_play(ALERT_VOICE + 2, VOICE_PRIORITY);
            }
            if (minute == 4 && second == 0 && frame == 0) {
                voice_play(ALERT_VOICE + 3, VOICE_PRIORITY);
            }
            if (minute == 4 && second == 55 && frame == 0) {
                voice_play(ALERT_VOICE + 4, VOICE_PRIORITY);
            }
        }
    }
    if (kill_areas_task && !kill_areas_task->child) {
        for (i = 0; i < FALL_PLAYERS; i++) {
            if (player_work[i] && !(player_records[i]->flags & PLAYER_MISSED)) {
                /* @hack spelling: the height tested in an if of its own, where
                 * one test of all three would do */
                if (player_work[i]->pos.y < FALL_OUT_HEIGHT) {
                    player_fall_out(i);
                }
            }
        }
    }
}

void security_background_step(struct task *task)
{
    struct task_work *work = task->work;
    struct land_entry *entry;
    Uint32 flags;
    int remaining;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    Sint32 block;
    /* @hack redundant: unused, a point that no line reads */
    NJS_POINT3 unused;

    njSetBackColor(top_left, top_right, bottom_left);
    if (game_state.steps < START_STEPS) {
        work->submode = 0;
    }
    if (work->submode & SECURITY_ALT_FOG) {
        fog_set(&security_alt_fog);
    } else {
        fog_set(&security_fog);
    }
    /* @hack redundant: block, the current camera's land blocks, read where
     * nothing uses them */
    if (camera_current_index == 0) {
        block = game_state.land_blocks[0];
    } else {
        block = game_state.land_blocks[1];
    }
    remaining = security_land.count;
    entry = security_land.entries;
    njPushMatrixEx();
    /* an entry whose size class isn't 0 is solid only on camera 0's floor.
     * any other draws only there, unless it has SECURITY_BOTH_FLOORS */
#define SET_ENTRY_FLAGS(other_floor, bit)                                      \
    if (other_floor) {                                                         \
        flags &= ~(bit);                                                       \
    } else {                                                                   \
        flags |= (bit);                                                        \
    }                                                                          \
    entry->flags = flags
#define SET_FLOOR(other_floor)                                                 \
    while (remaining--) {                                                      \
        flags = entry->flags;                                                  \
        if (flags & WORLD_SIZE_CLASS_BITS) {                                   \
            SET_ENTRY_FLAGS(other_floor, WORLD_SOLID);                         \
        } else if (!(flags & SECURITY_BOTH_FLOORS)) {                          \
            SET_ENTRY_FLAGS(other_floor, LAND_DRAWN);                          \
        }                                                                      \
        entry++;                                                               \
    }
    if (camera_views[0]->pos.y > security_floor_height) {
        SET_FLOOR(entry->center.y <= security_floor_height);
    } else {
        SET_FLOOR(entry->center.y > security_floor_height);
    }
#undef SET_FLOOR
#undef SET_ENTRY_FLAGS
    njPopMatrixEx();
}

void security_background_draw(struct task *task)
{
    if (!(camera_views[0]->pos.y > security_floor_height)) {
        security_cash_boxes_draw();
    }
}

void security_background_destroy(struct task *task)
{
    if (security_background == task) {
        security_background = NULL;
    }
}
