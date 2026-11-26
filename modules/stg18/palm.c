/* PALM in Dry Lagoon's object list: a palm tree that stands in the collision
 * world */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg18/lagoon_util.h"
#include "stg18/palm.h"
#include "stg18/palm/text.inc"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

#define BUILDS 1
#define SCALE(work) ((work)->scl.x + 1.0F)
#define PALM_REACH 87.0F
#define REACH_MARGIN 10.0F
/* the radius palm_init gives palm_model. the draw clips by it only while the
 * palm has no build */
#define PALM_RADIUS 150.0F

/* Palm. */

static NJS_TEXNAME palm_texlist_names[3] = {
#include "stg18/palm/palm_texlist_names.inc"
};

static NJS_TEXLIST palm_texlist = {
#include "stg18/palm/palm_texlist.inc"
};

static Sint16 palm_polygons[1021] = {
#include "stg18/palm/palm_polygons.inc"
};

static Sint32 palm_vertices[859] = {
#include "stg18/palm/palm_vertices.inc"
};

static NJS_CNK_MODEL palm_model[1] = {
#include "stg18/palm/palm_model.inc"
};

static NJS_CNK_OBJECT palm_object = {
#include "stg18/palm/palm_object.inc"
};

static NJS_MATERIAL palm_collision_materials = {
#include "stg18/palm/palm_collision_materials.inc"
};

static Uint16 palm_collision_strips[70] = {
#include "stg18/palm/palm_collision_strips.inc"
};

static NJS_MESHSET palm_collision_faces = {
#include "stg18/palm/palm_collision_faces.inc"
};

static NJS_POINT3 palm_collision_points[30] = {
#include "stg18/palm/palm_collision_points.inc"
};

static NJS_VECTOR palm_collision_normals[30] = {
#include "stg18/palm/palm_collision_normals.inc"
};

static NJS_MODEL palm_collision_model = {
#include "stg18/palm/palm_collision_model.inc"
};

static NJS_OBJECT palm_collision = {
#include "stg18/palm/palm_collision.inc"
};

NJS_TEXLIST *palm_texlists[] = {&palm_texlist, NULL};
static Bool palm_built = FALSE;
static struct model_build palm_builds[BUILDS] = {
    {MODEL_BUILD_ALONE, &palm_texlist, &palm_object, NULL},
};

static void palm_destroy(struct task *task);
static void palm_step(struct task *task);
static void palm_draw(struct task *task);

Uint8 *palm_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, palm_builds, BUILDS) == BUILDS) {
        palm_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void palm_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    struct angles ang;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = palm_draw;
    task->step = palm_step;
    task->destructor = palm_destroy;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies palm_collision through null where world_pool_alloc
     * has none to lend, though palm_destroy tests task->extra for null.
     * palm_step hands player_near the null object's pos untested: the copy at
     * 0x8c506c32, the test at 0x8c506d06 and the read at 0x8c506d52 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the palm, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = palm_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    /* @hack spelling: task->work read again for scl.x, where work would do */
    object->scl[0] = object->scl[1] = object->scl[2] = SCALE(task->work);
    /* the palm turns about z, then x, then y. current_matrix_angles_xyz
     * gives that turn as the angles the copy takes, about x, then y, then z */
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
    object->ang[0] = ang.x;
    object->ang[1] = ang.y;
    object->ang[2] = ang.z;
    njPopMatrixEx();
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID, task, object);
    palm_model->r = PALM_RADIUS;
}

static void palm_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void palm_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    /* @hack spelling: task->work read again for scl.x, where work would do */
    if (player_near((NJS_POINT3 *)object->pos,
                    (SCALE(task->work) * PALM_REACH) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void palm_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&palm_texlist);
    /* @hack spelling: task->work read again for scl.x in both branches, where
     * work would do */
    if (palm_built) {
        lagoon_direct_draw_scaled(palm_builds[0].built, SCALE(task->work));
    } else {
        lagoon_model_draw_scaled(palm_model, SCALE(task->work));
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
