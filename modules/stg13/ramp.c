/* the ramp, TJUMPDAI in City Escape's object list: a solid ramp */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg13/ramp.h"
#include "stg13/ramp/text.inc"
#include "system/task.h"
#include "system/util.h"

/* a ramp's modes, in its work's mode, as player 0 steps onto the ramp and
 * off it */
enum ramp_mode { MODE_WAIT, MODE_STOOD_ON, MODE_LEFT };

/* how far past its collision model's radius a ramp reaches, for the world's
 * size class and for player_near */
#define REACH_MARGIN 5.0F
/* the placement's state once player 0 has stepped off the ramp */
#define STATE_LEFT 1

/* a ramp's record, from the heap, in task->motion */
struct ramp {
    /* the copy of ramp_collision in the collision world, placed as the work
     * is */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the center of object's model, in world space */
};

#define RAMP(task) ((struct ramp *)(task)->motion.p)

static NJS_MATERIAL ramp_collision_materials[1] = {
#include "stg13/ramp/ramp_collision_materials.inc"
};

static Sint16 ramp_collision_indices[19] = {
#include "stg13/ramp/ramp_collision_indices.inc"
};

static NJS_MESHSET ramp_collision_meshsets[1] = {
#include "stg13/ramp/ramp_collision_meshsets.inc"
};

static NJS_POINT3 ramp_collision_points[8] = {
#include "stg13/ramp/ramp_collision_points.inc"
};

static NJS_VECTOR ramp_collision_normals[8] = {
#include "stg13/ramp/ramp_collision_normals.inc"
};

static NJS_MODEL ramp_collision_model = {
#include "stg13/ramp/ramp_collision_model.inc"
};

static NJS_OBJECT ramp_collision = {
#include "stg13/ramp/ramp_collision.inc"
};

static NJS_TEXNAME ramp_texture_names[4] = {
#include "stg13/ramp/ramp_texture_names.inc"
};

static NJS_TEXLIST ramp_texlist = {
#include "stg13/ramp/ramp_texlist.inc"
};

static Sint16 ramp_display_polygons[321] = {
#include "stg13/ramp/ramp_display_polygons.inc"
};

static Sint32 ramp_display_vertices[311] = {
#include "stg13/ramp/ramp_display_vertices.inc"
};

static NJS_CNK_MODEL ramp_model = {
#include "stg13/ramp/ramp_model.inc"
};

NJS_CNK_OBJECT ramp_object = {
#include "stg13/ramp/ramp_object.inc"
};

NJS_TEXLIST *ramp_texlists[2] = {
#include "stg13/ramp/ramp_texlists.inc"
};

static void ramp_destroy(struct task *task);
static void ramp_step(struct task *task);
static void ramp_draw(struct task *task);

void ramp_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct ramp));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = ramp_draw;
    task->step = ramp_step;
    task->destructor = ramp_destroy;
    work->mode = MODE_WAIT;
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    object = &RAMP(task)->object;
    *object = ramp_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &RAMP(task)->center);
    njPopMatrixEx();
    work->id = WORK_ID_RAMP;
}

static void ramp_destroy(struct task *task)
{
    /* @hack redundant: the test of the copy's model, which always holds, as
     * the model is ramp_collision's */
    if (RAMP(task)->object.model != NULL) {
        world_remove(task, &RAMP(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void ramp_step(struct task *task)
{
    struct task_work *work = task->work;
    struct ramp *ramp;
    struct player *player;
    Float radius;

    /* @hack spelling: an else after the return, where the test of the model
     * after the if would do */
    /* @hack redundant: the test of the copy's model, which always holds, as
     * ramp_destroy's does */
    if (placement_try_leave(task)) {
        return;
    } else if (RAMP(task)->object.model != NULL) {
        /* @hack spelling: the assignment of ramp inside the sum, where a
         * statement of its own would do */
        radius = (ramp = RAMP(task))->object.model->r + REACH_MARGIN;
        if (player_near(&ramp->center, radius)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    if (work->mode == MODE_WAIT) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((player = player_records[0]) != NULL &&
            player->ground_task == task) {
            work->mode = MODE_STOOD_ON;
        }
    } else if (work->mode == MODE_STOOD_ON) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((player = player_records[0]) != NULL &&
            player->ground_task != task) {
            work->mode = MODE_LEFT;
            placement_set_state(task, STATE_LEFT);
        }
    }
}

static void ramp_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&ramp_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(&ramp_model);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
