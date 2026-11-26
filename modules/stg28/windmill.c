/* the windmill, WINDMILL in Pyramid Cave's object list: a base with a rotor
 * beside it. the rotor turns about the base's x axis */
#include <ninja.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg28/cave_util.h"
#include "stg28/windmill.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define WINDMILL_SHAPES 3
/* the flags of the rotor's shape in the collision world */
#define ROTOR_WORLD                                                            \
    (WORLD_SIZE_CLASS(1) | WORLD_TURNED | WORLD_MOVING | GROUND_CRASHES |      \
     WORLD_SOLID)
/* the rotor's shape in the collision world collides only while a player lies
 * within REACH of its pos */
#define REACH 88.74F

/* the rotor's Angle about the base's x axis, in task's unk_3c */
#define ROTOR_ANGLE(task) ((task)->unk_3c.l)
/* the Angle the rotor turns a step, which the init moves from the work's
 * ang.z */
#define ROTOR_TURN(work) ((work)->timer)
/* how far along the base's x axis the rotor stands */
#define ROTOR_X(work) ((work)->scl.x)

#include "stg28/windmill/text.inc"
#include "stg28/models/windmill.h"

static void windmill_destroy(struct task *task);
static void windmill_step(struct task *task);
static void windmill_draw(struct task *task);

NJS_TEXLIST *cave_windmill_texlists[3] = {
#include "stg28/windmill/cave_windmill_texlists.inc"
};

/* The thin box and its two end spheres push players, enemies and objects. */
static struct collision_shape windmill_shapes[WINDMILL_SHAPES] = {
#include "stg28/windmill/windmill_shapes.inc"
};

void cave_windmill_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 rotor;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)WINDMILL_EDITOR_ANGLE_Y;
        (void)WINDMILL_EDITOR_SPEED;
        (void)WINDMILL_EDITOR_X;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = windmill_draw;
    task->step = windmill_step;
    task->destructor = windmill_destroy;
    ROTOR_ANGLE(task) = rand() * (ANGLE_MASK * RAND_SCALE);
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c50a824).
     * windmill_destroy tests task->extra for null (TST at 0x8c50a8de).
     * windmill_step writes the copy's ang untested (MOV.L at 0x8c50a914) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the windmill, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_windmill_rotor_collision;
    /* @hack spelling: the work read again through task, where work
     * would do */
    ROTOR_TURN(task->work) = task->work->ang.z;
    task->work->ang.z = 0;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = ROTOR_ANGLE(task);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    /* @hack spelling: the work read again through task, where work
     * would do */
    cave_point_turn_y(ROTOR_X(task->work), 0.0F, work->ang.y, &rotor);
    object->pos[0] = rotor.x + work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = rotor.z + work->pos.z;
    world_add(ROTOR_WORLD, task, object);
    collision_init(task, windmill_shapes, WINDMILL_SHAPES, COLLISION_OBJECT);
}

static void windmill_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void windmill_step(struct task *task)
{
    NJS_OBJECT *object;
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    object->ang[0] = ROTOR_ANGLE(task);
    /* @hack spelling: each flag written as an assignment of the flags
     * with it set or cleared, where |= and &= would do */
    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags = work->flags | WORLD_ADDED;
    } else {
        work->flags = work->flags & ~WORLD_ADDED;
    }
    collision_register(task);
    /* @hack spelling: the work read again through task, where work
     * would do */
    ROTOR_ANGLE(task) += ROTOR_TURN(task->work);
}

static void windmill_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cave_windmill_base_texlist);
    njCnkEasyDrawModel(&cave_windmill_base_model);
    /* @hack spelling: the work read again through task, where work would do */
    njTranslate(NULL, ROTOR_X(task->work), 0.0F, 0.0F);
    njRotateX(NULL, ROTOR_ANGLE(task));
    njSetTexture(&cave_windmill_rotor_texlist);
    njCnkEasyDrawModel(&cave_windmill_rotor_model);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
