/* the dragonfly swarm, KAZANI in the stage's object list: dragonflies that
 * fly about a box. each steers toward targets of its own. the flight follows
 * stg10/seagulls.c's, with tunings and turns of its own */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "stg03/dragonfly.h"
#include "stg03/dragonfly_swarm.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the low byte of the work's ang.x, plus 1, is the count of dragonflies */
#define COUNT_BYTE 0xff
/* the step caps the low byte of ang.x at COUNT_BYTE_CAP. the count, which the
 * init fixed, stays */
#define COUNT_BYTE_CAP 20

/* a dragonfly, in the swarm's axes: about the work's pos, turned by its
 * ang.y */
struct swarm_fly {
    NJS_POINT3 pos;
    NJS_POINT3 target;
    NJS_VECTOR spd;
    /* what the dragonfly's speed gains toward its target each step */
    Float accel;
    /* its pitch, heading and bank, which each step eases toward their aims */
    struct angles ang;
    /* a wing motion's frame, which the step runs on and nothing reads */
    Float frame;
    /* the steps it keeps its target before it picks another */
    Sint32 target_steps;
    /* the steps left of its burst. while it isn't 0, accel grows each step */
    Sint32 burst_steps;
};

/* the swarm's record, in task->motion */
struct dragonfly_swarm {
    struct swarm_fly *flies;
    Sint32 count;
};

#define SWARM(task) ((struct dragonfly_swarm *)(task)->motion.p)

/* a dragonfly's target starts at 0 in x. in y it lies within half
 * TARGET_SPREAD_Y of 0, and in z within half TARGET_SPREAD_Z. it then turns
 * about y by a random whole count of quarter turns, below TARGET_TURNS. only
 * then does it scale by scl */
#define TARGET_SPREAD_Y 2.0F
#define TARGET_SPREAD_Z 1.9F
#define TARGET_TURNS 16
/* a dragonfly keeps a target TARGET_STEPS steps, plus up to
 * TARGET_STEPS_RANDOM more */
#define TARGET_STEPS 120
#define TARGET_STEPS_RANDOM 1000.0F
/* a dragonfly starts in a spread of POS_SPREAD times scl about the box's
 * middle. it starts in a burst of FIRST_BURST steps. its accel starts up to
 * FIRST_ACCEL_RANDOM above what its first target gives */
#define POS_SPREAD 2.5F
#define FIRST_BURST 20.0F
#define FIRST_BURST_RANDOM 50.0F
#define FIRST_ACCEL_RANDOM 0.1F
/* the frame runs no slower than MIN_FRAME_STEP a step once it is past 1, and
 * wraps at FRAME_WRAP */
#define MIN_FRAME_STEP 0.01F
#define FRAME_WRAP 9.0F
/* a slow dragonfly out of a burst starts one with a chance of BURST_CHANCE a
 * step. the burst lasts BURST_STEPS steps, plus up to BURST_STEPS_RANDOM
 * more. a new target sets a burst of up to RETARGET_BURST_RANDOM steps */
#define BURST_CHANCE 0.1F
#define BURST_STEPS 6
#define BURST_STEPS_RANDOM 12.0F
#define RETARGET_BURST_RANDOM 10.0F
/* on each step that starts no burst, a dragonfly keeps swarm_drag of its
 * speed. along y it keeps CLIMB_DRAG of that again */
#define CLIMB_DRAG 0.99F
/* a dragonfly within TARGET_REACHED of its target picks another */
#define TARGET_REACHED 70.0F
/* a dragonfly turns its heading and pitch only while it moves faster than
 * MOVING_SPEED */
#define MOVING_SPEED 0.001F
/* the steepest pitch, up or down */
#define MAX_PITCH NJM_DEG_ANG(20)
/* each step moves the pitch, heading and bank TURN_SHARE of the way to their
 * aims. it cuts the share toward 0. before the share, it caps the heading's
 * way at MAX_HEADING_TURN, and the others' at MAX_TURN */
#define MAX_TURN NJM_DEG_ANG(8)
#define MAX_HEADING_TURN NJM_DEG_ANG(6)
#define TURN_SHARE 0.3F
/* the steer turns the direction to a dragonfly's target to within MAX_STEER
 * of the direction it moves. it does so only while the dragonfly's squared
 * speed across x and z passes STEER_SPEED_SQ. it then aims the bank at half
 * that turn, negated. a slower dragonfly aims its bank at BANK_EASE of the
 * bank. the step's share of that change cuts to 0 for a bank of size 100 or
 * less. such a bank stays */
#define STEER_SPEED_SQ 0.0001F
#define MAX_STEER EIGHTH_TURN
#define BANK_EASE 0.97F
/* the steer adds speed toward a target only while it lies farther than
 * STEER_MIN_DISTANCE. that keeps the division by the distance off 0 */
#define STEER_MIN_DISTANCE 0.0001F
/* the steer adds CLIMB of the dragonfly's accel up, and DIVE of it down */
#define CLIMB 0.5F
#define DIVE 0.7F
/* a dragonfly draws turned down by DRAW_PITCH more than its pitch. it doesn't
 * draw where a ball of DRAW_RADIUS about it lies wholly behind the camera */
#define DRAW_PITCH 0x200
#define DRAW_RADIUS 25.0F
/* a dragonfly's body pitches and rolls by up to PITCH and ROLL. its wings beat
 * by up to FLAP either way. each sways on a sine of the dragonfly's steps
 * times its _RATE. a dragonfly's steps are game_state.steps plus SWAY_LEAD for
 * each dragonfly before it. the body bobs by up to BOB, a quarter turn ahead
 * of the pitch */
#define SWAY_LEAD 55
#define BOB 12.0F
#define PITCH (12.0F * ANGLE_PER_DEGREE)
#define ROLL (15.0F * ANGLE_PER_DEGREE)
#define FLAP (20.0F * ANGLE_PER_DEGREE)
#define PITCH_RATE 0x100
#define ROLL_RATE 0xe0
#define FLAP_RATE 0x2e00

/* a dragonfly's frame runs at swarm_frame_rate times its accel a step */
static Float swarm_frame_rate = 5.0F;
/* a new target adds swarm_accel_base to accel, and up to swarm_accel_random
 * more. a burst adds swarm_burst_accel each step */
static Float swarm_accel_base = 0.5F;
static Float swarm_accel_random = 0.01F;
static Float swarm_burst_accel = 0.005F;
/* a dragonfly whose step brings it nearer its target by less than
 * swarm_slow_speed is slow. a dragonfly starts with up to that speed along
 * each axis */
static Float swarm_slow_speed = 0.1F;
static Float swarm_drag = 0.98F;
/* each step a dragonfly keeps swarm_accel_keep of its accel */
static Float swarm_accel_keep = 0.95F;
/* what each step adds to a dragonfly's speed along y. it is -0.0, so the add
 * changes no speed */
static Float swarm_gravity = -0.0F;
/* a dragonfly's speed is held to swarm_top_speed */
static Float swarm_top_speed = 3.0F;
static NJS_TEXLIST *dat_8c5fab38[] = {&dragonfly_texlist, NULL};

static void dragonfly_swarm_destroy(struct task *task);
static void dragonfly_swarm_steer(struct task *task, struct swarm_fly *fly);
static void dragonfly_swarm_step(struct task *task);
static void dragonfly_swarm_draw(struct task *task);

#pragma inline(dragonfly_swarm_next)
static void dragonfly_swarm_next(NJS_VECTOR *spd, NJS_POINT3 *pos,
                                 NJS_POINT3 *next)
{
    Float *add = &spd->x;
    Float *src = &pos->x;
    Float *dst = &next->x;

    *dst++ = *add++ + *src++;
    *dst++ = *add++ + *src++;
    *dst = *add + *src;
}

/* give fly a new target and the steps it keeps it, and add to fly's accel */
static void dragonfly_swarm_pick_target(struct task *task,
                                        struct swarm_fly *fly)
{
    struct task_work *work = task->work;
    NJS_VECTOR target;

    target.z = (((Float)rand() * RAND_SCALE) - 0.5F) * TARGET_SPREAD_Z;
    target.y = (((Float)rand() * RAND_SCALE) - 0.5F) * TARGET_SPREAD_Y;
    target.x = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njScale(NULL, work->scl.x, work->scl.y, work->scl.z);
    njRotateY(NULL, (int)((Float)rand() * (TARGET_TURNS * RAND_SCALE)) *
                        (Float)QUARTER_TURN);
    njCalcVector(NULL, &target, &target);
    njPopMatrix(1);
    fly->target = target;
    fly->accel +=
        ((swarm_accel_random * rand()) * RAND_SCALE) + swarm_accel_base;
    fly->target_steps =
        (int)((Float)rand() * (TARGET_STEPS_RANDOM * RAND_SCALE)) +
        TARGET_STEPS;
}

void dragonfly_swarm_init(struct task *task)
{
    struct task_work *work = task->work;
    struct swarm_fly *fly;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct dragonfly_swarm));
    if (task->motion.p == NULL) {
        return;
    }
    SWARM(task)->flies =
        syCalloc((work->ang.x & COUNT_BYTE) + 1, sizeof(struct swarm_fly));
    if (SWARM(task)->flies == NULL) {
        syFree(task->motion.p);
        task->motion.p = NULL;
        return;
    }
    SWARM(task)->count = (work->ang.x & COUNT_BYTE) + 1;
    for (i = 0, fly = SWARM(task)->flies; i < SWARM(task)->count; i++, fly++) {
        dragonfly_swarm_pick_target(task, fly);
        fly->pos.x =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.x * POS_SPREAD;
        fly->pos.y =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.y * POS_SPREAD;
        fly->pos.z =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.z * POS_SPREAD;
        fly->spd.x =
            (((Float)rand() * RAND_SCALE) - 0.5F) * swarm_slow_speed * 2.0F;
        fly->spd.y =
            (((Float)rand() * RAND_SCALE) - 0.5F) * swarm_slow_speed * 2.0F;
        fly->spd.z =
            (((Float)rand() * RAND_SCALE) - 0.5F) * swarm_slow_speed * 2.0F;
        /* @bug logic: the first burst never gets its random steps. the cast
         * to int cuts rand() * RAND_SCALE, which is below 1, to 0 before
         * FIRST_BURST_RANDOM scales it, at 0x8c50784e.
         * dragonfly_swarm_pick_target's turn scales before its cast, at
         * 0x8c50763c */
        fly->burst_steps += FIRST_BURST + ((int)((Float)rand() * RAND_SCALE) *
                                           FIRST_BURST_RANDOM);
        fly->accel += (Float)rand() * (FIRST_ACCEL_RANDOM * RAND_SCALE);
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = dragonfly_swarm_draw;
    task->step = dragonfly_swarm_step;
    task->destructor = dragonfly_swarm_destroy;
    work->timer = 0;
}

static void dragonfly_swarm_destroy(struct task *task)
{
    if (SWARM(task)->flies != NULL) {
        syFree(SWARM(task)->flies);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* add fly's accel to its speed, along its way to its target as the steer
 * turns it. set the bank's aim too */
static void dragonfly_swarm_steer(struct task *task, struct swarm_fly *fly)
{
    NJS_VECTOR way;
    Angle target_ang;
    Angle moving;
    Sint16 turn;
    Float length;

    if ((fly->spd.x * fly->spd.x) + (fly->spd.z * fly->spd.z) >
        STEER_SPEED_SQ) {
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' differences written out would do */
        {
            Float *src = &fly->target.x;
            Float *sub = &fly->pos.x;
            Float *dst = &way.x;

            *dst++ = *src++ - *sub++;
            *dst++ = *src++ - *sub++;
            *dst = *src - *sub;
        }
        target_ang = atan2f(way.x, way.z) * ANGLE_PER_RADIAN_F;
        moving = atan2f(fly->spd.x, fly->spd.z) * ANGLE_PER_RADIAN_F;
        turn = target_ang - moving;
        if (turn > MAX_STEER || turn < -MAX_STEER) {
            if (turn > MAX_STEER) {
                turn = MAX_STEER;
            }
            if (turn < -MAX_STEER) {
                turn = -MAX_STEER;
            }
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, turn - target_ang + moving);
            njCalcVector(NULL, &way, &way);
            njPopMatrix(1);
        }
        fly->ang.z = -(turn / 2);
    } else {
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' differences written out would do */
        {
            Float *src = &fly->target.x;
            Float *sub = &fly->pos.x;
            Float *dst = &way.x;

            *dst++ = *src++ - *sub++;
            *dst++ = *src++ - *sub++;
            *dst = *src - *sub;
        }
        /* @hack type: the bank's low half read and written as a Sint16,
         * where the whole Angle would do. the bank stays within a Sint16 */
        {
            Sint16 *bank = (Sint16 *)&fly->ang.z;
            Float value = *bank;

            *bank = value * BANK_EASE;
        }
    }
    length = njScalor(&way);
    if (length > STEER_MIN_DISTANCE) {
        length = fly->accel / length;
        /* @hack spelling: a Float pointer stepped through way, where the
         * members scaled one by one would do */
        {
            Float *dst = &way.x;

            *dst++ *= length;
            *dst++ *= length;
            *dst *= length;
        }
        if (way.y > 0.0F) {
            way.y *= CLIMB;
        } else {
            way.y *= DIVE;
        }
        /* @hack spelling: two Float pointers stepped through the vectors,
         * where the members' sums written out would do */
        {
            Float *dst = &fly->spd.x;
            Float *src = &way.x;

            *dst++ += *src++;
            *dst++ += *src++;
            *dst += *src;
        }
    }
}

static void dragonfly_swarm_step(struct task *task)
{
    struct task_work *work = task->work;
    struct swarm_fly *fly;
    int i;
    Sint16 turn;
    Float approach;
    struct angles before;
    NJS_POINT3 next;
    Float rate;
    Float length;
    Sint16 pitch;

    if (placement_try_leave(task)) {
        return;
    }
    work->ang.x &= COUNT_BYTE;
    if (work->ang.x > COUNT_BYTE_CAP) {
        work->ang.x = COUNT_BYTE_CAP;
    }
    if (game_state.time_stopped) {
        return;
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's registers */
    if (0) {
        i = SWARM(task)->count;
        work->ang.x = 0;
        placement_try_leave(task);
        work->ang.y = i * i;
        work->ang.z = i + i;
        SWARM(task)->count = i * i;
        task->motion.p = NULL;
    }
    for (i = 0, fly = SWARM(task)->flies; i < SWARM(task)->count; i++, fly++) {
        before = fly->ang;
        rate = fly->accel * swarm_frame_rate;
        if (rate < MIN_FRAME_STEP && fly->frame > 1.0F) {
            rate = MIN_FRAME_STEP;
        }
        fly->frame += rate;
        while (fly->frame > FRAME_WRAP) {
            fly->frame -= FRAME_WRAP;
        }
        /* @hack spelling: the next position worked out by
         * dragonfly_swarm_next, where three assignments would do */
        dragonfly_swarm_next(&fly->spd, &fly->pos, &next);
        /* @hack spelling: the minus of the next distance plus the present
         * one, where the present one less the next would do */
        approach = -njDistanceP2P(&next, &fly->target) +
                   njDistanceP2P(&fly->pos, &fly->target);
        if (fly->burst_steps) {
            fly->accel += swarm_burst_accel;
            fly->burst_steps--;
        }
        if (approach < swarm_slow_speed &&
            (Float)rand() * RAND_SCALE < BURST_CHANCE &&
            fly->burst_steps == 0) {
            fly->burst_steps =
                (int)((Float)rand() * (BURST_STEPS_RANDOM * RAND_SCALE)) +
                BURST_STEPS;
        } else {
            /* @hack spelling: a Float pointer stepped through spd, where
             * the members scaled one by one would do */
            {
                Float *dst = &fly->spd.x;

                *dst++ *= swarm_drag;
                *dst++ *= swarm_drag;
                *dst *= swarm_drag;
            }
            fly->spd.y *= CLIMB_DRAG;
        }
        fly->spd.y += swarm_gravity;
        dragonfly_swarm_steer(task, fly);
        fly->accel *= swarm_accel_keep;
        /* @hack spelling: two Float pointers stepped through the vectors,
         * where the members' sums written out would do */
        {
            Float *dst = &fly->pos.x;
            Float *src = &fly->spd.x;

            *dst++ += *src++;
            *dst++ += *src++;
            *dst += *src;
        }
        length = njScalor(&fly->spd);
        if (length > swarm_top_speed) {
            length = swarm_top_speed / length;
            /* @hack spelling: a Float pointer stepped through spd, where
             * the members scaled one by one would do */
            {
                Float *dst = &fly->spd.x;

                *dst++ *= length;
                *dst++ *= length;
                *dst *= length;
            }
        }
        if (njDistanceP2P(&fly->target, &fly->pos) < TARGET_REACHED ||
            fly->target_steps-- <= 0) {
            dragonfly_swarm_pick_target(task, fly);
            /* @hack type: the burst's steps cast through a Sint8, where int
             * would do. the value stays below RETARGET_BURST_RANDOM */
            fly->burst_steps =
                (Sint8)((Float)rand() * (RETARGET_BURST_RANDOM * RAND_SCALE));
        }
        if (njScalor(&fly->spd) > MOVING_SPEED) {
            fly->ang.x = atan2f(fly->spd.y, njSqrt((fly->spd.x * fly->spd.x) +
                                                   (fly->spd.z * fly->spd.z))) *
                         ANGLE_PER_RADIAN_F;
            fly->ang.y = atan2f(fly->spd.x, fly->spd.z) * ANGLE_PER_RADIAN_F;
            pitch = fly->ang.x;
            if (pitch > MAX_PITCH) {
                pitch = MAX_PITCH;
            }
            if (pitch < -MAX_PITCH) {
                pitch = -MAX_PITCH;
            }
            fly->ang.x = pitch;
        }
        turn = fly->ang.x - before.x;
        if (turn > MAX_TURN) {
            turn = MAX_TURN;
        }
        if (turn < -MAX_TURN) {
            turn = -MAX_TURN;
        }
        /* @hack type: the turn scaled through a Sint16 pointer and a Float,
         * where turn *= TURN_SHARE would do */
        {
            Sint16 *at = &turn;
            Float value = *at;

            *at = value * TURN_SHARE;
        }
        fly->ang.x = before.x + turn;
        turn = fly->ang.y - before.y;
        if (turn > MAX_HEADING_TURN) {
            turn = MAX_HEADING_TURN;
        }
        if (turn < -MAX_HEADING_TURN) {
            turn = -MAX_HEADING_TURN;
        }
        /* @hack type: the turn scaled through a Sint16 pointer and a Float,
         * where turn *= TURN_SHARE would do */
        {
            Sint16 *at = &turn;
            Float value = *at;

            *at = value * TURN_SHARE;
        }
        fly->ang.y = before.y + turn;
        turn = fly->ang.z - before.z;
        if (turn > MAX_TURN) {
            turn = MAX_TURN;
        }
        if (turn < -MAX_TURN) {
            turn = -MAX_TURN;
        }
        /* @hack type: the turn scaled through a Sint16 pointer and a Float,
         * where turn *= TURN_SHARE would do */
        {
            Sint16 *at = &turn;
            Float value = *at;

            *at = value * TURN_SHARE;
        }
        fly->ang.z = before.z + turn;
    }
}

static void dragonfly_swarm_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct swarm_fly *fly;
    NJS_CNK_OBJECT *object;
    int i;
    Sint32 sway;
    Angle pitch;
    Angle roll;
    Angle flap;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- KAMOMENUM";
        (void)"<- ANGLE Y";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- SCL Z";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&dragonfly_texlist);
    njPushMatrixEx();
    for (i = 0, fly = SWARM(task)->flies; i < SWARM(task)->count; i++, fly++) {
        if (sphere_in_front_no_depth(&fly->pos, DRAW_RADIUS)) {
            sway = (i * SWAY_LEAD) + game_state.steps;
            object = &dragonfly_object;
            pitch = njSin(sway * PITCH_RATE) * PITCH;
            roll = njSin(sway * ROLL_RATE) * ROLL;
            njTranslateEx(&fly->pos);
            njTranslate(NULL, object->pos[0],
                        object->pos[1] +
                            (njSin((sway * PITCH_RATE) + QUARTER_TURN) * BOB),
                        object->pos[2]);
            njRotateY(NULL, fly->ang.y);
            njRotateX(NULL, -fly->ang.x + pitch - DRAW_PITCH);
            njRotateZ(NULL, fly->ang.z + roll);
            njTranslate(NULL, -object->pos[0], -object->pos[1],
                        -object->pos[2]);
            njCnkEasyDrawModel(object->model);
            object = object->child;
            flap = njSin(sway * FLAP_RATE) * FLAP;
            njPushMatrixEx();
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateZ(NULL, flap);
            njCnkEasyDrawModel(object->model);
            njPopMatrixEx();
            object = object->sibling;
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateZ(NULL, -flap);
            njRotateY(NULL, HALF_TURN);
            njCnkEasyDrawModel(object->model);
            matrix_load_parent();
        }
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * draw the image's registers and frame slots */
    if (0) {
        matrix_load_parent();
        njRotateY(NULL, (fly->ang.y * 55) + 0xe0);
        njRotateZ(NULL, (fly->ang.z / 55) + 0xe0);
    }
    njPopMatrix(2);
}
