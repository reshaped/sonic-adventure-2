/* Sky Rail's moving models: RAILOBJCL, RAILOBJCR, MOTOOL, RAILOBJBR and
 * RAILOBJBL in the stage's object list. the first two swing a part. the other
 * three turn their node's children */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg06/rail_objects.h"
#include "stg06/solid_models.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for a model's placement */
static const char rail_objects_edit_angle_x[12] = "<- ANGLE X";
static const char rail_objects_edit_angle_y[12] = "<- ANGLE Y";
static const char rail_objects_edit_angle_z[12] = "<- ANGLE Z";
static const char rail_objects_edit_rot_spd[12] = "<- ROT SPD";

#include "stg06/rail_objects/text.inc"
#include "stg06/scenery_art/rail_objects.h"

/* a swinging model's node and texture list, in task->motion and task->force */
#define SWING_NODE(task) ((NJS_CNK_OBJECT *)(task)->motion.p)
#define SWING_TEXLIST(task) ((NJS_TEXLIST *)(task)->force.p)
/* a turning model's record, from the heap */
#define SOLID(task) ((struct solid_model *)(task)->motion.p)

/* a turning model takes its size class from its collision model's radius plus
 * REACH_MARGIN. a player is near it within that reach of the collision model's
 * center */
#define REACH_MARGIN 15.0F
#define SWING_SHAPES 1
#define TURNING_SHAPES 1

NJS_TEXLIST *rail_objects_texlists[6] = {
#include "stg06/rail_objects/rail_objects_texlists.inc"
};
/* clang-format off */
static struct collision_shape railobjcl_shapes[SWING_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {-23.0F, 16.0F, 0.0F}, 8.0F, 14.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape railobjcr_shapes[SWING_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {23.0F, 16.0F, 0.0F}, 8.0F, 14.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* a swinging part swings swing_degrees either way about x, on a sine of the
 * work's timer. the timer gains swing_rate a step */
static Angle swing_rate = 0x300;
static Float swing_degrees = 55.0F;
/* clang-format off */
static struct collision_shape motool_shapes[TURNING_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, 30.0F, 3.0F, 0.0F, 0, {0, 0, QUARTER_TURN}},
};
static struct collision_shape railobjbr_shapes[TURNING_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {24.0F, 24.0F, 0.0F}, 10.0F, 24.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape railobjbl_shapes[TURNING_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {-24.0F, 24.0F, 0.0F}, 10.0F, 24.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void swing_destroy(struct task *task);
static void swing_step(struct task *task);
static void swing_draw(struct task *task);
static void turning_start(struct task *task, NJS_TEXLIST *texlist,
                          NJS_CNK_OBJECT *node, NJS_OBJECT *collision,
                          Bool about_x, Uint32 flags,
                          struct collision_shape *shapes, int count);
static void turning_destroy(struct task *task);
static void turning_step(struct task *task);
static void turning_draw_x(struct task *task);
static void turning_draw_z(struct task *task);

void railobjcl_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = swing_draw;
    task->step = swing_step;
    task->destructor = swing_destroy;
    work->submode = 0;
    task->force.p = &railobjcl_texlist;
    task->motion.p = &railobjcl_object;
    work->timer = RANDOM_TURN();
    collision_init(task, railobjcl_shapes, SWING_SHAPES, COLLISION_OBJECT);
}

void railobjcr_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = swing_draw;
    task->step = swing_step;
    task->destructor = swing_destroy;
    work->submode = 0;
    task->force.p = &railobjcr_texlist;
    task->motion.p = &railobjcr_object;
    work->timer = RANDOM_TURN();
    collision_init(task, railobjcr_shapes, SWING_SHAPES, COLLISION_OBJECT);
}

/* task_destroy frees task->motion and task->force once the destructor returns.
 * they point at a static node and texture list, so the destructor clears
 * them */
static void swing_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
}

static void swing_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer += swing_rate;
        /* @hack redundant: an empty else */
    } else {
    }
    collision_register(task);
}

static void swing_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *node = SWING_NODE(task);

    njSetTexture(SWING_TEXLIST(task));
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (node->ang[1]) {
        njRotateY(NULL, node->ang[1]);
    }
    njCnkEasyDrawModel(node->model);
    node = node->child;
    njTranslateV(NULL, (NJS_VECTOR *)node->pos);
    njRotateY(NULL, node->ang[1]);
    njRotateX(NULL,
              (Angle)(njSin(work->timer) * swing_degrees * ANGLE_PER_DEGREE));
    njCnkEasyDrawModel(node->model);
    njPopMatrix(1);
}

void motool_init(struct task *task)
{
    turning_start(task, &motool_texlist, &motool_object, &motool_collision,
                  TRUE, 0, motool_shapes, TURNING_SHAPES);
}

void railobjbr_init(struct task *task)
{
    turning_start(task, &railobjbr_texlist, &railobjbr_object, NULL, TRUE, 0,
                  railobjbr_shapes, TURNING_SHAPES);
}

void railobjbl_init(struct task *task)
{
    turning_start(task, &railobjbl_texlist, &railobjbl_object, NULL, TRUE, 0,
                  railobjbl_shapes, TURNING_SHAPES);
}

/* set task up to draw node with texlist. the draw turns node's children about
 * x, or about z where about_x is FALSE. a null node draws nothing. where
 * collision isn't null, a copy of it stands in the collision world under flags
 * and WORLD_SOLID. a non-null shapes then gives task a collision record of
 * count shapes. where collision is null, task gets neither */
static void turning_start(struct task *task, NJS_TEXLIST *texlist,
                          NJS_CNK_OBJECT *node, NJS_OBJECT *collision,
                          Bool about_x, Uint32 flags,
                          struct collision_shape *shapes, int count)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 world_flags;

    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct solid_model));
    if (task->motion.p == NULL) {
        return;
    }
    SOLID(task)->node = node;
    SOLID(task)->texlist = texlist;
    if (SOLID(task)->node != NULL) {
        if (about_x) {
            task->draw = turning_draw_x;
        } else {
            task->draw = turning_draw_z;
        }
    }
    task->step = turning_step;
    task->destructor = turning_destroy;
    /* @bug logic: RAILOBJBR and RAILOBJBL never collide. each passes shapes,
     * at 0x8c502554 and 0x8c502574, but no collision object, at 0x8c502568
     * and 0x8c502588. the return here, at 0x8c50266e, comes before
     * collision_init takes the shapes */
    if (collision == NULL) {
        return;
    }
    object = task->motion.p;
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
    world_flags = flags | WORLD_SOLID;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            world_flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            world_flags |= WORLD_SIZE_CLASS(2);
        } else {
            world_flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(world_flags, task, object);
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &SOLID(task)->center);
    njPopMatrix(1);
}

static void turning_destroy(struct task *task)
{
    if (SOLID(task)->object.model != NULL) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void turning_step(struct task *task)
{
    struct task_work *work = task->work;
    struct solid_model *solid;
    NJS_OBJECT *object;

    /* @hack redundant: the else after the return */
    if (!work->submode && placement_try_leave(task)) {
        return;
    } else if (SOLID(task)->object.model != NULL) {
        /* @hack spelling: solid set inside the expression that sets object,
         * where a statement of its own would do */
        object = &(solid = task->motion.p)->object;
        if (player_near(&solid->center, object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
    if (!game_state.time_stopped) {
        work->timer += (Angle)(work->scl.x * ANGLE_PER_DEGREE);
    }
}

static void turning_draw_x(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *node = SOLID(task)->node;

    njSetTexture(SOLID(task)->texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(node->model);
    njPushMatrixEx();
    node = node->child;
    /* @hack spelling: a test, then a do while, where a for would do */
    if (node != NULL) {
        do {
            njTranslateEx((NJS_VECTOR *)node->pos);
            njRotateX(NULL, work->timer);
            njRotateY(NULL, node->ang[1]);
            njCnkEasyDrawModel(node->model);
            matrix_load_parent();
            node = node->sibling;
        } while (node != NULL);
    }
    njPopMatrix(2);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void turning_draw_z(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *node = SOLID(task)->node;

    njSetTexture(SOLID(task)->texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(node->model);
    njPushMatrixEx();
    node = node->child;
    /* @hack spelling: a test, then a do while, where a for would do */
    if (node != NULL) {
        do {
            njTranslateEx((NJS_VECTOR *)node->pos);
            njRotateZ(NULL, work->timer);
            njRotateY(NULL, node->ang[1]);
            njCnkEasyDrawModel(node->model);
            matrix_load_parent();
            node = node->sibling;
        } while (node != NULL);
    }
    njPopMatrix(2);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
