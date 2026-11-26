/* the container 0, MHCONT0 and MHCONT0_F in Metal Harbor's object list. it
 * slides to and fro along a rail on its x, and rebounds at each end */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg10/container_0.h"
#include "stg10/container_0/text.inc"
#include "system/areas.h"
#include "system/task.h"
#include "system/util.h"

/* a container's modes, in its work's mode. it slides toward +x in MODE_OUT
 * and toward -x in MODE_BACK. container_0_init sets MODE_START, which slides as
 * MODE_OUT does */
enum container_0_mode { MODE_START, MODE_OUT, MODE_BACK };

/* a container's record, in task->motion */
struct container_0 {
    /* the copy of container_0_collision that the collision world holds,
     * placed where the container lies */
    NJS_OBJECT object;
    /* the container's place on the rail, from its middle, in the rail's
     * axes. only x changes */
    NJS_POINT3 offset;
};
#define CONTAINER(task) ((struct container_0 *)(task)->motion.p)
/* task's TASK_FORCES records, which TASK_FORCE allocates */
#define FORCES(task) ((struct task_force *)(task)->force.p)

/* the container's speed along the rail, a Float kept in the pointer
 * task->extra. the task has no TASK_EXTRA record. task_destroy frees a non-null
 * extra, so the destructor clears the word */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define SPEED(task) (*(Float *)&(task)->extra)

/* the work's timer is 0 while the container slides. it counts the steps
 * from the container's arrival at an end. the container leaves that end once
 * the count passes SETTLE_STEPS */
#define SETTLE_STEPS 30
/* a sliding step adds ACCEL_SHARE of the top speed to the speed, up to the
 * top speed */
#define ACCEL_SHARE 0.1F
/* scl.y plus TOP_SPEED_MARGIN is the top speed */
#define TOP_SPEED_MARGIN 0.1F
/* while the container settles at an end, each step sets its speed anew. the
 * new speed is SPRING times the distance left to the end, plus DAMPING times
 * the old. a container that passes the end goes back to it, and its speed
 * becomes -REBOUND times itself */
#define SPRING 0.6F
#define DAMPING 0.92F
#define REBOUND 0.6F
/* a container whose ends lie QUIET_RUN or less from the middle plays no
 * sound */
#define QUIET_RUN 10.0F
#define SOUND_LEAVE_BACK 0 /* as it leaves the -x end */
#define SOUND_LEAVE_OUT 1 /* as it leaves the +x end */
#define SOUND_ARRIVE 2 /* as it meets either end */
/* a player is near the world's copy within its model's radius plus
 * REACH_MARGIN of the model's center. the world collects the copy only while
 * one is near. the same sum gives the copy's size class */
#define REACH_MARGIN 5.0F
#define CONTAINER_SHAPES 1

/* The stepped shipping container, including its collision hull. */

static NJS_TEXNAME container_0_textures[7] = {
#include "stg10/container_0/container_0_textures.inc"
};

static NJS_TEXLIST container_0_texlist = {
#include "stg10/container_0/container_0_texlist.inc"
};

static Sint16 container_0_polygons[709] = {
#include "stg10/container_0/container_0_polygons.inc"
};

static Sint32 container_0_vertices[679] = {
#include "stg10/container_0/container_0_vertices.inc"
};

static NJS_CNK_MODEL container_0_model = {
#include "stg10/container_0/container_0_model.inc"
};

static NJS_CNK_OBJECT container_0_object = {
#include "stg10/container_0/container_0_object.inc"
};

static NJS_MATERIAL container_0_collision_materials[1] = {
#include "stg10/container_0/container_0_collision_materials.inc"
};

static Sint16 container_0_collision_indices_0[33] = {
#include "stg10/container_0/container_0_collision_indices_0.inc"
};

static NJS_MESHSET container_0_collision_meshsets[1] = {
#include "stg10/container_0/container_0_collision_meshsets.inc"
};

static NJS_POINT3 container_0_collision_points[16] = {
#include "stg10/container_0/container_0_collision_points.inc"
};

static NJS_VECTOR container_0_collision_normals[16] = {
#include "stg10/container_0/container_0_collision_normals.inc"
};

static NJS_MODEL container_0_collision_model = {
#include "stg10/container_0/container_0_collision_model.inc"
};

static NJS_OBJECT container_0_collision = {
#include "stg10/container_0/container_0_collision.inc"
};

NJS_TEXLIST *container_0_texlists[2] = {
#include "stg10/container_0/container_0_texlists.inc"
};
/* clang-format off */
/* the box the container's child collides with */
static struct collision_shape container_0_shapes[CONTAINER_SHAPES] = {
#include "stg10/container_0/container_0_shapes.inc"
};
/* clang-format on */
/* the build of container_0_object that container_0_build last made, or null
 * where it made none */
static NJS_DIRECT_HEAD *container_0_built = NULL;

static void container_0_destroy(struct task *task);
static void container_0_step(struct task *task);
static void container_0_draw(struct task *task);

Uint8 *container_0_build(Uint8 *buf)
{
    Uint8 *end;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&container_0_texlist);
    end = buf + njCnkDirectObjectCompileSize(&container_0_object);
    if (end < DEMO_AREA) {
        njCnkDirectObjectCompile(&container_0_object, buf);
        container_0_built = (NJS_DIRECT_HEAD *)buf;
        buf = end;
    } else {
        container_0_built = NULL;
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    return buf;
}

void container_0_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float size;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct container_0));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = container_0_draw;
    task->step = container_0_step;
    task->destructor = container_0_destroy;
    work->mode = MODE_START;
    object = &CONTAINER(task)->object;
    *object = container_0_collision;
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
    model_object_create(task, NULL, NULL, container_0_shapes, CONTAINER_SHAPES,
                        0);
    if (task->child != NULL) {
        task->child->work->pos = work->pos;
        task->child->work->ang = work->ang;
    }
    SPEED(task) = 0.0F;
}

static void container_0_destroy(struct task *task)
{
    if (CONTAINER(task)->object.model != NULL) {
        world_remove(task, &CONTAINER(task)->object);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void container_0_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
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
        if (work->timer) {
            CONTAINER(task)->offset.x += SPEED(task);
            if (CONTAINER(task)->offset.x > work->scl.x) {
                CONTAINER(task)->offset.x = work->scl.x;
                SPEED(task) = -(SPEED(task) * REBOUND);
            } else {
                SPEED(task) =
                    ((work->scl.x - CONTAINER(task)->offset.x) * SPRING) +
                    (SPEED(task) * DAMPING);
            }
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = MODE_BACK;
                work->timer = 0;
                SPEED(task) = 0.0F;
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_LEAVE_OUT, NULL, 0, 0, &work->pos);
                }
            }
        } else {
            SPEED(task) += (work->scl.y + TOP_SPEED_MARGIN) * ACCEL_SHARE;
            if (SPEED(task) > work->scl.y + TOP_SPEED_MARGIN) {
                SPEED(task) = work->scl.y + TOP_SPEED_MARGIN;
            }
            CONTAINER(task)->offset.x += SPEED(task);
            if (CONTAINER(task)->offset.x > work->scl.x) {
                CONTAINER(task)->offset.x = work->scl.x;
                work->timer++;
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_ARRIVE, NULL, 0, 0, &work->pos);
                }
            }
        }
        break;
    case MODE_BACK:
        if (work->timer) {
            CONTAINER(task)->offset.x -= SPEED(task);
            if (CONTAINER(task)->offset.x < -work->scl.x) {
                CONTAINER(task)->offset.x = -work->scl.x;
                SPEED(task) = -(SPEED(task) * REBOUND);
            } else {
                SPEED(task) =
                    ((work->scl.x + CONTAINER(task)->offset.x) * SPRING) +
                    (SPEED(task) * DAMPING);
            }
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = MODE_OUT;
                work->timer = 0;
                SPEED(task) = 0.0F;
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_LEAVE_BACK, NULL, 0, 0, &work->pos);
                }
            }
        } else {
            SPEED(task) += (work->scl.y + TOP_SPEED_MARGIN) * ACCEL_SHARE;
            if (SPEED(task) > work->scl.y + TOP_SPEED_MARGIN) {
                SPEED(task) = work->scl.y + TOP_SPEED_MARGIN;
            }
            CONTAINER(task)->offset.x -= SPEED(task);
            if (CONTAINER(task)->offset.x < -work->scl.x) {
                CONTAINER(task)->offset.x = -work->scl.x;
                work->timer++;
                if (work->scl.x > QUIET_RUN) {
                    sound_play_at(SOUND_ARRIVE, NULL, 0, 0, &work->pos);
                }
            }
        }
        break;
    }
    if (task->child != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &CONTAINER(task)->offset, &task->child->work->pos);
        njPopMatrix(1);
    }
    if (CONTAINER(task)->object.model != NULL) {
        object = &CONTAINER(task)->object;
        /* @hack spelling: three Float pointers, each cast from a whole
         * vector, stepped through the vectors, where the members'
         * differences written out would do */
        dst = (Float *)&moved;
        src = (Float *)&CONTAINER(task)->offset;
        sub = (Float *)&before;
        *dst++ = *src++ - *sub++;
        *dst++ = *src++ - *sub++;
        *dst = *src - *sub;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &CONTAINER(task)->offset, (NJS_POINT3 *)object->pos);
        njCalcVector(NULL, &moved, &FORCES(task)[0].speed);
        njTranslateV(NULL, &object->model->center);
        njCalcPoint(NULL, &CONTAINER(task)->offset, &center);
        njPopMatrix(1);
        FORCES(task)[1].speed = FORCES(task)[0].speed;
        if (player_near(&center, object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void container_0_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- MOVE LENGTH";
        (void)"<- MOVE SPD";
    }
    njSetTexture(&container_0_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslateV(NULL, &CONTAINER(task)->offset);
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (container_0_built != NULL) {
        njDirectDrawObject(container_0_built);
    } else {
        njCnkEasyDrawModel(&container_0_model);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
}
