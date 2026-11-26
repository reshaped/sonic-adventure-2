#include "stg11/lock_on_missile/text.inc"
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "effect/burst.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/missile.h"
#include "shared/rocket_thrust.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "stg11/lock_on_missile.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

struct lock_on_missile {
    Angle roll;
    Angle yaw;
    Angle pitch;
    /* the missile's position. a landed missile points the *hit of each target
     * that waits for it here */
    NJS_POINT3 pos;
    /* the aim's charge, which only delays the flight. each step of the aim
     * that leaves the missile facing its destination adds CHARGE_STEP. the
     * missile flies once the charge reaches FULL_CHARGE */
    Float charge;
    Angle spin; /* the cover's turn about its y axis */
};
#define MISSILE(task) ((struct lock_on_missile *)(task)->motion.p)

/* a missile's modes, in its work's mode. a new work's mode is MODE_NEW, which
 * init replaces. no code sets MODE_UNUSED */
enum missile_mode {
    MODE_NEW,
    MODE_UNUSED,
    MODE_WAIT_TARGET, /* hidden in its base until it can launch */
    /* shown at START_HEIGHT until a target that waits for it lacks
     * TARGET_UNSHUT */
    MODE_WAIT_SHUTTER,
    MODE_RISE,
    MODE_WAIT_SHOT, /* risen until its weak point is hurt */
    MODE_AIM, /* turning toward its destination, then charging */
    MODE_SPENT, /* hidden in its base, never to launch again */
    MODE_FLY,
    /* hidden at its destination, until the next step returns it to its base */
    MODE_LANDED
};

/* MISSILE_LAUNCH_BITS of ang.x, modulo MISSILE_LAUNCHES, pick the launch. the
 * launches' names are the set editor's labels. MISSILE_SWITCH keeps the
 * missile in its base while the switch of its id is off. the other two add no
 * condition */
enum missile_launch {
    MISSILE_EVERY,
    MISSILE_ONCE,
    MISSILE_SWITCH,
    MISSILE_LAUNCHES
};

/* the missile waits for its shutter START_HEIGHT above the base's pos, or
 * below a base upside down. it then rises by RISE_STEP a step to
 * RAISED_HEIGHT */
#define START_HEIGHT 9.0F
#define RAISED_HEIGHT 18.0F
#define RISE_STEP 0.05F
/* how far the cover turns a step in MODE_RISE and MODE_WAIT_SHOT, in Angle
 * units */
#define COVER_SPIN 0x100
/* the thrown cover's speed along the world's y and z */
#define COVER_SPEED_Y 0.8F
#define COVER_SPEED_Z (-0.3F)
/* a hurt to the weak point sets bit WEAK_POINT_INDEX of the collision
 * child's submode */
#define WEAK_POINT_INDEX 0
/* the lamp blinks in every mode but MODE_NEW, MODE_WAIT_TARGET and
 * MODE_WAIT_SHUTTER. these are blink_light_gray's period, rest and fade, in
 * steps */
#define LAMP_BLINK_PERIOD 80
#define LAMP_BLINK_REST 40
#define LAMP_BLINK_FADE 8

/* the most the aim turns yaw or pitch in a step, in Angle units. the aim
 * turns pitch only on a step where yaw already faces the destination */
#define AIM_TURN 320
/* a risen missile bobs by up to BOB_HEIGHT about its risen place. it rolls by
 * up to ROLL_DEGREES either way. BOB and ROLL give the bob and the roll at a
 * count of steps. their phases turn BOB_STEP and ROLL_STEP a step, in Angle
 * units */
#define BOB_HEIGHT 0.3F
#define BOB_STEP 0x600
#define BOB(steps) (njSin((steps) * BOB_STEP) * BOB_HEIGHT)
#define ROLL_DEGREES 10.0F
#define ROLL_STEP 0x400
#define ROLL(steps)                                                            \
    (njSin((steps) * ROLL_STEP) * (ROLL_DEGREES * ANGLE_PER_DEGREE))
#define THRUST_CHANCE 0.6F /* the chance of a puff at each step of the aim */
#define CHARGE_STEP 0.07F
#define FULL_CHARGE 1.0F
/* how far the missile flies in a step. a missile nearer than that to its
 * destination lands */
#define FLIGHT_STEP 5.1F
/* a flying missile gives off a puff only while farther than THRUST_DISTANCE
 * from its destination. it gives off none at a step whose game_state.steps
 * has FLIGHT_PUFF_SKIP set */
#define THRUST_DISTANCE (FLIGHT_STEP * 2)
#define FLIGHT_PUFF_SKIP 2
/* a flying missile's puff moves at FLIGHT_PUFF_SPEED of the missile's move
 * in that step. each part of its speed then changes at random by up to half
 * of FLIGHT_JITTER either way */
#define FLIGHT_PUFF_SPEED 0.5F
#define FLIGHT_JITTER 0.3F
/* a flying missile's puff takes a random size from FLIGHT_SIZE_MIN up to
 * FLIGHT_SIZE_MIN + FLIGHT_SIZE_RANGE */
#define FLIGHT_SIZE_MIN 2.0F
#define FLIGHT_SIZE_RANGE 1.5F
/* the thrust leaves the missile at TAIL_X on its x axis, which its yaw and
 * pitch turn */
#define TAIL_X (-0.2F)
/* BURST_SCALE is the size scale of the burst at a landing. the burst's speed
 * is tail scaled by BURST_SPEED_SCALE */
#define BURST_SCALE 15.0F
#define BURST_SPEED_SCALE 0.2F

/* the steps of a flight the collision child has counted, in the missile
 * task's unk_3c. the count stops at FLIGHT_SAFE_STEPS + 1. it is 0 outside a
 * flight */
#define STEPS_FLOWN(task) ((task)->unk_3c.l)
/* the missile's shape takes FLIGHT_ATTACK in the SHAPE_ATTACK bits of its
 * levels field once STEPS_FLOWN passes FLIGHT_SAFE_STEPS. it has no attack
 * outside a flight */
#define FLIGHT_SAFE_STEPS 15
#define FLIGHT_ATTACK SHAPE_ATTACK_LEVEL(2)

#define SOUND_LAUNCH 0x1005
#define SOUND_THRUST 0x1019
/* the priority of SOUND_THRUST. each call plays it for THRUST_SOUND_STEPS + 1
 * steps */
#define THRUST_PRIORITY 1
#define THRUST_SOUND_STEPS 30

#define BASE_BOXES 2

static NJS_CNK_OBJECT lock_on_missile_body[1];
static NJS_CNK_OBJECT lock_on_missile_cover[1];
static NJS_CNK_OBJECT lock_on_missile_lamp[1];
static NJS_CNK_OBJECT lock_on_missile_base[1];

static struct collision_shape base_boxes[BASE_BOXES];
static NJS_VECTOR thrust_spd;
static Float thrust_jitter;
static Float thrust_rise;
static Float thrust_size_min;
static Float thrust_size_range;
static struct collision_shape flight_shape;

static void lock_on_missile_destroy(struct task *task);
static void lock_on_missile_step(struct task *task);
static void lock_on_missile_draw(struct task *task);
static void lock_on_missile_collision_init(struct task *task);
static void lock_on_missile_collision_step(struct task *task);
static void lock_on_missile_collision_destroy(struct task *task);

void lock_on_missile_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct lock_on_missile));
    if (task->motion.p == NULL) {
        return;
    }
#ifdef NONMATCHING
    /* this side makes the collision child before it sets the callbacks. where
     * task_create_child fails, it frees the record, and the init runs again at
     * the task's next step */
    if (task_create_child(TASK_WORK, lock_on_missile_collision_init, task) ==
        NULL) {
        syFree(task->motion.p);
        task->motion.p = NULL;
        return;
    }
#endif
    task->draw = lock_on_missile_draw;
    task->step = lock_on_missile_step;
    task->destructor = lock_on_missile_destroy;
    work->mode = MODE_WAIT_TARGET;
    MISSILE(task)->pos = work->pos;
    MISSILE(task)->pitch = 0;
    MISSILE(task)->roll = 0;
    MISSILE(task)->yaw = work->ang.y;
    MISSILE(task)->charge = 0.0F;
    work->ang.x &= MISSILE_LAUNCH_BITS | MISSILE_UPSIDE_DOWN;
    work->ang.z &= MISSILE_ID;
    /* @hack type: the half turn and 0 as unsigned, where signed would do */
    work->ang.z += (work->ang.x & MISSILE_UPSIDE_DOWN) ? (Uint32)HALF_TURN : 0U;
#ifdef NONMATCHING
    /* this side makes the child above */
#else
    task_create_child(TASK_WORK, lock_on_missile_collision_init, task);
#endif
    if (task->placement != NULL && placement_get_state(task)) {
        work->mode = MODE_SPENT;
    }
    collision_init(task, base_boxes, BASE_BOXES, COLLISION_OBJECT);
}

#ifdef NONMATCHING
/* set to null each target's *hit that points at position. this side's
 * MODE_LANDED step and destructor both call it, where the matching side clears
 * the targets in MODE_LANDED alone */
static void clear_target_positions(const NJS_POINT3 *position)
{
    int i;

    for (i = 0; i < MISSILE_TARGETS; i++) {
        NJS_POINT3 **hit = missile_targets[i].hit;
        if (hit != NULL && *hit == position) {
            *hit = NULL;
        }
    }
}
#endif

static void lock_on_missile_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == MODE_SPENT && task->placement != NULL) {
        placement_set_state(task, MISSILE_SPENT_STATE);
    }
#ifdef NONMATCHING
    /* this side sets each *hit that still points at the missile's position to
     * null. a missile destroyed in MODE_LANDED would leave it pointing into the
     * freed record */
    if (task->motion.p != NULL) {
        clear_target_positions(&MISSILE(task)->pos);
    }
#endif
    task_destroy_children(task);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void lock_on_missile_step(struct task *task)
{
    struct task_work *work = task->work;
    struct lock_on_missile *missile;
    struct missile_target *target;
    NJS_VECTOR vec;
    Float distance;
    int i;

    if (work->mode < MODE_FLY && placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: an else no path reaches, which masks game_state_steps,
     * game_state's steps, with MISSILE_ID and then MISSILE_UPSIDE_DOWN */
    if (1) {
    } else {
        game_state_steps = MISSILE_ID & game_state_steps;
        game_state_steps &= MISSILE_UPSIDE_DOWN;
    }
    /* @hack redundant: an if and its else, both empty */
    if (1) {
    } else {
    }
    if (!game_state.time_stopped || work->submode != 0) {
        switch (work->mode) {
        case MODE_WAIT_TARGET: {
            /* @hack redundant: two counts of the targets that wait for the
             * missile, split by TARGET_UNSHUT, where one count would do */
            int unshut_count;
            int shut_count;
            /* @hack spelling: pos copied a part at a time, x, z and y,
             * where one assignment would do */
            MISSILE(task)->pos.x = work->pos.x;
            MISSILE(task)->pos.z = work->pos.z;
            MISSILE(task)->pos.y = work->pos.y;
            MISSILE(task)->pitch = 0;
            MISSILE(task)->yaw = work->ang.y;
            MISSILE(task)->roll = 0;
            MISSILE(task)->spin = 0;
            if (task->child != NULL) {
                task->child->work->submode = 0;
            }
            work->mode = MODE_WAIT_SHUTTER;
            /* @hack spelling: the test of the switch inside the test of the
             * launch, where one test joined by && would do */
            if ((work->ang.x & MISSILE_LAUNCH_BITS) % MISSILE_LAUNCHES ==
                MISSILE_SWITCH) {
                if (!switch_is_on(work->ang.z & MISSILE_ID)) {
                    work->mode = MODE_WAIT_TARGET;
                }
            }
            shut_count = 0;
            unshut_count = 0;
            for (i = 0, target = missile_targets; i < MISSILE_TARGETS;
                 i++, target++) {
                if (TARGET_MISSILE_ID(target) == (work->ang.z & MISSILE_ID)) {
                    if (target->id & TARGET_UNSHUT) {
                        unshut_count++;
                    } else {
                        shut_count++;
                    }
                }
            }
            if (!(unshut_count || shut_count)) {
                work->mode = MODE_WAIT_TARGET;
            }
            if (work->mode == MODE_WAIT_SHUTTER) {
                MISSILE(task)->pos.y =
                    work->pos.y + ((work->ang.x & MISSILE_UPSIDE_DOWN)
                                       ? -START_HEIGHT
                                       : START_HEIGHT);
                /* @hack redundant: a second test of MODE_WAIT_SHUTTER, inside
                 * the first */
                if (work->mode == MODE_WAIT_SHUTTER) {
                    sound_play_at(SOUND_LAUNCH, NULL, 0, 0, &work->pos);
                }
            }
            break;
        }
        case MODE_WAIT_SHUTTER: {
            int shut_count;
            /* @hack spelling: found, set from shut_count, where a test of
             * shut_count would do */
            Bool found;

            found = FALSE;
            shut_count = 0;
            for (i = 0, target = missile_targets; i < MISSILE_TARGETS;
                 i++, target++) {
                if (TARGET_MISSILE_ID(target) == (work->ang.z & MISSILE_ID)) {
                    /* @hack spelling: an empty branch for a target with
                     * TARGET_UNSHUT, where a test of the bit clear would do */
                    if (target->id & TARGET_UNSHUT) {
                    } else {
                        shut_count++;
                    }
                }
            }
            /* @hack redundant: a test of i that no path reaches */
            if (0) {
                if (i) {
                }
            }
            if (shut_count) {
                found = TRUE;
            }
            if (found) {
                work->mode = MODE_RISE;
            }
            break;
        }
        case MODE_RISE:
            MISSILE(task)->spin += COVER_SPIN;
            if (work->ang.x & MISSILE_UPSIDE_DOWN) {
                MISSILE(task)->pos.y -= RISE_STEP;
                if (MISSILE(task)->pos.y <= work->pos.y - RAISED_HEIGHT) {
                    MISSILE(task)->pos.y = work->pos.y - RAISED_HEIGHT;
                    work->mode = MODE_WAIT_SHOT;
                }
            } else {
                MISSILE(task)->pos.y += RISE_STEP;
                if (MISSILE(task)->pos.y >= work->pos.y + RAISED_HEIGHT) {
                    MISSILE(task)->pos.y = work->pos.y + RAISED_HEIGHT;
                    work->mode = MODE_WAIT_SHOT;
                }
            }
            if (work->mode == MODE_WAIT_SHOT) {
                if (task->child != NULL && task->child->child == NULL) {
                    /* @bug logic: for a base upside down, places the weak
                     * point where an upright base has its cover. draw turns
                     * that base a half turn about z, so the cover it draws
                     * lies about 49 units away (njCalcPoint at 0x8c506af2,
                     * njRotateZ at 0x8c5074da) */
                    njPushMatrixEx();
                    njUnitMatrix(NULL);
                    njTranslateEx(&work->pos);
                    njRotateY(NULL, work->ang.y);
                    njCalcPoint(NULL, (NJS_POINT3 *)lock_on_missile_cover->pos,
                                &vec);
                    njPopMatrixEx();
                    weak_point_create(task->child, WEAK_POINT_INDEX, &vec);
                }
            }
            break;
        case MODE_WAIT_SHOT: {
            struct angles cover_ang;
            /* @hack redundant: a local that only sizes the frame */
            NJS_VECTOR unused;
            NJS_VECTOR cover_spd;

            MISSILE(task)->spin += COVER_SPIN;
            MISSILE(task)->roll = ROLL(work->timer);
            if (work->ang.x & MISSILE_UPSIDE_DOWN) {
                MISSILE(task)->pos.y =
                    work->pos.y + BOB(work->timer++) - RAISED_HEIGHT;
                if (task->child != NULL && task->child->work->submode) {
                    MISSILE(task)->charge = 0.0F;
                    work->mode = MODE_AIM;
                    work->timer = 0;
                }
            } else {
                MISSILE(task)->pos.y =
                    work->pos.y + BOB(work->timer++) + RAISED_HEIGHT;
                if (task->child != NULL && task->child->work->submode) {
                    MISSILE(task)->charge = 0.0F;
                    work->mode = MODE_AIM;
                }
            }
            /* @hack spelling: the break in an else, where breaking first for
             * any mode but MODE_AIM would do */
            if (work->mode == MODE_AIM) {
                /* @bug logic: for a base upside down, throws the cover from
                 * where an upright base has it, as for the weak point above
                 * (njCalcPoint at 0x8c506c5e) */
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateY(NULL, work->ang.y);
                njCalcPoint(NULL, (NJS_POINT3 *)lock_on_missile_cover->pos,
                            &vec);
                njPopMatrix(1);
            } else {
                break;
            }
            cover_ang.x = 0;
            cover_ang.y = work->ang.y + MISSILE(task)->spin;
            cover_ang.z = 0;
            cover_spd.x = 0.0F;
            cover_spd.y = COVER_SPEED_Y;
            cover_spd.z = COVER_SPEED_Z;
            debris_launch_flags(
                &vec, &cover_ang, work->pos.y, &cover_spd,
                lock_on_missile_cover->model, &lock_on_missile_texlist,
                DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH);
            break;
        }
        case MODE_AIM: {
            struct angles aim;
            NJS_VECTOR tail;
            Sint16 turn_yaw;
            Sint16 turn_pitch;
            Float dx;
            Float dz;

            /* @hack spelling: the way to the destination as -pos + scl,
             * where scl - pos would do */
            vec.x = -MISSILE(task)->pos.x + work->scl.x;
            vec.y = -MISSILE(task)->pos.y + work->scl.y;
            vec.z = -MISSILE(task)->pos.z + work->scl.z;
            /* @hack spelling: dz and dx copied from vec in the test, where
             * vec.z and vec.x read in place would do */
            if (dz = vec.z, dx = vec.x, work->ang.x & MISSILE_UPSIDE_DOWN) {
                aim.z = -(Angle)(atan2f(vec.y, njSqrt((dz * dz) + (dx * dx))) *
                                 ANGLE_PER_RADIAN);
                aim.y = -(Angle)(atan2f(vec.z, vec.x) * ANGLE_PER_RADIAN) +
                        HALF_TURN;
            } else {
                aim.z = (Angle)(atan2f(vec.y, njSqrt((dz * dz) + (dx * dx))) *
                                ANGLE_PER_RADIAN);
                aim.y = -(Angle)(atan2f(vec.z, vec.x) * ANGLE_PER_RADIAN);
            }
            /* @hack spelling: each turn as -angle + aim, where aim - angle
             * would do */
            turn_pitch = -(Sint16)MISSILE(task)->pitch + aim.z;
            turn_yaw = -(Sint16)MISSILE(task)->yaw + aim.y;
            if (turn_yaw > 0) {
                if (turn_yaw > AIM_TURN) {
                    turn_yaw = AIM_TURN;
                }
            } else if (turn_yaw < -AIM_TURN) {
                turn_yaw = -AIM_TURN;
            }
            if (turn_yaw == 0) {
                if (turn_pitch > 0) {
                    if (turn_pitch > AIM_TURN) {
                        turn_pitch = AIM_TURN;
                    }
                } else if (turn_pitch < -AIM_TURN) {
                    turn_pitch = -AIM_TURN;
                }
            } else {
                turn_pitch = 0;
            }
            MISSILE(task)->yaw += turn_yaw;
            MISSILE(task)->pitch += turn_pitch;
            MISSILE(task)->roll = ROLL(work->timer);
            if (work->ang.x & MISSILE_UPSIDE_DOWN) {
                MISSILE(task)->pos.y =
                    work->pos.y + BOB(work->timer++) - RAISED_HEIGHT;
            } else {
                MISSILE(task)->pos.y =
                    work->pos.y + BOB(work->timer++) + RAISED_HEIGHT;
            }
            tail.x = TAIL_X;
            tail.y = 0.0F;
            tail.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &MISSILE(task)->pos);
            njRotateY(NULL, MISSILE(task)->yaw);
            njRotateZ(NULL, MISSILE(task)->pitch);
            njCalcPoint(NULL, &tail, &tail);
            /* @hack spelling: vec reused for the puff's speed, where a local of
             * its own would do */
            njCalcVector(NULL, &thrust_spd, &vec);
            njPopMatrix(1);
            sound_play_at_for(SOUND_THRUST, work, THRUST_PRIORITY, 0,
                              THRUST_SOUND_STEPS, &tail);
            if (rand() * RAND_SCALE < THRUST_CHANCE) {
                Float size;
                vec.x += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
                vec.y += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
                vec.z += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
                size =
                    (rand() * thrust_size_range * RAND_SCALE) + thrust_size_min;
                rocket_thrust_settle_create(&tail, &vec, size,
                                            work->pos.y + thrust_rise + size);
            }
            /* @hack spelling: missile, a local for the record, where
             * MISSILE(task) read in place would do */
            missile = task->motion.p;
            if ((Uint16)missile->yaw == (Uint16)aim.y &&
                (Uint16)missile->pitch == (Uint16)aim.z) {
                MISSILE(task)->charge += CHARGE_STEP;
                if (MISSILE(task)->charge >= FULL_CHARGE) {
                    MISSILE(task)->charge = FULL_CHARGE;
                    work->timer = 0;
                    work->mode = MODE_FLY;
                }
            }
            break;
        }
        case MODE_FLY: {
            NJS_VECTOR tail;
            Float scale;

            /* @hack spelling: the way to the destination as -pos + scl,
             * where scl - pos would do */
            vec.x = -MISSILE(task)->pos.x + work->scl.x;
            vec.y = -MISSILE(task)->pos.y + work->scl.y;
            vec.z = -MISSILE(task)->pos.z + work->scl.z;
            /* @hack spelling: scale, a copy of distance that the test reads,
             * where distance would do */
            distance = scale = njScalor(&vec);
            /* @hack redundant: a scaling of vec by scale that no path
             * reaches */
            if (0) {
                vec.x *= scale;
                vec.y *= scale;
                vec.z *= scale;
            }
            if (scale < FLIGHT_STEP) {
                MISSILE(task)->pos.x += vec.x;
                MISSILE(task)->pos.y += vec.y;
                MISSILE(task)->pos.z += vec.z;
                for (i = 0, target = missile_targets; i < MISSILE_TARGETS;
                     i++, target++) {
                    if (TARGET_MISSILE_ID(target) ==
                            (work->ang.z & MISSILE_ID) &&
                        target->hit != NULL) {
                        *target->hit = &MISSILE(task)->pos;
                    }
                }
                work->mode = MODE_LANDED;
            } else {
                scale = FLIGHT_STEP / scale;
                vec.x *= scale;
                vec.y *= scale;
                vec.z *= scale;
                MISSILE(task)->pos.x += vec.x;
                MISSILE(task)->pos.y += vec.y;
                MISSILE(task)->pos.z += vec.z;
            }
            if (work->mode == MODE_LANDED) {
#ifdef NONMATCHING
                /* this side repairs the bug tagged below: it gives the burst
                 * the missile's last move */
                tail = vec;
#endif
                /* @bug uninitialized: scales tail as the burst's speed before
                 * the step sets tail (FMOV.S at 0x8c507242, 0x8c507250 and
                 * 0x8c50725a) */
                tail.x *= BURST_SPEED_SCALE;
                tail.y *= BURST_SPEED_SCALE;
                tail.z *= BURST_SPEED_SCALE;
                burst_create_wobbling(&MISSILE(task)->pos, &tail, BURST_SCALE);
            }
            tail.x = TAIL_X;
            tail.y = 0.0F;
            tail.z = 0.0F;
            vec.x *= FLIGHT_PUFF_SPEED;
            vec.y *= FLIGHT_PUFF_SPEED;
            vec.z *= FLIGHT_PUFF_SPEED;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &MISSILE(task)->pos);
            njRotateY(NULL, MISSILE(task)->yaw);
            njRotateZ(NULL, MISSILE(task)->pitch);
            njCalcPoint(NULL, &tail, &tail);
            njPopMatrix(1);
            sound_play_at_for(SOUND_THRUST, work, THRUST_PRIORITY, 0,
                              THRUST_SOUND_STEPS, &tail);
            vec.x += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
            vec.y += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
            vec.z += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
            /* @hack type: game_state's steps read as game_state_steps, where
             * game_state.steps would do */
            if (!(game_state_steps & FLIGHT_PUFF_SKIP) &&
                distance > THRUST_DISTANCE) {
                rocket_thrust_create(
                    &tail, &vec,
                    (rand() * (FLIGHT_SIZE_RANGE * RAND_SCALE)) +
                        FLIGHT_SIZE_MIN);
            }
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side: the flight
             * pops no matrix it didn't push */
            break;
#else
            /* @bug logic: pops a matrix the step didn't push, at each step of
             * the flight (JSR at 0x8c507370, after the pop at 0x8c5072d6) */
            njPopMatrix(1);
            break;
#endif
        }
        case MODE_LANDED:
            work->mode = MODE_SPENT;
            MISSILE(task)->pos = work->pos;
            MISSILE(task)->pitch = 0;
            MISSILE(task)->yaw = work->ang.y;
            MISSILE(task)->roll = 0;
            MISSILE(task)->charge = 0.0F;
            work->submode = 0;
#ifdef NONMATCHING
            /* this side clears the targets with clear_target_positions, which
             * its destructor shares */
            clear_target_positions(&MISSILE(task)->pos);
#else
            for (i = 0, target = missile_targets; i < MISSILE_TARGETS;
                 i++, target++) {
                if (target->hit != NULL &&
                    *target->hit == &MISSILE(task)->pos) {
                    *target->hit = NULL;
                }
            }
#endif
            break;
        /* @hack redundant: MODE_SPENT's case, which the default would take as
         * well */
        case MODE_SPENT:
            break;
        default:
            break;
        }
    }
    collision_register(task);
}

static void lock_on_missile_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&lock_on_missile_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    /* @hack spelling: njRotateY in both arms, where one call before the test
     * would do */
    if (work->ang.x & MISSILE_UPSIDE_DOWN) {
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, HALF_TURN);
    } else {
        njRotateY(NULL, work->ang.y);
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(lock_on_missile_base->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)lock_on_missile_lamp->pos);
    if (work->mode == MODE_NEW || work->mode == MODE_WAIT_TARGET ||
        work->mode == MODE_WAIT_SHUTTER) {
        njCnkEasyDrawModel(lock_on_missile_lamp->model);
    } else {
        model_draw_tinted(lock_on_missile_lamp->model,
                          blink_light_gray(LAMP_BLINK_PERIOD, LAMP_BLINK_REST,
                                           LAMP_BLINK_FADE));
    }
    njPopMatrixEx();
    if (work->mode != MODE_AIM && work->mode != MODE_FLY &&
        work->mode != MODE_LANDED && work->mode != MODE_SPENT) {
        njTranslateEx((NJS_VECTOR *)lock_on_missile_cover->pos);
        njRotateY(NULL, MISSILE(task)->spin);
        njCnkEasyDrawModel(lock_on_missile_cover->model);
    }
    njPopMatrix(1);
    njPushMatrix(NULL);
    njTranslateV(NULL, &MISSILE(task)->pos);
    njRotateY(NULL, MISSILE(task)->yaw);
    njRotateZ(NULL, MISSILE(task)->pitch);
    njRotateX(NULL, MISSILE(task)->roll);
    if (work->ang.x & MISSILE_UPSIDE_DOWN) {
        njRotateZ(NULL, HALF_TURN);
    }
    if (work->mode != MODE_WAIT_TARGET && work->mode != MODE_SPENT &&
        work->mode != MODE_LANDED) {
        njCnkEasyDrawModel(lock_on_missile_body->model);
    }
    njPopMatrix(1);
}

#define FOLLOW_MISSILE(work, parent, parent_work)                              \
    do {                                                                       \
        (work)->pos = MISSILE(parent)->pos;                                    \
        (work)->ang.x = 0;                                                     \
        (work)->ang.y =                                                        \
            MISSILE(parent)->yaw +                                             \
            (((parent_work)->ang.x & MISSILE_UPSIDE_DOWN) ? HALF_TURN : 0);    \
        (work)->ang.z = MISSILE(parent)->pitch;                                \
    } while (0)

/* the first step of the child that carries the missile's collision */
static void lock_on_missile_collision_init(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    struct task_work *parent_work = parent->work;

    if (parent->motion.p != NULL) {
#ifdef NONMATCHING
        /* where collision_init fails, this side leaves this init as the child's
         * step, to run again at the next */
        if (!collision_init(task, &flight_shape, 1, COLLISION_OBJECT)) {
            return;
        }
#else
        collision_init(task, &flight_shape, 1, COLLISION_OBJECT);
#endif
        task->step = lock_on_missile_collision_step;
        task->destructor = lock_on_missile_collision_destroy;
        FOLLOW_MISSILE(work, parent, parent_work);
    }
}

static void lock_on_missile_collision_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    struct task_work *parent_work = parent->work;

    if (parent->motion.p != NULL) {
        FOLLOW_MISSILE(work, parent, parent_work);
        if (parent_work->mode == MODE_FLY) {
            if (STEPS_FLOWN(parent) > FLIGHT_SAFE_STEPS) {
                work->collision->shapes->levels =
                    (work->collision->shapes->levels & ~SHAPE_ATTACK) |
                    FLIGHT_ATTACK;
            } else {
                STEPS_FLOWN(parent)++;
            }
        } else {
            /* @hack spelling: levels assigned its own value masked, where
             * &= would do */
            work->collision->shapes->levels =
                work->collision->shapes->levels & ~SHAPE_ATTACK;
            STEPS_FLOWN(parent) = 0;
        }
        collision_register(task);
    }
}

static void lock_on_missile_collision_destroy(struct task *task)
{
    /* @hack redundant: the reads of the missile and its work, which nothing
     * uses */
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
}

static Sint16 lock_on_missile_body_polygons[433] = {
#include "stg11/lock_on_missile/lock_on_missile_body_polygons.inc"
};

static Sint32 lock_on_missile_body_vertices[279] = {
#include "stg11/lock_on_missile/lock_on_missile_body_vertices.inc"
};

static NJS_CNK_MODEL lock_on_missile_body_model = {
#include "stg11/lock_on_missile/lock_on_missile_body_model.inc"
};

static NJS_CNK_OBJECT lock_on_missile_body[1] = {
#include "stg11/lock_on_missile/lock_on_missile_body.inc"
};

static Sint16 lock_on_missile_cover_polygons[147] = {
#include "stg11/lock_on_missile/lock_on_missile_cover_polygons.inc"
};

static Sint32 lock_on_missile_cover_vertices[143] = {
#include "stg11/lock_on_missile/lock_on_missile_cover_vertices.inc"
};

static NJS_CNK_MODEL lock_on_missile_cover_model = {
#include "stg11/lock_on_missile/lock_on_missile_cover_model.inc"
};

static NJS_CNK_OBJECT lock_on_missile_cover[1] = {
#include "stg11/lock_on_missile/lock_on_missile_cover.inc"
};

static Sint16 lock_on_missile_lamp_polygons[25] = {
#include "stg11/lock_on_missile/lock_on_missile_lamp_polygons.inc"
};

static Sint32 lock_on_missile_lamp_vertices[19] = {
#include "stg11/lock_on_missile/lock_on_missile_lamp_vertices.inc"
};

static NJS_CNK_MODEL lock_on_missile_lamp_model = {
#include "stg11/lock_on_missile/lock_on_missile_lamp_model.inc"
};

static NJS_CNK_OBJECT lock_on_missile_lamp[1] = {
#include "stg11/lock_on_missile/lock_on_missile_lamp.inc"
};

static Sint16 lock_on_missile_base_polygons[383] = {
#include "stg11/lock_on_missile/lock_on_missile_base_polygons.inc"
};

static Sint32 lock_on_missile_base_vertices[379] = {
#include "stg11/lock_on_missile/lock_on_missile_base_vertices.inc"
};

static NJS_CNK_MODEL lock_on_missile_base_model = {
#include "stg11/lock_on_missile/lock_on_missile_base_model.inc"
};

static NJS_CNK_OBJECT lock_on_missile_base[1] = {
#include "stg11/lock_on_missile/lock_on_missile_base.inc"
};

NJS_TEXLIST *lock_on_missile_texlists[4] = {
#include "stg11/lock_on_missile/lock_on_missile_texlists.inc"
};
/* the base's collision shapes */
static struct collision_shape base_boxes[BASE_BOXES] = {
#include "stg11/lock_on_missile/base_boxes.inc"
};
/* an aiming missile's puff of thrust moves at thrust_spd, turned by the
 * missile's yaw and pitch */
static NJS_VECTOR thrust_spd = {
#include "stg11/lock_on_missile/thrust_spd.inc"
};
/* each part of a puff's speed then changes at random by up to half of
 * thrust_jitter either way */
static Float thrust_jitter = {
#include "stg11/lock_on_missile/thrust_jitter.inc"
};
/* a puff settles on a floor thrust_rise plus the puff's size above the base's
 * pos */
static Float thrust_rise = {
#include "stg11/lock_on_missile/thrust_rise.inc"
};
/* a puff takes a random size from thrust_size_min up to thrust_size_min +
 * thrust_size_range */
static Float thrust_size_min = {
#include "stg11/lock_on_missile/thrust_size_min.inc"
};
static Float thrust_size_range = {
#include "stg11/lock_on_missile/thrust_size_range.inc"
};
/* the set editor's labels of the launches, then of each for a base upside
 * down */
static const char *launch_labels[2 * MISSILE_LAUNCHES] = {
#include "stg11/lock_on_missile/launch_labels.inc"
};
/* the missile's collision shape, which the child carries */
static struct collision_shape flight_shape = {
#include "stg11/lock_on_missile/flight_shape.inc"
};
