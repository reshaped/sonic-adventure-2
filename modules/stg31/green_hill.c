/* Green Hill, stg31: the stage's init, exit and step, and its background.
 * the step keeps a task that makes the stage's three rides. the background's
 * step scrolls the textures of green_hill_terrain_scrolls' models. a model
 * there without a scroll takes a tint that pulses instead. the background's
 * draw draws the sky about the camera, then the far land and its water */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/land_draw.h"
#include "shared/messenger.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/vertex_colors.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg31/background.h"
#include "stg31/green_hill.h"
#include "stg31/object_archives.h"
#include "stg31/scenery_land.h"
#include "stg31/green_hill_sounds.h"
#include "stg31/path_ride.h"
#include "stg31/scenery.h"
#include "stg31/sky.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* the task levels of the background's task and of the rides' */
#define BACKGROUND_LEVEL 1
#define RIDES_LEVEL 2
/* the least the set file is read as, in entries */
#define SET_ENTRIES 2048
/* the far water shows each of its WATER_FRAMES textures for
 * WATER_FRAME_STEPS steps, in turn */
#define WATER_FRAMES 5
#define WATER_FRAME_STEPS 10

/* a player of any character below the water's surface, at any depth,
 * splashes into it and misses */
static struct kill_area water_areas[3] = {
#include "stg31/green_hill/water_areas.inc"
};
static struct light stage_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file green_hill_init read, until green_hill_exit frees it */
static struct set_head *green_hill_set = NULL;
/* the background's task and the rides', or null while none lives */
static struct task *background_task = NULL;
static struct task *rides_task = NULL;

static void rides_step(struct task *task);
static void rides_destroy(struct task *task);
static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

void green_hill_init(void)
{
    struct task *task;
    Uint8 *buffer;

    world_land_start(&green_hill_land);
    texture_load_shared(green_hill_pvms, green_hill_texture_groups);
    texture_await_shared(green_hill_pvms, green_hill_texture_groups);
    texture_load_shared(green_hill_background_pvms,
                        green_hill_background_groups);
    texture_await_shared(green_hill_background_pvms,
                         green_hill_background_groups);
    green_hill_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&green_hill_objects, green_hill_set);
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
    lights_load("stg31_light.bin");
    buffer = (Uint8 *)world_build_land(&green_hill_land,
                                       (Uint32)STAGE_MODEL_BUILD_AREA);
    buffer = object_builds_run(buffer);
    green_hill_palm_build(buffer);
    music_fade_in(0);
    music_play_or_defer("g_hill.adx");
    path_create_manager(green_hill_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(water_areas);
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: a second test of drop_rings, which the first
     * settles */
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
    njFogEnable();
    messenger_messages_load("mh0031");
    hint_box_hints_load("bh0031");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    object_builds_init();
    sound_load_stage("se_ac_gh.mlt", &green_hill_sounds);
}

void green_hill_exit(void)
{
    object_builds_exit();
    fog_stop();
    njFogDisable();
    hint_box_hints_free();
    messenger_messages_free();
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(green_hill_pvms, green_hill_texture_groups);
    texture_release_shared(green_hill_background_pvms,
                           green_hill_background_groups);
    set_end();
    if (green_hill_set) {
        loader_free(green_hill_set);
        green_hill_set = NULL;
    }
    world_land_stop();
}

static void rides_step(struct task *task)
{
    if (!task->child) {
        green_hill_path_ride_create(&green_hill_ride_z, task);
        green_hill_path_ride_create(&green_hill_ride_x_upper, task);
        green_hill_path_ride_create(&green_hill_ride_x_lower, task);
    }
}

static void rides_destroy(struct task *task)
{
    if (task == rides_task) {
        rides_task = NULL;
    }
}

void green_hill_step(struct task *task)
{
    struct task_work *work = task->work;

    if (rides_task == NULL) {
        rides_task =
            task_create(TASK_WORK, RIDES_LEVEL, rides_step, "ObjSetManExec");
        if (rides_task) {
            rides_task->destructor = rides_destroy;
            rides_step(rides_task);
        }
    }
    /* @hack redundant: a switch of one empty case, where nothing would do */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
    Sint32 frame;
    int phase;
    int scroll;
    Uint32 tint;
    struct terrain_scroll *entry;
    unsigned int index;

    /* @hack spelling: the assignment inside the expression, where a
     * statement of its own would do */
    phase = (Uint16)((frame = game_state.steps) * green_hill_scroll_rate);
    /* @hack spelling: scroll, a copy of phase, where phase would do */
    scroll = phase;
    entry = green_hill_terrain_scrolls;
    tint = color_blend(green_hill_tint_dark, green_hill_tint_light,
                       0.5F + (0.5F * njSin(frame * green_hill_tint_rate)));
    for (index = 0; index < GREEN_HILL_SCROLL_MODELS; ++index, ++entry) {
        if (entry->anim) {
            texture_anim_scroll(entry->object->model, entry->anim,
                                (entry->speed_u * scroll) & (SCROLL_WRAP - 1),
                                (entry->speed_v * scroll) & (SCROLL_WRAP - 1));
        } else {
            vlist_set_color(entry->object->model->vlist, tint);
        }
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

#define DRAW_LAYER(layer)                                                      \
    do {                                                                       \
        njSetTexture(&layer##_texlist);                                        \
        njCnkEasyDrawObject((layer));                                          \
    } while (0)

#define DRAW_SCROLLING_LAYER(layer, u, v)                                      \
    do {                                                                       \
        njSetTexture(&layer##_texlist);                                        \
        texture_anim_scroll((layer)->model, &layer##_uv,                       \
                            ((u) * game_state.steps) & (SCROLL_WRAP - 1),      \
                            ((v) * game_state.steps) & (SCROLL_WRAP - 1));     \
        njCnkEasyDrawObject((layer));                                          \
    } while (0)

static void background_draw(struct task *task)
{
    NJS_TEXLIST water;

    njFogDisable();
    njPushMatrixEx();
    njTranslate(NULL, camera_views[camera_current_index]->pos.x, 0.0F,
                camera_views[camera_current_index]->pos.z);
    njPushMatrixEx();
    njRotateY(NULL, game_state.steps * green_hill_sky_turn);
    DRAW_LAYER(green_hill_sky_west);
    DRAW_LAYER(green_hill_sky_south);
    DRAW_LAYER(green_hill_sky_east);
    DRAW_LAYER(green_hill_sky_north);
    njPopMatrixEx();
    DRAW_LAYER(green_hill_horizon);
    DRAW_SCROLLING_LAYER(green_hill_horizon_scroll, green_hill_horizon_u,
                         green_hill_horizon_v);
    DRAW_SCROLLING_LAYER(green_hill_lower_1, green_hill_lower_u,
                         green_hill_lower_v);
    DRAW_SCROLLING_LAYER(green_hill_lower_2, green_hill_lower_u,
                         green_hill_lower_v);
    DRAW_SCROLLING_LAYER(green_hill_lower_3, green_hill_lower_u,
                         green_hill_lower_v);
    DRAW_SCROLLING_LAYER(green_hill_lower_4, green_hill_lower_u,
                         green_hill_lower_v);
    DRAW_SCROLLING_LAYER(green_hill_lower_5, green_hill_lower_u,
                         green_hill_lower_v);
    DRAW_SCROLLING_LAYER(green_hill_lower_6, green_hill_lower_u,
                         green_hill_lower_v);
    DRAW_SCROLLING_LAYER(green_hill_lower_7, green_hill_lower_u,
                         green_hill_lower_v);
    njPopMatrixEx();
    land_table_draw(&green_hill_distant_land);
    water = green_hill_water_texlist;
    water.textures += (game_state.steps / WATER_FRAME_STEPS) % WATER_FRAMES;
    water.nbTexture = 1;
    green_hill_water_land.texlist = &water;
    land_table_draw(&green_hill_water_land);
    green_hill_water_land.texlist = NULL;
    njFogEnable();
}
