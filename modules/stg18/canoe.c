/* the canoe, CANOE in Dry Lagoon's object list: a boat that stands in the
 * collision world */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg18/canoe.h"
#include "stg18/canoe/text.inc"
#include "stg18/lagoon_util.h"
#include "system/task.h"
#include "system/util.h"

#define SCALE(work) ((work)->scl.x + 1.0F)
/* the copy collides only while a player lies within REACH times the scale
 * plus REACH_MARGIN of pos */
#define REACH 110.0F
#define REACH_MARGIN 10.0F
#define CANOE_WORLD_FLAGS (WORLD_SIZE_CLASS(1) | WORLD_SOLID)

/* Canoe. */

static NJS_TEXNAME canoe_texlist_names[4] = {
#include "stg18/canoe/canoe_texlist_names.inc"
};

static NJS_TEXLIST canoe_texlist = {
#include "stg18/canoe/canoe_texlist.inc"
};

static Sint16 canoe_polygons[585] = {
#include "stg18/canoe/canoe_polygons.inc"
};

static Sint32 canoe_vertices[303] = {
#include "stg18/canoe/canoe_vertices.inc"
};

static NJS_CNK_MODEL canoe_model = {
#include "stg18/canoe/canoe_model.inc"
};

static NJS_CNK_OBJECT canoe_object = {
#include "stg18/canoe/canoe_object.inc"
};

static NJS_MATERIAL canoe_collision_materials = {
#include "stg18/canoe/canoe_collision_materials.inc"
};

static Uint16 canoe_collision_strips[190] = {
#include "stg18/canoe/canoe_collision_strips.inc"
};

static NJS_MESHSET canoe_collision_faces = {
#include "stg18/canoe/canoe_collision_faces.inc"
};

static NJS_POINT3 canoe_collision_points[49] = {
#include "stg18/canoe/canoe_collision_points.inc"
};

static NJS_VECTOR canoe_collision_normals[49] = {
#include "stg18/canoe/canoe_collision_normals.inc"
};

static NJS_MODEL canoe_collision_model = {
#include "stg18/canoe/canoe_collision_model.inc"
};

static NJS_OBJECT canoe_collision = {
#include "stg18/canoe/canoe_collision.inc"
};

static void canoe_destroy(struct task *task);
static void canoe_step(struct task *task);
static void canoe_draw(struct task *task);

void canoe_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = canoe_draw;
    task->step = canoe_step;
    task->destructor = canoe_destroy;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies canoe_collision through null where world_pool_alloc
     * has none to lend, though canoe_destroy tests task->extra for null.
     * canoe_step hands player_near the null object's pos untested: the copy
     * at 0x8c5069ec, the test at 0x8c506a7e and the read at 0x8c506aca */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the canoe, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = canoe_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    /* @hack spelling: scl.x read through task->work, where work would do */
    object->scl[0] = object->scl[1] = object->scl[2] = SCALE(task->work);
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(CANOE_WORLD_FLAGS, task, object);
}

static void canoe_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void canoe_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    /* @hack spelling: scl.x read through task->work, where work would do */
    if (player_near((NJS_POINT3 *)object->pos,
                    (SCALE(task->work) * REACH) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void canoe_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njSetTexture(&canoe_texlist);
    /* @hack spelling: scl.x read through task->work, where work would do */
    lagoon_model_draw_scaled(&canoe_model, SCALE(task->work));
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

NJS_TEXLIST *canoe_texlists[] = {
    &canoe_texlist,
    NULL,
};
