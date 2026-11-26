/* the wood fence, SAKU in a stage's object list: a fence that stands in the
 * players' way */
#include "shared/wood_fence/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/wood_fence.h"
#include "system/task.h"

static NJS_TEXNAME wood_fence_textures[2] = {
#include "shared/wood_fence/wood_fence_textures.inc"
};

static NJS_TEXLIST wood_fence_texlist = {
#include "shared/wood_fence/wood_fence_texlist.inc"
};

static Sint16 wood_fence_polygons[225] = {
#include "shared/wood_fence/wood_fence_polygons.inc"
};

static Sint32 wood_fence_vertices[171] = {
#include "shared/wood_fence/wood_fence_vertices.inc"
};

static NJS_CNK_MODEL wood_fence_model = {
#include "shared/wood_fence/wood_fence_model.inc"
};

static NJS_CNK_OBJECT wood_fence_object = {
#include "shared/wood_fence/wood_fence_object.inc"
};

static NJS_MATERIAL wood_fence_collision_materials[1] = {
#include "shared/wood_fence/wood_fence_collision_materials.inc"
};

static Sint16 wood_fence_collision_indices_0[19] = {
#include "shared/wood_fence/wood_fence_collision_indices_0.inc"
};

static NJS_MESHSET wood_fence_collision_meshsets[1] = {
#include "shared/wood_fence/wood_fence_collision_meshsets.inc"
};

static NJS_POINT3 wood_fence_collision_points[8] = {
#include "shared/wood_fence/wood_fence_collision_points.inc"
};

static NJS_VECTOR wood_fence_collision_normals[8] = {
#include "shared/wood_fence/wood_fence_collision_normals.inc"
};

static NJS_MODEL wood_fence_collision_model = {
#include "shared/wood_fence/wood_fence_collision_model.inc"
};

static NJS_OBJECT wood_fence_collision = {
#include "shared/wood_fence/wood_fence_collision.inc"
};

NJS_TEXLIST *wood_fence_texlists[2] = {
#include "shared/wood_fence/wood_fence_texlists.inc"
};

/* the scale a scl axis gives the fence and its box along that axis */
#define SCALE(scl) ((scl) + 1.0F)

/* the fence's box at a scale of 1 */
/* clang-format off */
static struct collision_shape wood_fence_shape = {
#include "shared/wood_fence/wood_fence_shape.inc"
};
/* clang-format on */

static void wood_fence_destroy(struct task *task);
static void wood_fence_step(struct task *task);
static void wood_fence_draw(struct task *task);

void wood_fence_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = wood_fence_draw;
    task->step = wood_fence_step;
    task->destructor = wood_fence_destroy;
    collision_init(task, &wood_fence_shape, 1, COLLISION_OBJECT);
    if (work->collision != NULL) {
        work->collision->shapes->center.y *= SCALE(work->scl.y);
        work->collision->shapes->radius *= SCALE(work->scl.x);
        work->collision->shapes->size_y *= SCALE(work->scl.y);
        work->collision->shapes->size_z *= SCALE(work->scl.z);
        work->collision->shapes->ang.y = work->ang.y;
        work->collision->shapes->ang.z = work->ang.z;
    }
    collision_fit_radius(task);
}

static void wood_fence_destroy(struct task *task)
{
}

static void wood_fence_step(struct task *task)
{
    /* @hack redundant: work, a local nothing reads */
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void wood_fence_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&wood_fence_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScale(NULL, SCALE(work->scl.x), SCALE(work->scl.y), SCALE(work->scl.z));
    njCnkEasyDrawModel(&wood_fence_model);
    njPopMatrixEx();
}
