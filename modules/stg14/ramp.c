/* the ramp, TJUMPDAI in Radical Highway's object list: a solid ramp. it is
 * stg13/ramp.c's ramp with a texture list for texture_load_shared, and a
 * texture animation in its draw */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "stg14/ramp.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "stg14/ramp/text.inc"

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
    /* the copy of highway_ramp_collision in the collision world, placed as
     * the work is */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the center of object's model, in world space */
};

#define RAMP(task) ((struct ramp *)(task)->motion.p)

/* Ramp and solid surface beneath its scrolling lane. */

static NJS_MATERIAL highway_ramp_collision_materials[1] = {
#include "stg14/ramp/highway_ramp_collision_materials.inc"
};

static Sint16 highway_ramp_collision_indices_0[14] = {
#include "stg14/ramp/highway_ramp_collision_indices_0.inc"
};

static NJS_MESHSET highway_ramp_collision_meshsets[1] = {
#include "stg14/ramp/highway_ramp_collision_meshsets.inc"
};

static NJS_POINT3 highway_ramp_collision_points[8] = {
#include "stg14/ramp/highway_ramp_collision_points.inc"
};

static NJS_VECTOR highway_ramp_collision_normals[8] = {
#include "stg14/ramp/highway_ramp_collision_normals.inc"
};

static NJS_MODEL highway_ramp_collision_model = {
#include "stg14/ramp/highway_ramp_collision_model.inc"
};

static NJS_OBJECT highway_ramp_collision = {
#include "stg14/ramp/highway_ramp_collision.inc"
};

static NJS_TEXNAME highway_ramp_textures[4] = {
#include "stg14/ramp/highway_ramp_textures.inc"
};

static NJS_TEXLIST highway_ramp_texlist = {
#include "stg14/ramp/highway_ramp_texlist.inc"
};

static Sint16 highway_ramp_scroll_polygons[39] = {
#include "stg14/ramp/highway_ramp_scroll_polygons.inc"
};

static Sint32 highway_ramp_scroll_vertices[35] = {
#include "stg14/ramp/highway_ramp_scroll_vertices.inc"
};

static NJS_CNK_MODEL highway_ramp_scroll_mesh = {
#include "stg14/ramp/highway_ramp_scroll_mesh.inc"
};

static NJS_CNK_OBJECT highway_ramp_scroll = {
#include "stg14/ramp/highway_ramp_scroll.inc"
};

static Sint16 highway_ramp_polygons[211] = {
#include "stg14/ramp/highway_ramp_polygons.inc"
};

static Sint32 highway_ramp_vertices[211] = {
#include "stg14/ramp/highway_ramp_vertices.inc"
};

static NJS_CNK_MODEL highway_ramp_mesh = {
#include "stg14/ramp/highway_ramp_mesh.inc"
};

static NJS_CNK_OBJECT highway_ramp_object[1] = {
#include "stg14/ramp/highway_ramp_object.inc"
};

static Sint16 highway_ramp_uvs[22] = {
#include "stg14/ramp/highway_ramp_uvs.inc"
};

/* Scroll the longitudinal texture coordinate by -38 per frame. */
static struct texture_anim highway_ramp_anim = {
#include "stg14/ramp/highway_ramp_anim.inc"
};

NJS_TEXLIST *highway_ramp_texlists[2] = {
#include "stg14/ramp/highway_ramp_texlists.inc"
};

static void ramp_destroy(struct task *task);
static void ramp_step(struct task *task);
static void ramp_draw(struct task *task);

void highway_ramp_init(struct task *task)
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
    *object = highway_ramp_collision;
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
     * the model is highway_ramp_collision's */
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

    njSetTexture(&highway_ramp_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    /* @hack redundant: the texture list set again, though nothing since the
     * first set has changed it */
    njSetTexture(&highway_ramp_texlist);
    texture_anim_apply(highway_ramp_object->child->model, &highway_ramp_anim,
                       game_state.steps);
    njCnkEasyDrawObject(highway_ramp_object);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
