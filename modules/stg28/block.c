/* the block, BLOCK in Pyramid Cave's object list: a solid block, turned and
 * scaled as its work says */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg28/block.h"
#include "stg28/cave_util.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/block/text.inc"
#include "stg28/models/block.h"

#define BLOCK_KINDS 1
#define KIND(work) ((Sint32)njAbs((work)->scl.z) % BLOCK_KINDS)
#define SCALE_X(work) ((work)->scl.x + 1.0F)
#define SCALE_Y(work) ((work)->scl.y + 1.0F)
#define SCALE_Z(work) ((work)->scl.z + 1.0F)

/* task's unk_3c counts 0 to COUNTS - 1 and round again, one a step, from a
 * random start */
#define COUNT(task) ((task)->unk_3c.l)
#define COUNTS 16

/* the block's shape in the collision world collides only while a player lies
 * near its pos. near is within REACH_MARGIN plus BLOCK_REACH times the block's
 * largest scale */
#define BLOCK_REACH 200.0F
#define REACH_MARGIN 10.0F

KEPT static NJS_CNK_MODEL *block_models[BLOCK_KINDS] = {
#include "stg28/block/block_models.inc"
};
NJS_TEXLIST *cave_block_texlists[BLOCK_KINDS + 1] = {
#include "stg28/block/cave_block_texlists.inc"
};
static char *block_kind_names[BLOCK_KINDS] = {
#include "stg28/block/block_kind_names.inc"
};

static void block_destroy(struct task *task);
static void block_step(struct task *task);
static void block_draw(struct task *task);

void cave_block_init(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    struct angles ang;

    if (!placement_try_leave(task)) {
        task->draw = block_draw;
        task->step = block_step;
        task->destructor = block_destroy;
        object = world_pool_alloc();
        /* @bug memory: copies the collision object through null where
         * world_pool_alloc has none to lend (MOV.L at 0x8c509c2a).
         * block_destroy tests task->extra for null (TST at 0x8c509cdc).
         * block_step hands player_near the copy's pos untested (ADD at
         * 0x8c509d48) */
#ifdef NONMATCHING
        /* this side repairs the tag above: where the pool has no object, the
         * init destroys the block, and set.c makes it again on a later pass */
        if (object == NULL) {
            task_destroy(task);
            return;
        }
#endif
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y);
        current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
        njPopMatrixEx();
        task->extra.p = object;
        *object = cave_block_collision;
        object->evalflags &=
            ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
        object->scl[0] = SCALE_X(task->work);
        object->scl[1] = SCALE_Y(task->work);
        object->scl[2] = SCALE_Z(task->work);
        object->ang[0] = ang.x;
        object->ang[1] = ang.y;
        object->ang[2] = ang.z;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        world_add(WORLD_SOLID, task, object);
        COUNT(task) = rand() * (COUNTS * RAND_SCALE);
    }
}

static void block_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void block_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float size;

    if (!placement_try_leave(task)) {
        object = task->extra.p;
        size = SCALE_X(task->work) > SCALE_Y(task->work) ? SCALE_X(task->work)
                                                         : SCALE_Y(task->work);
        size = size > SCALE_Z(task->work) ? size : SCALE_Z(task->work);
        if (player_near((NJS_POINT3 *)object->pos,
                        REACH_MARGIN + (size * BLOCK_REACH))) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        if (++COUNT(task) >= COUNTS) {
            COUNT(task) = 0;
        }
    }
}

static void block_draw(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = SCALE_X(task->work);
    scale.y = SCALE_Y(task->work);
    scale.z = SCALE_Z(task->work);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(cave_block_texlists[KIND(task->work)]);
    cave_model_draw_scaled_v(block_models[KIND(task->work)], &scale);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
