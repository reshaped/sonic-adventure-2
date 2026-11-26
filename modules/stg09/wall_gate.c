/* the wall gate, WALL in Prison Lane's object list. its door stands raised
 * while an elevator waits near it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/dust.h"
#include "stg09/door.h"
#include "stg09/elevator.h"
#include "stg09/wall_gate.h"
#include "system/angle.h"
#include "system/task.h"

#include "stg09/wall_gate/text.inc"

/* the gate's modes, in the work's mode. in MODE_SHUT the door has dropped,
 * and bounces at the sill */
enum wall_gate_mode { MODE_RAISED, MODE_DROPPING, MODE_SHUT };

/* how high the door stands until it drops */
#define DOOR_RAISE 50.0F
/* the door stays raised while a waiting elevator lies within WAIT_RANGE of
 * the gate. the search for the nearest starts at NEAREST_START, which the first
 * elevator found replaces unread */
#define WAIT_RANGE 450.0F
#define NEAREST_START 600.0F
#define DUST_HALF_WIDTH 30.0F

#define WALL_GATE_SHAPES 5
#define DOOR_SHAPE 1

/* the wall's node, whose child is the door */
static NJS_TEXLIST wall_gate_texlist;
static NJS_CNK_OBJECT wall_gate_object;

static struct collision_shape wall_gate_shapes[WALL_GATE_SHAPES];

static void wall_gate_destroy(struct task *task);
static void wall_gate_step(struct task *task);
static void wall_gate_draw(struct task *task);

void wall_gate_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct door));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = wall_gate_draw;
    task->step = wall_gate_step;
    task->destructor = wall_gate_destroy;
    DOOR(task)->travel = DOOR_RAISE;
    collision_init(task, wall_gate_shapes, WALL_GATE_SHAPES, COLLISION_OBJECT);
    if (task->placement != NULL) {
        if (placement_get_state(task)) {
            work->mode = MODE_SHUT;
            DOOR(task)->travel = 0.0F;
        }
    }
}

static void wall_gate_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL && work->mode == MODE_SHUT) {
        placement_set_state(task, DOOR_STATE_DONE);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void wall_gate_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_RAISED: {
        struct task *nearest = NULL;
        Float nearest_distance = NEAREST_START;
        struct task **elevator = &elevator_tasks[0];
        struct task **end = elevator_tasks + ELEVATORS;

        while (elevator < end) {
            struct task *other = *elevator++;

            if (other != NULL && other->work->submode != ELEVATOR_SLIDING) {
                Float distance;

                if ((distance = njDistanceP2P(&work->pos, &other->work->pos)) <
                    WAIT_RANGE) {
                    if (nearest != NULL) {
                        if (nearest_distance > distance) {
                            nearest = other;
                            nearest_distance = distance;
                        }
                    } else {
                        nearest_distance = distance;
                        nearest = other;
                    }
                }
            }
        }
        if (nearest != NULL) {
            break;
        }
        work->mode = MODE_DROPPING;
        work->timer = 0;
        break;
    }
    case MODE_DROPPING:
        DOOR(task)->travel -= work->scl.x;
        if (DOOR(task)->travel <= 0.0F) {
            NJS_POINT3 sill;
            struct angles ang;

            sill.x = 0.0F;
            sill.y = 0.0F;
            sill.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &sill, &sill);
            njPopMatrix(1);
            ang.x = 0;
            ang.y = work->ang.y;
            ang.z = 0;
            dust_short_rect(&sill, DUST_HALF_WIDTH, DOOR_DUST_HALF_DEPTH, &ang,
                            DOOR_DUST_SCALE, DOOR_DUST_DENSITY,
                            DOOR_DUST_SPEED);
            work->mode = MODE_SHUT;
            DOOR(task)->speed += DOOR_KICK;
            work->timer = 0;
        }
        break;
    case MODE_SHUT:
        DOOR(task)->travel += DOOR(task)->speed;
        DOOR(task)->speed = -(DOOR(task)->travel * DOOR_SPRING) +
                            (DOOR(task)->speed * DOOR_DAMPING);
        if (DOOR(task)->travel <= 0.0F) {
            DOOR(task)->travel = 0.0F;
            DOOR(task)->speed = njAbs(DOOR(task)->speed);
        }
    }
    if (work->collision != NULL) {
        work->collision->shapes[DOOR_SHAPE].center.y =
            wall_gate_shapes[DOOR_SHAPE].center.y + DOOR(task)->travel;
        collision_register(task);
    }
}

static void wall_gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *door;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)WALL_GATE_EDITOR_ENE_ID;
        (void)WALL_GATE_EDITOR_ANGLE_Y;
        (void)WALL_GATE_EDITOR_SHUT_SPEED;
    }
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&wall_gate_texlist);
    object = &wall_gate_object;
    njCnkEasyDrawModel(object->model);
    door = object->child;
    njTranslate(NULL, 0.0F, DOOR(task)->travel, 0.0F);
    njCnkEasyDrawModel(door->model);
    njPopMatrix(1);
}

#include "stg09/fixtures/wall_gate.h"

NJS_TEXLIST *wall_gate_texlists[3] = {
#include "stg09/wall_gate/wall_gate_texlists.inc"
};
/* the sill, the door, the two posts and the lintel. wall_gate_step drops
 * the record's copy of the door's box with the door */
static struct collision_shape wall_gate_shapes[WALL_GATE_SHAPES] = {
#include "stg09/wall_gate/wall_gate_shapes.inc"
};
