/* the water shaft of Rouge's route: its background, bubbles and colliders,
 * the clear camera and the pool's cover */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/texture_anim.h"
#include "shared/common_object.h"
#include "shared/core_clear_switch.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "stg37/background.h"
#include "stg37/core_rouge.h"
#include "player/player.h"
#include "shared/messenger.h"
#include "shared/stage_util.h"
#include "shared/switch.h"
#include "shared/core_scenery_art.h"
#include "stg37/water_cover.h"
#include "stg37/water_scenery.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of the background's task, and of the other tasks here */
#define BACKGROUND_LEVEL 1
#define TASK_LEVEL 2
/* the steps the clear camera or the cover has spent in its submode. each task
 * zeroes it as it starts, and as it enters a submode it times */
#define SUBMODE_STEPS(task) ((task)->unk_3c.l)
/* the clear camera plays SHAFT_SOUND under SHAFT_KEY(task), and WATER_SOUND
 * under WATER_KEY(task). the two keys keep each sound_set_volume of
 * SOUND_ANY_ID to one sound. each sound stops SOUND_STEPS + 1 steps after the
 * last step that plays it */
#define SHAFT_SOUND 5
#define WATER_SOUND 22
#define SHAFT_KEY(task) ((char *)(task) + 2)
#define WATER_KEY(task) ((char *)(task) + 1)
#define SOUND_PRIORITY 1
#define SOUND_STEPS 10
/* @hack type: camera 0's land_block_mask read as a Uint32, through a cast of
 * the array's address, where game_state.land_block_mask[0] would do */
#define UNSIGNED_BLOCK_MASK (*(Uint32 *)&game_state.land_block_mask)

/* a bubble's record, at its task's extra. the bubble moves at each step
 * whose game_state.steps differs from last_step, so it stands still while
 * time stops */
struct bubble {
    Uint32 last_step;
    /* its model in bubble_frames, which a move on an odd step advances */
    Sint32 frame;
    Sint32 age; /* the moves it has made, which set its pulse's angle */
    Float surface; /* the y it ends past */
    Float unk_10;
    Float rise; /* how far it rises a move */
    Float unk_18;
    Float scale; /* its scale, which the pulse swings */
};
#define BUBBLE(task) ((struct bubble *)(task)->extra.p)

/* the shaft's axis rises SHAFT_HEIGHT from its foot, at x 0, y 0 and z
 * SHAFT_Z */
#define SHAFT_Z (-240.0F)
#define SHAFT_HEIGHT 280.0F
/* the background's volume of SHAFT_SOUND is far_volume at sound_range from
 * the shaft's side. it rises evenly to near_volume at the side. the side lies
 * shaft_radius out from the shaft's axis */
static int near_volume = 120;
static int far_volume = -25;
static Float sound_range = 300.0F;
static Float shaft_radius = 25.0F;
/* bubble_chance is the chance that a call of START_BUBBLE starts a bubble.
 * the bubble reaches its surface in bubble_lifetime moves. its scale is drawn
 * at random from bubble_scale_min up to bubble_scale_min + bubble_scale_range.
 * its pulse swings that scale by bubble_pulse of itself, and the pulse's angle
 * turns bubble_pulse_rate a move */
static Float bubble_chance = 0.05F;
static int bubble_lifetime = 960;
static Angle bubble_pulse_rate = 1024;
static Float bubble_pulse = 0.1F;
static Float bubble_scale_range = 0.5F;
static Float bubble_scale_min = 0.5F;
/* clang-format off */
static struct collision_shape poison_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_UNK_2000,
    {0.0F, 0.0F, 0.0F}, 100.0F, 5.0F, 100.0F, 0, {0, 0, 0}};
static struct collision_shape fall_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F}, 30.0F, 240.0F, 7.5F, 0, {0, 0, 0}};
/* clang-format on */
static int dat_8c5c5038 = 0;
/* the steps the clear camera shows the water rising for */
static int water_rise_steps = 600;
static int dat_8c5c5040 = 150;
/* the clear camera's volumes of WATER_SOUND and SHAFT_SOUND */
static int water_volume = 25;
static int shaft_volume = 50;

static void background_step(struct task *task);
static void background_draw(struct task *task);
static void poison_start(void);
static void fall_start(void);
static void lower_bubble_start(void);
static void upper_bubble_start(void);
static void bubble_step(struct task *task);
static void bubble_draw(struct task *task);
static void bubble_destroy(struct task *task);
static void clear_camera_step(struct task *task);
static void clear_camera_draw(struct task *task);

void core_rouge_background_init(void)
{
    struct task *task =
        task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        task->draw = background_draw;
    }
}

void core_rouge_colliders_start(void)
{
    poison_start();
    fall_start();
}

void core_cover_collision_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

/* @hack spelling: WORLD_ADDED set by a call that can also clear it, where
 * task->work->flags |= WORLD_ADDED would do */
#pragma inline(set_world_added)
static void set_world_added(struct task *task, Bool added)
{
    if (added) {
        task->work->flags |= WORLD_ADDED;
    } else {
        task->work->flags &= ~WORLD_ADDED;
    }
}

void core_cover_collision_step(struct task *task)
{
    set_world_added(task, TRUE);
}

static void background_step(struct task *task)
{
    /* @hack redundant: work, a read of task's work that nothing uses */
    struct task_work *work = task->work;
    NJS_LINE axis;
    Float distance;
    Float gain;
    int volume;

    if (!game_state.time_stopped) {
        if (game_state.land_block_mask[0] & LAND_BLOCKS(1, 3)) {
            upper_bubble_start();
        }
        if (game_state.land_block_mask[0] & LAND_BLOCKS(2, 3)) {
            lower_bubble_start();
        }
        /* @hack redundant: game_state.time_stopped tested a second time */
        if (!game_state.time_stopped) {
            axis.px = 0;
            axis.py = 0;
            axis.pz = SHAFT_Z;
            axis.vx = 0;
            axis.vy = SHAFT_HEIGHT;
            axis.vz = 0;
            distance =
                njSqrt(point_segment_distance2(
                    &camera_views[camera_current_index]->pos, &axis, NULL)) -
                shaft_radius;
            if (distance <= sound_range) {
                gain = (near_volume - far_volume) / sound_range;
                gain *= sound_range - distance;
                volume = gain + far_volume;
                sound_play_for(SHAFT_SOUND, task, SOUND_PRIORITY, 0,
                               SOUND_STEPS);
                sound_set_volume(task, SOUND_ANY_ID, volume);
            }
        }
    }
}

#include "shared/core_water_geometry.h"

/* the steps each cell of a 4-by-4 sheet shows for, on the walls and on the
 * columns */
#define WALL_CELL_STEPS 2
#define COLUMN_CELL_STEPS 1
/* the columns rise and fall COLUMN_SPEED a step, and start over every
 * COLUMN_STEPS steps */
#define COLUMN_STEPS 180
#define COLUMN_SPEED 3.527778F
#define SHAFT_ROTOR_SLOWDOWN 2
/* the textures of surface_textures, which the background's draw shows in turn,
 * one a step */
#define SURFACE_TEXTURES 16

static void background_draw(struct task *task)
{
    NJS_CNK_MODEL *model;
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
    Float frame;
    int i;

    if (UNSIGNED_BLOCK_MASK & LAND_BLOCK(1)) {
        njSetTexture(&entry_light_textures);
        model = &entry_light;
        if (game_state.steps & 1) {
            njCnkEasyDrawModel(model);
        } else {
            model_draw_tinted(model, COLOR_HALF);
        }
    }
    njSetTexture(&shaft_textures);
    core_draw_sheet_children(&shaft_entry_walls, WALL_CELL_STEPS,
                             LAND_BLOCK(1));
    core_draw_sheet_children(&shaft_walls, WALL_CELL_STEPS, LAND_BLOCKS(1, 2));
    njSetTexture(&column_textures);
    njPushMatrixEx();
    njTranslate(NULL, 0, game_state.steps % COLUMN_STEPS * COLUMN_SPEED, 0);
    core_draw_sheet_children(&rising_columns, COLUMN_CELL_STEPS,
                             LAND_BLOCKS(1, 2));
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, 0, -(game_state.steps % COLUMN_STEPS * COLUMN_SPEED), 0);
    core_draw_sheet_children(&falling_columns, COLUMN_CELL_STEPS,
                             LAND_BLOCKS(1, 2));
    njPopMatrixEx();
    if (UNSIGNED_BLOCK_MASK & LAND_BLOCKS(1, 2)) {
        motion = &shaft_rotor_motion;
        frame = MOTION_FRAME(motion, SHAFT_ROTOR_SLOWDOWN);
        njSetTexture(&rotor_textures);
        njPushMatrixEx();
        object = &shaft_rotor;
        /* @hack spelling: &shaft_rotor in the first draw, where object would
         * do */
        njCnkEasyDrawMotion(&shaft_rotor, motion, frame);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawMotion(object, motion, frame);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawMotion(object, motion, frame);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawMotion(object, motion, frame);
        njPopMatrixEx();
    }
    njSetTexture(&poison_wall_textures);
    core_draw_sheet_at_children(&poison_wall_model, &poison_walls,
                                WALL_CELL_STEPS, LAND_BLOCKS(2, 3));
    i = game_state.steps % GLOW_TEXTURES;
    njSetTexture(&core_block_glow_texlist);
    core_draw_texture_children(&poison_surface, i, LAND_BLOCKS(2, 3));
    draw_control_set(DRAW_CONTROL_SHADOW);
    i = game_state.steps % WALL_FRAMES;
    njSetTexture(&inner_wall_textures);
    core_draw_at_children(inner_wall_frames[i], &inner_wall_shaft,
                          LAND_BLOCKS(1, 2));
    core_draw_at_children(inner_wall_frames[i], &inner_wall_entry,
                          LAND_BLOCK(1));
    /* @hack redundant: i taken again from game_state.steps, where the inner
     * walls' i would do */
    i = game_state.steps % WALL_FRAMES;
    njSetTexture(&middle_wall_textures);
    core_draw_at_children(middle_wall_frames[i], &middle_wall_shaft,
                          LAND_BLOCKS(1, 2));
    core_draw_at_children(middle_wall_frames[i], &middle_wall_lower,
                          LAND_BLOCKS(2, 3));
    /* @hack redundant: i taken again from game_state.steps, where the middle
     * walls' i would do */
    i = game_state.steps % WALL_FRAMES;
    njSetTexture(&outer_wall_textures);
    core_draw_at_children(outer_wall_frames[i], &outer_wall_shaft,
                          LAND_BLOCKS(1, 2));
    core_draw_at_children(outer_wall_frames[i], &outer_wall_lower,
                          LAND_BLOCKS(2, 3));
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njSetTexture(&lower_wall_textures);
    core_draw_sheet_at_children(&lower_wall_model, &lower_walls,
                                WALL_CELL_STEPS, LAND_BLOCKS(2, 3));
    i = game_state.steps % SURFACE_TEXTURES;
    njSetTexture(&surface_textures);
    core_draw_texture_object(&left_surface, i, LAND_BLOCK(1));
    core_draw_texture_object(&right_surface, i, LAND_BLOCK(1));
    core_draw_texture_object(surface_frames[game_state.steps % SURFACE_FRAMES],
                             i, LAND_BLOCK(1));
    core_draw_texture_object(&lower_surface, i, LAND_BLOCK(1));
    i = game_state.steps % MOVING_WATER_TEXTURES;
    njSetTexture(&moving_water_textures);
    texture_anim_apply(&moving_water_model, &moving_water_uv, game_state.steps);
    core_draw_texture_object(&moving_water, i, LAND_BLOCK(1));
    if (UNSIGNED_BLOCK_MASK & LAND_BLOCKS(1, 2)) {
        i = game_state.steps % ROTOR_WATER_FRAMES;
        njSetTexture(&rotor_water_textures);
        njCnkEasyDrawModel(rotor_water_frames[i]);
    }
}

/* CREATE_BUBBLE_TASK makes a bubble's task, BubbleExec, and
 * ALLOCATE_BUBBLE_RECORD its record at the task's extra. START_BUBBLE writes
 * the record without testing the allocation */
#ifdef NONMATCHING
/* this side allocates the record as it makes the task, and returns null where
 * either allocation fails */
static struct task *create_bubble_task(void)
{
    struct task *task =
        task_create(TASK_WORK, TASK_LEVEL, bubble_step, "BubbleExec");
    if (task == NULL) {
        return NULL;
    }
    task->extra.p = syMalloc(sizeof(struct bubble));
    if (task->extra.p == NULL) {
        task_destroy(task);
        return NULL;
    }
    return task;
}
#define CREATE_BUBBLE_TASK() create_bubble_task()
#define ALLOCATE_BUBBLE_RECORD(task)
#else
#define CREATE_BUBBLE_TASK()                                                   \
    task_create(TASK_WORK, TASK_LEVEL, bubble_step, "BubbleExec")
#define ALLOCATE_BUBBLE_RECORD(task)                                           \
    ((task)->extra.p = syMalloc(sizeof(struct bubble)))
#endif

/* start a bubble, as often as bubble_chance says, at y base_y and at x base_x
 * or -base_x. its z lies up to z_range toward -z from base_z, and it rises to
 * surface_y. the macro declares task, so it must open a block */
#define START_BUBBLE(base_x, base_y, base_z, z_range, surface_y)               \
    struct task *task;                                                         \
    if (rand() * RAND_SCALE < bubble_chance) {                                 \
        if ((task = CREATE_BUBBLE_TASK()) != NULL) {                           \
            task->draw = bubble_draw;                                          \
            task->destructor = bubble_destroy;                                 \
            ALLOCATE_BUBBLE_RECORD(task);                                      \
            /* @hack redundant: y and z stored in both branches, where one     \
             * store of each after the test would do */                        \
            if (rand() * RAND_SCALE < 0.5F) {                                  \
                task->work->pos.x = (base_x);                                  \
                task->work->pos.y = (base_y);                                  \
                task->work->pos.z =                                            \
                    (base_z) - (rand() * ((z_range) * RAND_SCALE));            \
            } else {                                                           \
                task->work->pos.x = -(base_x);                                 \
                task->work->pos.y = (base_y);                                  \
                task->work->pos.z =                                            \
                    (base_z) - (rand() * ((z_range) * RAND_SCALE));            \
            }                                                                  \
            BUBBLE(task)->unk_10 = BUBBLE(task)->unk_18 = 0.0F;                \
            BUBBLE(task)->rise = ((surface_y) - (base_y)) / bubble_lifetime;   \
            BUBBLE(task)->frame = 0;                                           \
            BUBBLE(task)->age = 0;                                             \
            BUBBLE(task)->last_step = game_state.steps;                        \
            BUBBLE(task)->surface = (surface_y);                               \
            BUBBLE(task)->scale =                                              \
                (rand() * bubble_scale_range * RAND_SCALE) + bubble_scale_min; \
        }                                                                      \
    }

/* START_BUBBLE's places for the lower and the upper bubbles */
#define LOWER_BUBBLE_X 135.0F
#define LOWER_BUBBLE_Y (-150.0F)
#define LOWER_BUBBLE_Z (-800.0F)
#define LOWER_BUBBLE_Z_RANGE 150.0F
#define LOWER_SURFACE_Y 25.0F
#define UPPER_BUBBLE_X 115.0F
#define UPPER_BUBBLE_Y (-190.0F)
#define UPPER_BUBBLE_Z (-495.0F)
#define UPPER_BUBBLE_Z_RANGE 110.0F
#define UPPER_SURFACE_Y 150.0F

static void lower_bubble_start(void)
{
    START_BUBBLE(LOWER_BUBBLE_X, LOWER_BUBBLE_Y, LOWER_BUBBLE_Z,
                 LOWER_BUBBLE_Z_RANGE, LOWER_SURFACE_Y)
}

static void upper_bubble_start(void)
{
    START_BUBBLE(UPPER_BUBBLE_X, UPPER_BUBBLE_Y, UPPER_BUBBLE_Z,
                 UPPER_BUBBLE_Z_RANGE, UPPER_SURFACE_Y)
}

static void bubble_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void bubble_step(struct task *task)
{
    if (BUBBLE(task)->last_step != game_state.steps) {
        if (game_state.steps & 1) {
            if (++BUBBLE(task)->frame >= BUBBLE_FRAMES) {
                BUBBLE(task)->frame = 0;
            }
        }
        task->work->pos.y += BUBBLE(task)->rise;
        BUBBLE(task)->last_step = game_state.steps;
        BUBBLE(task)->age++;
        if (task->work->pos.y > BUBBLE(task)->surface) {
            task_kill(task);
        }
    }
}

static void bubble_draw(struct task *task)
{
    NJS_VECTOR pos;
    Float scale;
    scale =
        BUBBLE(task)->scale *
        (1.0F + (njCos(BUBBLE(task)->age * bubble_pulse_rate) * bubble_pulse));
    njPushMatrixEx();
    njTranslateEx(&task->work->pos);
    njGetTranslation(NULL, &pos);
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njScale(NULL, scale, scale, scale);
    njSetTexture(&bubble_textures);
    njCnkEasyDrawModel(bubble_frames[BUBBLE(task)->frame]);
    njPopMatrixEx();
}

/* where the poison's and the fall's colliders stand, and the shapes each one's
 * collision holds */
#define POISON_X 0.0F
#define POISON_Y 133.0F
#define POISON_Z (-550.0F)
#define FALL_X 0.0F
#define FALL_Y 250.0F
#define FALL_Z (-243.0F)
#define COLLIDER_SHAPES 1

static void poison_step(struct task *task)
{
    collision_register(task);
}

static void poison_start(void)
{
    struct task *task =
        task_create(TASK_WORK, TASK_LEVEL, poison_step, "PoisonColi");
    if (task) {
        struct task_work *work = task->work;
        work->pos.x = POISON_X;
        work->pos.y = POISON_Y;
        work->pos.z = POISON_Z;
        work->ang.x = work->ang.y = work->ang.z = 0;
        collision_init(task, &poison_shape, COLLIDER_SHAPES, COLLISION_OBJECT);
    }
}

static void fall_step(struct task *task)
{
    if (game_state.time_stopped == 0) {
        collision_register(task);
    }
}

static void fall_start(void)
{
    struct task *task =
        task_create(TASK_WORK, TASK_LEVEL, fall_step, "FallColi");
    if (task) {
        struct task_work *work = task->work;
        work->pos.x = FALL_X;
        work->pos.y = FALL_Y;
        work->pos.z = FALL_Z;
        work->ang.x = work->ang.y = work->ang.z = 0;
        collision_init(task, &fall_shape, COLLIDER_SHAPES, COLLISION_OBJECT);
    }
}

/* the clear camera's submodes. the clear camera is BgAnimeExec */
enum {
    /* it draws the cover's panels while it waits for the clear switch to
     * reach SWITCH_DOWN */
    CLEAR_WAITING,
    /* with player 0's pad off and camera 0 in CAMERA_MODE_EASYSET, it shows
     * the splashes */
    CLEAR_SPLASH,
    /* it shows the water rising for water_rise_steps, then clears the stage
     * and takes camera 0's top mode off */
    CLEAR_RISING,
    CLEAR_DONE
};
#define CLEAR_VOICE 1758
#define VOICE_PRIORITY 0
/* CLEAR_SPLASH ends once SUBMODE_STEPS passes SPLASH_STEPS. each splash plays
 * from its start, a frame a step. the first splash shows its first frame until
 * its start */
#define SPLASH_STEPS 120
#define FIRST_SPLASH_START 30
#define SECOND_SPLASH_START 103
#define WATER_SPLASH_START 105
/* how far the rising water climbs over CLEAR_RISING */
#define RISE_HEIGHT 10.0F
/* look from pos at target in camera 0's CAMERA_MODE_EASYSET */
#define EASYSET_VIEW(pos_x, pos_y, pos_z, target_x, target_y, target_z)        \
    camera_easyset_states[0].pos.x = (pos_x);                                  \
    camera_easyset_states[0].pos.y = (pos_y);                                  \
    camera_easyset_states[0].pos.z = (pos_z);                                  \
    camera_easyset_states[0].target.x = (target_x);                            \
    camera_easyset_states[0].target.y = (target_y);                            \
    camera_easyset_states[0].target.z = (target_z)

void core_rouge_camera_start(void)
{
    struct task *task =
        task_create(TASK_WORK, TASK_LEVEL, clear_camera_step, "BgAnimeExec");
    if (task) {
        task->draw = clear_camera_draw;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it returns where task_create
     * fails */
    if (!task) {
        return;
    }
#endif
    /* @bug memory: writes SUBMODE_STEPS and the submode through a null task
     * where task_create fails. the test at 0x8c50168e guards the draw's store
     * alone, and the store at 0x8c50169c follows it */
    SUBMODE_STEPS(task) = 0;
    task->work->submode = CLEAR_WAITING;
    dat_8c5c5038 = 0;
}

static void clear_camera_step(struct task *task)
{
    /* @hack spelling: work for the first read of the submode, where task->work
     * would do */
    struct task_work *work = task->work;
    if (work->submode >= CLEAR_SPLASH) {
        sound_play_for(SHAFT_SOUND, SHAFT_KEY(task), SOUND_PRIORITY, 0,
                       SOUND_STEPS);
        sound_set_volume(SHAFT_KEY(task), SOUND_ANY_ID, shaft_volume);
        sound_play_for(WATER_SOUND, WATER_KEY(task), SOUND_PRIORITY, 0,
                       SOUND_STEPS);
        sound_set_volume(WATER_KEY(task), SOUND_ANY_ID, water_volume);
    }
    switch (task->work->submode) {
    case CLEAR_WAITING:
        if (core_clear_switch && core_clear_switch->extra.p &&
            CLEAR_SWITCH(core_clear_switch)->mode == SWITCH_DOWN) {
            SUBMODE_STEPS(task) = 0;
            task->work->submode++;
            pad_disable_player(0);
            camera_push_mode(0, CAMERA_MODE_EASYSET);
            camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
            EASYSET_VIEW(-205.0F, 385.0F, -30.0F, -270.0F, 215.0F, 25.0F);
            camera_easyset_states[0].type = CAMERA_POS_TARGET;
            voice_play(CLEAR_VOICE, VOICE_PRIORITY);
        }
        break;
    case CLEAR_SPLASH:
        if (++SUBMODE_STEPS(task) > SPLASH_STEPS) {
            SUBMODE_STEPS(task) = 0;
            task->work->submode++;
            EASYSET_VIEW(-25.0F, 35.0F, 170.0F, 80.0F, 110.0F, -5.0F);
        }
        break;
    case CLEAR_RISING:
        if (++SUBMODE_STEPS(task) >= water_rise_steps) {
            stage_clear(0);
            camera_remove_mode(0, camera_top(0));
            task->work->submode++;
        }
        break;
    case CLEAR_DONE:
        break;
    }
}

static void clear_camera_draw(struct task *task)
{
    int frame;
    switch (task->work->submode) {
    case CLEAR_WAITING:
        njSetTexture(&cover_panel_textures);
        core_draw_child_models(&cover_panel_positions, LAND_BLOCK(1));
        break;
    case CLEAR_SPLASH:
        /* @hack redundant: SUBMODE_STEPS tested against 0, which it never
         * falls below */
        if (SUBMODE_STEPS(task) >= 0 &&
            SUBMODE_STEPS(task) < FIRST_SPLASH_START) {
            njSetTexture(&splash_textures);
            njCnkEasyDrawModel(first_splash_frames[0]);
            /* @hack redundant: SUBMODE_STEPS tested against
             * FIRST_SPLASH_START, which the else holds already */
        } else if (SUBMODE_STEPS(task) >= FIRST_SPLASH_START &&
                   SUBMODE_STEPS(task) <=
                       FIRST_SPLASH_START + FIRST_SPLASH_FRAMES - 1) {
            njSetTexture(&splash_textures);
            njCnkEasyDrawModel(
                first_splash_frames[SUBMODE_STEPS(task) - FIRST_SPLASH_START]);
        } else if (SUBMODE_STEPS(task) >= SECOND_SPLASH_START &&
                   SUBMODE_STEPS(task) <=
                       SECOND_SPLASH_START + SECOND_SPLASH_FRAMES - 1) {
            njSetTexture(&splash_textures);
            njCnkEasyDrawModel(second_splash_frames[SUBMODE_STEPS(task) -
                                                    SECOND_SPLASH_START]);
        }
        if (SUBMODE_STEPS(task) >= WATER_SPLASH_START &&
            SUBMODE_STEPS(task) <=
                WATER_SPLASH_START + WATER_SPLASH_FRAMES - 1) {
            njSetTexture(&splash_water_textures);
            njCnkEasyDrawModel(
                water_splash_frames[SUBMODE_STEPS(task) - WATER_SPLASH_START]);
        }
        break;
    case CLEAR_RISING:
    default:
        njPushMatrixEx();
        frame = game_state.steps % ROTOR_WATER_FRAMES;
        njSetTexture(&rotor_water_textures);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawModel(rotor_water_frames[frame]);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawModel(rotor_water_frames[frame]);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawModel(rotor_water_frames[frame]);
        njPopMatrixEx();
        njPushMatrixEx();
        frame = game_state.steps % MOVING_WATER_TEXTURES;
        njTranslate(
            NULL, 0,
            ((Float)SUBMODE_STEPS(task) / water_rise_steps) * RISE_HEIGHT, 0);
        njSetTexture(&moving_water_textures);
        core_draw_texture_object(&rising_water, frame, LAND_BLOCK(1));
        njPopMatrixEx();
        break;
    }
}

/* the cover's submodes. the cover is BgCoverOpenExec */
enum {
    /* it draws the cover shut until switch COVER_SWITCH turns on */
    COVER_SHUT,
    /* with player 0's pad off and camera 0 in CAMERA_MODE_EASYSET, it plays
     * the cover's opening for cover_hold_steps. player 0's hit timer stays at
     * HOLD_HIT_TIMER. as the hold ends, the open rim's collision takes the
     * place of the rim's and the lid's */
    COVER_HOLD,
    /* it takes camera 0's top mode off */
    COVER_RELEASE,
    COVER_OPEN
};
#define COVER_SWITCH 4
#define COVER_MESSAGE 22
#define HOLD_HIT_TIMER 1
/* the land blocks in which the shut cover, the open cover and the ripples
 * draw */
#define COVER_BLOCKS LAND_BLOCKS(1, 2)
/* the steps of COVER_HOLD over which the cover's motion plays, a frame a
 * step */
#define MOTION_START 120
#define MOTION_END 150
/* the radii by which world_size_class_flags classes the collision objects */
#define RIM_RADIUS 54.1F
#define LID_RADIUS 80.0F
#define OPEN_RIM_RADIUS 80.0F
/* the cover's task keeps the rim's collision task at extra, and the lid's at
 * motion. each collision task keeps its object at its own extra */
#define RIM(task) ((task)->extra.task)
#define LID(task) ((task)->motion.task)
/* @hack type: unk_3c read as element 0 of a Float array through its address,
 * where (task)->unk_3c.f would do */
#define RADIUS(task) (((Float *)&(task)->unk_3c)[0])

static int cover_hold_steps = 480;
/* the current camera's place and target as the hold begins. camera 0's
 * CAMERA_MODE_EASYSET takes them for its last step */
static NJS_POINT3 saved_position;
static NJS_POINT3 saved_target;

static void cover_destroy(struct task *task);
static void cover_step(struct task *task);
static void cover_draw(struct task *task);
static void cover_draw_closed(void);

#ifdef NONMATCHING
/* this side's collision task for the cover, a child of parent. it puts its
 * own copy of shape in the world under flags and the size class of radius.
 * it returns the task, or null on a failure of task_create_child, of
 * world_pool_alloc or of world_add */
static struct task *create_cover_collision(struct task *parent,
                                           const NJS_OBJECT *shape,
                                           Float radius, Uint32 flags)
{
    struct task *child;
    NJS_OBJECT *object;

    child = task_create_child(TASK_WORK, core_cover_collision_step, parent);
    if (!child) {
        return NULL;
    }
    child->name = "BgColiExec";
    child->destructor = core_cover_collision_destroy;
    object = world_pool_alloc();
    if (!object) {
        return NULL;
    }
    *object = *shape;
    world_add(world_size_class_flags(radius) | flags, child, object);
    if (!(child->work->flags & WORLD_ADDED)) {
        world_pool_free(object);
        return NULL;
    }
    child->extra.p = object;
    return child;
}
#endif

void core_rouge_cover_start(void)
{
    struct task *task;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it tests the task
     * before it stores the destructor. where a collision task fails, it
     * destroys the cover's task */
    task = task_create(TASK_WORK, TASK_LEVEL, cover_step, "BgCoverOpenExec");
    if (!task) {
        return;
    }
    task->destructor = cover_destroy;
    task->extra.task = create_cover_collision(task, &cover_closed_rim_collision,
                                              RIM_RADIUS, WORLD_SOLID);
    if (!task->extra.task) {
        task_destroy(task);
        return;
    }
    task->motion.task = create_cover_collision(
        task, &cover_lid_collision, LID_RADIUS, WORLD_SOLID | WORLD_NO_CLIMB);
    if (!task->motion.task) {
        task_destroy(task);
        return;
    }
    task->draw = cover_draw;
#else
    NJS_OBJECT *object;
    Uint32 flags;

    task = task_create(TASK_WORK, TASK_LEVEL, cover_step, "BgCoverOpenExec");
    /* @bug memory: stores the destructor through a null task where
     * task_create fails. the store at 0x8c501b3e runs in the delay slot of
     * the branch at 0x8c501b3c that tests the task */
    task->destructor = cover_destroy;
    if (task) {
        task->draw = cover_draw;
        SUBMODE_STEPS(task) = 0;
        task->work->submode = COVER_SHUT;
        object = world_pool_alloc();
        task->extra.task = task_create(TASK_WORK, TASK_LEVEL,
                                       core_cover_collision_step, "BgColiExec");
        RIM(task)->destructor = core_cover_collision_destroy;
        RIM(task)->extra.p = object;
        /* @bug memory: writes the rim's and the lid's collision objects through
         * null where world_pool_alloc finds every object lent. the copies
         * start at 0x8c501b76 and 0x8c501bec. world_add reads through the
         * null too, from the calls at 0x8c501bc4 and 0x8c501c44, while the
         * world has room. core_cover_collision_destroy tests extra for that
         * null at 0x8c500654 */
        *object = cover_closed_rim_collision;
        RADIUS(RIM(task)) = RIM_RADIUS;
        flags = world_size_class_flags(RADIUS(RIM(task))) | WORLD_SOLID;
        world_add(flags, RIM(task), object);
        object = world_pool_alloc();
        task->motion.task = task_create(
            TASK_WORK, TASK_LEVEL, core_cover_collision_step, "BgColiExec");
        LID(task)->destructor = core_cover_collision_destroy;
        LID(task)->extra.p = object;
        *object = cover_lid_collision;
        RADIUS(LID(task)) = LID_RADIUS;
        flags = world_size_class_flags(RADIUS(LID(task))) |
                (WORLD_SOLID | WORLD_NO_CLIMB);
        world_add(flags, LID(task), object);
    }
#endif
}

#ifdef NONMATCHING
/* this side takes the cover's CAMERA_MODE_EASYSET off camera 0's stack at its
 * own index. work->timer keeps that index from the push. the matching side
 * takes off camera 0's top mode, whatever pushed it */
static void release_cover_camera(struct task *task)
{
    int slot = task->work->timer;

    if (slot > 0 && slot <= camera_top(0) &&
        camera_mode_at(0, slot) == CAMERA_MODE_EASYSET) {
        camera_remove_mode(0, slot);
    }
    task->work->timer = 0;
}
#endif

static void cover_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* where the cover dies in its hold, this side ends the hold as COVER_HOLD
     * does. it turns player 0's pad on, zeroes his hit timer and takes the
     * view back */
    if (task->work->submode == COVER_HOLD) {
        pad_enable_player(0);
        if (player_work[0]) {
            player_work[0]->timer = 0;
        }
        camera_easyset_states[0].pos = saved_position;
        camera_easyset_states[0].target = saved_target;
    }
    release_cover_camera(task);
#endif
    task->extra.task = NULL;
    task->motion.task = NULL;
}

static void cover_step(struct task *task)
{
    NJS_OBJECT *object;
    Uint32 flags;
    /* @hack spelling: work for the first read of the submode, where task->work
     * would do */
    struct task_work *work = task->work;

    switch (work->submode) {
    case COVER_SHUT:
        if (switch_is_on(COVER_SWITCH)) {
#ifdef NONMATCHING
            /* this side waits for player 0, the current camera and the push
             * of the cover's mode. it keeps the mode's index */
            if (!player_work[0] || !camera_views[camera_current_index] ||
                !camera_push_mode(0, CAMERA_MODE_EASYSET)) {
                break;
            }
            task->work->timer = camera_top(0);
#endif
            pad_disable_player(0);
            player_work[0]->timer = HOLD_HIT_TIMER;
            SUBMODE_STEPS(task) = 0;
            task->work->submode++;
            saved_position = camera_views[camera_current_index]->pos;
            saved_target = camera_views[camera_current_index]->target;
#ifdef NONMATCHING
            /* this side pushed the mode above */
            camera_set_adjust(0, task->work->timer, CAMERA_ADJUST_NONE);
#else
            camera_push_mode(0, CAMERA_MODE_EASYSET);
            camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
#endif
            EASYSET_VIEW(60.0F, 130.0F, 180.0F, 10.0F, 60.0F, 0.0F);
            camera_easyset_states[0].type = CAMERA_POS_TARGET;
            messenger_create(&task->work->pos, 0, 0, COVER_MESSAGE,
                             MESSENGER_TALK_WINDOW, NULL);
        }
        break;
    case COVER_HOLD:
        player_work[0]->timer = HOLD_HIT_TIMER;
        if (++SUBMODE_STEPS(task) >= cover_hold_steps) {
            pad_enable_player(0);
            player_work[0]->timer = 0;
            camera_easyset_states[0].pos = saved_position;
            camera_easyset_states[0].target = saved_target;
            task->work->submode++;
            world_remove(RIM(task), RIM(task)->extra.p);
#ifdef NONMATCHING
            /* this side keeps the rim's pool object for the open rim. the
             * matching side gives it back and borrows one again */
            object = RIM(task)->extra.p;
            *object = cover_open_rim_collision;
            flags = world_size_class_flags(OPEN_RIM_RADIUS) |
                    (WORLD_SOLID | WORLD_NO_CLIMB);
            task_kill(LID(task));
            task->motion.task = NULL;
#else
            world_pool_free(RIM(task)->extra.p);
            task_kill(LID(task));
            object = world_pool_alloc();
            RIM(task)->extra.p = object;
            /* @bug memory: writes the open rim's collision object through null
             * where the rim got none in core_rouge_cover_start and every
             * object is lent. the copy starts at 0x8c501e02, and world_add
             * reads through the null from the call at 0x8c501e5a while the
             * world has room. core_cover_collision_destroy tests extra for
             * that null at 0x8c500654 */
            *object = cover_open_rim_collision;
            RADIUS(RIM(task)) = OPEN_RIM_RADIUS;
            flags = world_size_class_flags(RADIUS(RIM(task))) |
                    (WORLD_SOLID | WORLD_NO_CLIMB);
#endif
            world_add(flags, RIM(task), object);
        }
        break;
    case COVER_RELEASE:
#ifdef NONMATCHING
        /* this side takes off the mode the cover pushed, as
         * release_cover_camera says */
        release_cover_camera(task);
#else
        camera_remove_mode(0, camera_top(0));
#endif
        task->work->submode++;
        break;
    case COVER_OPEN:
        break;
    }
}

static void cover_draw(struct task *task)
{
    int frame;
    switch (task->work->submode) {
    case COVER_SHUT:
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&cover_panel_textures);
        core_draw_child_models(&cover_panel_positions, LAND_BLOCK(1));
        draw_control_clear(DRAW_CONTROL_SHADOW);
        cover_draw_closed();
        break;
    case COVER_HOLD:
        njSetTexture(&cover_motion_textures);
        if (SUBMODE_STEPS(task) < MOTION_START) {
            njCnkEasyDrawMotion(cover_motion_object, cover_motion, 0.0F);
            /* @hack redundant: SUBMODE_STEPS tested against MOTION_START,
             * which the else holds already */
        } else if (SUBMODE_STEPS(task) >= MOTION_START &&
                   SUBMODE_STEPS(task) < MOTION_END) {
            njCnkEasyDrawMotion(cover_motion_object, cover_motion,
                                SUBMODE_STEPS(task) - MOTION_START);
        } else {
            njCnkEasyDrawMotion(cover_motion_object, cover_motion,
                                cover_motion->nbFrame - 1);
        }
        if (SUBMODE_STEPS(task) < COVER_RIPPLE_FRAMES) {
            frame = game_state.steps % COVER_RIPPLE_FRAMES;
            njSetTexture(&cover_ripple_textures);
            if (game_state.steps % (2 * COVER_RIPPLE_FRAMES) /
                COVER_RIPPLE_FRAMES) {
                core_draw_at_children(cover_ripple_frames_a[frame],
                                      &cover_ripple_positions_a, COVER_BLOCKS);
            } else {
                core_draw_at_children(cover_ripple_frames_b[frame],
                                      &cover_ripple_positions_b, COVER_BLOCKS);
            }
        }
        break;
    default:
        if (game_state.land_block_mask[0] & COVER_BLOCKS) {
            draw_control_set(DRAW_CONTROL_SHADOW);
            njSetTexture(&cover_open_textures);
            njCnkEasyDrawObject(&cover_open_object);
            draw_control_clear(DRAW_CONTROL_SHADOW);
        }
        break;
    }
}

static void cover_draw_closed(void)
{
    int frame;
    if (game_state.land_block_mask[0] & COVER_BLOCKS) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&cover_closed_textures);
        njCnkEasyDrawObject(&cover_closed_object);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
    frame = game_state.steps % COVER_RIPPLE_FRAMES;
    njSetTexture(&cover_ripple_textures);
    if (game_state.steps % (2 * COVER_RIPPLE_FRAMES) / COVER_RIPPLE_FRAMES) {
        core_draw_at_children(cover_ripple_frames_a[frame],
                              &cover_ripple_positions_a, COVER_BLOCKS);
    } else {
        core_draw_at_children(cover_ripple_frames_b[frame],
                              &cover_ripple_positions_b, COVER_BLOCKS);
    }
}

void core_draw_tinted_children(NJS_CNK_OBJECT *root, Uint32 color)
{
    NJS_CNK_OBJECT *child;
    for (child = root->child; child; child = child->sibling) {
        model_draw_tinted(child->model, color);
    }
}
