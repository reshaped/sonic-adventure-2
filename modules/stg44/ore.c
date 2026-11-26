/* the ore, ORE in Mad Space's object list: a rock that a player's attacks
 * crack */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg44/ore.h"
#include "stg44/scenery_shapes.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a rock's shapes: the whole rock's, and the smaller one the crack leaves. the
 * init turns the whole one on and the cracked one off, and the crack swaps
 * them */
#define ORE_SHAPES 2
#define WHOLE_SHAPE 0
#define CRACKED_SHAPE 1
#define SMALL(work) ((work)->scl.x >= 1.0F)
/* the work's submode counts the rock's hits up to CRACKED. a small rock's
 * first hit sets CRACKED at once */
#define DAMAGE(work) ((work)->submode)
#define CRACKED 2
/* each attack that touches the rock sets the work's tick to HIT_COOLDOWN.
 * each step without one counts the tick down. an attack hits only while the
 * tick is 0 */
#define HIT_COOLDOWN 15
/* the rock sways about x. scl.y holds its tilt in degrees, and scl.z the
 * tilt's speed a step */
#define TILT(work) ((work)->scl.y)
#define TILT_SPEED(work) ((work)->scl.z)
#define HIT_SWAY 2.0F
#define SWAY_PULL 0.62F
#define SWAY_DAMPING 0.93F
/* the rock's tint runs from cool_color to warm_color and back with the sine of
 * game_state.steps * its rate. the rate is SMALL_PULSE_RATE for a small rock,
 * and LARGE_PULSE_RATE for a large one */
#define SMALL_PULSE_RATE 0x230
#define LARGE_PULSE_RATE 0x200
/* object_sound_play's kinds for a hit and for the crack */
#define HIT_SOUND_KIND 2
#define CRACK_SOUND_KIND 3
#define CRACK_SCORE 20
/* the crack throws a column of CHIPS_PER_RAY chips along each of RAYS
 * headings. row n of a column starts n * ROW_SPACING of the whole shape's
 * size_y up. it starts on a ring of RING_SCALE of the shape's radius. the
 * row's speed up is
 * (n - LEVEL_ROW) * ROW_RISE, plus up to RISE_SPREAD. then each part of a
 * chip's speed takes a scale from SPEED_LEAST up to SPEED_LEAST plus
 * SPEED_SPREAD */
#define RAYS 3
#define CHIPS_PER_RAY 5
#define RING_SCALE 0.6F
#define ROW_SPACING 0.2F
#define LEVEL_ROW 1.5F
#define ROW_RISE 0.5F
#define RISE_SPREAD 0.3F
#define SPEED_LEAST 0.5F
#define SPEED_SPREAD 0.8F
#define CHIP_FLAGS (DEBRIS_FADE_COLORS | DEBRIS_FIELD)
#define CHIP_FLOOR (-100000.0F)

NJS_TEXLIST *mad_space_ore_textures[] = {
    &mad_space_ore_large_texlist,         &mad_space_ore_small_texlist,
    &mad_space_ore_large_cracked_texlist, &mad_space_ore_small_cracked_texlist,
    &mad_space_ore_first_chip_texlist,    &mad_space_ore_second_chip_texlist,
    &mad_space_ore_third_chip_texlist,    NULL,
};
/* clang-format off */
static struct collision_shape large_shapes[ORE_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_HURTABLE,
     {0, 25.0F, 0}, 20.0F, 25.0F, 0, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_HURTABLE,
     {0, 10.0F, 0}, 10.0F, 10.0F, 0, 0, {0, 0, 0}},
};
static struct collision_shape small_shapes[ORE_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_HURTABLE,
     {0, 12.5F, 0}, 12.5F, 12.5F, 0, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_HURTABLE,
     {0, 6.5F, 0}, 12.5F, 6.5F, 0, 0, {0, 0, 0}},
};
/* clang-format on */
static Uint32 cool_color = 0xffd0e0ff;
static Uint32 warm_color = 0xffffe0d0;

static void ore_destroy(struct task *task);
static void ore_draw(struct task *task);
static void ore_step(struct task *task);

void mad_space_ore_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = ore_step;
    task->draw = ore_draw;
    task->destructor = ore_destroy;
    if (SMALL(work)) {
        collision_init(task, small_shapes, ORE_SHAPES, COLLISION_OBJECT);
    } else {
        collision_init(task, large_shapes, ORE_SHAPES, COLLISION_OBJECT);
    }
    /* @bug memory: reads the record's shape list through null where
     * collision_init gets no record from the heap (MOV.L at 0x8c5050c2). the
     * crack tests the record before it turns the shapes (TST at 0x8c5053ce) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where collision_init got no record, the
     * init destroys the rock, and set.c makes it again on a later pass. it
     * clears the destructor first, so the placement keeps its damage:
     * ore_destroy would save a DAMAGE the init hasn't read yet */
    if (work->collision == NULL) {
        task->destructor = NULL;
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes[WHOLE_SHAPE].flags &= ~SHAPE_OFF;
    work->collision->shapes[CRACKED_SHAPE].flags |= SHAPE_OFF;
    /* @bug logic: a rock that comes back cracked draws cracked, but collides
     * as the whole rock. the init turns the whole shape on and the cracked one
     * off (at 0x8c5050c8 and 0x8c5050d4) before it reads the hits back (MOV.B
     * at 0x8c5050e2). only the crack swaps the shapes */
    if (task->placement) {
        DAMAGE(work) = placement_get_state(task);
    }
    TILT(work) = 0.0F;
    TILT_SPEED(work) = 0.0F;
}

static void ore_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement) {
        placement_set_state(task, DAMAGE(work));
    }
}

#define ORE_PULSE(rate)                                                        \
    color_blend(cool_color, warm_color,                                        \
                (njSin(game_state.steps * (rate)) + 1.0F) * 0.5F)

static void ore_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- MODEL";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (TILT(work) != 0.0F) {
        njRotateX(NULL, (Angle)(TILT(work) * ANGLE_PER_DEGREE));
    }
    if (SMALL(work)) {
        if (DAMAGE(work) == CRACKED) {
            njSetTexture(&mad_space_ore_small_cracked_texlist);
            model_draw_tinted(mad_space_ore_small_cracked_object->model,
                              ORE_PULSE(SMALL_PULSE_RATE));
        } else {
            njSetTexture(&mad_space_ore_small_texlist);
            model_draw_tinted(mad_space_ore_small_object->model,
                              ORE_PULSE(SMALL_PULSE_RATE));
        }
    } else if (DAMAGE(work) == CRACKED) {
        njSetTexture(&mad_space_ore_large_cracked_texlist);
        model_draw_tinted(mad_space_ore_large_cracked_object->model,
                          ORE_PULSE(LARGE_PULSE_RATE));
    } else {
        njSetTexture(&mad_space_ore_large_texlist);
        model_draw_tinted(mad_space_ore_large_object->model,
                          ORE_PULSE(LARGE_PULSE_RATE));
    }
    njPopMatrixEx();
}

/* launch a chip of object and textures from ore_step's pos, turn and
 * velocity */
#define LAUNCH_CHIP(object, textures)                                          \
    debris_launch_built_flags(&pos, &turn, CHIP_FLOOR, &velocity,              \
                              (object)->model, NULL, &(textures), CHIP_FLAGS)

static void ore_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR velocity;
    struct angles turn;
    Float *axis;
    Float radius;
    Float half_height;
    Bool attacked;
    int hits;
    int ray;
    int row;
    Angle heading;

    if (placement_try_leave(task)) {
        return;
    }
    hits = collision_get_hit_count(task);
    attacked = FALSE;
    while (hits--) {
        if (player_attacks_task(task, collision_get_hit(task, hits)->task)) {
            attacked = TRUE;
            break;
        }
    }
    if (!work->tick && attacked && DAMAGE(work) < CRACKED) {
        TILT_SPEED(work) = HIT_SWAY;
        DAMAGE(work)++;
        if (SMALL(work)) {
            DAMAGE(work) = CRACKED;
        }
        if (DAMAGE(work) == CRACKED) {
            if (work->collision) {
                work->collision->shapes[CRACKED_SHAPE].flags &= ~SHAPE_OFF;
                work->collision->shapes[WHOLE_SHAPE].flags |= SHAPE_OFF;
            }
            object_sound_play(CRACK_SOUND_KIND, &work->pos);
            score_add(CRACK_SCORE);
            half_height = work->collision->shapes[WHOLE_SHAPE].size_y;
            radius = work->collision->shapes[WHOLE_SHAPE].radius * RING_SCALE;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
            for (ray = 0; ray < RAYS; ray++) {
                heading = ray * (FULL_TURN / RAYS);
                for (row = 0; row < CHIPS_PER_RAY; row++) {
                    velocity.x = njSin(heading);
                    velocity.z = njCos(heading);
                    velocity.y = ((row - LEVEL_ROW) * ROW_RISE) +
                                 (rand() * (RISE_SPREAD * RAND_SCALE));
                    pos.x = velocity.x * radius;
                    pos.y = half_height * row * ROW_SPACING;
                    pos.z = velocity.z * radius;
                    turn.x = 0;
                    turn.y = work->ang.y + heading;
                    turn.z = 0;
                    njCalcPoint(NULL, &pos, &pos);
                    njCalcVector(NULL, &velocity, &velocity);
                    /* @hack spelling: the three scales through axis, a
                     * pointer stepped along velocity, where velocity.x, .y
                     * and .z by name would do */
                    axis = &velocity.x;
                    *axis++ *=
                        (rand() * (SPEED_SPREAD * RAND_SCALE)) + SPEED_LEAST;
                    *axis++ *=
                        (rand() * (SPEED_SPREAD * RAND_SCALE)) + SPEED_LEAST;
                    *axis *=
                        (rand() * (SPEED_SPREAD * RAND_SCALE)) + SPEED_LEAST;
                    if (SMALL(work)) {
                        if ((ray + row) & 1) {
                            LAUNCH_CHIP(mad_space_ore_second_chip,
                                        mad_space_ore_second_chip_texlist);
                        } else {
                            LAUNCH_CHIP(mad_space_ore_third_chip,
                                        mad_space_ore_third_chip_texlist);
                        }
                    } else if ((ray + row) & 1) {
                        LAUNCH_CHIP(mad_space_ore_first_chip,
                                    mad_space_ore_first_chip_texlist);
                    } else {
                        LAUNCH_CHIP(mad_space_ore_second_chip,
                                    mad_space_ore_second_chip_texlist);
                    }
                }
                /* @hack redundant: a test of row, in a block no path
                 * reaches */
                if (0) {
                    if (row < CHIPS_PER_RAY) {
                    }
                }
            }
            njPopMatrixEx();
        } else {
            object_sound_play(HIT_SOUND_KIND, &work->pos);
        }
    }
    if (attacked) {
        work->tick = HIT_COOLDOWN;
    } else if (work->tick) {
        work->tick--;
    }
    TILT(work) += TILT_SPEED(work);
    TILT_SPEED(work) =
        (-TILT(work) * SWAY_PULL) + (TILT_SPEED(work) * SWAY_DAMPING);
    collision_register(task);
}
