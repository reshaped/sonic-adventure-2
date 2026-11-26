#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "boss_last1/body.h"
#include "boss_last1/lift.h"
#include "player/player.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

enum lift_mode { LIFT_RISE, LIFT_HOVER, LIFT_STOP, LIFT_RELEASE, LIFT_DONE };
#define AXIS_RADIUS 40.0F
/* player 0 touches the hovering lift where it lies below the lift's height.
 * its speed along y must also be CONTACT_SPEED or less. the touch rumbles its
 * pad by vibration_start's CONTACT_POWER, CONTACT_FREQ and CONTACT_TIME */
#define CONTACT_SPEED 1.0F
#define CONTACT_POWER 1
#define CONTACT_FREQ 5
#define CONTACT_TIME 0
/* a player 0 that isn't rising takes CLIMB_UPDRAFT times the climbing lift's
 * rise as its updraft */
#define CLIMB_UPDRAFT 2.0F
/* a release lasts RELEASE_STEPS. it lifts player 0 until the lift's timer
 * comes down to RELEASE_LIFT_END. it rumbles player 0's pad by RELEASE_POWER,
 * RELEASE_FREQ and RELEASE_TIME */
#define RELEASE_STEPS 20
#define RELEASE_LIFT_END 10
#define RELEASE_POWER 2
#define RELEASE_FREQ 10
#define RELEASE_TIME 1
/* a sphere deeper than BOUNCE_DEPTH below the lift's height bounces by
 * fixed_impulse. one nearer bounces by bounce_gain times its depth */
#define BOUNCE_DEPTH 4.0F
/* drag slows only a sphere faster than DRAG_SPEED */
#define DRAG_SPEED 0.05F
#define SQUARE(value) ((value) * (value))

/* the lift's task_work. boss lies in ang.x's word */
struct lift_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Uint16 timer;
    const struct task_work *boss;
    Angle ang_y;
    Angle ang_z;
    NJS_POINT3 pos;
    NJS_POINT3 scl;
    struct collision_work *collision;
};
struct lift_parameters {
    Float rise; /* the lift's climb each step */
    Float ceiling;
    /* player 0's updraft for contact_steps after it touches the hovering
     * lift */
    Float hover_lift;
    Float bounce_gain;
    Float unused_10;
    Float min_impulse;
    Float gravity;
    Float unused_1c;
    Float max_speed;
    Float max_impulse;
    Float fixed_impulse;
    /* each step, a sphere loses a random fraction of its speed, up to drag */
    Float drag;
    /* a sphere's gain of speed each step once the lift is released */
    Float release_accel;
    Float release_lift; /* player 0's updraft in the release's first steps */
    Uint16 contact_steps;
    Sint32 bounce_steps;
};
static struct lift_parameters lift = {2.0F,  150.0F, 0.22F, 0.4F, 1.2F, 0.005F,
                                      0.01F, 0.008F, 2.0F,  1.8F, 1.8F, 0.01F,
                                      0.05F, 0.65F,  3,     3};
static struct task *active_lift = NULL;

static Bool boss_keeps_lift(struct task *task)
{
    struct lift_work *work = (struct lift_work *)task->work;
    const struct task_work *boss = work->boss;
    /* @hack spelling: a return of 0 once both modes are ruled out, and of 1
     * after, where a return of the test for either would do */
    /* @hack type: the mode narrowed to Uint8 in each test, where the mode
     * alone compares the same */
    if ((Uint8)boss->mode != BIOLIZARD_SPHERE_ATTACK &&
        (Uint8)boss->mode != BIOLIZARD_PAUSE_ATTACK) {
        return 0;
    }
    return 1;
}

void biolizard_lift_sphere(NJS_POINT3 *pos, Float *speed, Sint16 *timer)
{
    Float velocity = *speed;
    Uint16 count = *timer;
    struct task *task = active_lift;
    struct lift_work *work;
    Float impulse;
    Float excess;

    if (!task) {
        return;
    }
    work = (struct lift_work *)task->work;
    if (work->mode == LIFT_RISE && pos->y < work->pos.y) {
        pos->y = work->pos.y + (rand() * RAND_SCALE);
    }
    if (work->mode >= LIFT_RELEASE) {
        impulse = lift.release_accel;
        velocity += impulse;
    } else {
        if (njAbs(velocity) > DRAG_SPEED) {
            velocity *= -lift.drag + (rand() * lift.drag * RAND_SCALE) + 1.0F;
        }
        if (count) {
            --count;
        } else if (pos->y < work->pos.y) {
            excess = work->pos.y - pos->y - BOUNCE_DEPTH;
            if (excess > 0.0F) {
                impulse = lift.fixed_impulse;
            } else {
                impulse = lift.bounce_gain * (excess + BOUNCE_DEPTH);
            }
            if (impulse > lift.max_impulse) {
                impulse = lift.max_impulse;
            }
            if (impulse < lift.min_impulse) {
                impulse = lift.min_impulse;
            }
            count = lift.bounce_steps;
            velocity += impulse;
        }
        if (velocity > lift.max_speed) {
            velocity = lift.max_speed;
        }
        /* @hack spelling: the negated gravity added, where -= would do */
        velocity += -lift.gravity;
    }
    /* @hack spelling: the sum assigned, where += would do */
    pos->y = pos->y + velocity;
    *speed = velocity;
    *timer = count;
}

/* the lift leaves player 0 alone on a path or within AXIS_RADIUS of the y
 * axis. it also leaves player 0 alone in a miss begun in a current, which
 * keeps ANIM_IN_CURRENT */
/* @hack spelling: lift_player, rise and release return on each of these in
 * turn, where one test of the three would do */
#define ON_PATH() (player_work[0]->flags & PLAYER_WORK_ON_PATH)
#define NEAR_AXIS()                                                            \
    (njSqrt(SQUARE(player_work[0]->pos.x) + SQUARE(player_work[0]->pos.z)) <   \
     AXIS_RADIUS)
#define MISSED_IN_CURRENT()                                                    \
    (player_work[0]->mode == PLAYER_MODE_MISS &&                               \
     player_records[0]->anim.current == ANIM_IN_CURRENT)

static void lift_player(struct task *task)
{
    struct lift_work *work = (struct lift_work *)task->work;
    if (ON_PATH()) {
        return;
    }
    if (NEAR_AXIS()) {
        return;
    }
    if (MISSED_IN_CURRENT()) {
        return;
    }
    if (player_work[0]->pos.y < work->pos.y) {
        if (player_motion[0]->spd.y <= CONTACT_SPEED) {
            vibration_start(0, CONTACT_POWER, CONTACT_FREQ, CONTACT_TIME);
            work->timer = lift.contact_steps;
        }
    }
    if (work->timer) {
        player_updraft(0, 0.0F, lift.hover_lift, 0.0F);
        --work->timer;
    }
}

static void rise(struct task *task)
{
    struct lift_work *work = (struct lift_work *)task->work;
    if (work->pos.y < lift.ceiling) {
        work->pos.y += lift.rise;
        if (work->pos.y >= lift.ceiling) {
            work->pos.y = lift.ceiling;
            work->mode = LIFT_HOVER;
            work->submode = 0;
        }
    }
    if (!boss_keeps_lift(task)) {
        work->mode = LIFT_STOP;
        work->submode = 0;
    }
    if (ON_PATH()) {
        return;
    }
    if (NEAR_AXIS()) {
        return;
    }
    if (MISSED_IN_CURRENT()) {
        return;
    }
    if (player_motion[0]->spd.y <= 0.0F) {
        player_updraft(0, 0.0F, lift.rise * CLIMB_UPDRAFT, 0.0F);
    }
}

static void hover(struct task *task)
{
    struct lift_work *work = (struct lift_work *)task->work;
    if (!boss_keeps_lift(task)) {
        work->mode = LIFT_STOP;
        work->submode = 0;
    }
    lift_player(task);
}

static void release(struct task *task)
{
    struct lift_work *work = (struct lift_work *)task->work;
    if (work->timer) {
        --work->timer;
    } else {
        work->mode = LIFT_DONE;
        work->submode = 0;
    }
    if (ON_PATH()) {
        return;
    }
    if (NEAR_AXIS()) {
        return;
    }
    if (MISSED_IN_CURRENT()) {
        return;
    }
    if (work->timer > RELEASE_LIFT_END) {
        player_updraft(0, 0.0F, lift.release_lift, 0.0F);
    }
}

static void lift_step(struct task *task)
{
    struct lift_work *work = (struct lift_work *)task->work;
    if (!game_state.time_stopped) {
        switch (work->mode) {
        case LIFT_RISE:
            rise(task);
            break;
        case LIFT_HOVER:
            hover(task);
            break;
        case LIFT_STOP:
            /* @hack spelling: the kill in a loop that runs once, where the
             * call alone would do */
            do {
                task_kill(task);
            } while (0);
            break;
        case LIFT_RELEASE:
            release(task);
            break;
        /* @hack redundant: LIFT_DONE's case, which does nothing */
        case LIFT_DONE:
            break;
        }
    }
}

static void lift_destroy(struct task *task)
{
    active_lift = NULL;
}

struct task *biolizard_lift_create(struct task *parent)
{
    struct task *task;
    struct lift_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, lift_step, parent);
    if (!task) {
        return NULL;
    }
    work = (struct lift_work *)task->work;
    work->pos.y = 0.0F;
    work->boss = parent->work;
    task->destructor = lift_destroy;
    active_lift = task;
    return task;
}

void biolizard_lift_release(void)
{
    struct task *task = active_lift;
    struct lift_work *work;
    if (!task) {
        return;
    }
    work = (struct lift_work *)task->work;
    work->timer = RELEASE_STEPS;
    work->mode = LIFT_RELEASE;
    vibration_start(0, RELEASE_POWER, RELEASE_FREQ, RELEASE_TIME);
}

void biolizard_lift_stop(void)
{
    struct task *task = active_lift;
    if (!task) {
        return;
    }
    ((struct lift_work *)task->work)->mode = LIFT_STOP;
}
