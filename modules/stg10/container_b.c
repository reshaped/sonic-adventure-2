/* the container B, MHCONTB in Metal Harbor's object list. it slides to and
 * fro along a rail, and stops dead at each end. it is container.c's container
 * with another model and box, and no spring */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg10/container_b.h"
#include "stg10/container_b/text.inc"
#include "system/task.h"
#include "system/util.h"

/* a container's modes, in its work's mode. it slides toward +z in MODE_OUT
 * and toward -z in MODE_BACK. container_b_init sets MODE_START, which slides as
 * MODE_OUT does */
enum container_b_mode { MODE_START, MODE_OUT, MODE_BACK };

/* a container's record, in task->motion */
struct container_b {
    /* the copy of container_b_collision that the collision world holds,
     * placed where the container lies */
    NJS_OBJECT object;
    /* the container's place on the rail, from its middle, in the rail's
     * axes. only z changes */
    NJS_POINT3 offset;
};
#define CONTAINER(task) ((struct container_b *)(task)->motion.p)
/* task's TASK_FORCES records, which TASK_FORCE allocates */
#define FORCES(task) ((struct task_force *)(task)->force.p)

/* the container's speed along the rail, a Float kept in the pointer
 * task->extra. the task has no TASK_EXTRA record. task_destroy frees a non-null
 * extra, so the destructor clears the word */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define SPEED(task) (*(Float *)&(task)->extra)

/* the work's timer is 0 while the container slides. it counts the steps the
 * container stands at an end, from its arrival. the container leaves that end
 * once the count passes WAIT_STEPS */
#define WAIT_STEPS 30
/* a sliding step adds ACCEL_SHARE of the top speed to the speed, up to the
 * top speed */
#define ACCEL_SHARE 0.1F
/* scl.y plus TOP_SPEED_MARGIN is the top speed */
#define TOP_SPEED_MARGIN 0.1F
/* a container whose ends lie QUIET_RUN or less from the middle plays no
 * sound */
#define QUIET_RUN 10.0F
#define SOUND_LEAVE_BACK 0 /* as it leaves the -z end */
#define SOUND_LEAVE_OUT 1 /* as it leaves the +z end */
#define SOUND_ARRIVE 2 /* as it meets either end */
/* a player is near the world's copy within its model's radius plus
 * REACH_MARGIN of the model's center. the world collects the copy only while
 * one is near. the same sum gives the copy's size class */
#define REACH_MARGIN 5.0F
#define CONTAINER_SHAPES 1

/* The second shipping container and its collision hull. */

static NJS_TEXNAME container_b_textures[6] = {
#include "stg10/container_b/container_b_textures.inc"
};

static NJS_TEXLIST container_b_texlist = {
#include "stg10/container_b/container_b_texlist.inc"
};

static Sint16 container_b_polygons[399] = {
#include "stg10/container_b/container_b_polygons.inc"
};

static Sint32 container_b_vertices[355] = {
#include "stg10/container_b/container_b_vertices.inc"
};

static NJS_CNK_MODEL container_b_model = {
#include "stg10/container_b/container_b_model.inc"
};

static NJS_CNK_OBJECT container_b_object = {
#include "stg10/container_b/container_b_object.inc"
};

static NJS_MATERIAL container_b_collision_materials[1] = {
#include "stg10/container_b/container_b_collision_materials.inc"
};

static Sint16 container_b_collision_indices_0[19] = {
#include "stg10/container_b/container_b_collision_indices_0.inc"
};

static NJS_MESHSET container_b_collision_meshsets[1] = {
#include "stg10/container_b/container_b_collision_meshsets.inc"
};

static NJS_POINT3 container_b_collision_points[8] = {
#include "stg10/container_b/container_b_collision_points.inc"
};

static NJS_VECTOR container_b_collision_normals[8] = {
#include "stg10/container_b/container_b_collision_normals.inc"
};

static NJS_MODEL container_b_collision_model = {
#include "stg10/container_b/container_b_collision_model.inc"
};

static NJS_OBJECT container_b_collision = {
#include "stg10/container_b/container_b_collision.inc"
};

NJS_TEXLIST *container_b_texlists[2] = {
#include "stg10/container_b/container_b_texlists.inc"
};
/* clang-format off */
/* the box the container's child collides with */
static struct collision_shape container_b_shapes[CONTAINER_SHAPES] = {
#include "stg10/container_b/container_b_shapes.inc"
};
/* clang-format on */

static void container_b_destroy(struct task *task);
static void container_b_step(struct task *task);
static void container_b_draw(struct task *task);

void container_b_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float size;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct container_b));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = container_b_draw;
    task->step = container_b_step;
    task->destructor = container_b_destroy;
    work->mode = MODE_START;
    object = &CONTAINER(task)->object;
    *object = container_b_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    /* @hack spelling: the angles set y, z, x, where x, y, z would do */
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    size = object->model->r + REACH_MARGIN;
    if (size <= WORLD_SIZE_CLASS_1_MAX) {
        if (size > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (size > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    model_object_create(task, NULL, NULL, container_b_shapes, CONTAINER_SHAPES,
                        0);
    if (task->child != NULL) {
        task->child->work->pos = work->pos;
        task->child->work->ang = work->ang;
    }
    SPEED(task) = 0.0F;
}

static void container_b_destroy(struct task *task)
{
    if (CONTAINER(task)->object.model != NULL) {
        world_remove(task, &CONTAINER(task)->object);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void container_b_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    struct container_b *container;
    NJS_POINT3 before;
    NJS_POINT3 center;
    NJS_VECTOR moved;
    Float *dst;
    Float *src;
    Float *sub;

    if (placement_try_leave(task)) {
        return;
    }
    before = CONTAINER(task)->offset;
    switch (work->mode) {
    default:
        SPEED(task) += (work->scl.y + TOP_SPEED_MARGIN) * ACCEL_SHARE;
        if (SPEED(task) > work->scl.y + TOP_SPEED_MARGIN) {
            SPEED(task) = work->scl.y + TOP_SPEED_MARGIN;
        }
        CONTAINER(task)->offset.z += SPEED(task);
        if (CONTAINER(task)->offset.z > work->scl.x) {
            CONTAINER(task)->offset.z = work->scl.x;
            SPEED(task) = 0.0F;
            if (!work->timer) {
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_ARRIVE, NULL, 0, 0, &work->pos);
                }
            }
            if (work->timer++ > WAIT_STEPS) {
                work->mode = MODE_BACK;
                work->timer = 0;
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_LEAVE_OUT, NULL, 0, 0, &work->pos);
                }
            }
        }
        break;
    case MODE_BACK:
        SPEED(task) += (work->scl.y + TOP_SPEED_MARGIN) * ACCEL_SHARE;
        if (SPEED(task) > work->scl.y + TOP_SPEED_MARGIN) {
            SPEED(task) = work->scl.y + TOP_SPEED_MARGIN;
        }
        CONTAINER(task)->offset.z -= SPEED(task);
        if (CONTAINER(task)->offset.z < -work->scl.x) {
            CONTAINER(task)->offset.z = -work->scl.x;
            SPEED(task) = 0.0F;
            if (!work->timer) {
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_ARRIVE, NULL, 0, 0, &work->pos);
                }
            }
            if (work->timer++ > WAIT_STEPS) {
                work->mode = MODE_OUT;
                work->timer = 0;
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_LEAVE_BACK, NULL, 0, 0, &work->pos);
                }
            }
        }
        break;
    }
    if (task->child != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &CONTAINER(task)->offset, &task->child->work->pos);
        njPopMatrixEx();
    }
    if (CONTAINER(task)->object.model != NULL) {
        /* @hack spelling: container, a local for task's record, where
         * CONTAINER(task) read in place would do */
        container = CONTAINER(task);
        object = &container->object;
        /* @hack spelling: three Float pointers, each cast from a whole
         * vector, stepped through the vectors, where the members'
         * differences written out would do */
        dst = (Float *)&moved;
        src = (Float *)&container->offset;
        sub = (Float *)&before;
        *dst++ = *src++ - *sub++;
        *dst++ = *src++ - *sub++;
        *dst = *src - *sub;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &CONTAINER(task)->offset, (NJS_POINT3 *)object->pos);
        njCalcVector(NULL, &moved, &FORCES(task)[0].speed);
        njTranslateEx(&object->model->center);
        njCalcPoint(NULL, &CONTAINER(task)->offset, &center);
        njPopMatrixEx();
        FORCES(task)[1].speed = FORCES(task)[0].speed;
        if (player_near(&center, object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void container_b_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- MOVE LENGTH";
        (void)"<- MOVE SPD";
    }
    njSetTexture(&container_b_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslateEx(&CONTAINER(task)->offset);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&container_b_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
