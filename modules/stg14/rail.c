/* the rail, RBR in Radical Highway's object list: a solid model with a rail
 * along its top. the players grind on the rail */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg14/rail.h"
#include "system/task.h"

#include "stg14/rail/text.inc"

/* task->unk_3c holds the rail's copy of rail_path, placed as the work places
 * the rail. it is null where the heap couldn't give the copy */
#define PATH(task) ((task)->unk_3c.p)

/* the rail's collision box, centered BOX_RISE above pos */
#define BOX_RISE 9.0F
#define BOX_HALF_X 10.0F
#define BOX_HALF_Y 9.0F
#define BOX_HALF_Z 46.0F
#define RAIL_SHAPES 1

/* The stationary guardrail carrying the four-sample grind path. */

static NJS_TEXNAME highway_rail_textures[4] = {
#include "stg14/rail/highway_rail_textures.inc"
};

static NJS_TEXLIST highway_rail_texlist = {
#include "stg14/rail/highway_rail_texlist.inc"
};

static Sint16 highway_rail_polygons[563] = {
#include "stg14/rail/highway_rail_polygons.inc"
};

static Sint32 highway_rail_vertices[659] = {
#include "stg14/rail/highway_rail_vertices.inc"
};

static NJS_CNK_MODEL highway_rail_mesh = {
#include "stg14/rail/highway_rail_mesh.inc"
};

static NJS_CNK_OBJECT highway_rail_object = {
#include "stg14/rail/highway_rail_object.inc"
};

/* the rail's path, in the model's space */
static struct path_point rail_points[4] = {
#include "stg14/rail/rail_points.inc"
};
static struct path rail_path = {
#include "stg14/rail/rail_path.inc"
};
NJS_TEXLIST *highway_rail_texlists[2] = {
#include "stg14/rail/highway_rail_texlists.inc"
};
static struct collision_shape rail_shape = {
#include "stg14/rail/rail_shape.inc"
};
/* the rail's build, or null while highway_rail_build has made none. nothing
 * calls highway_rail_build, so the rail draws as a chunk model */
static NJS_DIRECT_HEAD *rail_built = NULL;

/* build the rail's model at buf for njDirectDrawObject, and return buf past
 * the build. no header declares it, but it isn't static: SHC would leave out
 * a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint8 *highway_rail_build(Uint8 *buf);
/* NOLINTEND(misc-use-internal-linkage) */
static void rail_destroy(struct task *task);
static void rail_draw(struct task *task);
static void rail_step(struct task *task);

/* a copy of src from the Shinobi heap. its points' places are turned by ang
 * about y, then moved to pos. where the heap can't give the whole copy, it
 * frees what it took and returns null. src must be PATH_LAYOUT_SLOPES */
static struct path *rail_path_place(NJS_POINT3 *pos, Angle ang,
                                    struct path *src)
{
    struct path_point *from = src->points;
    struct path *path;
    struct path_point *points;
    int i;

    path = syCalloc(1, sizeof(struct path));
    points = syCalloc(src->count, sizeof(struct path_point));
    /* @hack spelling: the frees behind an else, though the copy's branch
     * returns, where tests of their own after it would do */
    if (path != NULL && points != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(pos);
        njRotateY(NULL, ang);
        for (i = 0; i < src->count; i++) {
            points[i].slope_x = from[i].slope_x;
            points[i].slope_z = from[i].slope_z;
            points[i].length = from[i].length;
            njCalcPoint(NULL, &from[i].pos, &points[i].pos);
        }
        njPopMatrixEx();
        *path = *src;
        path->points = points;
        return path;
    } else {
        if (path != NULL) {
            syFree(path);
        }
        if (points != NULL) {
            syFree(points);
        }
        return NULL;
    }
}

Uint8 *highway_rail_build(Uint8 *buf)
{
    njSetTexture(&highway_rail_texlist);
    rail_built = object_tree_build(&buf, &highway_rail_object);
    return buf;
}

void highway_rail_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct path_task *record;

    /* @hack redundant: a block no path reaches, kept for its string, which SHC
     * still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->step = rail_step;
    task->draw = rail_draw;
    task->destructor = rail_destroy;
    collision_init(task, &rail_shape, RAIL_SHAPES, COLLISION_OBJECT);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((PATH(task) = rail_path_place(&work->pos, work->ang.y, &rail_path)) ==
        NULL) {
        return;
    }
    child = task_create_child(0, path_rail_init, task);
    if (child == NULL) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((record = syCalloc(1, sizeof(struct path_task))) == NULL) {
        task_destroy(child);
    } else {
        child->extra.p = record;
        record->path = PATH(task);
    }
}

static void rail_destroy(struct task *task)
{
    struct path *path;

    if (PATH(task) != NULL) {
        path = PATH(task);
        /* @hack spelling: the two frees joined by a comma, where a statement
         * for each would do */
        syFree(path->points), syFree(path);
    }
}

static void rail_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&highway_rail_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (rail_built != NULL) {
        njDirectDrawObject(rail_built);
    } else {
        njCnkEasyDrawObject(&highway_rail_object);
    }
    njPopMatrixEx();
}

static void rail_step(struct task *task)
{
    /* @hack redundant: work, a local nothing reads */
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}
