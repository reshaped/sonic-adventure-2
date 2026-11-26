/* Final Rush, stg30: the stage's init, exit and step, and its background. the
 * background task, BgExec, draws the sky around the current camera, animates
 * the sky's scenery and throws meteors. it starts the rockfall's boulder and
 * shards. while player 0 runs along the rockfall, it blends the fog toward the
 * rockfall's */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg30/sky_land.h"
#include "stg30/paths.h"
#include "stg30/terrain.h"
#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/space_sky_textures.h"
#include "shared/vertex_colors.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg30/final_rush.h"
#include "stg30/object_archives.h"
#include "stg30/meteor.h"
#include "stg30/rockfall.h"
#include "stg30/scenery.h"
#include "stg30/sky.h"
#include "stg30/space_motion.h"
#include "stg30/tunnel_sound.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

#ifdef NONMATCHING
/* the plain form the hack tag names */
#define FOG_KEPT_CLEAR() (game_state.unk_25 || game_state.unk_09)
#else
/* @hack type: game_state's unk_09 and unk_25 read as dat_8c0db215 and
 * dat_8c0db231, where the fields would do */
extern Sint8 dat_8c0db215;
extern Sint8 dat_8c0db231;
#define FOG_KEPT_CLEAR() (dat_8c0db231 || dat_8c0db215)
#endif

/* the set file's block holds at least SET_ENTRIES entries, its head counted as
 * one */
#define SET_ENTRIES 2048
#define BACKGROUND_LEVEL 1
#define METEOR_LEVEL 2
#define PICK_RANGE 100.0F
/* while kill_areas_task has a child, player 0 sheds a spark in each step whose
 * RANDOM_UNIT() is above SPARK_ABOVE. the child is the miss of a player that
 * fell out */
#define SPARK_ABOVE 0.5F

/* a boulder starts BOULDER_LIFT above rockfall_start. it starts when player 0
 * arrives within BOULDER_NEAR of rockfall_start, and below it. another starts
 * only BOULDER_WAIT_STEPS steps or more after the last step that found player
 * 0 there. BOULDER_WAIT(task) counts those steps down */
#define BOULDER_LIFT 50.0F
#define BOULDER_NEAR 400.0F
#define BOULDER_WAIT(task) ((task)->unk_3c.l)
#define BOULDER_WAIT_STEPS 80
#define BOULDER_DELAY 80.0F /* the boulder's steps before it moves */
#define BOULDER_SPEED 1.0F /* the boulder's speed toward -z, a step */
#define BOULDER_SOUND 9 /* a sound in final_rush_sound_list */

/* player 0 is on the rockfall's stretch while its y is below ROCKFALL_Y and its
 * z below ROCKFALL_Z. there it runs along the rockfall while it lies within
 * LINE_NEAR of the line through rockfall_start and rockfall_end. its nearest
 * point on the line must lie between the two */
#define ROCKFALL_Y (-21022.0F)
#define ROCKFALL_Z 11350.0F
#define LINE_NEAR 500.0F
/* BgExec's work->tick, from 0 to FOG_STEPS, blends the fog from base_fog toward
 * rockfall_fog. it rises by 1 a step while player 0 runs along the rockfall. it
 * falls by 1 a step while player 0 is within LINE_NEAR of the line past an end.
 * elsewhere on the stretch it holds, and off the stretch it is 0 */
#define FOG_STEPS 30

/* along the rockfall, the background plays fragment sounds, shakes player 0's
 * pad and starts shards */
#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side: it plays no fragment
 * sound without camera 0 */
#define FRAGMENT_SOUND_DUE()                                                   \
    (camera_views[0] != NULL && RANDOM_UNIT() < fragment_sound_chance)
#else
/* @bug memory: the original reads camera 0 at 0x8c500ab2 for the sound
 * position. It tests the view before spawning a meteor, but not here. */
#define FRAGMENT_SOUND_DUE() (RANDOM_UNIT() < fragment_sound_chance)
#endif
#define FIRST_FRAGMENT_SOUND 4 /* a sound in final_rush_sound_list */
#define FRAGMENT_SOUNDS 3
#define SOUND_SCATTER 200.0F /* a fragment sound's spread about camera 0 */
#define RUMBLE_PERIOD 13 /* the steps between shakes of player 0's pad */
#define RUMBLE_POWER 1
#define RUMBLE_TIME 1
#define RUMBLE_FREQ 20
#define RUMBLE_FREQ_RANGE 5.0F
#define ROCKS_MAX 15 /* the rocks at which shards stop starting */
/* how far past player 0 along +z a shard aims */
#define SHARD_OFFSET_Z 250.0F
#define SHARD_OFFSET_Z_RANGE 500.0F
/* a shard's start above the line's point nearest its aim */
#define SHARD_LIFT 20.0F
#define SHARD_LIFT_RANGE 30.0F
#define SHARD_SCATTER 150.0F /* a shard start's spread along x and z */
/* the share of player 0's speed a shard takes, and the spread about it */
#define PLAYER_SPEED_SHARE 0.3F
#define SPEED_SCATTER 3.0F

#define ORBIT_TILT 0x3000 /* the orbits' turn about x at step 0 */
#define SKY_LAND_CLIP 4500.0F /* the sky land's far clip in z */
#define STAR_BUILDS 1
#define ORBIT_BUILDS 3
/* each sky model's index in stars_builds or orbit_builds */
#define STARS 0
#define EARTH 0
#define ORBIT_128 1
#define ORBIT_64 2
#define SKY_PUSHES 2 /* the camera's push and the orbits' */

#include "stg30/kill_planes.h"

static struct kill_area rush_kill_areas[19] = {
#include "stg30/final_rush/rush_kill_areas.inc"
};
static struct light stage_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *stage_set = NULL;
/* the background task, or null while there is none */
static struct task *background_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color block_colors[] = {
    {0xff000000, 0xffffffff},
    {0xffffff00,       0xc2},
    {0xffff0000,     0xc180},
    {0xffff00ff,     0x2104},
    {0xff00ffff,      0x30c},
    {0xffff7d00,      0x208},
    {0xff0000ff,      0x618},
    {0xff007d00,      0xc10},
    {0xff7d007d,      0x860},
    {0xff7d0000,     0x9840},
    {0xff007d7d,       0x80},
    {0xff00ff00,       0x26},
    {0xff00007d,      0x430},
    {0xff7d7d00,     0x1000},
    {0xff7d7d7d,     0x3000},
    {0xffffffff,     0x6000},
    {0xffff7dfc,     0x4000},
};
/* clang-format on */
static NJS_POINT3 block_origin = {
#include "stg30/final_rush/block_origin.inc"
};
static NJS_POINT3 block_cell = {
#include "stg30/final_rush/block_cell.inc"
};

/* @hack type: each setting an array of one, where the setting itself would
 * do */
static struct fog_setting base_fog[1]; /* off the rockfall */
static struct fog_setting rockfall_fog[1]; /* the rockfall's */
static struct fog_setting clear_fog[1]; /* while FOG_KEPT_CLEAR() holds */

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

static Uint8 *sky_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, stars_builds, STAR_BUILDS) ==
        STAR_BUILDS) {
        sky_built = TRUE;
    }
    njFogDisable();
    if (sky_built && model_builds_compile(&buffer, orbit_builds,
                                          ORBIT_BUILDS) == ORBIT_BUILDS) {
        sky_built = TRUE;
    } else {
        sky_built = FALSE;
    }
    njFogEnable();
    return buffer;
}

void stg30_init(void)
{
    struct task *task;
    Uint8 *buffer;

    world_land_start(&rush_land);
    world_load_block_grid("FINALRUSH.BMP", block_colors, &block_origin,
                          &block_cell);
    texture_load_shared(rush_pvms, rush_texture_groups);
    texture_await_shared(rush_pvms, rush_texture_groups);
#ifdef NONMATCHING
    /* this side loads the sky's textures only where sky_earth_texture, the
     * first name of their list, isn't empty. a build without the disc's data
     * leaves it empty */
    if (sky_earth_texture[0] != '\0')
#endif
    {
        texture_load_shared(rush_sky_pvms, rush_sky_groups);
        texture_await_shared(rush_sky_pvms, rush_sky_groups);
    }
    stage_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&stg30_objects, stage_set);
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
    lights_load("stg30_light.bin");
    buffer = object_builds_run(
        (Uint8 *)world_build_land(&rush_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    buffer = sky_build(buffer);
    rockfall_build(buffer);
    music_fade_in(0);
    music_play_or_defer("f_rush.adx");
    path_create_manager(rush_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(rush_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: a second test of drop_rings, which the first test leaves
     * non-null */
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    njFogEnable();
    messenger_messages_load("mh0030");
    hint_box_hints_load("bh0030");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    fog_start();
    fog_read("stg30_fogA.bin", base_fog);
    fog_read("stg30_fogB.bin", rockfall_fog);
    fog_clear(clear_fog);
    sound_load_stage("se_ac_fr.mlt", &final_rush_sound_list);
    object_builds_init();
    fog_set_blend(base_fog, rockfall_fog, 0.0F);
    debris_friction = 0.99F;
    debris_gravity = 0.038F;
    debris_life = 90;
    debris_life_spread = 30.0F;
}

void stg30_exit(void)
{
    object_builds_exit();
    fog_stop();
    njFogDisable();
    hint_box_hints_free();
    messenger_messages_free();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(rush_pvms, rush_texture_groups);
#ifdef NONMATCHING
    /* this side releases the sky's textures only where the init loaded them:
     * where sky_earth_texture isn't empty */
    if (sky_earth_texture[0] != '\0')
#endif
    {
        texture_release_shared(rush_sky_pvms, rush_sky_groups);
    }
    set_end();
    if (stage_set != NULL) {
        loader_free(stage_set);
        stage_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void stg30_step(struct task *task)
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

#ifdef NONMATCHING
#else
/* @hack spelling: line_direction steps each pointer on its own, where the three
 * differences by name would do */
#pragma inline(line_direction)
static void line_direction(Float *from, Float *to, Float *result)
{
    *result++ = *to++ - *from;
    from++;
    *result++ = *to++ - *from;
    from++;
    *result = *to - *from;
}

#endif

static void background_step(struct task *task)
{
    /* @hack spelling: background, a copy of task, where task would do */
    struct task *background = task;
    /* @hack spelling: source_work and work, two copies of the work's pointer,
     * where work alone would do */
    struct task_work *source_work = background->work;
    struct task_work *work = source_work;
    struct scenery_strip *entry;
    struct task *spawned;
    Uint32 steps;
    Uint32 color;
    int offset;
    Uint32 i;

    /* @hack spelling: steps set inside the product, where a statement of its
     * own would do */
    /* @hack type: the offset cut to a Uint16, where the whole product would do:
     * the scrolls wrap at SCROLL_WRAP, which divides 0x10000 */
    offset = (Uint16)((steps = game_state.steps) * sky_scroll_rate);
    entry = scenery_strips;
    /* @hack type: the steps cast to an Angle before the product, where the
     * product cast would do */
    color = color_blend(sky_pulse_dim, sky_pulse_bright,
                        (njSin((Angle)steps * sky_pulse_rate) * 0.5F) + 0.5F);
    for (i = 0; i < SCENERY_STRIPS; i++, entry++) {
        if (entry->motion == SCENERY_SCROLL) {
            texture_anim_scroll(entry->object->model, entry->anim,
                                (entry->u_speed * offset) & (SCROLL_WRAP - 1),
                                (entry->v_speed * offset) & (SCROLL_WRAP - 1));
        } else {
            vlist_set_color(entry->object->model->vlist, color);
        }
    }
    if (player_work[0] != NULL && kill_areas_task != NULL &&
        kill_areas_task->child != NULL && RANDOM_UNIT() > SPARK_ABOVE) {
        burst_create_wobbling(&player_work[0]->pos, &player_motion[0]->spd,
                              (RANDOM_UNIT() * spark_size_range) + spark_size);
    }
    if (camera_views[0] != NULL && RANDOM_UNIT() < meteor_chance) {
        NJS_POINT3 pos;

        pos.x = (RANDOM_UNIT() * meteor_spread) + meteor_reach;
        pos.y = meteor_height;
        pos.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&camera_views[0]->pos);
        njRotateY(NULL, RANDOM_TWO_TURNS());
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrixEx();
        spawned = task_create(TASK_WORK, METEOR_LEVEL, meteor_fall_init,
                              "ObjectMeteoFall");
        if (spawned != NULL) {
            spawned->work->pos = pos;
            spawned->work->scl.y = meteor_fall_speed;
            spawned->work->scl.z =
                meteor_drift * ((rand() * (2.0F * RAND_SCALE)) - 1.0F);
            spawned->work->scl.x =
                meteor_drift * ((rand() * (2.0F * RAND_SCALE)) - 1.0F);
            spawned->work->ang.z =
                _builtin_fabsf(meteor_drop / meteor_fall_speed);
        }
    }
    if (player_work[0] != NULL) {
        if (njDistanceP2P(&rockfall_start, &player_work[0]->pos) <
                BOULDER_NEAR &&
            player_work[0]->pos.y < rockfall_start.y) {
            if (BOULDER_WAIT(background) == 0) {
                spawned = task_create(TASK_WORK, ROCK_LEVEL, rockfall_init,
                                      "ObjectBurst");
                if (spawned != NULL) {
                    sound_play(BOULDER_SOUND, NULL, 0, 0);
                    spawned->work->pos = rockfall_start;
                    spawned->work->pos.y += BOULDER_LIFT;
                    spawned->work->scl.x = BOULDER;
                    spawned->work->scl.y = 0.0F;
                    spawned->work->scl.z = BOULDER_DELAY;
                    spawned->work->ang.x = 0;
                    spawned->work->ang.y = 0;
                    spawned->work->ang.z = 0;
                    if (rockfall_attach(spawned)) {
                        ROCKFALL(spawned)->velocity.x = 0.0F;
                        ROCKFALL(spawned)->velocity.y = 0.0F;
                        ROCKFALL(spawned)->velocity.z = -BOULDER_SPEED;
                        spawned->work->mode = ROCK_MOVING;
                    }
                }
            }
            BOULDER_WAIT(background) = BOULDER_WAIT_STEPS;
        }
        if (BOULDER_WAIT(background)) {
            BOULDER_WAIT(background)--;
        }
        if (player_work[0]->pos.y < ROCKFALL_Y &&
            player_work[0]->pos.z < ROCKFALL_Z) {
            NJS_LINE line;
            NJS_POINT3 closest;
            NJS_POINT3 player_pos;
            NJS_POINT3 sound_pos;
            Float length;
            Float to_start;
            Float to_end;
            Float dist;
            int sound_index;

#ifdef NONMATCHING
            /* the plain form the hack tag names */
            line.vx = rockfall_start.x - rockfall_end.x;
            line.vy = rockfall_start.y - rockfall_end.y;
            line.vz = rockfall_start.z - rockfall_end.z;
            line.px = (rockfall_start.x + rockfall_end.x) * 0.5F;
            line.py = (rockfall_start.y + rockfall_end.y) * 0.5F;
            line.pz = (rockfall_start.z + rockfall_end.z) * 0.5F;
#else
            /* @hack type: the points' coordinates through Float pointers, where
             * their fields would do */
            line_direction(&rockfall_end.x, (Float *)&rockfall_start, &line.vx);
            line.px = (rockfall_start.x + ((Float *)&rockfall_end)[0]) * 0.5F;
            line.py = (rockfall_start.y + ((Float *)&rockfall_end)[1]) * 0.5F;
            line.pz = (rockfall_start.z + ((Float *)&rockfall_end)[2]) * 0.5F;
#endif
            player_pos = player_work[0]->pos;
            dist = njDistanceP2L(&player_pos, &line, &closest);
            if (dist < LINE_NEAR) {
                length = njDistanceP2P(&rockfall_start, &rockfall_end);
                /* @hack redundant: a call no path reaches */
                if (0) {
                    task_create(length, 0, NULL, NULL);
                }
                to_start = njDistanceP2P(&rockfall_start, &closest);
                to_end = njDistanceP2P(&rockfall_end, &closest);
                if (length > to_start && length > to_end) {
                    if (FRAGMENT_SOUND_DUE()) {
                        sound_pos.x = camera_views[0]->pos.x +
                                      ((RANDOM_UNIT() - 0.5F) * SOUND_SCATTER);
                        sound_pos.y = camera_views[0]->pos.y +
                                      ((RANDOM_UNIT() - 0.5F) * SOUND_SCATTER);
                        sound_pos.z = camera_views[0]->pos.z +
                                      ((RANDOM_UNIT() - 0.5F) * SOUND_SCATTER);
                        sound_index =
                            (int)(rand() * (PICK_RANGE * RAND_SCALE)) %
                            FRAGMENT_SOUNDS;
                        sound_play_at(sound_index + FIRST_FRAGMENT_SOUND, NULL,
                                      0, fragment_sound_volumes[sound_index],
                                      &sound_pos);
                    }
                    if (game_state.steps % RUMBLE_PERIOD == 0) {
                        vibration_start(
                            0, RUMBLE_POWER,
                            (int)(rand() * (RUMBLE_FREQ_RANGE * RAND_SCALE)) +
                                RUMBLE_FREQ,
                            RUMBLE_TIME);
                    }
                    /* @hack redundant: a store no path reaches */
                    if (0) {
                        sound_pos.x = sound_index;
                    }
                    if (work->tick < FOG_STEPS) {
                        work->tick++;
                    }
                    if (rockfall_count < ROCKS_MAX) {
                        spawned = task_create(TASK_WORK, ROCK_LEVEL,
                                              rockfall_init, "ObjectBurst");
                        if (spawned != NULL) {
                            player_pos.z +=
                                (rand() * (SHARD_OFFSET_Z_RANGE * RAND_SCALE)) +
                                SHARD_OFFSET_Z;
                            njDistanceP2L(&player_pos, &line, &closest);
                            spawned->work->pos = closest;
                            spawned->work->pos.y += SHARD_LIFT;
                            spawned->work->pos.x +=
                                (RANDOM_UNIT() - 0.5F) * SHARD_SCATTER;
                            spawned->work->pos.y +=
                                rand() * (SHARD_LIFT_RANGE * RAND_SCALE);
                            spawned->work->pos.z +=
                                (RANDOM_UNIT() - 0.5F) * SHARD_SCATTER;
                            spawned->work->scl.y = 0.0F;
                            spawned->work->scl.z = 0.0F;
                            spawned->work->ang.x = 0;
                            spawned->work->ang.y = 0;
                            spawned->work->ang.z = 0;
                            spawned->work->scl.x =
                                ((int)(rand() * (PICK_RANGE * RAND_SCALE)) %
                                 SHARD_KINDS) +
                                FIRST_SHARD;
                            spawned->work->timer =
                                (int)(rand() *
                                      (SHARD_LIFE_RANGE * RAND_SCALE)) +
                                SHARD_LIFE;
                            if (rockfall_attach(spawned)) {
                                /* @hack spelling: player 0's motion through
                                 * player_tasks[0], where player_motion[0] would
                                 * do */
                                ROCKFALL(spawned)->velocity.x =
                                    (((struct player_motion *)player_tasks[0]
                                          ->motion.p)
                                         ->spd.x *
                                     PLAYER_SPEED_SHARE) +
                                    ((RANDOM_UNIT() - 0.5F) * SPEED_SCATTER);
                                ROCKFALL(spawned)->velocity.y =
                                    (((struct player_motion *)player_tasks[0]
                                          ->motion.p)
                                         ->spd.y *
                                     PLAYER_SPEED_SHARE) +
                                    ((RANDOM_UNIT() - 0.5F) * SPEED_SCATTER);
                                ROCKFALL(spawned)->velocity.z =
                                    (((struct player_motion *)player_tasks[0]
                                          ->motion.p)
                                         ->spd.z *
                                     PLAYER_SPEED_SHARE) +
                                    ((RANDOM_UNIT() - 0.5F) * SPEED_SCATTER);
                                spawned->work->mode = ROCK_MOVING;
                            }
                        }
                    }
                } else if (work->tick) {
                    work->tick--;
                }
            }
        } else {
            work->tick = 0;
        }
    }
    if (FOG_KEPT_CLEAR()) {
        fog_set_blend(clear_fog, clear_fog, 1.0F);
    } else {
        fog_set_blend(base_fog, rockfall_fog, work->tick * (1.0F / FOG_STEPS));
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

static void background_draw(struct task *task)
{
    struct task_work *work;
    struct land_entry *entry;
    Sint32 mask;
    int entries_left;
    NJS_POINT3 pos;
    /* @hack redundant: frame_space, eight bytes that only size the frame */
    Uint8 frame_space[8];

    (void)frame_space;

    /* @hack redundant: the read of task->work, which nothing uses */
    work = task->work;
    (void)work;
    njPushMatrixEx();
    njTranslateEx(&camera_views[camera_current_index]->pos);
    njSetTexture(&background_1_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, stars_height, 0.0F);
    njScale(NULL, stars_scale, 1.0F, stars_scale);
    if (sky_built) {
        njDirectDrawObject(stars_builds[STARS].built);
    } else {
        njCnkEasyDrawObject(&rush_stars_object);
    }
    njPopMatrixEx();
    njFogDisable();
    njSetTexture(&background_0_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, earth_height, 0.0F);
    njScale(NULL, earth_scale, 1.0F, earth_scale);
    if (sky_built) {
        njDirectDrawObject(orbit_builds[EARTH].built);
    } else {
        njCnkEasyDrawObject(&rush_earth_object);
    }
    njPopMatrixEx();
    njPushMatrixEx();
    njSetTexture(&background_2_texlist);
    njTranslateEx((NJS_VECTOR *)rush_orbit_128_object->pos);
    njTranslate(NULL, rush_orbit_128_object->model->center.x,
                rush_orbit_128_object->model->center.y,
                rush_orbit_128_object->model->center.z);
    njRotateX(NULL, (orbit_128_pitch * game_state.steps) + ORBIT_TILT);
    njRotateY(NULL, orbit_128_yaw * game_state.steps);
    njScale(NULL, orbit_scale, orbit_scale, orbit_scale);
    njTranslate(NULL, -rush_orbit_128_object->model->center.x,
                -rush_orbit_128_object->model->center.y,
                -rush_orbit_128_object->model->center.z);
    if (sky_built) {
        njDirectDrawObject(orbit_builds[ORBIT_128].built);
    } else {
        njCnkEasyDrawModel(rush_orbit_128_object->model);
    }
    matrix_load_parent();
    njSetTexture(&background_3_texlist);
    njTranslateEx((NJS_VECTOR *)rush_orbit_64_object->pos);
    njTranslate(NULL, rush_orbit_64_object->model->center.x,
                rush_orbit_64_object->model->center.y,
                rush_orbit_64_object->model->center.z);
    njRotateX(NULL, (orbit_64_pitch * game_state.steps) + ORBIT_TILT);
    njRotateY(NULL, orbit_64_yaw * game_state.steps);
    njScale(NULL, orbit_scale, orbit_scale, orbit_scale);
    njTranslate(NULL, -rush_orbit_64_object->model->center.x,
                -rush_orbit_64_object->model->center.y,
                -rush_orbit_64_object->model->center.z);
    if (sky_built) {
        njDirectDrawObject(orbit_builds[ORBIT_64].built);
    } else {
        njCnkEasyDrawModel(rush_orbit_64_object->model);
    }
    matrix_load_parent();
    njPopMatrix(SKY_PUSHES);
    njFogEnable();
    njFogDisable();
    if (camera_current_index == 0) {
        mask = game_state.land_blocks[0];
    } else {
        mask = game_state.land_blocks[1];
    }
    entries_left = rush_sky_land.count;
    entry = rush_sky_land.entries;
    njPushMatrixEx();
    njSetTexture(&rush_sky_land_texlist);
    for (; entries_left--; entry++) {
        if (entry->blocks == 0 || (entry->blocks & mask)) {
            njCalcPoint(NULL, &entry->center, &pos);
            if (pos.z < SKY_LAND_CLIP && pos.z > -entry->radius) {
                NJS_CNK_OBJECT *object = (NJS_CNK_OBJECT *)entry->object;

                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, XYZ_TURN);
                njCnkEasyDrawModel(object->model);
                matrix_load_parent();
            }
        }
    }
    njPopMatrixEx();
}
