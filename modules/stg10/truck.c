/* the truck, TRUCK in Metal Harbor's object list. it rolls to and fro along a
 * rail on two wheels, and rebounds at each end. its lamp blinks */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/vertex_colors.h"
#include "stg10/truck.h"
#include "stg10/truck/text.inc"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a truck's modes, in its work's mode. it rolls toward +z in MODE_OUT and
 * toward -z in MODE_BACK. harbor_truck_init sets MODE_START, which rolls as
 * MODE_OUT does */
enum harbor_truck_mode { MODE_START, MODE_OUT, MODE_BACK };

/* a truck's record, in task->motion */
struct truck {
    /* the copy of harbor_truck_collision that the collision world holds,
     * placed where the truck lies */
    NJS_OBJECT object;
    /* the truck's place on the rail, from its middle, in the rail's
     * axes. only z changes */
    NJS_POINT3 offset;
    /* the wheels' turn about x, which stands still while the truck
     * settles */
    Angle roll;
};
#define TRUCK(task) ((struct truck *)(task)->motion.p)

/* the truck's speed along the rail, a Float kept in the pointer task->extra.
 * the task has no TASK_EXTRA record. task_destroy frees a non-null extra, so
 * the destructor clears the word */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define SPEED(task) (*(Float *)&(task)->extra)

/* the work's timer is 0 while the truck rolls. it counts the steps from the
 * truck's arrival at an end. the truck leaves that end once the count passes
 * SETTLE_STEPS */
#define SETTLE_STEPS 30
/* a rolling step adds ACCEL_SHARE of the top speed to the speed, up to the
 * top speed */
#define ACCEL_SHARE 0.1F
/* scl.y plus TOP_SPEED_MARGIN is the top speed */
#define TOP_SPEED_MARGIN 0.1F
/* while the truck settles at an end, each step sets its speed anew. the new
 * speed is SPRING times the distance left to the end, plus DAMPING times the
 * old. a truck that passes the end goes back to it, and its speed becomes
 * -REBOUND times itself */
#define SPRING 0.6F
#define DAMPING 0.92F
#define REBOUND 0.6F
/* a player is near the world's copy within its model's radius plus
 * REACH_MARGIN of the model's center. the world collects the copy only while
 * one is near. the same sum gives the copy's size class */
#define REACH_MARGIN 5.0F
#define TRUCK_SHAPES 1

/* Harbor truck body, wheels and lamp, with its solid collision hull. */

static NJS_TEXNAME harbor_truck_textures[12] = {
#include "stg10/truck/harbor_truck_textures.inc"
};

static NJS_TEXLIST harbor_truck_texlist = {
#include "stg10/truck/harbor_truck_texlist.inc"
};

static Sint16 harbor_truck_lamp_polygons[57] = {
#include "stg10/truck/harbor_truck_lamp_polygons.inc"
};

static Sint32 harbor_truck_lamp_vertices[51] = {
#include "stg10/truck/harbor_truck_lamp_vertices.inc"
};

static NJS_CNK_MODEL harbor_truck_lamp_mesh = {
#include "stg10/truck/harbor_truck_lamp_mesh.inc"
};

static NJS_CNK_OBJECT harbor_truck_lamp_object[1] = {
#include "stg10/truck/harbor_truck_lamp_object.inc"
};

static Sint16 harbor_truck_out_wheel_polygons[39] = {
#include "stg10/truck/harbor_truck_out_wheel_polygons.inc"
};

static Sint32 harbor_truck_out_wheel_vertices[35] = {
#include "stg10/truck/harbor_truck_out_wheel_vertices.inc"
};

static NJS_CNK_MODEL harbor_truck_out_wheel_mesh = {
#include "stg10/truck/harbor_truck_out_wheel_mesh.inc"
};

static NJS_CNK_OBJECT harbor_truck_out_wheel_object[1] = {
#include "stg10/truck/harbor_truck_out_wheel_object.inc"
};

static Sint16 harbor_truck_back_wheel_polygons[39] = {
#include "stg10/truck/harbor_truck_back_wheel_polygons.inc"
};

static Sint32 harbor_truck_back_wheel_vertices[35] = {
#include "stg10/truck/harbor_truck_back_wheel_vertices.inc"
};

static NJS_CNK_MODEL harbor_truck_back_wheel_mesh = {
#include "stg10/truck/harbor_truck_back_wheel_mesh.inc"
};

static NJS_CNK_OBJECT harbor_truck_back_wheel_object[1] = {
#include "stg10/truck/harbor_truck_back_wheel_object.inc"
};

static Sint16 harbor_truck_body_polygons[659] = {
#include "stg10/truck/harbor_truck_body_polygons.inc"
};

static Sint32 harbor_truck_body_vertices[619] = {
#include "stg10/truck/harbor_truck_body_vertices.inc"
};

static NJS_CNK_MODEL harbor_truck_body_mesh = {
#include "stg10/truck/harbor_truck_body_mesh.inc"
};

static NJS_CNK_OBJECT harbor_truck_body_object[1] = {
#include "stg10/truck/harbor_truck_body_object.inc"
};

static NJS_MATERIAL harbor_truck_collision_materials[1] = {
#include "stg10/truck/harbor_truck_collision_materials.inc"
};

static Sint16 harbor_truck_collision_indices_0[31] = {
#include "stg10/truck/harbor_truck_collision_indices_0.inc"
};

static NJS_MESHSET harbor_truck_collision_meshsets[1] = {
#include "stg10/truck/harbor_truck_collision_meshsets.inc"
};

static NJS_POINT3 harbor_truck_collision_points[14] = {
#include "stg10/truck/harbor_truck_collision_points.inc"
};

static NJS_VECTOR harbor_truck_collision_normals[14] = {
#include "stg10/truck/harbor_truck_collision_normals.inc"
};

static NJS_MODEL harbor_truck_collision_model = {
#include "stg10/truck/harbor_truck_collision_model.inc"
};

static NJS_OBJECT harbor_truck_collision = {
#include "stg10/truck/harbor_truck_collision.inc"
};

NJS_TEXLIST *harbor_truck_texlists[2] = {
#include "stg10/truck/harbor_truck_texlists.inc"
};
/* the wheels' turn, in degrees a step, for each unit of the truck's speed */
static Float harbor_truck_wheel_turn = 7.0F;
/* the lamp's blink, in steps of game_state.steps. it is gray through step
 * LAMP_GRAY_LAST of each BLINK_STEPS, and white after */
#define BLINK_STEPS 48
#define LAMP_GRAY_LAST 20
#define LAMP_ON 0xffffffff
#define LAMP_OFF 0xff808080

/* clang-format off */
/* the box the truck's child collides with */
static struct collision_shape harbor_truck_shapes[TRUCK_SHAPES] = {
#include "stg10/truck/harbor_truck_shapes.inc"
};
/* clang-format on */

static void harbor_truck_destroy(struct task *task);
static void harbor_truck_step(struct task *task);
static void harbor_truck_draw(struct task *task);

void harbor_truck_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float size;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct truck));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = harbor_truck_draw;
    task->step = harbor_truck_step;
    task->destructor = harbor_truck_destroy;
    work->mode = MODE_START;
    object = &TRUCK(task)->object;
    *object = harbor_truck_collision;
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
    model_object_create(task, NULL, NULL, harbor_truck_shapes, TRUCK_SHAPES, 0);
    if (task->child != NULL) {
        task->child->work->pos = work->pos;
        task->child->work->ang = work->ang;
    }
    SPEED(task) = 0.0F;
}

static void harbor_truck_destroy(struct task *task)
{
    if (TRUCK(task)->object.model != NULL) {
        world_remove(task, &TRUCK(task)->object);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void harbor_truck_step(struct task *task)
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
    before = TRUCK(task)->offset;
    switch (work->mode) {
    default:
        if (work->timer) {
            TRUCK(task)->offset.z += SPEED(task);
            if (TRUCK(task)->offset.z > work->scl.x) {
                TRUCK(task)->offset.z = work->scl.x;
                SPEED(task) = -(SPEED(task) * REBOUND);
            } else {
                SPEED(task) = ((work->scl.x - TRUCK(task)->offset.z) * SPRING) +
                              (SPEED(task) * DAMPING);
            }
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = MODE_BACK;
                work->timer = 0;
                SPEED(task) = 0.0F;
            }
        } else {
            SPEED(task) += (work->scl.y + TOP_SPEED_MARGIN) * ACCEL_SHARE;
            if (SPEED(task) > work->scl.y + TOP_SPEED_MARGIN) {
                SPEED(task) = work->scl.y + TOP_SPEED_MARGIN;
            }
            TRUCK(task)->offset.z += SPEED(task);
            if (TRUCK(task)->offset.z > work->scl.x) {
                TRUCK(task)->offset.z = work->scl.x;
                work->timer++;
            }
            TRUCK(task)->roll += (Angle)(SPEED(task) * harbor_truck_wheel_turn *
                                         ANGLE_PER_DEGREE);
        }
        break;
    case MODE_BACK:
        if (work->timer) {
            TRUCK(task)->offset.z -= SPEED(task);
            if (TRUCK(task)->offset.z < -work->scl.x) {
                TRUCK(task)->offset.z = -work->scl.x;
                SPEED(task) = -(SPEED(task) * REBOUND);
            } else {
                SPEED(task) = ((work->scl.x + TRUCK(task)->offset.z) * SPRING) +
                              (SPEED(task) * DAMPING);
            }
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = MODE_OUT;
                work->timer = 0;
                SPEED(task) = 0.0F;
            }
        } else {
            SPEED(task) += (work->scl.y + TOP_SPEED_MARGIN) * ACCEL_SHARE;
            if (SPEED(task) > work->scl.y + TOP_SPEED_MARGIN) {
                SPEED(task) = work->scl.y + TOP_SPEED_MARGIN;
            }
            TRUCK(task)->roll -= (Angle)(SPEED(task) * harbor_truck_wheel_turn *
                                         ANGLE_PER_DEGREE);
            TRUCK(task)->offset.z -= SPEED(task);
            if (TRUCK(task)->offset.z < -work->scl.x) {
                TRUCK(task)->offset.z = -work->scl.x;
                work->timer++;
            }
        }
        break;
    }
    if (task->child != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &TRUCK(task)->offset, &task->child->work->pos);
        njPopMatrixEx();
    }
    if (TRUCK(task)->object.model != NULL) {
        object = &TRUCK(task)->object;
        /* @hack spelling: three Float pointers, each cast from a whole
         * vector, stepped through the vectors, where the members'
         * differences written out would do */
        dst = (Float *)&moved;
        src = (Float *)&TRUCK(task)->offset;
        sub = (Float *)&before;
        *dst++ = *src++ - *sub++;
        *dst++ = *src++ - *sub++;
        *dst = *src - *sub;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &TRUCK(task)->offset, (NJS_POINT3 *)object->pos);
        njCalcVector(NULL, &moved, &TASK_FORCES_OF(task)[0].speed);
        njTranslateEx(&object->model->center);
        njCalcPoint(NULL, &TRUCK(task)->offset, &center);
        njPopMatrixEx();
        TASK_FORCES_OF(task)[1].speed = TASK_FORCES_OF(task)[0].speed;
        if (player_near(&center, object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void harbor_truck_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- MOVE LENGTH";
        (void)"<- MOVE SPD";
    }
    njSetTexture(&harbor_truck_texlist);
    njPushMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslateEx(&TRUCK(task)->offset);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(harbor_truck_body_object->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)harbor_truck_lamp_object->pos);
    if (game_state.steps % BLINK_STEPS > LAMP_GRAY_LAST) {
        color = LAMP_ON;
    } else {
        color = LAMP_OFF;
    }
    vlist_set_color(harbor_truck_lamp_object->model->vlist, color);
    njCnkEasyDrawModel(harbor_truck_lamp_object->model);
    matrix_load_parent();
    njTranslateEx((NJS_VECTOR *)harbor_truck_out_wheel_object->pos);
    njRotateX(NULL, TRUCK(task)->roll);
    njCnkEasyDrawModel(harbor_truck_out_wheel_object->model);
    matrix_load_parent();
    njTranslateEx((NJS_VECTOR *)harbor_truck_back_wheel_object->pos);
    njRotateX(NULL, TRUCK(task)->roll);
    njCnkEasyDrawModel(harbor_truck_back_wheel_object->model);
    njPopMatrix(2);
}
