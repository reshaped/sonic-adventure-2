/* the floating foothold, UKIASIBA in Aquatic Mine's object list. it floats on
 * the water of stg07/water.h. a player who lands on it sinks and tilts it, and
 * it springs back */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/water_ripple.h"
#include "sound/sound.h"
#include "stg07/floating_foothold.h"
#include "stg07/water.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg07/floating_foothold/text.inc"

#include "stg07/scenery/floating_foothold.h"

/* the foothold's record, in task->motion. offset's x and z are a sway across
 * the water, which only tilts the foothold. offset's y is the foothold's height
 * over the water's surface. speed is the offset's speed a step */
struct floating_foothold {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_VECTOR offset;
    NJS_VECTOR speed;
    Float top; /* the most the work's pos.y reaches */
    Float bottom; /* the least */
};

#define FOOTHOLD(task) ((struct floating_foothold *)(task)->motion.p)

/* a byte of the work's timer for each player, which each step counts down to
 * 0. a step that finds the player on the foothold sets the byte to RIDE_STEPS.
 * where the byte had reached 0, that step counts a landing */
/* @hack type: a player's byte read through the timer's address, where a work
 * with a Uint8 ride[2] in the timer's place would do */
#define RIDE(work, i) (((Uint8 *)&(work)->timer)[i])
#define RIDE_STEPS 30
/* the foothold carries the players of slots 0 and 1, each by one of task's
 * TASK_FORCES force records */
#define RIDERS 2
#define FORCES(task) ((struct task_force *)(task)->force.p)
/* after a landing, the first RIPPLES steps at which speed.y changes sign each
 * ripple the water. the work's tick counts the ripples, and each plays
 * RIPPLE_SOUND of the stage's list. a ripple starts at a size of RIPPLE_SCALE
 * and grows by RIPPLE_GROWTH a step. it fades once past RIPPLE_FADE_SIZE */
#define RIPPLES 4
#define RIPPLE_SOUND 3
#define RIPPLE_SCALE 4.5F
#define RIPPLE_GROWTH 0.03F
#define RIPPLE_FADE_SIZE 11.0F

/* the foothold takes its size class from its model's radius plus
 * CLASS_MARGIN. it collides while a player lies within the radius plus
 * REACH_MARGIN of the copy's pos */
#define CLASS_MARGIN 5.0F
#define REACH_MARGIN 15.0F
/* the step leans the foothold's up axis along (offset.x, TILT_HEIGHT,
 * offset.z) */
#define TILT_HEIGHT 20.0F
/* the water's lift is whole at LIFT_DEPTH or more under the surface. it is
 * none at LIFT_DEPTH or more over it. between, it falls by LIFT_SCALE for each
 * unit of height */
#define LIFT_DEPTH 5.0F
#define LIFT_SCALE 0.1F
/* each step gives a player's task_force the move and the turn the foothold
 * makes under the player. a move shorter than LEAST_MOVE gives neither */
#define LEAST_MOVE 0.01F

NJS_TEXLIST *floating_foothold_texlists[3] = {
#include "stg07/floating_foothold/floating_foothold_texlists.inc"
};
/* each step, the speed keeps damping of itself and loses spring times the
 * offset. along y the water's lift scales that loss, and the speed loses
 * gravity too */
static Float spring = 0.025F;
static Float gravity = 0.07F;
static Float damping = 0.95F;
/* a player's push on the foothold as it lands, and at each step it rides */
static Float landing_push = 0.05F;
static Float riding_push = 0.002F;
static NJS_VECTOR up = {0.0F, 1.0F, 0.0F};

static void floating_foothold_destroy(struct task *task);
static void floating_foothold_step(struct task *task);
static void floating_foothold_draw(struct task *task);

/* set *out to the point in, in an object's own axes. pos places the object,
 * and ang turns it as the foothold's draw does */
static void to_frame(NJS_POINT3 *out, NJS_POINT3 *in, struct angles *ang,
                     NJS_POINT3 *pos)
{
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -ang->x);
    njRotateY(NULL, -ang->y);
    njRotateZ(NULL, -ang->z);
    njTranslate(NULL, -pos->x, -pos->y, -pos->z);
    njCalcPoint(NULL, in, out);
    njPopMatrix(1);
}

static void to_task_frame(NJS_POINT3 *out, NJS_POINT3 *in, struct task *task)
{
    to_frame(out, in, &task->work->ang, &task->work->pos);
}

void floating_foothold_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct floating_foothold));
    if (task->motion.p == NULL) {
        return;
    }
    task->step = floating_foothold_step;
    task->destructor = floating_foothold_destroy;
    work->submode = 0;
    object = task->motion.p;
    *object = floating_foothold_collision;
    object->evalflags =
        (object->evalflags &
         ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)) |
        NJD_EVAL_ZXY_ANG;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    object->scl[0] = 1.0F;
    object->scl[1] = 1.0F;
    object->scl[2] = 1.0F;
    flags = WORLD_MOVING | GROUND_GRIP_MORE | WORLD_SOLID;
    radius = object->model->r + CLASS_MARGIN;
    if (!(radius > WORLD_SMALL_SIZE_CLASS_1_MAX)) {
        if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags = WORLD_MOVING | GROUND_GRIP_MORE | WORLD_SOLID |
                    WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags = WORLD_MOVING | GROUND_GRIP_MORE | WORLD_SOLID |
                    WORLD_SIZE_CLASS(2);
        } else {
            flags = WORLD_MOVING | GROUND_GRIP_MORE | WORLD_SOLID |
                    WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    FOOTHOLD(task)->top = work->scl.y + work->pos.y;
    FOOTHOLD(task)->bottom = -work->scl.y + work->pos.y;
    task->draw = floating_foothold_draw;
    work->tick = RIPPLES;
}

static void floating_foothold_destroy(struct task *task)
{
    if (FOOTHOLD(task)->object.model != NULL) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void floating_foothold_ripple(struct task_work *work, Float surface)
{
    NJS_VECTOR growth;
    NJS_POINT3 pos;

    growth.y = 0.0F;
    growth.x = RIPPLE_GROWTH;
    growth.z = RIPPLE_FADE_SIZE;
    pos.x = work->pos.x;
    pos.y = surface;
    pos.z = work->pos.z;
    ripple_create(&pos, &growth, RIPPLE_SCALE);
    sound_play(RIPPLE_SOUND, NULL, 0, 0);
}

static void floating_foothold_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *foothold_task;
    int i;
    Bool landed[RIDERS];
    NJS_VECTOR local;
    NJS_POINT3 old_pos;
    struct angles old_ang;
    NJS_VECTOR point;
    NJS_VECTOR move;
    struct angles turn;
    NJS_VECTOR pivot;
    Float dot;
    Float push;
    Float height;
    Float surface;
    Float old_speed_y;
    Float lift;
    NJS_VECTOR *tilt;
    struct floating_foothold *foothold;

    if (placement_try_leave(task)) {
        return;
    }
    for (i = 0; i < RIDERS; i++) {
        landed[i] = FALSE;
        if (RIDE(work, i)) {
            RIDE(work, i)--;
        }
        if (player_work[i] != NULL && player_on_task(i, task)) {
            /* @hack spelling: the ride set in each arm, where one store
             * after the test would do */
            if (!RIDE(work, i)) {
                landed[i] = TRUE;
                RIDE(work, i) = RIDE_STEPS;
            } else {
                RIDE(work, i) = RIDE_STEPS;
            }
        }
    }
    for (i = 0; i < RIDERS; i++) {
        if (landed[i]) {
            to_task_frame(&local, &player_work[i]->pos, task);
            dot = njInnerProduct(&local, &up);
            local.y = 0.0F;
            push = ((1.0F - dot) * landing_push) / njScalor(&local);
            FOOTHOLD(task)->speed.x += local.x * push;
            FOOTHOLD(task)->speed.z += local.z * push;
            FOOTHOLD(task)->speed.y -= dot * landing_push;
        } else if (RIDE(work, i) == RIDE_STEPS) {
            to_task_frame(&local, &player_work[i]->pos, task);
            dot = njInnerProduct(&local, &up);
            local.y = 0.0F;
            push = ((1.0F - dot) * riding_push) / njScalor(&local);
            FOOTHOLD(task)->speed.x += local.x * push;
            FOOTHOLD(task)->speed.z += local.z * push;
            FOOTHOLD(task)->speed.y -= dot * riding_push;
        }
    }
    surface = water_height(&work->pos);
    height = FOOTHOLD(task)->offset.y;
    old_speed_y = FOOTHOLD(task)->speed.y;
    /* @hack spelling: foothold_task, a copy of task for the rest of the step,
     * where task would do */
    foothold_task = task;
    FOOTHOLD(foothold_task)->offset.x += FOOTHOLD(foothold_task)->speed.x;
    height += FOOTHOLD(foothold_task)->speed.y;
    FOOTHOLD(foothold_task)->offset.z += FOOTHOLD(foothold_task)->speed.z;
    lift = height + surface;
    if (lift > FOOTHOLD(foothold_task)->top) {
        height = FOOTHOLD(foothold_task)->top - surface;
        FOOTHOLD(foothold_task)->speed.y = 0.0F;
        FOOTHOLD(foothold_task)->speed.x = 0.0F;
        FOOTHOLD(foothold_task)->speed.z = 0.0F;
    } else if (lift < FOOTHOLD(foothold_task)->bottom) {
        height = FOOTHOLD(foothold_task)->bottom - surface;
        FOOTHOLD(foothold_task)->speed.y = 0.0F;
        FOOTHOLD(foothold_task)->speed.x = 0.0F;
        FOOTHOLD(foothold_task)->speed.z = 0.0F;
    }
    FOOTHOLD(foothold_task)->speed.x =
        -(FOOTHOLD(foothold_task)->offset.x * spring) +
        (FOOTHOLD(foothold_task)->speed.x * damping);
    lift = (height + LIFT_DEPTH) * LIFT_SCALE;
    if (lift > 1.0F) {
        lift = 1.0F;
    } else if (lift < 0.0F) {
        lift = 0.0F;
    }
    lift = 1.0F - lift;
    FOOTHOLD(foothold_task)->speed.y =
        (-(height * spring * lift) - gravity) +
        (FOOTHOLD(foothold_task)->speed.y * damping);
    FOOTHOLD(foothold_task)->speed.z =
        -(FOOTHOLD(foothold_task)->offset.z * spring) +
        (FOOTHOLD(foothold_task)->speed.z * damping);
    old_ang = work->ang;
    old_pos = work->pos;
    work->pos.y = height + surface;
    if (landed[0] || landed[1]) {
        work->tick = 0;
    } else if (work->tick < RIPPLES) {
        if ((old_speed_y >= 0.0F && FOOTHOLD(foothold_task)->speed.y < 0.0F) ||
            (old_speed_y <= 0.0F && FOOTHOLD(foothold_task)->speed.y > 0.0F)) {
            floating_foothold_ripple(work, surface);
            work->tick++;
        }
    }
    FOOTHOLD(foothold_task)->offset.y = height;
    /* @hack spelling: tilt, a pointer to point, where point itself would
     * do */
    tilt = &point;
    *tilt = FOOTHOLD(foothold_task)->offset;
    tilt->y = TILT_HEIGHT;
    work->ang.z = -(Angle)(atan2f(-tilt->x, njSqrt((tilt->y * tilt->y) +
                                                   (tilt->z * tilt->z))) *
                           ANGLE_PER_RADIAN);
    work->ang.x = (Angle)(atan2f(-tilt->z, tilt->y) * ANGLE_PER_RADIAN);
    foothold = FOOTHOLD(foothold_task);
    move.x = work->pos.x - old_pos.x;
    move.y = work->pos.y - old_pos.y;
    move.z = work->pos.z - old_pos.z;
    turn.x = work->ang.x - old_ang.x;
    turn.y = work->ang.y - old_ang.y;
    turn.z = work->ang.z - old_ang.z;
    for (i = 0; i < RIDERS; i++) {
        if (player_work[i] != NULL) {
            to_frame(&point, &player_work[i]->pos, &old_ang, &old_pos);
            pivot = point;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&move);
            njRotateX(NULL, -old_ang.x);
            njRotateY(NULL, -old_ang.y);
            njRotateZ(NULL, -old_ang.z);
            njTranslate(NULL, -pivot.x, -pivot.y, -pivot.z);
            njRotateZ(NULL, turn.z);
            njRotateY(NULL, turn.y);
            njRotateX(NULL, turn.x);
            njCalcPoint(NULL, &point, &point);
            njPopMatrixEx();
            if (njScalor(&point) < LEAST_MOVE) {
                point.x = point.y = point.z = 0.0F;
                turn.x = turn.y = turn.z = 0;
            }
            FORCES(foothold_task)[i].speed = point;
            FORCES(foothold_task)[i].turn.x = turn.x;
            FORCES(foothold_task)[i].turn.y = turn.y;
            FORCES(foothold_task)[i].turn.z = turn.z;
        }
    }
    foothold->object.pos[1] = work->pos.y - foothold->object.model->center.y;
    foothold->object.ang[0] = work->ang.x;
    foothold->object.ang[1] = work->ang.y;
    foothold->object.ang[2] = work->ang.z;
    if (player_near((NJS_POINT3 *)foothold->object.pos,
                    foothold->object.model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void floating_foothold_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- FLOAT RANGE";
    }
    njSetTexture(&floating_foothold_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njTranslate(NULL, -floating_foothold_model.center.x,
                -floating_foothold_model.center.y,
                -floating_foothold_model.center.z);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&floating_foothold_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
}
