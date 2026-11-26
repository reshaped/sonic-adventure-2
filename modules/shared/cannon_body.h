/* MUJINGUN, the cannon that stg24/reactor.c and stg36/cannon.c each compile,
 * and its homing shots. stg24/reactor.h and stg36/cannon.h give its
 * placement.
 *
 * the includer defines CANNON_INIT as the init's name, and SHOT_HIT and
 * SHOT_FLAGS as a shot's levels and flags. it defines SHOT_CONTROLLER, a
 * pointer. a shot whose submode is CONTROLLED_SHOT dies once SHOT_CONTROLLER
 * is null. ENGINE_REACTOR selects Eternal Engine's cannon, which fires while
 * time stands still. that cannon adds turret_shot_create, which makes
 * controlled shots */
#ifndef SHARED_CANNON_BODY_H
#define SHARED_CANNON_BODY_H
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/trail.h"
#include "effect/burst.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/white_breath.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/cannon_parts.h"

/* a shot's state, at its task's extra */
struct cannon_shot {
    Float homing; /* how hard the shot steers, from 0 to 1 */
    /* the shot dies early once player 0 lies farther than range from origin */
    NJS_POINT3 origin;
    Float range;
};
#define SHOT(task) ((struct cannon_shot *)(task)->extra.p)
#define VELOCITY(work) ((work)->scl)
#define HEADING(work) ((work)->timer)
/* the barrel's height above pos as it bobs */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define BOB(task) (*(Float *)&(task)->unk_3c)
/* a shot's submode where SHOT_CONTROLLER keeps it alive */
#define CONTROLLED_SHOT 1

/* a shot's timer counts its steps. the shot dies past SHOT_LIFETIME, and
 * blinks past SHOT_BLINK_AGE. while player 0 is out of range, the timer jumps
 * to SHOT_OUT_OF_RANGE_AGE where it is below it */
#define SHOT_LIFETIME 1200
#define SHOT_OUT_OF_RANGE_AGE 1020
#define SHOT_BLINK_AGE 1080
/* a shot that dies leaves a puff, and one that hits leaves a burst. each
 * takes its share of the shot's speed, at scale SHOT_PUFF_SCALE */
#define PUFF_SPEED 0.3F
#define BURST_SPEED 0.05F
#define SHOT_PUFF_SCALE 3.0F
/* a shot draws bright past SHOT_LAST_DIM of each SHOT_FLASH_PERIOD steps.
 * past SHOT_BLINK_AGE it draws only on steps whose SHOT_BLINK bit is clear.
 * its SHOT_SPARKS sparks draw at SHOT_SPARK_SCALE plus its homing */
#define SHOT_FLASH_PERIOD 50
#define SHOT_LAST_DIM 30
#define SHOT_BLINK 2
#define SHOT_SPARKS 4
#define SHOT_SPARK_SCALE 0.1F
#define SHOT_TRAIL_POINTS 12
/* Eternal Engine's shots take ENGINE_SHOT_RADIUS and ENGINE_SHOT_SOUND. a
 * turret's shot has TURRET_SHOT_RANGE about its muzzle as its range */
#define ENGINE_SHOT_RADIUS 5.0F
#define ENGINE_SHOT_SOUND 22
#define ENGINE_SHOT_SOUND_STEPS 5
#define TURRET_SHOT_RANGE 600.0F
/* a shot's homing gains HOMING_GAIN a step, to 1. it spins SHOT_SPIN a step
 * at full homing */
#define HOMING_GAIN 0.02F
#define SHOT_SPIN (12.0F * ANGLE_PER_DEGREE)
/* a shot steers toward a point AIM_HEIGHT above the player that player_near
 * finds within HOMING_RANGE. it stops steering within STEER_LEAST of it */
#define HOMING_RANGE 500.0F
#define AIM_HEIGHT 5.0F
#define STEER_LEAST 15.0F
#define SHOT_DRAG 0.998F

/* the init holds scl.x, the cannon's fire rate, to MAX_FIRE */
#define MAX_FIRE 100.0F
/* scl.z's whole part is the range. its fraction times ORIGIN_SCALE is how far
 * along the cannon's -x the range's center lies */
#define ORIGIN_SCALE 1000.0F
/* a step's chance to fire, in percent, is scl.x times FIRE_PERCENT */
#define FIRE_PERCENT 0.3F
/* the barrel turns TURN_SHARE of the way toward player 0 a step, and
 * MAX_TURN Angle units at most */
#define TURN_SHARE 0.3F
#define MAX_TURN 128
/* the barrel bobs BOB_HEIGHT each way. its sine advances BOB_RATE Angle units
 * a step */
#define BOB_RATE 528
#define BOB_HEIGHT 3.5F
/* a shot leaves MUZZLE_REACH along the barrel's -x, at scl.y +
 * SHOT_LEAST_SPEED a step */
#define MUZZLE_REACH 20.0F
#define SHOT_LEAST_SPEED 0.5F
/* the barrel's texture animation advances BARREL_ANIM_RATE a step, in
 * texture_anim_apply's time */
#define BARREL_ANIM_RATE 5

static Float shot_pull = 0.01F;
static Float shot_gravity = 0.0005F;
/* clang-format off */
static struct collision_shape shot_shape[] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHOT_HIT, SHOT_FLAGS,
     {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static struct trail_style shot_trail = {
    TRAIL_SQUARE | TRAIL_ODD_STEPS, 2.2F, 1.6F, 0x80e080ff, 0x08ff9080, NULL, 0,
};
/* clang-format off */
static struct collision_shape cannon_shape[] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void shot_step(struct task *task);
static void shot_destroy(struct task *task);
static void shot_draw(struct task *task);

#ifdef ENGINE_REACTOR
static struct task *turret_shot_create(NJS_POINT3 *pos, NJS_VECTOR *speed)
{
    struct task *task;
    struct task_work *work;
    task = task_create(TASK_WORK, 3, shot_step, "DrrTamaExec");
    if (task) {
        task->extra.p = syCalloc(1, sizeof(struct cannon_shot));
        if (!task->extra.p) {
            task_destroy(task);
            task = NULL;
        }
    }
    if (task) {
        work = task->work;
        work->pos = *pos;
        VELOCITY(work) = *speed;
        task->destructor = shot_destroy;
        task->step = shot_step;
#ifdef NONMATCHING
        /* this side returns null where enemy_work_alloc gives no work, before
         * the draw that reads it is set */
        if (enemy_work_alloc(task) == NULL) {
            return NULL;
        }
        task->draw = shot_draw;
#else
        task->draw = shot_draw;
        enemy_work_alloc(task);
#endif
        collision_init(task, shot_shape, 1, COLLISION_ITEM);
        collision_set_radius(task, ENGINE_SHOT_RADIUS);
        task->force.p = trail_alloc(&shot_trail, SHOT_TRAIL_POINTS);
        SHOT(task)->origin = *pos;
        SHOT(task)->range = TURRET_SHOT_RANGE;
        work->submode = CONTROLLED_SHOT;
    }
    return task;
}
#endif

static struct task *shot_create(NJS_POINT3 *pos, NJS_VECTOR *speed,
                                NJS_POINT3 *origin, Float range)
{
    struct task *task;
    struct task_work *work;
    task = task_create(TASK_WORK, 3, shot_step, "DrrTamaExec");
    if (task) {
        task->extra.p = syCalloc(1, sizeof(struct cannon_shot));
        if (!task->extra.p) {
            task_destroy(task);
            task = NULL;
        }
    }
    if (task) {
        work = task->work;
        work->pos = *pos;
        VELOCITY(work) = *speed;
        task->destructor = shot_destroy;
        task->step = shot_step;
#ifdef NONMATCHING
        /* this side returns null where enemy_work_alloc gives no work, before
         * the draw that reads it is set */
        if (enemy_work_alloc(task) == NULL) {
            return NULL;
        }
        task->draw = shot_draw;
#else
        task->draw = shot_draw;
        enemy_work_alloc(task);
#endif
        collision_init(task, shot_shape, 1, COLLISION_ITEM);
#ifdef ENGINE_REACTOR
        collision_set_radius(task, ENGINE_SHOT_RADIUS);
#endif
        task->force.p = trail_alloc(&shot_trail, SHOT_TRAIL_POINTS);
        SHOT(task)->origin = *origin;
        SHOT(task)->range = range;
    }
    return task;
}

#ifdef NONMATCHING
/* the plain form the hack tag names */
#define SCALE_VECTOR(vec, factor)                                              \
    do {                                                                       \
        (vec).x *= (factor);                                                   \
        (vec).y *= (factor);                                                   \
        (vec).z *= (factor);                                                   \
    } while (0)
#define ADD_VECTOR(dst, src)                                                   \
    do {                                                                       \
        (dst).x += (src).x;                                                    \
        (dst).y += (src).y;                                                    \
        (dst).z += (src).z;                                                    \
    } while (0)
#else
/* @hack spelling: the vectors' components walked by pointers, where named
 * components would do */
#define SCALE_VECTOR(vec, factor)                                              \
    do {                                                                       \
        component = &(vec).x;                                                  \
        *component++ *= (factor);                                              \
        *component++ *= (factor);                                              \
        *component *= (factor);                                                \
    } while (0)
#define ADD_VECTOR(dst, src)                                                   \
    do {                                                                       \
        component = &(dst).x;                                                  \
        other = &(src).x;                                                      \
        *component++ += *other++;                                              \
        *component++ += *other++;                                              \
        *component += *other;                                                  \
    } while (0)
#endif

static void shot_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *target;
    struct task_work *leader;
    NJS_VECTOR steering;
#ifdef NONMATCHING
#else
    Float *component;
    Float *other;
    Float *out;
#endif
    Float homing;
    Float distance;
    Float scale;
    int player;
    SHOT(task)->homing += HOMING_GAIN;
    if (SHOT(task)->homing > 1.0F) {
        SHOT(task)->homing = 1.0F;
    }
    homing = SHOT(task)->homing;
    work->ang.y += (Angle)(homing * SHOT_SPIN);
    if ((player = player_near(&work->pos, HOMING_RANGE))) {
#ifdef NONMATCHING
        /* the plain form the hack tag names */
        target = player_work[player - 1];
        steering.x = target->pos.x - work->pos.x;
        steering.y = target->pos.y - work->pos.y;
        steering.z = target->pos.z - work->pos.z;
#else
        /* @hack spelling: the difference's components walked by pointers,
         * where named components would do */
        out = &steering.x;
        target = player_work[player - 1];
        other = &work->pos.x;
        component = &target->pos.x;
        *out++ = *component++ - *other++;
        *out++ = *component++ - *other++;
        *out = *component - *other;
#endif
        steering.y += AIM_HEIGHT;
        distance = njScalor(&steering);
        if (distance > STEER_LEAST) {
            scale = shot_pull / distance * SHOT(task)->homing;
            SCALE_VECTOR(steering, scale);
            ADD_VECTOR(VELOCITY(work), steering);
        }
    }
    VELOCITY(work).y = VELOCITY(work).y - shot_gravity;
    SCALE_VECTOR(VELOCITY(work), SHOT_DRAG);
    ADD_VECTOR(work->pos, VELOCITY(work));
    if (work->timer++ > SHOT_LIFETIME || (work->submode && !SHOT_CONTROLLER)) {
        SCALE_VECTOR(VELOCITY(work), PUFF_SPEED);
        white_breath_fade_create(&work->pos, &VELOCITY(work), SHOT_PUFF_SCALE);
        task_kill(task);
    } else {
#ifdef ENGINE_REACTOR
        sound_play_at_for(ENGINE_SHOT_SOUND, work, 1, 0,
                          ENGINE_SHOT_SOUND_STEPS, &work->pos);
#endif
        if (work->collision) {
            if (collision_touched(task) || (work->flags & WORK_HURT)) {
                SCALE_VECTOR(VELOCITY(work), BURST_SPEED);
                burst_create_wobbling(&work->pos, &VELOCITY(work),
                                      SHOT_PUFF_SCALE);
                task_kill(task);
            } else {
                collision_register(task);
            }
        }
        if (!(leader = player_work[0]) ||
            (work->timer < SHOT_OUT_OF_RANGE_AGE &&
             njDistanceP2P(&SHOT(task)->origin, &leader->pos) >
                 SHOT(task)->range)) {
            work->timer = SHOT_OUT_OF_RANGE_AGE;
        }
    }
}

static void shot_destroy(struct task *task)
{
    trail_free(&shot_trail, task->force.p);
    enemy_destroy(task);
    task->force.p = NULL;
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void shot_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *node = &cannon_shot_model;
    NJS_POINT3 origin;
    Float scale;
    int i;
    njPushMatrixEx();
    /* @hack redundant: a test of node that no path reaches */
    if (0) {
        if (node) {
        }
    }
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    origin.x = 0.0F;
    origin.y = 0.0F;
    origin.z = 0.0F;
    trail_draw(&shot_trail, task->force.p, &origin);
    if (work->timer % SHOT_FLASH_PERIOD > SHOT_LAST_DIM) {
        njSetTexture(&cannon_shot_bright);
    } else {
        njSetTexture(&cannon_shot_dim);
    }
    if (work->timer <= SHOT_BLINK_AGE || !(work->timer & SHOT_BLINK)) {
        njCnkEasyDrawModel(node->model);
        scale = SHOT(task)->homing + SHOT_SPARK_SCALE;
        node = node->child;
        njScale(NULL, scale, scale, scale);
        for (i = 0; i < SHOT_SPARKS; ++i) {
            njCnkEasyDrawModel(node->model);
            node = node->sibling;
        }
    }
    njPopMatrixEx();
}

static void cannon_draw(struct task *task);
static void cannon_destroy(struct task *task);
static void cannon_step(struct task *task);

void CANNON_INIT(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = cannon_draw;
    task->step = cannon_step;
    task->destructor = cannon_destroy;
    if (work->scl.x > MAX_FIRE) {
        work->scl.x = MAX_FIRE;
    }
    HEADING(work) = work->ang.y;
    collision_init(task, cannon_shape, 1, COLLISION_OBJECT);
}

static void cannon_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&cannon_texlist);
#ifndef ENGINE_REACTOR
    texture_anim_apply(&cannon_barrel, &cannon_barrel_anim,
                       game_state.steps * BARREL_ANIM_RATE);
#endif
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + BOB(task), work->pos.z);
    njRotateY(NULL, HEADING(work));
    njCnkEasyDrawModel(&cannon_barrel);
    njPopMatrixEx();
}

static void cannon_destroy(struct task *task)
{
}

static void cannon_step(struct task *task)
{
    struct task_work *work = task->work;
    Bool (*push)(void);
    void (*rotate)(NJS_MATRIX *, Angle);
    Bool (*pop)(void);
    Float distance_scale;
    Float zero;
    NJS_VECTOR aim;
    NJS_POINT3 muzzle;
    NJS_POINT3 origin;
    Float range;
    Float offset;
#ifdef NONMATCHING
#else
    Float *left;
    Float *right;
    Float *out;
#endif
    Sint16 turn;
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the matrix calls made through pointers, and
     * ORIGIN_SCALE and 0 held in locals, where the calls and the constants
     * would do */
    distance_scale = ORIGIN_SCALE;
    pop = njPopMatrixEx;
    rotate = njRotateY;
    push = njPushMatrixEx;
    zero = 0.0F;
    /* @hack redundant: a call of njPopMatrixEx that no path reaches */
    if (0) {
        njPopMatrixEx();
    }
    collision_register(task);
    BOB(task) = njSin(game_state.steps * BOB_RATE) * BOB_HEIGHT;
    if (!player_work[0]) {
        return;
    }
    range = njRoundOff(work->scl.z);
    offset = -(njFraction(work->scl.z) * distance_scale);
    origin.x = offset;
    origin.z = zero;
    origin.y = zero;
    push();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    rotate(NULL, work->ang.y);
    njCalcPoint(NULL, &origin, &origin);
    pop();
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    aim.x = player_work[0]->pos.x - origin.x;
    aim.y = player_work[0]->pos.y - origin.y;
    aim.z = player_work[0]->pos.z - origin.z;
#else
    /* @hack spelling: the difference's components walked by pointers, each
     * pointer stepped by a statement of its own, where named components
     * would do */
    out = &aim.x;
    left = &player_work[0]->pos.x;
    right = &origin.x;
    *out = *left - *right;
    out++;
    left = left + 1;
    right = right + 1;
    *out = *left - *right;
    out++;
    left = left + 1;
    right = right + 1;
    *out = *left - *right;
#endif
    aim.y = zero;
    if (!(njScalor2(&aim) < range * range)) {
        return;
    }
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    aim.x = work->pos.x - player_work[0]->pos.x;
    aim.y = work->pos.y - player_work[0]->pos.y;
    aim.z = work->pos.z - player_work[0]->pos.z;
#else
    /* @hack spelling: the difference's components walked by pointers, where
     * named components would do */
    out = &aim.x;
    left = &work->pos.x;
    right = &player_work[0]->pos.x;
    *out = *left - *right++;
    out++;
    left++;
    *out++ = *left++ - *right++;
    *out = *left - *right;
#endif
    turn = (Angle)(atan2f(aim.x, aim.z) * (Float)ANGLE_PER_RADIAN) -
           QUARTER_TURN - HEADING(work);
    /* @hack type: turn scaled through a pointer to it, where turn *=
     * TURN_SHARE would do */
    {
        Sint16 *angle = &turn;
        Float amount = *angle;
        *angle = amount * TURN_SHARE;
    }
    if (turn > MAX_TURN) {
        turn = MAX_TURN;
    }
    if (turn < -MAX_TURN) {
        turn = -MAX_TURN;
    }
    HEADING(work) += turn;
#ifdef ENGINE_REACTOR
#define CANNON_FIRES                                                           \
    (rand() * (100.0F * RAND_SCALE) < work->scl.x * FIRE_PERCENT)
#else
/* @hack type: game_state's time_stopped read as game_state_time_stopped, where
 * game_state.time_stopped would do */
#define CANNON_FIRES                                                           \
    (game_state_time_stopped == 0 &&                                           \
     rand() * (100.0F * RAND_SCALE) < work->scl.x * FIRE_PERCENT)
#endif
    if (CANNON_FIRES) {
        muzzle.x = -MUZZLE_REACH;
        muzzle.y = zero;
        muzzle.z = zero;
        aim.x = -work->scl.y - SHOT_LEAST_SPEED;
        aim.y = zero;
        aim.z = zero;
        push();
        njUnitMatrix(NULL);
        njTranslate(NULL, work->pos.x, work->pos.y + BOB(task), work->pos.z);
        rotate(NULL, HEADING(work));
        njCalcPoint(NULL, &muzzle, &muzzle);
        njCalcVector(NULL, &aim, &aim);
        pop();
        shot_create(&muzzle, &aim, &origin, range);
    }
}

#endif
