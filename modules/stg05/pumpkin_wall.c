/* the pumpkin walls, PUMSASI and PUMWALL in Pumpkin Hill's object list. the
 * players stand on them and run against them */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg05/pumpkin_wall.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for the wall's placement */
static const char pumpkin_wall_edit_angle_x[12] = "<- ANGLE X";
static const char pumpkin_wall_edit_angle_y[12] = "<- ANGLE Y";
static const char pumpkin_wall_edit_angle_z[12] = "<- ANGLE Z";

#include "stg05/pumpkin_wall/text.inc"
#include "stg05/scenery_art/pumpkin_wall.h"

/* a wall's record, in task->motion */
struct pumpkin_wall {
    /* the wall's copy of its collision object in the collision world, placed
     * and turned as the wall is */
    NJS_OBJECT object;
    NJS_POINT3 center; /* object's model's center, in the world's axes */
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model; /* the model the wall draws, or null for none */
};

#define WALL(task) ((struct pumpkin_wall *)(task)->motion.p)

/* in the work's submode: the task is PUMSASI's child. it never calls
 * placement_try_leave, and leaves only with its parent */
#define WALL_CHILD 1
/* the wall takes its size class from its model's radius plus REACH_MARGIN. its
 * copy collides while a player lies within that reach of center */
#define REACH_MARGIN 15.0F

NJS_TEXLIST *pumpkin_wall_texlists[] = {&pumpkin_wall_texlist, &pumsasi_texlist,
                                        NULL};

static void pumpkin_wall_start(struct task *task, NJS_TEXLIST *texlist,
                               NJS_CNK_MODEL *model, NJS_OBJECT *collision,
                               Bool shadowed, Uint32 world_flags);
static void pumpkin_wall_destroy(struct task *task);
static void pumpkin_wall_step(struct task *task);
static void pumpkin_wall_draw(struct task *task);
static void pumpkin_wall_draw_shadowed(struct task *task);

void pumsasi_init(struct task *task)
{
    struct task *child;

    if (placement_try_leave(task)) {
        return;
    }
    if (task->child == NULL) {
        child =
            task_create_child(TASK_WORK | TASK_FORCE, pumpkin_wall_step, task);
        if (child != NULL) {
            child->work->submode = WALL_CHILD;
            pumpkin_wall_start(child, &pumsasi_texlist, NULL,
                               &pumsasi_child_collision, TRUE, 0);
        }
    }
    /* @hack spelling: task's shadowed written as 1.0F ? TRUE : TRUE, where
     * TRUE would do */
    pumpkin_wall_start(task, &pumsasi_texlist, &pumsasi_model,
                       /* NOLINTNEXTLINE(misc-redundant-expression) */
                       &pumsasi_collision, 1.0F ? TRUE : TRUE, WORLD_DUSTY);
}

void pumpkin_wall_init(struct task *task)
{
    pumpkin_wall_start(task, &pumpkin_wall_texlist, &pumpkin_wall_model,
                       &pumpkin_wall_collision, FALSE, 0);
}

/* set task up as a wall that draws model with texlist, or draws nothing where
 * model is null. the wall's copy of collision goes in the world under
 * world_flags and WORLD_SOLID. a shadowed wall draws under
 * DRAW_CONTROL_SHADOW */
static void pumpkin_wall_start(struct task *task, NJS_TEXLIST *texlist,
                               NJS_CNK_MODEL *model, NJS_OBJECT *collision,
                               Bool shadowed, Uint32 world_flags)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float reach;

    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct pumpkin_wall));
    if (task->motion.p == NULL) {
        return;
    }
    WALL(task)->model = model;
    WALL(task)->texlist = texlist;
    if (WALL(task)->model != NULL) {
        if (shadowed) {
            task->draw = pumpkin_wall_draw_shadowed;
        } else {
            task->draw = pumpkin_wall_draw;
        }
    }
    task->step = pumpkin_wall_step;
    task->destructor = pumpkin_wall_destroy;
    object = &WALL(task)->object;
    *object = *collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, (NJS_POINT3 *)object->pos, (NJS_POINT3 *)object->pos);
    njPopMatrix(1);
    flags = world_flags | WORLD_SOLID;
    reach = object->model->r + REACH_MARGIN;
    if (reach <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &WALL(task)->center);
    njPopMatrix(1);
}

static void pumpkin_wall_destroy(struct task *task)
{
    if (WALL(task)->object.model != NULL) {
        world_remove(task, &WALL(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void pumpkin_wall_step(struct task *task)
{
    struct task_work *work;

    /* @hack spelling: work assigned inside the test, where an initializer
     * would do */
    /* @hack redundant: the else after the return */
    if (!(work = task->work)->submode && placement_try_leave(task)) {
        return;
    } else if (WALL(task)->object.model != NULL) {
        struct pumpkin_wall *wall;
        /* @hack spelling: wall assigned inside object's initializer, where
         * statements of their own would do */
        NJS_OBJECT *object = &(wall = WALL(task))->object;

        if (player_near(&wall->center, object->model->r + REACH_MARGIN)) {
            /* @hack spelling: the flag set as flags = flags |
             * WORLD_ADDED, where |= would do */
            work->flags = work->flags | WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void pumpkin_wall_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(WALL(task)->texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(WALL(task)->model);
    njPopMatrix(1);
}

static void pumpkin_wall_draw_shadowed(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(WALL(task)->texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(WALL(task)->model);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
