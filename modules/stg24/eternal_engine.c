/* Eternal Engine, stg24: the stage's init, exit and step, and its background.
 * the step makes a manager, which makes the reactor and plays two voices */
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
#include "shared/blast.h"
#include "shared/bomb_hat.h"
#include "shared/capsule_bomb.h"
#include "shared/common_object.h"
#include "shared/black_smoke.h"
#include "shared/core_shutter.h"
#include "shared/field.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/kazobj.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "draw/texture_anim.h"
#include "sound/adx.h"
#include "stg24/sounds.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg24/eternal_engine.h"
#include "stg24/object_archives.h"
#include "stg24/hatch.h"
#include "stg24/reactor.h"
#include "stg24/scenery.h"
#include "system/areas.h"
#include "system/flags.h"
#include "system/sequence.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

static struct fog_setting engine_fog;
#include "stg24/kill_planes.h"

/* room for the set file's entries */
#define SET_ENTRIES 2048
/* the task levels of the background's task and of the reactor's manager */
#define BACKGROUND_LEVEL 1
#define MANAGER_LEVEL 2
#define SCROLL_OBJECTS 10
/* the manager keeps the reactor as its child while player_near finds a player
 * within REACTOR_RANGE of reactor_position. it kills the reactor where none
 * does, or where player_work[0] is null */
#define REACTOR_RANGE 1500.0F
/* in the Hero story, the manager's timer starts at FIRST_VOICE_STEP once
 * player 0 comes near voice_position. near is within VOICE_RADIUS in x and z,
 * and within VOICE_HEIGHT in y. the timer then counts the steps, and holds at
 * VOICE_TIMER_MAX. FIRST_VOICE plays at FIRST_VOICE_STEP, and SECOND_VOICE at
 * SECOND_VOICE_STEP. each plays once in the manager's life */
#define VOICE_RADIUS 150.0F
#define VOICE_HEIGHT 30.0F
#define FIRST_VOICE 205
#define SECOND_VOICE 206
#define FIRST_VOICE_STEP 1
#define SECOND_VOICE_STEP 240
#define VOICE_TIMER_MAX 10000
#define VOICE_PRIORITY 0
/* the background hides the sky while the current camera lies near x =
 * SKY_HIDE_X, z = SKY_HIDE_Z. near is within SKY_HIDE_RADIUS in x and z */
#define SKY_HIDE_RADIUS 180.0F
#define SKY_HIDE_X (-11.0F)
#define SKY_HIDE_Z (-9150.0F)
#define SKY_BUILDS 2
/* both skies' turn about x at step 0, which grows by engine_sky_pitch each
 * step. the inner sky draws scaled, and turned INNER_SKY_TILT more about x */
#define SKY_TILT 0x3000
#define INNER_SKY_TILT 0x1000
#define INNER_SKY_SCALE_XZ 0.7F
#define INNER_SKY_SCALE_Y 0.75F
/* the scenery passes draw an entry whose center lies less than this far ahead
 * of the camera */
#define SCENERY_DRAW_DEPTH 2000.0F

static struct kill_area stage_kill_areas[6] = {
#include "stg24/eternal_engine/stage_kill_areas.inc"
};
static struct light engine_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
static struct set_head *engine_set = NULL;
static struct task *background_task = NULL;
static struct land_block_color block_colors[] = {
    {0xff000000, -1},      {0xffffff00, 0x18432}, {0xffff0000, 0xd826},
    {0xffff00ff, 0x30104}, {0xff00ffff, 0x830c},  {0xffff7d00, 0x6008},
    {0xff0000ff, 0x218},   {0xff007d00, 0x20c10}, {0xff7d007d, 0x10860},
    {0xff7d0000, 0x3040},  {0xff007d7d, 0x610c0}, {0xff00ff00, 0x4600},
    {0xff00007d, 0x2180},
};
static NJS_POINT3 grid_origin = {-3216.0F, 0.0F, 419.203125F};
static NJS_POINT3 grid_cell = {19.801981F, 0.0F, 19.920319F};
static struct task *reactor_manager = NULL;
static NJS_POINT3 reactor_position = {-5.7805023F, -1786.0F, -9150.0F};
static NJS_POINT3 voice_position = {-9.0F, -1480.0F, -9150.0F};

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);
static Uint8 *build_background(Uint8 *start);

void stg24_init(void)
{
    struct task *task;
    Uint8 *cursor;

    world_land_start(&eternal_engine_land);
    world_load_block_grid("ETERNAL_ENGINE.BMP", block_colors, &grid_origin,
                          &grid_cell);
    texture_load_shared(engine_bg_pvms, engine_bg_groups);
    texture_await_shared(engine_bg_pvms, engine_bg_groups);
    texture_load_shared(engine_object_pvms, engine_object_groups);
    texture_await_shared(engine_object_pvms, engine_object_groups);
    engine_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&stg24_objects, engine_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &engine_lights[0]);
    light_set(1, &engine_lights[1]);
    light_set(2, &engine_lights[0]);
    light_set(3, &engine_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg24_light.bin");
    cursor = (Uint8 *)world_build_land(&eternal_engine_land,
                                       (Uint32)STAGE_MODEL_BUILD_AREA);
    cursor = object_builds_run(cursor);
    cursor = kazobj_build(cursor);
    cursor = capsule_bomb_build(cursor);
    cursor = hatches_build(cursor);
    cursor = build_background(cursor);
    core_shutter_build(cursor);
    music_fade_in(0);
    music_play_or_defer("e_engine.adx");
    path_create_manager(eternal_engine_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(stage_kill_areas);
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
    njFogEnable();
    messenger_messages_load("mh0024");
    hint_box_hints_load("bh0024");
    broken_down_smoke_create(0, NULL);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    fog_start();
    fog_read("stg24_fogA.bin", &engine_fog);
    blast_load_texture();
    sound_load_stage("se_ac_ee.mlt", &engine_sounds);
}

void stg24_exit(void)
{
    fog_stop();
    njFogDisable();
    hint_box_hints_free();
    messenger_messages_free();
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
    texture_release_shared(engine_object_pvms, engine_object_groups);
    texture_release_shared(engine_bg_pvms, engine_bg_groups);
    set_end();
    if (engine_set != NULL) {
        loader_free(engine_set);
        engine_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

static void player_fall_from_hatch(int slot)
{
    /* kill_areas_task's child is a miss under way, as player_fall_out makes
     * one */
    if (kill_areas_task && !kill_areas_task->child) {
        if (player_work[slot] &&
            !(player_records[slot]->flags & PLAYER_MISSED)) {
            player_fall_out(slot);
        }
    }
}

static void reactor_manager_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *reactor;
    Float x;
    Float y;
    Float z;
    Float distance;

    if (player_work[0] && player_near(&reactor_position, REACTOR_RANGE)) {
        if (!task->child) {
            reactor = task_create_child(TASK_WORK, reactor_lid_init, task);
            if (reactor) {
                reactor->work->pos = reactor_position;
                reactor->work->ang.x = 0;
                reactor->work->ang.y = 0;
                reactor->work->ang.z = 0;
            }
        }
    } else if (task->child) {
        task_kill(task->child);
    }
    if (!work->timer) {
        if (game_flags[FLAG_SEQUENCE] == SEQUENCE_HERO) {
            /* @bug memory: reads player 0's pos with no test for null, though
             * the reactor's test above tests player_work[0], TST at
             * 0x8c500406. the read is FMOV.S at 0x8c500506 */
#ifdef NONMATCHING
            /* this side repairs the tag above: without player 0's work, the
             * manager tests for the voice at a later step. the timer is 0
             * here, so the return skips nothing else */
            if (player_work[0] == NULL) {
                return;
            }
#endif
            x = voice_position.x - player_work[0]->pos.x;
            y = voice_position.y - player_work[0]->pos.y;
            z = voice_position.z - player_work[0]->pos.z;
            distance = njSqrt((x * x) + (z * z));
            if (distance < VOICE_RADIUS && njAbs(y) < VOICE_HEIGHT) {
                work->timer = FIRST_VOICE_STEP;
            }
        }
    }
    if (work->timer) {
        if (work->timer == FIRST_VOICE_STEP) {
            voice_play(FIRST_VOICE, VOICE_PRIORITY);
        }
        if (work->timer == SECOND_VOICE_STEP) {
            voice_play(SECOND_VOICE, VOICE_PRIORITY);
        }
        if (work->timer < VOICE_TIMER_MAX) {
            work->timer++;
        }
    }
}

static void reactor_manager_destroy(struct task *task)
{
    if (task == reactor_manager) {
        reactor_manager = NULL;
    }
}

#pragma inline(add_field_speed)
static void add_field_speed(const NJS_VECTOR *speed, NJS_VECTOR *position)
{
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    position->x += speed->x;
    position->y += speed->y;
    position->z += speed->z;
#else
    /* @hack spelling: two Float pointers stepped through the vectors, where
     * the members' sums written out would do */
    const Float *src = &speed->x;
    Float *dst = &position->x;
    *dst++ += *src++;
    *dst++ += *src++;
    *dst += *src;
#endif
}

void stg24_step(struct task *task)
{
    int slot;
    struct task_work *work = task->work;

    if (!reactor_manager) {
        reactor_manager = task_create(TASK_WORK, MANAGER_LEVEL,
                                      reactor_manager_step, "ObjSetManExec");
        if (reactor_manager) {
            reactor_manager->destructor = reactor_manager_destroy;
        }
    }
    fog_set(&engine_fog);
    for (slot = 0; slot < GAME_PLAYERS; slot++) {
        if (player_work[slot]) {
            struct field_sample sample;

            field_sample_at(&player_work[slot]->pos, &sample);
            if (sample.flags & FIELD_SPD) {
                NJS_POINT3 pos;

                pos = player_work[slot]->pos;
                add_field_speed(&sample.spd, &pos);
                player_set_pos(slot, pos.x, pos.y, pos.z);
            }
            if (sample.flags & FIELD_LIFTS) {
                NJS_POINT3 pos;

                pos = player_work[slot]->pos;
                pos.y += player_records[slot]->consts.gravity;
                player_set_pos(slot, pos.x, pos.y, pos.z);
            }
            if (sample.flags & FIELD_KILLS) {
                player_fall_from_hatch(slot);
            }
        }
    }
    /* @hack redundant: a test of slot's FIELD_KILLS bit, in a block no path
     * reaches */
    if (0) {
        if (slot & FIELD_KILLS) {
        }
    }
    /* @hack redundant: a switch of one empty case on the work's mode */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
    struct scroll_entry *entry;
    unsigned int i;

    for (i = 0, entry = engine_scrolls; i < SCROLL_OBJECTS; i++, entry++) {
        texture_anim_apply(entry->object->model, entry->animation,
                           game_state.steps * engine_scroll_speed);
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

static Uint8 *build_background(Uint8 *start)
{
    Uint8 *buffer = start;

    /* @hack redundant: a jump to the statement after it */
    goto compile;
compile:
    njFogDisable();
    if (model_builds_compile(&buffer, engine_sky_builds, SKY_BUILDS) ==
        SKY_BUILDS) {
        engine_sky_built = TRUE;
    }
    njFogEnable();
    return buffer;
}

static void background_draw(struct task *task)
{
    Float x;
    Float z;
    struct game_state *state;
    void (*translate)(NJS_VECTOR *);

    x = SKY_HIDE_X - camera_views[camera_current_index]->pos.x;
    z = SKY_HIDE_Z - camera_views[camera_current_index]->pos.z;
    /* @hack spelling: translate, a local for njTranslateEx, where
     * njTranslateEx called by name would do */
    translate = njTranslateEx;
    /* @hack spelling: state, game_state's address, set inside the sky's test,
     * where a statement of its own before it would do */
    if (state = &game_state,
        !((x * x) + (z * z) < SKY_HIDE_RADIUS * SKY_HIDE_RADIUS)) {
        njFogDisable();
        njSetTexture(&engine_sky_texlist);
        njPushMatrixEx();
        translate(&camera_views[camera_current_index]->pos);
        njRotateX(NULL, (engine_sky_pitch * state->steps) + SKY_TILT);
        njPushMatrixEx();
        njRotateY(NULL, engine_outer_sky_spin * state->steps);
        if (engine_sky_built) {
            njDirectDrawObject(engine_sky_builds[0].built);
            matrix_load_parent();
            njScale(NULL, INNER_SKY_SCALE_XZ, INNER_SKY_SCALE_Y,
                    INNER_SKY_SCALE_XZ);
            njRotateX(NULL, INNER_SKY_TILT);
            njRotateY(NULL, engine_inner_sky_spin * state->steps);
            njDirectDrawObject(engine_sky_builds[1].built);
        } else {
            njCnkEasyDrawObject(&engine_outer_sky);
            matrix_load_parent();
            njScale(NULL, INNER_SKY_SCALE_XZ, INNER_SKY_SCALE_Y,
                    INNER_SKY_SCALE_XZ);
            njRotateX(NULL, INNER_SKY_TILT);
            njRotateY(NULL, engine_inner_sky_spin * state->steps);
            njCnkEasyDrawObject(&engine_inner_sky);
        }
        njPopMatrix(2);
        njFogEnable();
    }
#define PULSE_SCENERY
#include "stg24/scenery_draw.h"
#undef PULSE_SCENERY
#include "stg24/scenery_draw.h"
}
