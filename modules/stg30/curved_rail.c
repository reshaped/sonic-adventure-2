/* Final Rush's curved grind rails, GLIND and GLIND_B, and GLIND_B's barrier */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg30/curved_rail.h"
#include "stg30/curved_rail_art.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* a rail's or a barrier's record, in its task's motion. object is the
 * collision object it puts in the world. center is its model's center as
 * object places it */
struct rail_collision {
    NJS_POINT3 center;
    NJS_OBJECT object;
};
#define RAIL(task) ((struct rail_collision *)(task)->motion.p)
/* the collision world collects a rail's or a barrier's object only while it is
 * near. it is near while player 0 or 1 is within its model's radius plus
 * NEAR_MARGIN of center. the same sum picks the object's WORLD_SIZE_CLASS */
#define NEAR_MARGIN 15.0F
/* the rail's kind, in its work's submode */
#define RAIL_PLAIN 0
#define RAIL_B 1

static void barrier_init(struct task *task);
static void barrier_destroy(struct task *task);
static void barrier_step(struct task *task);
static void rail_init(struct task *task);
static void rail_destroy(struct task *task);
static void rail_draw(struct task *task);
static void rail_step(struct task *task);

#define CURVED_RAIL
#include "shared/grind_path.h"

static void barrier_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    task->motion.p = syCalloc(1, sizeof(struct rail_collision));
    if (task->motion.p != NULL) {
        task->step = barrier_step;
        task->destructor = barrier_destroy;
        RAIL(task)->object = *curved_rail_b_collision->child;
        object = &RAIL(task)->object;
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->evalflags |= NJD_EVAL_BREAK;
        object->ang[2] = work->ang.z;
        object->ang[0] = work->ang.x;
        object->ang[1] = work->ang.y;
        flags = GROUND_CRASHES | WORLD_SOLID;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        radius = object->model->r + NEAR_MARGIN;
        if (radius <= WORLD_SIZE_CLASS_1_MAX) {
            if (radius > WORLD_SIZE_CLASS_2_MAX) {
                flags = WORLD_SIZE_CLASS(1) | GROUND_CRASHES | WORLD_SOLID;
            } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
                flags = WORLD_SIZE_CLASS(2) | GROUND_CRASHES | WORLD_SOLID;
            } else {
                flags = WORLD_SIZE_CLASS(3) | GROUND_CRASHES | WORLD_SOLID;
            }
        }
        world_add(flags, task, object);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx((NJS_VECTOR *)RAIL(task)->object.pos);
        njRotateEx(&work->ang.x, XYZ_TURN);
        njCalcPoint(NULL, &RAIL(task)->object.model->center,
                    &RAIL(task)->center);
        njPopMatrixEx();
    }
}

static void barrier_destroy(struct task *task)
{
    if (RAIL(task)->object.model != NULL) {
        world_remove(task, &RAIL(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void barrier_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: a goto to the next statement, where none would do */
    goto check_range;
check_range:
    if (player_near(&RAIL(task)->center,
                    RAIL(task)->object.model->r + NEAR_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

void curved_rail_init(struct task *task)
{
    task->work->submode = RAIL_PLAIN;
    rail_init(task);
}

void curved_rail_b_init(struct task *task)
{
    task->work->submode = RAIL_B;
    rail_init(task);
}

static void rail_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct path_task *record;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct rail_collision));
    if (task->motion.p != NULL) {
        task->step = rail_step;
        task->draw = rail_draw;
        task->destructor = rail_destroy;
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((task->extra.p = grind_rail_path_place(
                 &work->pos, &work->ang, &curved_rail_path)) != NULL) {
            child = task_create_child(0, path_rail_init, task);
            if (child != NULL) {
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((record = syCalloc(1, sizeof(struct path_task))) == NULL) {
                    task_destroy(child);
                } else {
                    child->extra.p = record;
                    record->path = task->extra.p;
                }
            }
        }
        /* @hack spelling: submode tested for truth, where submode !=
         * RAIL_PLAIN would do */
        if (work->submode) {
            RAIL(task)->object = *curved_rail_b_collision;
        } else {
            RAIL(task)->object = curved_rail_collision;
        }
        object = &RAIL(task)->object;
        /* the barrier, GLIND_B's child object, goes in the world by a task of
         * its own */
        object->child = NULL;
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->evalflags |= NJD_EVAL_BREAK;
        object->ang[2] = work->ang.z;
        object->ang[0] = work->ang.x;
        object->ang[1] = work->ang.y;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        flags = WORLD_SOLID;
        radius = object->model->r + NEAR_MARGIN;
        if (radius <= WORLD_SIZE_CLASS_1_MAX) {
            if (radius > WORLD_SIZE_CLASS_2_MAX) {
                flags = WORLD_SIZE_CLASS(1) | WORLD_SOLID;
            } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
                flags = WORLD_SIZE_CLASS(2) | WORLD_SOLID;
            } else {
                flags = WORLD_SIZE_CLASS(3) | WORLD_SOLID;
            }
        }
        world_add(flags, task, object);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx((NJS_VECTOR *)RAIL(task)->object.pos);
        njRotateEx(&work->ang.x, XYZ_TURN);
        njCalcPoint(NULL, &RAIL(task)->object.model->center,
                    &RAIL(task)->center);
        njPopMatrixEx();
        if (work->submode == RAIL_B) {
            child = task_create_child(TASK_WORK, barrier_init, task);
            if (child != NULL) {
                child->work->pos = work->pos;
                child->work->ang = work->ang;
            }
        }
    }
}

static void rail_destroy(struct task *task)
{
    struct path *path;

    if (task->extra.p != NULL) {
        path = task->extra.p;
        syFree(path->points);
        syFree(path);
    }
    task->extra.p = NULL;
    if (RAIL(task)->object.model != NULL) {
        world_remove(task, &RAIL(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void rail_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches. SHC still places its strings,
     * the SET editor's captions, in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- TEXSPD";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, XYZ_TURN);
    /* @hack spelling: submode tested for truth, where submode != RAIL_PLAIN
     * would do */
    if (work->submode) {
        njSetTexture(&curved_rail_b_texlist);
        object = &curved_rail_b_object;
    } else {
        njSetTexture(&curved_rail_texlist);
        object = &curved_rail_object;
    }
    njCnkEasyDrawModel(object->model);
    object = object->child;
    texture_anim_scroll(
        object->model, &curved_rail_anim,
        (game_state.steps * (Sint32)work->scl.x) & (SCROLL_WRAP - 1), 0);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

static void rail_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: an else after the return, where code after the if would
     * do */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (player_near(&RAIL(task)->center,
                        RAIL(task)->object.model->r + NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}
