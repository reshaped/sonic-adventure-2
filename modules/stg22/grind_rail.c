#include "stg22/fixture_models.h"
#include <sg_xpt.h>
#include <ninja.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg22/crazy_gadget.h"
#include "stg22/grind_rail.h"
#include "stg22/prop_textures.h"
#include "system/cache.h"
#include "system/task.h"

#include "stg22/grind_rail/text.inc"

/* the source path lies PATH_LIFT above the rail in y. under any gravity but
 * one along +y, place_path moves the path to PATH_LIFT against gravity from
 * the rail. it also tilts the points' slopes by gravity's angles. under +y it
 * does neither */
#define PATH_LIFT 2.0F
/* a rail whose scl.x is below RIDDEN_STRETCH carries no rider */
#define RIDDEN_STRETCH 0.0001F
/* gravity_orientation as the init found it, in task->unk_3c. a step that
 * finds it changed kills the rail */
#define INIT_ORIENTATION(task) ((task)->unk_3c.l)

static struct path_point rail_points[2] = {
#include "stg22/grind_rail/rail_points.inc"
};
static struct path rail_path = {
#include "stg22/grind_rail/rail_path.inc"
};
NJS_TEXLIST *gadget_grind_rail_texlists[2] = {
#include "stg22/grind_rail/gadget_grind_rail_texlists.inc"
};

static struct collision_shape rail_shape = {
#include "stg22/grind_rail/rail_shape.inc"
};

static void rail_destroy(struct task *task);
static void rail_draw(struct task *task);
static void rail_step(struct task *task);

#pragma inline(gravity_orientation)
static Uint32 gravity_orientation(void)
{
    return ((Uint32)(Uint16)world_gravity_ang_x << 16) |
           (Uint16)world_gravity_ang_z;
}

static struct path *place_path(NJS_POINT3 *position, Angle yaw,
                               struct path *source, Float scale)
{
    struct path_point *from = source->points;
    struct path *copy;
    struct path_point *points;
    int i;

    copy = syCalloc(1, sizeof(struct path));
    points = syCalloc(source->count, sizeof(struct path_point));
    /* @hack spelling: an else after the return, where the lines after the if
     * would do */
    if (copy && points) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        /* @bug logic: under +y gravity the path stays PATH_LIFT above the rail
         * in y, on gravity's side, with its slopes untilted. this test at
         * 0x8c508bc0 and the loop's at 0x8c508c14 skip the move and the tilt
         * under +y. the two do nothing only under -y, where the tests let
         * them run */
        if (!(world_gravity.y > GRAVITY_ON_AXIS)) {
            njTranslate(NULL, -(world_gravity.x * PATH_LIFT),
                        -(world_gravity.y * PATH_LIFT) - PATH_LIFT,
                        -(world_gravity.z * PATH_LIFT));
        }
        njTranslateEx(position);
        njRotateY(NULL, yaw);
        njScale(NULL, scale, 1.0F, 1.0F);
        for (i = 0; i < source->count; i++) {
            if (world_gravity.y > GRAVITY_ON_AXIS) {
                points[i].slope_x = from[i].slope_x;
                points[i].slope_z = from[i].slope_z;
            } else {
                points[i].slope_x = from[i].slope_x + world_gravity_ang_x;
                points[i].slope_z = from[i].slope_z + world_gravity_ang_z;
            }
            points[i].length = from[i].length * scale;
            njCalcPoint(NULL, &from[i].pos, &points[i].pos);
        }
        njPopMatrixEx();
        *copy = *source;
        copy->length = source->length * scale;
        copy->points = points;
        return copy;
    } else {
        if (copy) {
            syFree(copy);
        }
        if (points) {
            syFree(points);
        }
        return NULL;
    }
}

void gadget_grind_rail_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct path_task *record;
    Uint32 orientation;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = rail_step;
    task->draw = rail_draw;
    task->destructor = rail_destroy;
    /* @hack spelling: a goto past the path, where an if around it would do */
    if (work->scl.x < RIDDEN_STRETCH) {
        goto remember_gravity;
    }
    /* @hack spelling: the assignments to extra and record inside the tests,
     * where statements of their own would do */
    if ((task->extra.p = place_path(&work->pos, work->ang.y, &rail_path,
                                    work->scl.x + 1.0F))) {
        child = task_create_child(0, path_rail_init, task);
        if (child) {
            if (!(record = syCalloc(1, sizeof(struct path_task)))) {
                task_destroy(child);
            } else {
                child->extra.p = record;
                record->path = task->extra.p;
            }
        }
    }
remember_gravity:
    /* @hack spelling: orientation, a local for gravity_orientation's result,
     * where the call in the store would do */
    orientation = gravity_orientation();
    INIT_ORIENTATION(task) = orientation;
}

static void rail_destroy(struct task *task)
{
    struct path *path;

    if (task->extra.p) {
        path = task->extra.p;
        syFree(path->points);
        syFree(path);
    }
    task->extra.p = NULL;
}

static void rail_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *part;
    Float scale;
    Float radius;
    Float larger;

    njSetTexture(&gadget_grind_rail_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    part = gadget_grind_rail;
    scale = work->scl.x + 1.0F;
    njPushMatrixEx();
    njScale(NULL, scale, 1.0F, 1.0F);
    /* the cull takes the model's r. the draw scales it by 1 plus the larger
     * of scl.x and scl.y. every rail draws the one model, so the draw then
     * sets r back to radius */
    radius = part->model->r;
    larger =
        work->scl.x > work->scl.y ? work->scl.x + 1.0F : work->scl.y + 1.0F;
    part->model->r *= larger;
    njCnkEasyDrawModel(part->model);
    part->model->r = radius;
    matrix_load_parent();
    part = part->child;
    njTranslate(NULL, part->pos[0] * scale, part->pos[1], part->pos[2]);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(part->model);
    matrix_load_parent();
    part = part->sibling;
    njTranslate(NULL, part->pos[0] * scale, part->pos[1], part->pos[2]);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(part->model);
    matrix_load_parent();
    njPopMatrix(2);
}

static void rail_step(struct task *task)
{
    Uint32 orientation;

    /* @hack redundant: the set editor's labels, as statements that do
     * nothing */
    (void)RAIL_EDITOR_AXLE_ANGLE;
    (void)RAIL_EDITOR_YAW;
    (void)RAIL_EDITOR_SCALE;
    /* @hack spelling: an else after the return, where the lines after the if
     * would do */
    /* @hack spelling: an else after the return, where the lines after the if
     * would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        orientation = gravity_orientation();
        if (orientation != (Uint32)INIT_ORIENTATION(task)) {
            task_kill(task);
        }
    }
}
