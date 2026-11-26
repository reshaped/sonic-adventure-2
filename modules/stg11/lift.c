/* the lift, MLIFT in Iron Gate's object list. it is Prison Lane's lift of
 * stg09/lift.c, with a collision box under its top */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg11/lift.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "stg11/lift/text.inc"

/* the top's end, in the work's mode. while it moves, the mode is the end it
 * left */
enum lift_mode { MODE_DOWN = 1, MODE_UP };

/* in the work's submode: the top rests, moves to its other end, or never
 * moves */
#define LIFT_RESTING 0
#define LIFT_MOVING 1
#define LIFT_HELD 2
/* in the placement's state: the top is up, and it never moves */
#define STATE_UP 0x1
#define STATE_HELD 0x2

/* the resting steps since a player last stood on the top, in the work's
 * tick. the count also restarts as the top begins to move. iron_gate_lift_init
 * sets it to IDLE_STEPS. so an up top with no player on it sinks at its first
 * step */
#define IDLE(work) ((work)->tick)
/* the down top rises once a player has stood on it for more than RIDE_STEPS
 * in a row. the up top sinks once no player has for more than IDLE_STEPS. a
 * landing on the down top after more than LAND_IDLE_STEPS idle steps raises
 * dust and jolts the spring */
#define RIDE_STEPS 30
#define IDLE_STEPS 90
#define LAND_IDLE_STEPS 30
#define RIDE_COUNT_MAX 10000
/* the record's up_end lies scl.x + RISE_EXTRA above its base. the top and the
 * world's copy lie TOP_HEIGHT above the work's pos plus the bob */
#define RISE_EXTRA 20.0F
#define TOP_HEIGHT 3.0F
/* the spring's speed is set to START_KICK as the top starts, and to STOP_KICK
 * as it stops. it is set to LAND_KICK as a player lands */
#define START_KICK (-0.15F)
#define LAND_KICK (-0.08F)
#define STOP_KICK 0.3F
#define SPRING_DAMPING 0.94F
#define SPRING_STIFFNESS 0.5F
#define WAVES 4

/* the dust lift_dust raises. its scale is DUST_SCALE as the top starts or
 * stops, and LAND_DUST_SCALE as a player lands */
#define DUST_SCALE 7.0F
#define LAND_DUST_SCALE 3.0F
#define DUST_DENSITY 0.7F
#define DUST_HALF_SIZE 18.0F
#define DUST_SPEED 0.1F
#define DUST_HEIGHT 3.0F

#define MOVE_SOUND 0x1020
#define STOP_SOUND 0x1021
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 100
/* each _UPDATES is sound_play_at_for's duration. a play lasts its duration
 * + 1 sound updates. each step of a move plays the move sound again. its
 * duration is RISE_UPDATES as the top rises, and SINK_UPDATES as it sinks. so
 * the sinking top's move sound runs on for SINK_UPDATES + 1 updates after it
 * stops */
#define RISE_UPDATES 3
#define SINK_UPDATES 60
#define STOP_UPDATES 60
/* the down top's start rumbles player 0's pad */
#define VIBRATION_PLAYER 0
#define VIBRATION_POWER 2
#define VIBRATION_FREQ 15
#define VIBRATION_TIME 2

/* the world's copy collides while a player lies within its model's radius
 * plus REACH_MARGIN of the model's center. that reach also picks its
 * WORLD_SIZE_CLASS */
#define REACH_MARGIN 15.0F
/* the slots below RIDERS hold the players whose standing on the top
 * counts */
#define RIDERS 2
/* TASK_FORCE's records, by which the top carries the players */
#define FORCES(task) ((struct task_force *)(task)->force.p)
/* the collision record's box, which each step stretches from the work's pos
 * to BOX_HEIGHT above the bob */
#define BOX_HEIGHT 1.5F
#define LIFT_SHAPES 1

/* a term of the waves' sum: amp times the sine of freq times the step */
struct lift_wave {
    Sint32 freq;
    Float amp;
};

/* the lift's record, in task->motion */
struct lift {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_POINT3 up_end; /* the work's pos at the top's up end */
    NJS_POINT3 base; /* the base's pos, and the work's at the down end */
    Float bob; /* the waves' sum plus the spring */
    Float spring_spd;
    Float spring;
    /* the resting steps a player has stood on the top in a row, up to
     * RIDE_COUNT_MAX */
    Sint32 ride_steps;
};

#define LIFT(task) ((struct lift *)(task)->motion.p)

static NJS_TEXLIST lift_texlist;
/* the base's node, whose child is the top */
static NJS_CNK_OBJECT lift_object;
/* the top's collision object, a copy of which each lift puts in the world */
static NJS_OBJECT lift_collision;

static struct lift_wave lift_waves[WAVES];
static Float lift_speed;
static struct collision_shape lift_shapes[LIFT_SHAPES];

static void lift_destroy(struct task *task);
static void lift_step(struct task *task);
static void lift_draw(struct task *task);

/* the sum of count terms of waves at step. only step's low 16 bits count */
/* @hack type: step an int narrowed in the body, where a Uint16 parameter
 * would do */
static Float lift_wave_sum(struct lift_wave *waves, int count, int step)
{
    Float sum = 0.0F;
    Uint32 at = (Uint16)step;

    while (count--) {
        sum += njSin(waves->freq * at) * waves->amp;
        waves++;
    }
    return sum;
}

/* raise dust along the edges of a square, 2 * DUST_HALF_SIZE on a side. the
 * square lies DUST_HEIGHT above the work's pos, turned with the lift. scale
 * and density are dust_short_rect's */
static void lift_dust(struct task *task, Float scale, Float density)
{
    struct task_work *work = task->work;
    struct angles ang;
    NJS_POINT3 center;

    center.x = 0.0F;
    center.y = DUST_HEIGHT;
    center.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &center, &center);
    njPopMatrix(1);
    ang.x = 0;
    ang.y = work->ang.y;
    ang.z = 0;
    dust_short_rect(&center, DUST_HALF_SIZE, DUST_HALF_SIZE, &ang, scale,
                    density, DUST_SPEED);
}

void iron_gate_lift_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_VECTOR rise;
    Uint32 flags;
    Float radius;
    Sint8 state;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct lift));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = lift_draw;
    task->step = lift_step;
    task->destructor = lift_destroy;
    work->mode = MODE_DOWN;
    LIFT(task)->base = work->pos;
    IDLE(work) = IDLE_STEPS;
    LIFT(task)->ride_steps = 0;
    rise.x = 0.0F;
    rise.y = work->scl.x + RISE_EXTRA;
    rise.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &LIFT(task)->base);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &rise, &LIFT(task)->up_end);
    njPopMatrix(1);
    if (task->placement != NULL) {
        state = placement_get_state(task);
        if (state & STATE_UP) {
            work->mode = MODE_UP;
        } else {
            work->mode = MODE_DOWN;
        }
        if (state & STATE_HELD) {
            work->submode = LIFT_HELD;
        }
    } else {
        work->mode = MODE_DOWN;
    }
    if (work->mode == MODE_UP) {
        work->pos = LIFT(task)->up_end;
    } else {
        work->pos = LIFT(task)->base;
    }
    object = &LIFT(task)->object;
    *object = lift_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_MOVING;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    work->ang.x = 0;
    work->ang.z = 0;
    collision_init(task, lift_shapes, LIFT_SHAPES, COLLISION_OBJECT);
}

static void lift_destroy(struct task *task)
{
    struct task_work *work = task->work;

    /* @bug logic: the destructor sets STATE_UP where the top is down
     * (0x8c5041c8), and iron_gate_lift_init reads it as up (0x8c50403c). so
     * the placement's next task starts the top at its other end */
    if (task->placement != NULL) {
        placement_set_state(task,
                            (work->submode == LIFT_HELD ? STATE_HELD : 0) |
                                (work->mode == MODE_DOWN ? STATE_UP : 0));
    }
    if (LIFT(task)->object.model != NULL) {
        world_remove(task, &LIFT(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void lift_step(struct task *task)
{
    struct task_work *work = task->work;
    Bool standing;
    NJS_POINT3 last;
    NJS_VECTOR way;
    Float last_bob;
    Float share;
    int i;

    /* @hack redundant: an empty else after the return */
    if (placement_try_leave(task)) {
        return;
    } else {
    }
    last = work->pos;
    last_bob = LIFT(task)->bob;
    if (work->submode == LIFT_MOVING) {
        /* @hack spelling: whole, a constant for 1, where 1.0F in its place
         * would do */
        const Float whole = 1.0F;

        switch (work->mode) {
        case MODE_DOWN:
            way.x = LIFT(task)->up_end.x - work->pos.x;
            way.y = LIFT(task)->up_end.y - work->pos.y;
            way.z = LIFT(task)->up_end.z - work->pos.z;
            share = njScalor(&way);
            if (share > lift_speed) {
                share = lift_speed / share;
            } else {
                share = whole;
            }
            /* @hack redundant: a block no path reaches, which sets a share
             * of 1 to 1 */
            if (0) {
                if (share == whole) {
                    share = whole;
                }
            }
            way.x *= share;
            way.y *= share;
            way.z *= share;
            work->pos.x += way.x;
            work->pos.y += way.y;
            work->pos.z += way.z;
            if (share == whole) {
                lift_dust(task, DUST_SCALE, DUST_DENSITY);
                work->submode = LIFT_RESTING;
                work->mode = MODE_UP;
                sound_play_at_for(STOP_SOUND, work, SOUND_PRIORITY,
                                  SOUND_VOLUME, STOP_UPDATES, &work->pos);
                LIFT(task)->spring_spd = STOP_KICK;
            } else {
                sound_play_at_for(MOVE_SOUND, work, SOUND_PRIORITY,
                                  SOUND_VOLUME, RISE_UPDATES, &work->pos);
            }
            break;
        case MODE_UP:
            way.x = LIFT(task)->base.x - work->pos.x;
            way.y = LIFT(task)->base.y - work->pos.y;
            way.z = LIFT(task)->base.z - work->pos.z;
            share = njScalor(&way);
            if (share > lift_speed) {
                share = lift_speed / share;
            } else {
                share = whole;
            }
            /* @hack redundant: a block no path reaches, which sets a share
             * of 1 to 1 */
            if (0) {
                if (share == whole) {
                    share = whole;
                }
            }
            way.x *= share;
            way.y *= share;
            way.z *= share;
            work->pos.x += way.x;
            work->pos.y += way.y;
            work->pos.z += way.z;
            if (share == whole) {
                lift_dust(task, DUST_SCALE, DUST_DENSITY);
                work->submode = LIFT_RESTING;
                work->mode = MODE_DOWN;
                sound_play_at_for(STOP_SOUND, work, SOUND_PRIORITY,
                                  SOUND_VOLUME, STOP_UPDATES, &work->pos);
                LIFT(task)->spring_spd = STOP_KICK;
            } else {
                sound_play_at_for(MOVE_SOUND, work, SOUND_PRIORITY,
                                  SOUND_VOLUME, SINK_UPDATES, &work->pos);
            }
            break;
        }
    } else if (work->submode == LIFT_RESTING) {
        standing = FALSE;
        for (i = 0; i < RIDERS && player_work[i] != NULL; i++) {
            if (player_on_task(i, task)) {
                standing = TRUE;
                break;
            }
        }
        if (standing) {
            if (LIFT(task)->ride_steps < RIDE_COUNT_MAX) {
                LIFT(task)->ride_steps++;
            }
            if (LIFT(task)->ride_steps == 1 && IDLE(work) > LAND_IDLE_STEPS &&
                work->mode == MODE_DOWN) {
                lift_dust(task, LAND_DUST_SCALE, DUST_DENSITY);
                LIFT(task)->spring_spd = LAND_KICK;
            }
            IDLE(work) = 0;
        } else {
            LIFT(task)->ride_steps = 0;
            IDLE(work)++;
        }
        if ((work->mode != MODE_DOWN && IDLE(work) > IDLE_STEPS) ||
            (work->mode == MODE_DOWN && LIFT(task)->ride_steps > RIDE_STEPS)) {
            lift_dust(task, DUST_SCALE, DUST_DENSITY);
            work->submode = LIFT_MOVING;
            LIFT(task)->spring_spd = START_KICK;
            if (work->mode == MODE_DOWN) {
                vibration_start(VIBRATION_PLAYER, VIBRATION_POWER,
                                VIBRATION_FREQ, VIBRATION_TIME);
            }
            LIFT(task)->ride_steps = 0;
            IDLE(work) = 0;
        }
    }
    if (work->submode != LIFT_RESTING) {
        work->timer++;
    }
    LIFT(task)->bob = lift_wave_sum(lift_waves, WAVES, work->timer);
    LIFT(task)->spring_spd = -(LIFT(task)->spring * SPRING_STIFFNESS) +
                             (LIFT(task)->spring_spd * SPRING_DAMPING);
    LIFT(task)->spring += LIFT(task)->spring_spd;
    LIFT(task)->bob += LIFT(task)->spring;
    for (i = 0; i < TASK_FORCES && player_work[i] != NULL; i++) {
        FORCES(task)[i].speed.x = work->pos.x - last.x;
        FORCES(task)
        [i].speed.y = ((work->pos.y - last.y) + LIFT(task)->bob) - last_bob;
        FORCES(task)[i].speed.z = work->pos.z - last.z;
    }
    LIFT(task)->object.pos[0] = work->pos.x;
    LIFT(task)->object.pos[1] = work->pos.y + LIFT(task)->bob + TOP_HEIGHT;
    LIFT(task)->object.pos[2] = work->pos.z;
    if (LIFT(task)->object.model != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, (NJS_VECTOR *)LIFT(task)->object.pos);
        njRotateY(NULL, LIFT(task)->object.ang[1]);
        njCalcPoint(NULL, &LIFT(task)->object.model->center, &way);
        njPopMatrix(1);
        if (player_near(&way, LIFT(task)->object.model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        if (work->collision != NULL) {
            work->collision->shapes->center.y =
                (LIFT(task)->bob + BOX_HEIGHT) * 0.5F;
            work->collision->shapes->size_y =
                (LIFT(task)->bob + BOX_HEIGHT) * 0.5F;
            collision_fit_radius(task);
        }
        collision_register(task);
    }
}

static void lift_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *top;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- HEIGHT";
    }
    njSetTexture(&lift_texlist);
    object = &lift_object;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslate(NULL, LIFT(task)->base.x, LIFT(task)->base.y,
                LIFT(task)->base.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
    top = object->child;
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + LIFT(task)->bob + TOP_HEIGHT,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(top->model);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#include "shared/prison_fixtures/lift_textures.h"
#include "stg11/fixtures/lift_platform.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/prison_fixtures/lift_base.h"

NJS_TEXLIST *iron_gate_lift_texlists[3] = {
#include "stg11/lift/iron_gate_lift_texlists.inc"
};
static struct lift_wave lift_waves[WAVES] = {
    {12432, 0.1F},
    {3728, 0.14F},
    {240, 0.15F},
    {19, 0.3F},
};
/* how far the top moves a step, in units */
static Float lift_speed = 0.5F;
/* clang-format off */
static struct collision_shape lift_shapes[LIFT_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_HURTABLE, {0.0F, 0.5F, 0.0F}, 19.0F, 0.5F, 19.0F, 0, {0, 0, 0}},
};
/* clang-format on */
