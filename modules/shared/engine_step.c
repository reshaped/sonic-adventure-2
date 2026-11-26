/* the moving platforms of engine_step.h. under COLONY_STEP, which
 * stg27/step.c defines, the source is Lost Colony's STEP alone */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/core_platform_path.h"
#include "sound/sound.h"
#include "shared/engine_step.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a platform's record, at its task's motion. stop_timer counts the steps until
 * STOP_SOUND plays, or is 0 */
struct step_motion {
    struct core_platform_path path;
    Sint32 stop_timer;
};
#define MOTION(task) ((struct step_motion *)(task)->motion.p)
#define TRACK(task) (&MOTION(task)->path)
/* a rising or sinking platform vibrates player 0's pad each time it starts to
 * move */
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 20
#define IDLE_SOUND 10
#define TRAVEL_SOUND 17
#define ARRIVAL_SOUND 20
#define STOP_SOUND 18
#define IDLE_SOUND_STEPS 120
/* an Eternal Engine platform plays STOP_SOUND STOP_SOUND_DELAY steps after it
 * stops. a rising one plays it only at the top of its rise */
#define STOP_SOUND_DELAY 10
/* the scale of the set editor's box at an arc's pivot */
#define PIVOT_SCALE 5.0F
#ifdef COLONY_STEP
/* a platform within AXIS_RADIUS of the stage's y axis doesn't bob. at the far
 * end of a rise, a ride on it doesn't send it back */
#define AXIS_RADIUS 50.0F
#define LIGHT_HEIGHT 5.0F
/* the light volume is the idle sound's key */
#define IDLE_KEY(task) ((task)->child)
#define TRAVEL_SOUND_STEPS 30
/* the node is in the world's collision only while a player is within
 * NEAR_RADIUS of it */
#define NEAR_RADIUS(task) 80.0F
#else
#define IDLE_KEY(task) (task)
#define TRAVEL_SOUND_STEPS 5
#define NEAR_MARGIN 20.0F
#define NEAR_RADIUS(task) (TRACK(task)->collision.model->r + NEAR_MARGIN)
#endif
#define RAILS 2

/* the bob's height is bob_amplitude times the sine of its angle. the angle
 * turns bob_speed a step, from a start that origin's length sets */
static Sint32 bob_speed = 512;
static Float bob_amplitude = 3.0F;
/* STEP's rails, a box along each side */
/* clang-format off */
static struct collision_shape rails[RAILS] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {19.0F, 5.5F, 0.0F},
     2.5F, 5.5F, 21.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {-19.0F, 5.5F, 0.0F},
     2.5F, 5.5F, 21.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void step_destroy(struct task *task);
static void step_update(struct task *task);
static void step_draw(struct task *task);
#ifndef COLONY_STEP
static void step_b_draw(struct task *task);
#endif

#ifdef COLONY_STEP
#define STEP_INIT colony_step_init
#else
#define STEP_INIT engine_step_init
#endif
#define STEP_NAME(name) step_##name
#include "shared/engine_step_init_impl.h"
#undef STEP_INIT
#undef STEP_NAME
#ifndef COLONY_STEP
#define STEP_B
#define STEP_INIT engine_step_b_init
#define STEP_NAME(name) step_b_##name
#include "shared/engine_step_init_impl.h"
#undef STEP_NAME
#undef STEP_INIT
#undef STEP_B
#endif

static void step_destroy(struct task *task)
{
    core_path_release(task);
}

static void step_update(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 previous;
    NJS_VECTOR offset;
    Float fraction;
    Float speed;
    int period;
    Bool riding;
#ifdef COLONY_STEP
    Bool on_axis;
#endif
    Uint32 frame;
#ifdef COLONY_STEP
    NJS_POINT3 *light;
#endif

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, of the set editor's readout
     * and a restart of the path, whose strings SHC still places in this
     * source's constants */
    if (0) {
#ifdef COLONY_STEP
        (void)"<- ASIBATYPE";
        (void)"<- ROT Y";
#else
        njPrintC(NJM_LOCATION(17, 16), "<- ASIBATYPE");
        njPrintC(NJM_LOCATION(17, 17), "<- ROT Y");
#endif
        switch (PATH_KIND(work)) {
        case PATH_ARC:
            njPrintC(NJM_LOCATION(17, 18), "<- ROTY TO CENTER");
            njPrintC(NJM_LOCATION(17, 19), "<- RADIUS");
            break;
        case PATH_HORIZONTAL:
            njPrintC(NJM_LOCATION(17, 18), "<- ROTY-HORZ");
            njPrintC(NJM_LOCATION(17, 19), "<- LENGTH");
            break;
        case PATH_VERTICAL:
            njPrintC(NJM_LOCATION(17, 15), "   COLSIZE");
            njPrintC(NJM_LOCATION(17, 19), "<- LENGTH");
        }
        njPrintC(NJM_LOCATION(17, 20), "<- SPD");
        njPrintC(NJM_LOCATION(17, 21), "<- STOPTIME");
        TRACK(task)->origin = work->pos;
        if (work->scl.x < MIN_RADIUS) {
            TRACK(task)->pivot = work->pos;
        } else {
            previous.x = work->scl.x;
            previous.z = previous.y = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.z);
            njCalcPoint(NULL, &previous, &previous);
            njPopMatrixEx();
            TRACK(task)->pivot = previous;
        }
        TRACK(task)->bob_height = 0.0F;
        TRACK(task)->bob_phase = 0;
        TRACK(task)->orbit_angle = 0;
    } else {
        sound_play_at_for(IDLE_SOUND, IDLE_KEY(task), 1, 0, IDLE_SOUND_STEPS,
                          &work->pos);
        riding = player_on_task(0, task);
#ifdef COLONY_STEP
        if (AXIS_RADIUS >
            njSqrt((work->pos.x * work->pos.x) + (work->pos.z * work->pos.z))) {
            on_axis = TRUE;
        } else {
            on_axis = FALSE;
        }
#endif
        previous = work->pos;
        /* @hack spelling: frame set in the switch's expression, where an
         * assignment before the switch would do */
        switch (frame = game_state.steps, PATH_KIND(work)) {
        case PATH_STILL:
            break;
        case PATH_ARC:
            speed = ARC_SPEED(work);
            if (speed < MIN_ARC_SPEED) {
                speed = MIN_ARC_SPEED;
            }
            period = (int)((ARC_DEGREES / speed) + END_WAIT(work));
            SAMPLE_TRAVEL(fraction, frame, period, END_WAIT(work));
            TRACK(task)->orbit_angle = (Angle)(fraction * QUARTER_TURN);
            offset.x = work->scl.x;
            offset.y = 0.0F;
            offset.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&TRACK(task)->pivot);
            njRotateY(NULL, work->ang.z + TRACK(task)->orbit_angle + HALF_TURN);
            njCalcPoint(NULL, &offset, &work->pos);
            njPopMatrixEx();
            break;
        case PATH_VERTICAL:
            speed = PATH_SPEED(work);
            if (speed < MIN_PATH_SPEED) {
                speed = MIN_PATH_SPEED;
            }
            period = (int)((work->scl.x / speed) + END_WAIT(work));
            SAMPLE_TRAVEL(fraction, frame, period, END_WAIT(work));
            work->pos.y = TRACK(task)->origin.y + (fraction * work->scl.x);
            break;
        case PATH_HORIZONTAL:
            speed = PATH_SPEED(work);
            if (speed < MIN_PATH_SPEED) {
                speed = MIN_PATH_SPEED;
            }
            period = (int)((work->scl.x / speed) + END_WAIT(work));
            SAMPLE_TRAVEL(fraction, frame, period, END_WAIT(work));
            TRACK(task)->travel = fraction;
            offset.x = TRACK(task)->travel * work->scl.x;
            offset.y = 0.0F;
            offset.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&TRACK(task)->origin);
            njRotateY(NULL, work->ang.z);
            njCalcPoint(NULL, &offset, &work->pos);
            njPopMatrixEx();
            break;
        case PATH_RISE:
            if (work->submode & AWAY_FROM_START) {
                if (work->submode & IN_TRANSIT) {
                    sound_play_at_for(TRAVEL_SOUND, task, 1, 0,
                                      TRAVEL_SOUND_STEPS, &work->pos);
                    work->pos.y -= PATH_SPEED(work);
                    if (work->pos.y < TRACK(task)->origin.y) {
                        work->pos.y = TRACK(task)->origin.y;
                        work->submode &= ~IN_TRANSIT;
                        work->submode &= ~AWAY_FROM_START;
                        sound_play(ARRIVAL_SOUND, NULL, 0, 0);
                    }
                } else {
#ifdef COLONY_STEP
                    /* @hack redundant: a volatile read of player 0's pos that
                     * nothing uses */
                    volatile struct task_work *player = player_work[0];
                    (void)player->pos;
                    if (work->tick > RIDER_COOLDOWN) {
                        if ((!on_axis && riding) ||
                            player_near(&TRACK(task)->origin,
                                        TRIGGER_RADIUS(work))) {
                            work->submode |= IN_TRANSIT;
                            vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                            VIBRATION_UNTIMED);
                        }
                    }
#else
                    if (work->tick > RIDER_COOLDOWN &&
                        (riding || player_near(&TRACK(task)->origin,
                                               TRIGGER_RADIUS(work)))) {
                        work->submode |= IN_TRANSIT;
                        vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                        VIBRATION_UNTIMED);
                    }
#endif
                }
            } else if (work->submode & IN_TRANSIT) {
                sound_play_at_for(TRAVEL_SOUND, task, 1, 0, TRAVEL_SOUND_STEPS,
                                  &work->pos);
                work->pos.y += PATH_SPEED(work);
                if (work->pos.y > TRACK(task)->origin.y + work->scl.x) {
                    work->pos.y = TRACK(task)->origin.y + work->scl.x;
                    work->submode &= ~IN_TRANSIT;
                    work->submode |= AWAY_FROM_START;
                    sound_play(ARRIVAL_SOUND, NULL, 0, 0);
#ifndef COLONY_STEP
                    MOTION(task)->stop_timer = STOP_SOUND_DELAY;
#endif
                }
            } else if (riding && work->tick > RIDER_COOLDOWN) {
                work->submode |= IN_TRANSIT;
                vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_UNTIMED);
            }
            if (riding) {
                work->tick = 0;
            } else if (work->tick < MAX_EMPTY_STEPS) {
                work->tick++;
            }
            break;
        case PATH_SINK:
            if (work->submode & AWAY_FROM_START) {
                if (work->submode & IN_TRANSIT) {
                    sound_play_at_for(TRAVEL_SOUND, task, 1, 0,
                                      TRAVEL_SOUND_STEPS, &work->pos);
                    work->pos.y += PATH_SPEED(work);
                    if (work->pos.y > TRACK(task)->origin.y) {
                        work->pos.y = TRACK(task)->origin.y;
                        work->submode &= ~IN_TRANSIT;
                        work->submode &= ~AWAY_FROM_START;
                        sound_play(ARRIVAL_SOUND, NULL, 0, 0);
#ifndef COLONY_STEP
                        MOTION(task)->stop_timer = STOP_SOUND_DELAY;
#endif
                    }
                } else if (work->tick > RIDER_COOLDOWN &&
                           (riding || player_near(&TRACK(task)->origin,
                                                  TRIGGER_RADIUS(work)))) {
                    work->submode |= IN_TRANSIT;
                    vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                    VIBRATION_UNTIMED);
                }
            } else if (work->submode & IN_TRANSIT) {
                sound_play_at_for(TRAVEL_SOUND, task, 1, 0, TRAVEL_SOUND_STEPS,
                                  &work->pos);
                work->pos.y -= PATH_SPEED(work);
                if (work->pos.y < TRACK(task)->origin.y - work->scl.x) {
                    work->pos.y = TRACK(task)->origin.y - work->scl.x;
                    work->submode &= ~IN_TRANSIT;
                    work->submode |= AWAY_FROM_START;
                    sound_play(ARRIVAL_SOUND, NULL, 0, 0);
#ifndef COLONY_STEP
                    MOTION(task)->stop_timer = STOP_SOUND_DELAY;
#endif
                }
            } else if (riding && work->tick > RIDER_COOLDOWN) {
                work->submode |= IN_TRANSIT;
                vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_UNTIMED);
            }
            if (riding) {
                work->tick = 0;
            } else if (work->tick < MAX_EMPTY_STEPS) {
                work->tick++;
            }
            break;
        /* @hack redundant: a default that does nothing */
        default:
            break;
        }
        if (MOTION(task)->stop_timer) {
            MOTION(task)->stop_timer--;
            if (!MOTION(task)->stop_timer) {
                sound_play(STOP_SOUND, NULL, 0, 0);
            }
        }
        previous.y += TRACK(task)->bob_height;
#ifdef COLONY_STEP
        if (on_axis) {
            TRACK(task)->bob_phase = 0;
        } else {
#endif
            TRACK(task)->bob_phase =
                ((int)njScalor(&TRACK(task)->origin) + game_state.steps) *
                bob_speed;
#ifdef COLONY_STEP
        }
#endif
        TRACK(task)->bob_height = njSin(TRACK(task)->bob_phase) * bob_amplitude;
        if (task->force.p) {
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            offset.x = work->pos.x - previous.x;
            offset.y = work->pos.y - previous.y;
            offset.z = work->pos.z - previous.z;
#else
            /* @hack spelling: the displacement's components walked by
             * pointers, where named components would do */
            Float *out = &offset.x;
            Float *current = &work->pos.x;
            Float *before = &previous.x;
            *out++ = *current++ - *before++;
            *out++ = *current++ - *before++;
            *out = *current - *before;
#endif
            offset.y += TRACK(task)->bob_height;
            if (player_work[0]) {
                TASK_FORCES_OF(task)[0].speed = offset;
            }
            if (player_work[1]) {
                TASK_FORCES_OF(task)[1].speed = offset;
            }
        }
        TRACK(task)->collision.pos[0] = work->pos.x;
        TRACK(task)->collision.pos[1] = work->pos.y + TRACK(task)->bob_height;
        TRACK(task)->collision.pos[2] = work->pos.z;
        if (player_near((NJS_POINT3 *)TRACK(task)->collision.pos,
                        NEAR_RADIUS(task))) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
#ifndef COLONY_STEP
        if (task->child) {
            task->child->work->pos = *(NJS_POINT3 *)TRACK(task)->collision.pos;
            task->child->work->ang.y = work->ang.y;
        }
#endif
    }
#ifdef COLONY_STEP
    if (task->child) {
        /* @hack spelling: light, a pointer to previous, where previous itself
         * would do */
        light = &previous;
        *light = work->pos;
        light->y += LIGHT_HEIGHT;
        task->child->work->pos = *light;
        if (task->child->child) {
            task->child->child->work->pos =
                *(NJS_POINT3 *)TRACK(task)->collision.pos;
            task->child->child->work->ang.y = work->ang.y;
        }
    }
#endif
}

#define STEP_NAME(name) step_##name
#include "shared/engine_step_draw_impl.h"
#undef STEP_NAME
#ifndef COLONY_STEP
#define STEP_B
#define STEP_NAME(name) step_b_##name
#include "shared/engine_step_draw_impl.h"
#undef STEP_NAME
#undef STEP_B
#endif
