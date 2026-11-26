/* the launcher's draw, and its rocket's init and callbacks. shared/rocket.c
 * includes them for ROCKET, and stg30/stage_rocket.c for Final Rush's
 * STROCKET, after their rocket_step. a rocket is a child task of its
 * launcher */
#ifndef SHARED_ROCKET_BODY_H
#define SHARED_ROCKET_BODY_H
#include "vmu/vibration.h"

static void rocket_draw_base(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    ROCKET_TRANSLATE(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&rocket_base_texlist);
    njCnkSimpleDrawMotion(&rocket_base_object, &rocket_base_motion,
                          LAUNCHER(task)->base_frame);
    ROCKET_POP();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void rocket_body_init(struct task *task)
{
    struct task *launcher = task->parent;
    struct task_work *work = task->work;
    struct task_work *launcher_work = launcher->work;
    NJS_VECTOR start;

    if (launcher->motion.p == NULL) {
        return;
    }
    collision_init(task, body_shapes, BODY_SHAPES, COLLISION_OBJECT);
    /* @hack spelling: the mask written out, where &= would do */
    work->collision->shapes->levels =
        work->collision->shapes->levels & ~SHAPE_ATTACK;
    task->step = rocket_body_step;
    task->destructor = rocket_body_destroy;
    task->draw = rocket_body_draw;
    work->pos = launcher_work->pos;
    work->ang.x = 0;
    work->ang.y = launcher_work->ang.y;
    work->ang.z = (launcher_work->submode == ROCKET_UPRIGHT) ? QUARTER_TURN : 0;
#ifdef FINAL_RUSH_ROCKET
    /* @bug logic: an upright launcher creeps along its heading. each rocket
     * it makes moves it WAIT_UPRIGHT_X further (FMOV.S at 0x8c502ecc and
     * 0x8c502ed8), and rocket_step makes another after each launch (JSR at
     * 0x8c502738). the launcher's collision stays where stage_rocket_init put
     * it (0x8c502580), so the base draws further from it at each launch */
    if (launcher_work->submode == ROCKET_UPRIGHT) {
        start.x = WAIT_UPRIGHT_X;
        start.y = 0.0F;
        start.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, launcher_work->ang.y);
        njCalcVector(NULL, &start, &start);
        ROCKET_POP();
        launcher_work->pos.x += start.x;
        launcher_work->pos.z += start.z;
        work->collision->shapes->center.y = -body_shapes[BODY_HULL].center.y;
    }
#else
    /* @hack spelling: the whole move written in each branch, where one move
     * whose start.y the test picks would do */
    if (launcher_work->submode == ROCKET_UPRIGHT) {
        start.x = START_X;
        start.y = START_UPRIGHT_Y;
        start.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, launcher_work->ang.y);
        njCalcVector(NULL, &start, &start);
        ROCKET_POP();
        work->pos.x += start.x;
        work->pos.y += start.y;
        work->pos.z += start.z;
        work->collision->shapes->center.y = -body_shapes[BODY_HULL].center.y;
    } else {
        start.x = START_X;
        start.y = START_Y;
        start.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, launcher_work->ang.y);
        njCalcVector(NULL, &start, &start);
        ROCKET_POP();
        work->pos.x += start.x;
        work->pos.y += start.y;
        work->pos.z += start.z;
    }
#endif
    work->submode = NO_RIDER;
    /* @hack redundant: the mode and RIDING_STEPS set to 0, though
     * task_create_child left them 0 */
    work->mode = BODY_ON_BASE;
    RIDING_STEPS(task) = 0;
}

static void rocket_body_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct task *launcher = task->parent;
    struct task_work *launcher_work = launcher->work;
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;

    ROCKET_PUSH_BODY();
    ROCKET_TRANSLATE(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, QUARTER_TURN);
    njRotateZ(NULL, -QUARTER_TURN);
    njRotateX(NULL, QUARTER_TURN);
    /* @hack spelling: a call in each branch, where one call that picks the
     * sign of body_model_offset.x would do */
    if (launcher_work->submode == ROCKET_UPRIGHT) {
        njTranslate(NULL, body_model_offset.x, -body_model_offset.y,
                    -body_model_offset.z);
    } else {
        njTranslate(NULL, -body_model_offset.x, -body_model_offset.y,
                    -body_model_offset.z);
    }
    njSetTexture(&rocket_texlist);
    if (launcher_work->submode == ROCKET_UPRIGHT) {
        njRotateY(NULL, HALF_TURN);
    }
    /* @hack spelling: object and motion, locals set ahead of the test,
     * where each passed in place would do */
    motion = &rocket_motion;
    object = &rocket_object;
    if (LAUNCHER(launcher)->body == task) {
        njCnkSimpleDrawMotion(object, motion, LAUNCHER(launcher)->body_frame);
    } else {
        njCnkSimpleDrawMotion(object, motion, BODY_FRAMES);
    }
    ROCKET_POP();
}

static void rocket_body_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *launcher = task->parent;
    struct task_work *launcher_work = launcher->work;
    NJS_VECTOR vec;
    NJS_POINT3 point;
#ifndef FINAL_RUSH_ROCKET
    struct angles ang;
    NJS_POINT3 offset;
#endif
    Float distance;

    if (launcher->motion.p == NULL) {
        return;
    }
    if (work->submode >= 0) {
        if (RIDER_CAN_MOVE(launcher_work, work->submode)) {
#ifndef FINAL_RUSH_ROCKET
            if (!game_state.time_stopped) {
                CAUGHT_IN_STOP(launcher_work) = 0;
            }
#endif
            RIDING_STEPS(task)++;
            /* @hack spelling: the way to the place as -pos + scl, where
             * scl - pos would do */
            vec.x = -work->pos.x + launcher_work->scl.x;
            vec.y = -work->pos.y + launcher_work->scl.y;
            vec.z = -work->pos.z + launcher_work->scl.z;
            if (work->mode > BODY_HELD) {
                distance = njScalor(&vec);
                if (FLIGHT_OVER(distance)) {
                    /* @bug uninitialized: the step adds point to the
                     * rocket's pos before any path sets point, and the burst
                     * starts where that leaves the rocket. the FMOV.S are at
                     * 0xd2c, 0xd42 and 0xd50 of the object's code */
#ifdef NONMATCHING
                    /* this side repairs the bug tagged above: point is 0,
                     * and the burst starts at the rocket's pos */
                    point.x = 0.0F;
                    point.y = 0.0F;
                    point.z = 0.0F;
#endif
                    work->pos.x += point.x;
                    work->pos.y += point.y;
                    work->pos.z += point.z;
                    point.x = 0.0F;
                    point.y = 0.0F;
                    point.z = 0.0F;
                    burst_create_wobbling(&work->pos, &point, BURST_SCALE);
                    task_kill(task);
#ifndef FINAL_RUSH_ROCKET
                    player_request(work->submode, PLAYER_REQUEST_RELEASE, 0);
#endif
                    return;
                }
                /* @hack spelling: distance reused for the share of the way a
                 * step flies, where a local of its own would do */
                distance = FLIGHT_STEP / distance;
                vec.x *= distance;
                vec.y *= distance;
                vec.z *= distance;
                work->pos.x += vec.x;
                work->pos.y += vec.y;
                work->pos.z += vec.z;
                /* @hack redundant: the flight tested again inside the if that
                 * tests it, where the test of RIDING_STEPS alone would do */
                if (work->mode > BODY_HELD &&
                    RIDING_STEPS(task) % RUMBLE_STEPS == 0) {
                    /* @bug logic: no shake has RUMBLE_FIRST_POWER. here
                     * RIDING_STEPS is a positive multiple of RUMBLE_STEPS,
                     * never below RUMBLE_FIRST_STEPS. the CMP/GE is at 0xe64
                     * of the object's code */
                    /* @hack spelling: a call in each branch, where one call
                     * that picks the power would do */
                    if (RIDING_STEPS(task) < RUMBLE_FIRST_STEPS) {
                        vibration_start(work->submode, RUMBLE_FIRST_POWER,
                                        RUMBLE_FREQ, RUMBLE_TIME);
                    } else {
                        vibration_start(work->submode, RUMBLE_POWER,
                                        RUMBLE_FREQ, RUMBLE_TIME);
                    }
                }
            }
            if (work->mode > BODY_HELD && !(ROCKET_STEPS & FLIGHT_PUFF_BIT)) {
                point.x = 0.0F;
                point.y = FLIGHT_TAIL_Y;
                point.z = 0.0F;
                vec.x *= FLIGHT_PUFF_SHARE;
                vec.y *= FLIGHT_PUFF_SHARE;
                vec.z *= FLIGHT_PUFF_SHARE;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                ROCKET_TRANSLATE(&work->pos);
                njRotateY(NULL, work->ang.y);
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, QUARTER_TURN);
                njRotateZ(NULL, -QUARTER_TURN);
                njRotateX(NULL, QUARTER_TURN);
                /* @hack spelling: a call in each branch, where one call that
                 * picks the sign of body_model_offset.x would do */
                if (launcher_work->submode == ROCKET_UPRIGHT) {
                    njTranslate(NULL, body_model_offset.x, -body_model_offset.y,
                                -body_model_offset.z);
                } else {
                    njTranslate(NULL, -body_model_offset.x,
                                -body_model_offset.y, -body_model_offset.z);
                }
                vec.x += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
                vec.y += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
                vec.z += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
                njCalcPoint(NULL, &point, &point);
                rocket_thrust_create(
                    &point, &vec,
                    (rand() * (FLIGHT_SIZE_RANGE * RAND_SCALE)) +
                        FLIGHT_SIZE_MIN);
                ROCKET_POP();
            }
            work->collision->shapes[BODY_GRIP].flags |= SHAPE_OFF;
            if (RIDING_STEPS(task) > HULL_SAFE_STEPS) {
                work->collision->shapes[BODY_HULL].levels =
                    (work->collision->shapes[BODY_HULL].levels &
                     ~SHAPE_ATTACK) |
                    HULL_ATTACK;
            }
            /* @hack redundant: the rider tested again inside the if that
             * tests it, where the block alone would do */
            if (work->submode >= 0) {
                njPushMatrixEx();
                njUnitMatrix(NULL);
                ROCKET_TRANSLATE(&work->pos);
                njRotateY(NULL, work->ang.y);
#ifdef FINAL_RUSH_ROCKET
                /* @bug logic: a held rider stands at rider_flight_offset, not
                 * rider_hold_offset. the test compares the launcher's body
                 * with the launcher, not the rocket (CMP/EQ at 0x8c503374) */
                if (LAUNCHER(launcher)->body == launcher) {
                    njCalcPoint(NULL, &rider_hold_offset, &point);
                } else {
                    njCalcPoint(NULL, &rider_flight_offset, &point);
                }
#else
                njRotateZ(NULL, (work->mode != BODY_HELD &&
                                 launcher_work->submode == ROCKET_UPRIGHT)
                                    ? QUARTER_TURN
                                    : 0);
                current_matrix_angles_yxz(&ang.x, &ang.y, &ang.z);
                if (work->mode == BODY_HELD) {
                    njCalcPoint(NULL, &rider_hold_offset, &point);
                } else {
                    offset.z = 0.0F;
                    /* @bug uninitialized: a rider whose kind the switch
                     * leaves out takes its place from offset.x and offset.y
                     * unset (JSR at 0x1190 of the object's code) */
#ifdef NONMATCHING
                    /* this side repairs the tag above: a kind the switch
                     * leaves out takes an x and a y of 0 */
                    offset.x = 0.0F;
                    offset.y = 0.0F;
#endif
                    /* where each kind of rider sits in flight, from the
                     * rocket's pos in the rocket's axes */
                    switch (player_records[work->submode]->kind) {
                    case PLAYER_SONIC:
                    case PLAYER_SHADOW:
                        offset.x = -4.3F;
                        offset.y = -4.9F;
                        break;
                    case PLAYER_KIND_METALSONIC:
                        offset.x = -4.0F;
                        offset.y = -6.0F;
                        break;
                    case PLAYER_ROUGE:
                        offset.x = -3.5F;
                        offset.y = -5.6F;
                        break;
                    case PLAYER_KIND_AMY:
                        offset.x = -4.0F;
                        offset.y = -5.4F;
                        break;
                    case PLAYER_KIND_CHAOS0:
                        offset.x = -6.0F;
                        offset.y = -6.0F;
                        break;
                    case PLAYER_KIND_TICAL:
                        offset.x = -4.1F;
                        offset.y = -5.3F;
                        break;
                    case PLAYER_KNUCKLES:
                        offset.x = -5.6F;
                        offset.y = -6.0F;
                        break;
                    /* @hack redundant: a default that does nothing, where
                     * none would do */
                    default:
                        break;
                    }
                    njCalcPoint(NULL, &offset, &point);
                }
#endif
                ROCKET_POP();
                player_set_pos(work->submode, point.x, point.y, point.z);
#ifdef FINAL_RUSH_ROCKET
                player_work[work->submode]->ang.x = 0;
                player_work[work->submode]->ang.y = -work->ang.y;
                player_work[work->submode]->ang.z = 0;
#else
                player_work[work->submode]->ang.x = ang.x;
                player_work[work->submode]->ang.y = -ang.y;
                player_work[work->submode]->ang.z = ang.z;
#endif
                sound_play_for(SOUND_THRUST, launcher_work, THRUST_PRIORITY,
                               THRUST_VOLUME, THRUST_DURATION);
            }
        }
    } else {
        work->collision->shapes[BODY_GRIP].flags &= ~SHAPE_OFF;
        /* @hack spelling: the mask written out, where &= would do */
        work->collision->shapes[BODY_HULL].levels =
            work->collision->shapes[BODY_HULL].levels & ~SHAPE_ATTACK;
    }
    collision_register(task);
}

/* the destructor of a flying rocket sends its rider PLAYER_REQUEST_RELEASE,
 * and counts the rider off the launcher's riders. outside Final Rush, the step
 * that bursts a rocket has sent the same request already. the rider then gets
 * it twice */
static void rocket_body_destroy(struct task *task)
{
    struct task_work *work = task->work;
    struct task *launcher = task->parent;

    if (work->mode > BODY_HELD) {
        player_request(work->submode, PLAYER_REQUEST_RELEASE, 0);
        LAUNCHER(launcher)->riders--;
    }
    /* @hack spelling: one test inside the other, where && would do */
    if (launcher->motion.p != NULL) {
        if (LAUNCHER(launcher)->body == task) {
            LAUNCHER(launcher)->body = NULL;
        }
    }
    /* @hack redundant: force cleared, though task_create_child left it
     * null */
    task->force.p = NULL;
}

#endif
