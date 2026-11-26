/* the collision objects of a stage's object list: SPHERE, CCYL, CCUBE, CWALL
 * and CCIRCLE. each is a shape that no model draws, sized by its placement.
 * the Chao stages' lists name the first four C_SPHERE, C_CYL, C_CUBE and
 * WALL_PLANE */
#include "collision/collision.h"
#include "object/set.h"
#include "shared/collision_object.h"
#include "system/task.h"

/* a shape's radius, size_y and size_z are BASE_SIZE plus the work's scl.x,
 * scl.y and scl.z */
#define BASE_SIZE 10.0F
/* a CCIRCLE's range, squared as placement_try_leave_by takes it */
#define CIRCLE_RANGE (3000.0F * 3000.0F)

#define SHAPES 1

/* a collision object's modes, in its work's mode */
enum collision_object_mode { MODE_START, MODE_REGISTER };

/* the objects' shapes, one each. each step sizes its record's copy before the
 * checks read it, so no check reads the sizes here */
static struct collision_shape collision_sphere_shapes[SHAPES] = {
#include "shared/collision_object/collision_sphere_shapes.inc"
};
static struct collision_shape collision_cylinder_shapes[SHAPES] = {
#include "shared/collision_object/collision_cylinder_shapes.inc"
};
static struct collision_shape collision_cube_shapes[SHAPES] = {
#include "shared/collision_object/collision_cube_shapes.inc"
};
static struct collision_shape collision_wall_shapes[SHAPES] = {
#include "shared/collision_object/collision_wall_shapes.inc"
};
/* a box that no step here gives a record */
static struct collision_shape dat_collision_object_0[SHAPES] = {
#include "shared/collision_object/dat_collision_object_0.inc"
};
static struct collision_shape collision_circle_shapes[SHAPES] = {
#include "shared/collision_object/collision_circle_shapes.inc"
};

/* size the record's one shape to the work's scl, then fit the record's
 * radius to it */
static void collision_object_fit(struct task_work *work)
{
    struct collision_shape *shape = work->collision->shapes;

    shape->radius = work->scl.x + BASE_SIZE;
    shape->size_y = work->scl.y + BASE_SIZE;
    shape->size_z = work->scl.z + BASE_SIZE;
    collision_fit_record_radius(work->collision);
}

void collision_sphere_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_START:
        collision_init(task, collision_sphere_shapes, SHAPES, COLLISION_OBJECT);
        collision_object_fit(work);
        work->mode = MODE_REGISTER;
        break;
    case MODE_REGISTER:
        collision_register(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

void collision_cylinder_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_START:
        collision_init(task, collision_cylinder_shapes, SHAPES,
                       COLLISION_OBJECT);
        collision_object_fit(work);
        work->mode = MODE_REGISTER;
        break;
    case MODE_REGISTER:
        collision_register(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

void collision_cube_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_START:
        collision_init(task, collision_cube_shapes, SHAPES, COLLISION_OBJECT);
        collision_object_fit(work);
        work->mode = MODE_REGISTER;
        break;
    case MODE_REGISTER:
        collision_register(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

void collision_wall_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, which leaves its six
     * strings in the object */
    if (0) {
        (void)"< WALL >";
        (void)"<      >";
        (void)"->             ";
        (void)"-> ROT Y       ";
        (void)"-> YOKO        ";
        (void)"-> TATE        ";
    }
    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_START:
        collision_init(task, collision_wall_shapes, SHAPES, COLLISION_WALL);
        collision_object_fit(work);
        work->mode = MODE_REGISTER;
        break;
    case MODE_REGISTER:
        collision_register(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

void collision_circle_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave_by(task, CIRCLE_RANGE)) {
        return;
    }
    switch (work->mode) {
    case MODE_START:
        collision_init(task, collision_circle_shapes, SHAPES, COLLISION_WALL);
        collision_object_fit(work);
        work->mode = MODE_REGISTER;
        break;
    case MODE_REGISTER:
        collision_register(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}
