/* the mine tool, MITOOL in Aquatic Mine's object list: a solid model whose
 * wheel turns about x. a push at random speeds the wheel up, and drag slows it
 * down again. the wheel's surface is a child task. its copy of its collision
 * object turns with the wheel, and carries the players who stand on it */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg07/mine_tool.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg07/mine_tool/text.inc"

#include "stg07/scenery/mine_tool.h"

/* the record of the tool, and of the wheel's surface, in its task's motion */
struct mine_tool {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_POINT3 center; /* the collision model's center, in the world */
    /* the speed a push has yet to add to the wheel, in degrees a step. the
     * surface's stays 0 */
    Float push;
};

#define TOOL(task) ((struct mine_tool *)(task)->motion.p)
/* task's TASK_FORCES force records, one for each of players 0 and 1 */
#define FORCES(task) ((struct task_force *)(task)->force.p)
/* the wheel's angle about x, and its speed in degrees a step. the speed is a
 * Float in the word of task->extra */
#define WHEEL_ANGLE(work) ((work)->timer)
/* @hack type: the speed read as a Float through extra's address, where
 * (task)->extra.f would do */
#define WHEEL_SPEED(task) (*(Float *)&(task)->extra)
/* the most of a push that one step adds to the wheel's speed */
#define PUSH_STEP 0.008F
/* each copy collides while a player lies within its model's radius plus
 * REACH_MARGIN of its center. the same sum gives its size class */
#define REACH_MARGIN 15.0F

NJS_TEXLIST *mine_tool_texlists[2] = {
#include "stg07/mine_tool/mine_tool_texlists.inc"
};
/* a step keeps mine_tool_drag of the wheel's speed. while no push is left, a
 * new one comes by a chance of mine_tool_push_chance a step. a push is worth
 * up to mine_tool_push_most degrees a step, at random */
static Float mine_tool_drag = 0.988F;
static Float mine_tool_push_chance = 0.005F;
static Float mine_tool_push_most = 1.5F;

static void mine_tool_destroy(struct task *task);
static void mine_tool_step(struct task *task);
static void mine_tool_draw(struct task *task);
static void mine_tool_surface_init(struct task *task);
static void mine_tool_surface_destroy(struct task *task);
static void mine_tool_surface_step(struct task *task);

void mine_tool_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    Float reach;
    Uint32 flags;
    struct task *surface;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct mine_tool));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = mine_tool_draw;
    task->step = mine_tool_step;
    task->destructor = mine_tool_destroy;
    work->submode = 0;
    WHEEL_ANGLE(work) = (Float)rand() * (FULL_TURN * RAND_SCALE);
    object = &TOOL(task)->object;
    *object = mine_tool_collision;
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
    njCalcPoint(NULL, (NJS_POINT3 *)mine_tool_collision.pos, &pos);
    njPopMatrix(1);
    object->pos[0] = pos.x;
    object->pos[1] = pos.y;
    object->pos[2] = pos.z;
    flags = WORLD_SOLID | WORLD_NO_CLIMB;
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
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njTranslateV(NULL, (NJS_VECTOR *)mine_tool_collision.pos);
    njCalcPoint(NULL, &mine_tool_collision.model->center, &TOOL(task)->center);
    njPopMatrix(1);
    surface =
        task_create_child(TASK_WORK | TASK_FORCE, mine_tool_surface_init, task);
    if (surface != NULL) {
        surface->extra.p = &mine_tool_surface_collision;
        /* @hack redundant: the surface's pos and ang set again, which
         * task_create_child sets from the work already */
        surface->work->pos = work->pos;
        surface->work->ang = work->ang;
    }
    TOOL(task)->push = 0.0F;
}

/* task_destroy frees task->extra once the destructor returns. the tool's holds
 * the wheel's speed, not a block of the heap, so the destructor clears it */
static void mine_tool_destroy(struct task *task)
{
    if (TOOL(task)->object.model != NULL) {
        world_remove(task, &TOOL(task)->object);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void mine_tool_step(struct task *task)
{
    struct task_work *work = task->work;
    struct mine_tool *tool;
    Float reach;
    /* @hack redundant: added declared register, where a plain local would do */
    register Float added;

    /* @hack redundant: the else after the return */
    if (placement_try_leave(task)) {
        return;
    } else {
        WHEEL_ANGLE(work) += (Sint32)(WHEEL_SPEED(task) * ANGLE_PER_DEGREE);
        if (TOOL(task)->object.model != NULL) {
            /* @hack spelling: the assignment to tool inside the sum, where a
             * statement of its own would do */
            reach = (tool = task->motion.p)->object.model->r + REACH_MARGIN;
            if (player_near(&tool->center, reach)) {
                work->flags |= WORLD_ADDED;
            } else {
                work->flags &= ~WORLD_ADDED;
            }
        }
        if (TOOL(task)->push == 0.0F &&
            (Float)rand() * RAND_SCALE < mine_tool_push_chance) {
            TOOL(task)->push +=
                (Float)rand() * mine_tool_push_most * RAND_SCALE;
        }
        added = TOOL(task)->push;
        if (TOOL(task)->push > PUSH_STEP) {
            added = PUSH_STEP;
        }
        TOOL(task)->push -= added;
        WHEEL_SPEED(task) += added;
        WHEEL_SPEED(task) *= mine_tool_drag;
        if (task->child != NULL) {
            task->child->work->ang.x = WHEEL_ANGLE(work) + work->ang.x;
        }
    }
}

static void mine_tool_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(&mine_tool_texlist);
    object = &mine_tool_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateX(NULL, WHEEL_ANGLE(work));
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}

/* the surface's first step. task->extra holds the collision object whose copy
 * the surface puts in the collision world */
static void mine_tool_surface_init(struct task *task)
{
    /* @hack spelling: the copy of task, where task would do */
    struct task *surface = task;
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_OBJECT *source;
    NJS_POINT3 pos;
    Float reach;
    Uint32 flags;

    task->motion.p = syCalloc(1, sizeof(struct mine_tool));
    if (task->motion.p == NULL) {
        return;
    }
    task->step = mine_tool_surface_step;
    task->destructor = mine_tool_surface_destroy;
    work->submode = 0;
    object = &TOOL(task)->object;
    source = surface->extra.p;
    *object = *source;
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
    njCalcPoint(NULL, (NJS_POINT3 *)source->pos, &pos);
    njPopMatrix(1);
    object->pos[0] = pos.x;
    object->pos[1] = pos.y;
    object->pos[2] = pos.z;
    flags = WORLD_SOLID | GROUND_CRASHES | WORLD_MOVING;
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
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njTranslateV(NULL, (NJS_VECTOR *)source->pos);
    njCalcPoint(NULL, &source->model->center, &TOOL(task)->center);
    njPopMatrix(1);
}

/* the surface's task->extra points at mine_tool_surface_collision, not at a
 * block of the heap. the destructor clears it, as the tool's does */
static void mine_tool_surface_destroy(struct task *task)
{
    if (TOOL(task)->object.model != NULL) {
        world_remove(task, &TOOL(task)->object);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* a player's force record takes as its speed the move that the wheel's last
 * turn gives the player. the turn runs about the copy's x axis, from the copy's
 * ang.x to the work's, which mine_tool_step sets */
static void mine_tool_surface_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    int i;

    /* @hack redundant: a call no path reaches */
    if (0) {
        njPushMatrixEx();
    }
    if (TOOL(task)->object.model == NULL) {
        return;
    }
    njPushMatrixEx();
    for (i = 0; i < TASK_FORCES && player_work[i] != NULL; i++) {
        njUnitMatrix(NULL);
        njTranslateV(NULL, (NJS_VECTOR *)TOOL(task)->object.pos);
        njRotateZ(NULL, TOOL(task)->object.ang[2]);
        njRotateY(NULL, TOOL(task)->object.ang[1]);
        njRotateX(NULL, TOOL(task)->object.ang[0]);
        njRotateX(NULL, work->ang.x - TOOL(task)->object.ang[0]);
        njRotateX(NULL, -TOOL(task)->object.ang[0]);
        njRotateY(NULL, -TOOL(task)->object.ang[1]);
        njRotateZ(NULL, -TOOL(task)->object.ang[2]);
        njTranslate(NULL, -TOOL(task)->object.pos[0],
                    -TOOL(task)->object.pos[1], -TOOL(task)->object.pos[2]);
        njCalcPoint(NULL, &player_work[i]->pos, &pos);
        FORCES(task)[i].speed.x = pos.x - player_work[i]->pos.x;
        FORCES(task)[i].speed.y = pos.y - player_work[i]->pos.y;
        FORCES(task)[i].speed.z = pos.z - player_work[i]->pos.z;
    }
    njPopMatrix(1);
    TOOL(task)->object.ang[1] = work->ang.y;
    TOOL(task)->object.ang[2] = work->ang.z;
    TOOL(task)->object.ang[0] = work->ang.x;
    /* @hack spelling: the flags written work->flags = work->flags | and &,
     * where |= and &= would do */
    if (player_near(&TOOL(task)->center,
                    TOOL(task)->object.model->r + REACH_MARGIN)) {
        work->flags = work->flags | WORLD_ADDED;
    } else {
        work->flags = work->flags & ~WORLD_ADDED;
    }
}
