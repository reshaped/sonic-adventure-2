/* the store box, STORE in Death Chamber and Egg Quarters: a box that the
 * players stand on. it is blue, green or red. it stands in the collision
 * world, and has no collision record */
#include "shared/store_box/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/store_art.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "shared/store_box.h"
#include "shared/store_textures.h"
#include "system/task.h"
#include "system/util.h"

/* the two stages keep their scaled drawing helpers in separate overlays */
#ifndef STORE_BOX_DRAW_MODEL
#include "stg25/chamber_util.h"
#define STORE_BOX_DRAW_MODEL chamber_model_draw_scaled
#define STORE_BOX_DRAW_BUILT chamber_direct_draw_scaled
#endif

/* the box's color: the whole part of its work's |scl.z|, modulo COLORS */
#define COLORS 3
#define COLOR(work) ((int)_builtin_fabsf((work)->scl.z) % COLORS)
#define SCALE(work) ((work)->scl.x + 1.0F)
/* the world flags of the box's copy of store_box_collision */
#define BOX_WORLD (WORLD_SIZE_CLASS(2) | WORLD_SOLID)
/* the box's copy stays in the world only while a player lies within REACH of
 * its pos */
#define REACH 67.8F

static void store_box_destroy(struct task *task);
static void store_box_step(struct task *task);
static void store_box_draw(struct task *task);

/* each color's model */
static NJS_CNK_MODEL *store_box_models[COLORS] = {
#include "shared/store_box/store_box_models.inc"
};
NJS_TEXLIST *store_box_texlists[COLORS + 1] = {
#include "shared/store_box/store_box_texlists.inc"
};
static char *store_box_color_names[COLORS] = {
#include "shared/store_box/store_box_color_names.inc"
};
/* TRUE once a call of store_box_build has built all three boxes */
static Sint32 store_box_built = FALSE;
static struct model_build store_box_builds[COLORS] = {
#include "shared/store_box/store_box_builds.inc"
};

Uint8 *store_box_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, store_box_builds, COLORS) == COLORS) {
        store_box_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void store_box_init(struct task *task)
{
    /* @hack spelling: task->work read again for scl.x, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (!placement_try_leave(task)) {
        task->draw = store_box_draw;
        task->step = store_box_step;
        task->destructor = store_box_destroy;
        /* @hack spelling: a jump to the statement after it, where falling
         * through would do */
        goto alloc;
    alloc:
        object = task->extra.p = world_pool_alloc();
        /* @bug memory: copies the box's shape through null where
         * world_pool_alloc has none to lend, though store_box_destroy
         * tests task->extra for null. store_box_step hands player_near
         * the null object's pos untested: 0x6c, 0xfe and 0x13c of the
         * object's code */
#ifdef NONMATCHING
        /* this side repairs the tag above: where the pool has no object, the
         * init destroys the box, and set.c makes it again on a later pass */
        if (object == NULL) {
            task_destroy(task);
            return;
        }
#endif
        *object = store_box_collision;
        object->evalflags &=
            ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
        object->scl[0] = object->scl[1] = object->scl[2] = SCALE(task->work);
        object->ang[0] = work->ang.x;
        object->ang[1] = work->ang.y;
        object->ang[2] = work->ang.z;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        world_add(BOX_WORLD, task, object);
    }
}

static void store_box_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void store_box_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (!placement_try_leave(task)) {
        object = task->extra.p;
        if (player_near((NJS_POINT3 *)object->pos, REACH)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void store_box_draw(struct task *task)
{
    /* @hack spelling: task->work read again for scl and the color, where
     * work would do */
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(store_box_texlists[COLOR(task->work)]);
    if (store_box_built) {
        STORE_BOX_DRAW_BUILT(store_box_builds[COLOR(task->work)].built,
                             SCALE(task->work));
    } else {
        STORE_BOX_DRAW_MODEL(store_box_models[COLOR(task->work)],
                             SCALE(task->work));
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
