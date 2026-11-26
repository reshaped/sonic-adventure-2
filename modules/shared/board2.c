/* the second board, BOARD2 in Wild Canyon's object list: a painted board that
 * the players collide with. it comes bright or dark */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/board2.h"
#include "shared/canyon_util.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/board2/text.inc"

#define BOARD2_KINDS 2
/* @hack spelling: the work read through task, where the caller's work would
 * do */
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % BOARD2_KINDS)

/* the copy of the shape collides while a player is within the model's radius
 * plus REACH_MARGIN of it */
#define REACH_MARGIN 10.0F

#define BOARD2_WORLD (WORLD_SIZE_CLASS(2) | WORLD_SOLID)

/* Board2. */

static NJS_TEXNAME board2_bright_texlist_names[1] = {
#include "shared/board2/board2_bright_texlist_names.inc"
};

static NJS_TEXLIST board2_bright_texlist = {
#include "shared/board2/board2_bright_texlist.inc"
};

static Sint16 board2_bright_polygons[311] = {
#include "shared/board2/board2_bright_polygons.inc"
};

static Sint32 board2_bright_vertices[323] = {
#include "shared/board2/board2_bright_vertices.inc"
};

static NJS_CNK_MODEL board2_bright_model = {
#include "shared/board2/board2_bright_model.inc"
};

static NJS_CNK_OBJECT board2_bright_object = {
#include "shared/board2/board2_bright_object.inc"
};

static NJS_TEXNAME board2_dark_texlist_names[1] = {
#include "shared/board2/board2_dark_texlist_names.inc"
};

static NJS_TEXLIST board2_dark_texlist = {
#include "shared/board2/board2_dark_texlist.inc"
};

static Sint16 board2_dark_polygons[311] = {
#include "shared/board2/board2_dark_polygons.inc"
};

static Sint32 board2_dark_vertices[315] = {
#include "shared/board2/board2_dark_vertices.inc"
};

static NJS_CNK_MODEL board2_dark_model = {
#include "shared/board2/board2_dark_model.inc"
};

static NJS_CNK_OBJECT board2_dark_object = {
#include "shared/board2/board2_dark_object.inc"
};

/* The Ninja material stores its colors as packed words. */
static NJS_MATERIAL board2_collision_materials = {
#include "shared/board2/board2_collision_materials.inc"
};

static Uint16 board2_collision_strips[25] = {
#include "shared/board2/board2_collision_strips.inc"
};

static NJS_MESHSET board2_collision_faces = {
#include "shared/board2/board2_collision_faces.inc"
};

static NJS_POINT3 board2_collision_points[8] = {
#include "shared/board2/board2_collision_points.inc"
};

static NJS_VECTOR board2_collision_normals[8] = {
#include "shared/board2/board2_collision_normals.inc"
};

static NJS_MODEL board2_collision_model = {
#include "shared/board2/board2_collision_model.inc"
};

static NJS_OBJECT board2_collision = {
#include "shared/board2/board2_collision.inc"
};

static void board2_destroy(struct task *task);
static void board2_step(struct task *task);
static void board2_draw(struct task *task);

KEPT static NJS_CNK_MODEL *board2_models[BOARD2_KINDS] = {
#include "shared/board2/board2_models.inc"
};
NJS_TEXLIST *board2_texlists[BOARD2_KINDS + 1] = {
#include "shared/board2/board2_texlists.inc"
};

void board2_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (half_try_leave(task)) {
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = board2_draw;
    task->step = board2_step;
    task->destructor = board2_destroy;
    /* @hack redundant: an if with empty arms, which changes nothing */
    if (0) {
    } else {
    }
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the shape through null where the pool has no
     * object to lend. board2_destroy tests task->extra for that null (at 0xc8
     * of the object's code). the copy at 0x36 and board2_step's call of
     * player_near at 0x13e use it untested */
#ifdef NONMATCHING
    /* this side repairs the tag above: without an object, the board ends on
     * its next step, and its placement makes it again while in range */
    if (object == NULL) {
        task_kill(task);
        return;
    }
#endif
    *object = board2_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    /* @hack spelling: the work read again through task, where work would do */
    object->scl[0] = object->scl[1] = task->work->scl.x + 1.0F;
    object->scl[2] = 1.0F;
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(BOARD2_WORLD, task, object);
}

static void board2_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void board2_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (half_try_leave(task)) {
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, and an empty else after it.
     * the image holds none of the block's code. its locals and calls are
     * stand-ins that give the step the image's stack frame */
    if (0) {
        NJS_OBJECT *shape = task->extra.p;
        NJS_CNK_MODEL *model = board2_models[0];
        Float radius = model->r * work->scl.x;

        board2_draw(task);
        shape->scl[0] = radius;
        shape->scl[1] = model->r;
    } else {
    }
    object = task->extra.p;
    if (player_near((NJS_POINT3 *)object->pos,
                    board2_models[KIND(task)]->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void board2_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    /* @hack spelling: the work read again through task, where work would do */
    njScale(NULL, task->work->scl.x + 1.0F, task->work->scl.x + 1.0F, 1.0F);
    njSetTexture(board2_texlists[KIND(task)]);
    njCnkEasyDrawModel(board2_models[KIND(task)]);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static char *board2_names[BOARD2_KINDS] = {
#include "shared/board2/board2_names.inc"
};
