/* POOL in Dry Lagoon's object list: a well of water that the players can
 * stand in */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg18/lagoon_util.h"
#include "stg18/pool.h"
#include "stg18/pool/text.inc"
#include "stg18/water.h"
#include "system/game_state.h"
#include "system/task.h"

#define POOL_SHAPES 4
#define POOL_BUILDS 1
#define SCALE(work) ((work)->scl.x + 1.0F)
#define POOL_WORLD_FLAGS (WORLD_SOLID | WORLD_FLAT_THIN)
#define REACH 50.0F
#define REACH_MARGIN 10.0F
#define REACH_HEIGHT 20.0F

/* Pool. */

static NJS_MATERIAL pool_rim_collision_materials = {
#include "stg18/pool/pool_rim_collision_materials.inc"
};

static Uint16 pool_rim_collision_strips[33] = {
#include "stg18/pool/pool_rim_collision_strips.inc"
};

static NJS_MESHSET pool_rim_collision_faces = {
#include "stg18/pool/pool_rim_collision_faces.inc"
};

static NJS_POINT3 pool_rim_collision_points[16] = {
#include "stg18/pool/pool_rim_collision_points.inc"
};

static NJS_VECTOR pool_rim_collision_normals[16] = {
#include "stg18/pool/pool_rim_collision_normals.inc"
};

static NJS_MODEL pool_rim_collision_model = {
#include "stg18/pool/pool_rim_collision_model.inc"
};

static NJS_OBJECT pool_rim_collision = {
#include "stg18/pool/pool_rim_collision.inc"
};

static NJS_TEXNAME pool_texlist_names[3] = {
#include "stg18/pool/pool_texlist_names.inc"
};

static NJS_TEXLIST pool_texlist = {
#include "stg18/pool/pool_texlist.inc"
};

static Sint16 pool_polygons[197] = {
#include "stg18/pool/pool_polygons.inc"
};

static Sint32 pool_vertices[115] = {
#include "stg18/pool/pool_vertices.inc"
};

static NJS_CNK_MODEL pool_model = {
#include "stg18/pool/pool_model.inc"
};

static NJS_CNK_OBJECT pool_object = {
#include "stg18/pool/pool_object.inc"
};

static NJS_TEXNAME pool_texture_set_2_names[1] = {
#include "stg18/pool/pool_texture_set_2_names.inc"
};

static NJS_TEXLIST pool_texture_set_2 = {
#include "stg18/pool/pool_texture_set_2.inc"
};

static Sint16 pool_water_polygons[25] = {
#include "stg18/pool/pool_water_polygons.inc"
};

static Sint32 pool_water_vertices[27] = {
#include "stg18/pool/pool_water_vertices.inc"
};

static NJS_CNK_MODEL pool_water_model[1] = {
#include "stg18/pool/pool_water_model.inc"
};

static NJS_CNK_OBJECT pool_water_object = {
#include "stg18/pool/pool_water_object.inc"
};

static NJS_MATERIAL pool_water_collision_materials = {
#include "stg18/pool/pool_water_collision_materials.inc"
};

static Uint16 pool_water_collision_strips[5] = {
#include "stg18/pool/pool_water_collision_strips.inc"
};

static NJS_MESHSET pool_water_collision_faces = {
#include "stg18/pool/pool_water_collision_faces.inc"
};

static NJS_POINT3 pool_water_collision_points[4] = {
#include "stg18/pool/pool_water_collision_points.inc"
};

static NJS_VECTOR pool_water_collision_normals[4] = {
#include "stg18/pool/pool_water_collision_normals.inc"
};

static NJS_MODEL pool_water_collision_model = {
#include "stg18/pool/pool_water_collision_model.inc"
};

static NJS_OBJECT pool_water_collision = {
#include "stg18/pool/pool_water_collision.inc"
};

static void pool_destroy(struct task *task);
static void pool_step(struct task *task);
static void pool_draw(struct task *task);
static void pool_child_init(struct task *task);
static void pool_child_destroy(struct task *task);
static void pool_child_step(struct task *task);
static void pool_child_draw(struct task *task);

NJS_TEXLIST *pool_texlists[] = {&pool_texlist, NULL};
/* the well's four walls at a scale of 1, which pool_init scales in x and z */
/* clang-format off */
static struct collision_shape pool_shapes[POOL_SHAPES] = {
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 4.5F, 25.0F}, 25.0F, 4.5F,
     5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 4.5F, -25.0F}, 25.0F, 4.5F,
     5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {20.0F, 4.5F, 0.0F}, 5.0F, 4.5F,
     30.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {-20.0F, 4.5F, 0.0F}, 5.0F, 4.5F,
     30.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Bool pool_built = FALSE;
static struct model_build pool_builds[POOL_BUILDS] = {
    {MODEL_BUILD_ALONE, &pool_texlist, &pool_object, NULL},
};

Uint8 *pool_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, pool_builds, POOL_BUILDS) == POOL_BUILDS) {
        pool_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void pool_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the return for a failed child in an else, where an
     * early return would do */
    if (task_create_child(TASK_WORK, pool_child_init, task) != NULL) {
        task->draw = pool_draw;
        task->step = pool_step;
        task->destructor = pool_destroy;
    } else {
        return;
    }
    collision_init(task, pool_shapes, POOL_SHAPES, COLLISION_OBJECT);
    for (i = 0; i < POOL_SHAPES; i++) {
        /* @hack spelling: the scale read through task->work in each line,
         * where work would do */
        work->collision->shapes[i].center.x *= SCALE(task->work);
        work->collision->shapes[i].center.z *= SCALE(task->work);
        work->collision->shapes[i].radius *= SCALE(task->work);
        work->collision->shapes[i].size_z *= SCALE(task->work);
    }
    collision_fit_radius(task);
}

static void pool_destroy(struct task *task)
{
}

static void pool_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (!placement_try_leave(self)) {
        collision_register(self);
    }
}

static void pool_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    Sint16 tid;

    /* @hack spelling: tid, a local for the halfword's new value, built in two
     * statements, where one assignment to the halfword would do */
    tid = pool_water_model->plist[LAGOON_WATER_TID_AT];
    tid = (tid & NJD_FFM_MASK) |
          ((game_state.steps % LAGOON_WATER_FRAMES) + LAGOON_WATER_FIRST_FRAME);
    pool_water_model->plist[LAGOON_WATER_TID_AT] = tid;
    /* @hack spelling: the scale read through task->work, where work would
     * do */
    scale.x = scale.z = SCALE(task->work);
    scale.y = 1.0F;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&pool_texlist);
    if (pool_built) {
        lagoon_direct_draw_scaled_v(pool_builds[0].built, &scale);
    } else {
        lagoon_model_draw_scaled_v(&pool_model, &scale);
    }
    njSetTexture(&lagoon_water_texlist);
    njCnkEasyDrawModel(pool_water_model);
    njPopMatrixEx();
}

static void pool_child_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *pool = task->parent;
    NJS_OBJECT *object;
    Uint32 flags;

    work->pos = pool->work->pos;
    work->ang = pool->work->ang;
    work->scl = pool->work->scl;
    task->draw = pool_child_draw;
    task->step = pool_child_step;
    task->destructor = pool_child_destroy;
    /* @hack spelling: a jump to the statement after it, where falling through
     * would do */
    goto alloc;
alloc:
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies pool_water_collision through null where
     * world_pool_alloc has none to lend, though pool_child_destroy tests
     * task->extra for null. pool_child_step hands player_near the null
     * object's pos untested: the copy at 0x8c506226, the test at 0x8c5062d0
     * and the read at 0x8c5063d2 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where world_pool_alloc has no object
     * to lend, the child destroys its parent, the pool. task_destroy takes
     * the child with it. only the child puts the water's shape in the
     * collision world, and nothing makes a pool's child again. set.c makes
     * the pool again on a later pass */
    if (object == NULL) {
        task_destroy(pool);
        return;
    }
#endif
    *object = pool_water_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    /* @hack spelling: the scale read through task->work, where work would
     * do */
    object->scl[0] = object->scl[2] = SCALE(task->work);
    object->scl[1] = 1.0F;
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    /* @hack spelling: flags, a local for world_add's flags, where the
     * expression passed in place would do */
    /* @hack spelling: the scale read through task->work for the size class,
     * where work would do */
    flags =
        lagoon_size_class_flags(SCALE(task->work) * REACH) | POOL_WORLD_FLAGS;
    world_add(flags, task, object);
}

static void pool_child_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void pool_child_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *pool = task->parent;
    NJS_OBJECT *object;

    work->pos = pool->work->pos;
    work->ang = pool->work->ang;
    work->scl = pool->work->scl;
    object = task->extra.p;
    /* @hack spelling: the scale read through task->work, where work would
     * do */
    if (player_near((NJS_POINT3 *)object->pos,
                    (SCALE(task->work) * REACH) + REACH_MARGIN) &&
        object->pos[1] > player_work[0]->pos.y - REACH_HEIGHT &&
        player_work[0]->pos.y + REACH_HEIGHT > object->pos[1]) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void pool_child_draw(struct task *task)
{
}
