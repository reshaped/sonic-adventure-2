/* the pole, POLE in Wild Canyon's object list: a pillar that the players
 * collide with */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/pole.h"
#include "system/task.h"

#include "shared/pole/text.inc"

#define SCALE_XZ(work) ((work)->scl.x + 1.0F)
#define SCALE_Y(work) ((work)->scl.y + 1.0F)
/* the pole's reach is NEAR_RADIUS times its two scales, plus NEAR_MARGIN. the
 * step clears WORLD_ADDED while player_near finds no player within the reach.
 * world_collect_near_players then passes over the pole's shape */
#define NEAR_RADIUS 100.6382F
#define NEAR_MARGIN 10.0F

/* the placement editor's labels for ang.y, scl.x and scl.y, which no code
 * reads */
static const char pole_edit_yaw[20] = "<- ANGLE Y (   )";
static const char pole_edit_scale_xz[20] = "<- SCALE XZ(1.0+x)";
static const char pole_edit_scale_y[20] = "<- SCALE Y(1.0+x)";

static NJS_TEXNAME pole_texlist_names[3] = {
#include "shared/pole/pole_texlist_names.inc"
};

static NJS_TEXLIST pole_texlist = {
#include "shared/pole/pole_texlist.inc"
};

static Sint16 pole_polygons[131] = {
#include "shared/pole/pole_polygons.inc"
};

static Sint32 pole_vertices[79] = {
#include "shared/pole/pole_vertices.inc"
};

static NJS_CNK_MODEL pole_model = {
#include "shared/pole/pole_model.inc"
};

static NJS_CNK_OBJECT pole_object = {
#include "shared/pole/pole_object.inc"
};

/* The Ninja material stores its colors as packed words. */
static NJS_MATERIAL pole_collision_materials = {
#include "shared/pole/pole_collision_materials.inc"
};

static Uint16 pole_collision_strips[20] = {
#include "shared/pole/pole_collision_strips.inc"
};

static NJS_MESHSET pole_collision_faces = {
#include "shared/pole/pole_collision_faces.inc"
};

static NJS_POINT3 pole_collision_points[8] = {
#include "shared/pole/pole_collision_points.inc"
};

static NJS_VECTOR pole_collision_normals[8] = {
#include "shared/pole/pole_collision_normals.inc"
};

static NJS_MODEL pole_collision_model = {
#include "shared/pole/pole_collision_model.inc"
};

static NJS_OBJECT pole_collision = {
#include "shared/pole/pole_collision.inc"
};

static void pole_destroy(struct task *task);
static void pole_step(struct task *task);
static void pole_draw(struct task *task);

NJS_TEXLIST *pole_texlists[2] = {
#include "shared/pole/pole_texlists.inc"
};

void pole_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    /* @hack spelling: zero, a local for 0, where 0 stored in place would
     * do */
    Angle zero;

    if (!half_try_leave(task) && !placement_try_leave(task)) {
        task->draw = pole_draw;
        task->step = pole_step;
        task->destructor = pole_destroy;
        zero = 0;
        object = world_pool_alloc();
        task->extra.p = object;
        /* @bug memory: copies pole_collision through null where
         * world_pool_alloc has none to lend (at 0x3a of the object's code).
         * pole_destroy tests task->extra for that null (at 0xd2). pole_step
         * hands player_near the copy's pos untested (at 0x134) */
#ifdef NONMATCHING
        /* this side repairs the tag above: where the pool has no object, the
         * init destroys the pole, and set.c makes it again on a later pass */
        if (object == NULL) {
            task_destroy(task);
            return;
        }
#endif
        *object = pole_collision;
        object->evalflags &=
            ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
        object->scl[0] = object->scl[2] = SCALE_XZ(task->work);
        object->scl[1] = SCALE_Y(task->work);
        object->ang[0] = zero;
        object->ang[1] = work->ang.y;
        object->ang[2] = zero;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        world_add(WORLD_SOLID, task, object);
    }
}

static void pole_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void pole_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!half_try_leave(task) && !placement_try_leave(task)) {
        NJS_OBJECT *object = task->extra.p;

        if (player_near(
                (NJS_POINT3 *)object->pos,
                (SCALE_Y(task->work) * SCALE_XZ(task->work) * NEAR_RADIUS) +
                    NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void pole_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.z = SCALE_XZ(task->work);
    scale.y = SCALE_Y(task->work);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&pole_texlist);
    canyon_model_draw_scaled_v(&pole_model, &scale);
    njPopMatrixEx();
}
