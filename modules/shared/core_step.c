#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/core_platform_path.h"
#include "shared/core_step.h"
#include "shared/step_models.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define TRACK(task) ((struct core_platform_path *)(task)->motion.p)
/* the node is in the world's collision only while a player is within its
 * radius plus NEAR_MARGIN */
#define NEAR_MARGIN 20.0F
#define IDLE_SOUND 10
#define TRAVEL_SOUND 17
#define STOP_SOUND 18
#define IDLE_SOUND_STEPS 120
#define TRAVEL_SOUND_STEPS 30

/* the bob's height is bob_amplitude times the sine of its angle. the angle
 * turns bob_speed a step, from a start that origin's length sets */
static Sint32 bob_speed = 512;
static Float bob_amplitude = 3.0F;
/* the box of a horizontal STEP's child, under the platform */
/* clang-format off */
static struct collision_shape rider_shape[1] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, -8.5F, 0.0F}, 20.0F, 7.5F,
     20.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 idle_volume = 50;
static Sint32 travel_volume = 50;
static Sint32 stop_volume = 50;

static void core_step_destroy(struct task *task);
static void core_step_update(struct task *task);
static void core_step_draw(struct task *task);
static void core_step_b_draw(struct task *task);
static void core_step_rider(struct task *task);

void core_step_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
    NJS_POINT3 position;
    Uint32 flags;
    Float radius;
    struct task *child;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct core_platform_path));
    if (!task->motion.p) {
        return;
    }
    task->draw = core_step_draw;
    task->step = core_step_update;
    task->destructor = core_step_destroy;
    work->mode = 0;
    CORE_PATH_START(TRACK(task), work, position);
    TRACK(task)->collision = core_step_collision;
    collision = &TRACK(task)->collision;
    CORE_PATH_PLACE(collision, work);
    flags = WORLD_MOVING | WORLD_SOLID;
    radius = collision->model->r + CLASS_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, collision);
    /* @hack spelling: PATH_KIND at each test, where a local, as in
     * core_step_b_init, would do */
    if (PATH_KIND(work) == PATH_RISE || PATH_KIND(work) == PATH_SINK) {
        work->tick = START_READY;
    }
    if (PATH_KIND(work) == PATH_HORIZONTAL) {
        child = task_create_child(TASK_WORK, core_step_rider, task);
        if (child) {
            /* @hack spelling: task->work for work, and one assignment that
             * clears ang.x and ang.z, where work and two assignments would
             * do */
            child->work->pos = task->work->pos;
            child->work->ang.x = child->work->ang.z = 0;
            child->work->ang.y = task->work->ang.y;
            collision_init(child, rider_shape, 1, COLLISION_OBJECT);
        }
    }
}

void core_step_b_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
    NJS_POINT3 position;
    Uint32 flags;
    Float radius;
    int kind;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct core_platform_path));
    if (!task->motion.p) {
        return;
    }
    task->draw = core_step_b_draw;
    task->step = core_step_update;
    task->destructor = core_step_destroy;
    work->mode = 0;
    CORE_PATH_START(TRACK(task), work, position);
    TRACK(task)->collision = core_step_b_collision;
    collision = &TRACK(task)->collision;
    CORE_PATH_PLACE(collision, work);
    flags = WORLD_MOVING | WORLD_SOLID;
    radius = collision->model->r + CLASS_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, collision);
    /* @hack redundant: a test of flags that no path reaches */
    if (0) {
        if (flags) {
        }
    }
    kind = PATH_KIND(work);
    if (kind == PATH_RISE || kind == PATH_SINK) {
        work->tick = START_READY;
    }
}

static void core_step_destroy(struct task *task)
{
    core_path_release(task);
}

/* @hack spelling: the displacement's components walked by pointers, in an
 * inlined function, where named components would do */
#pragma inline(rider_displacement)
static void rider_displacement(const Float *previous, const Float *current,
                               Float *delta)
{
    *delta++ = *current++ - *previous++;
    *delta++ = *current++ - *previous++;
    *delta = *current - *previous;
}

static void core_step_update(struct task *task)
{
    struct task_work *work = task->work;
    Bool riding;
    Uint32 frame;
    int period;
    Float fraction;
    Float speed;
    NJS_POINT3 previous;
    NJS_VECTOR offset;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, of the set editor's readout, a
     * reset of the work and a restart of the path, whose strings SHC still
     * places in this source's constants */
    if (0) {
        njPrintC(NJM_LOCATION(17, 16), "<- ASIBATYPE");
        njPrintC(NJM_LOCATION(17, 17), "<- ROT Y");
        work->mode = work->submode = work->id = work->tick = 0;
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
        CORE_PATH_START(TRACK(task), work, previous);
    } else {
        if (game_state.time_stopped == 0) {
            sound_play_at_for(IDLE_SOUND, task->child, 1, idle_volume,
                              IDLE_SOUND_STEPS, &work->pos);
        }
        riding = player_on_task(0, task);
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
                    if (game_state.time_stopped == 0) {
                        sound_play_at_for(TRAVEL_SOUND, task, 1, travel_volume,
                                          TRAVEL_SOUND_STEPS, &work->pos);
                    }
                    work->pos.y -= PATH_SPEED(work);
                    if (work->pos.y < TRACK(task)->origin.y) {
                        work->pos.y = TRACK(task)->origin.y;
                        work->submode &= ~IN_TRANSIT;
                        work->submode &= ~AWAY_FROM_START;
                        sound_play_at(STOP_SOUND, NULL, 0, stop_volume,
                                      &work->pos);
                    }
                } else if (work->tick > RIDER_COOLDOWN) {
                    if (riding || player_near(&TRACK(task)->origin,
                                              TRIGGER_RADIUS(work))) {
                        work->submode |= IN_TRANSIT;
                    }
                }
            } else if (work->submode & IN_TRANSIT) {
                if (game_state.time_stopped == 0) {
                    sound_play_at_for(TRAVEL_SOUND, task, 1, travel_volume,
                                      TRAVEL_SOUND_STEPS, &work->pos);
                }
                work->pos.y += PATH_SPEED(work);
                if (work->pos.y > TRACK(task)->origin.y + work->scl.x) {
                    work->pos.y = TRACK(task)->origin.y + work->scl.x;
                    work->submode &= ~IN_TRANSIT;
                    work->submode |= AWAY_FROM_START;
                    sound_play_at(STOP_SOUND, NULL, 0, stop_volume, &work->pos);
                }
            } else if (riding && work->tick > RIDER_COOLDOWN) {
                work->submode |= IN_TRANSIT;
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
                    work->pos.y += PATH_SPEED(work);
                    if (work->pos.y > TRACK(task)->origin.y) {
                        work->pos.y = TRACK(task)->origin.y;
                        work->submode &= ~IN_TRANSIT;
                        work->submode &= ~AWAY_FROM_START;
                    }
                } else if (work->tick > RIDER_COOLDOWN) {
                    if (riding || player_near(&TRACK(task)->origin,
                                              TRIGGER_RADIUS(work))) {
                        work->submode |= IN_TRANSIT;
                    }
                }
            } else if (work->submode & IN_TRANSIT) {
                work->pos.y -= PATH_SPEED(work);
                if (work->pos.y < TRACK(task)->origin.y - work->scl.x) {
                    work->pos.y = TRACK(task)->origin.y - work->scl.x;
                    work->submode &= ~IN_TRANSIT;
                    work->submode |= AWAY_FROM_START;
                }
            } else if (riding && work->tick > RIDER_COOLDOWN) {
                work->submode |= IN_TRANSIT;
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
        previous.y += TRACK(task)->bob_height;
        TRACK(task)->bob_phase =
            ((int)njScalor(&TRACK(task)->origin) + game_state.steps) *
            bob_speed;
        TRACK(task)->bob_height = njSin(TRACK(task)->bob_phase) * bob_amplitude;
        if (task->force.p) {
#ifdef NONMATCHING
            /* the plain form of rider_displacement's hack: named components */
            offset.x = work->pos.x - previous.x;
            offset.y = work->pos.y - previous.y;
            offset.z = work->pos.z - previous.z;
#else
            rider_displacement(&previous.x, &work->pos.x, &offset.x);
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
                        TRACK(task)->collision.model->r + NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void core_step_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: a point nothing uses, which sizes the frame */
    NJS_POINT3 unused;
    NJS_POINT3 position;

    (void)unused;

    njSetTexture(&core_step_texlist);
    njPushMatrixEx();
    position = work->pos;
    position.y += TRACK(task)->bob_height;
    njTranslateEx(&position);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&core_step_model);
    njPopMatrixEx();
}

static void core_step_b_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: a point nothing uses, which sizes the frame */
    NJS_POINT3 unused;
    NJS_POINT3 position;

    (void)unused;

    njSetTexture(&core_step_b_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    position = work->pos;
    position.y += TRACK(task)->bob_height;
    njTranslateEx(&position);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&core_step_b_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}

static void core_step_rider(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    struct task_work *platform = parent->work;

    work->pos = platform->pos;
    work->pos.y += TRACK(parent)->bob_height;
    collision_register(task);
}
