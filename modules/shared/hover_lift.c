/* the hover lift, SHOUKOU, SHOUKOUK and BOXSHOUKOU in a stage's object
 * list. it is a platform of one of three shapes. it rides up and down along y,
 * and rests after each ride */
#include "system/port.h"
#include "shared/hover_lift/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/hover_lift.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a lift's shape, in its work's submode */
enum hover_lift_shape { SHAPE_SHOUKOU, SHAPE_SHOUKOUK, SHAPE_BOXSHOUKOU };

/* the half of its cycle a lift is in, in its work's mode. each half is a
 * rest, then a ride. MODE_FIRST_HALF's ride turns from 0 to HALF_TURN, and
 * MODE_SECOND_HALF's from HALF_TURN to a full turn */
enum hover_lift_mode { MODE_FIRST_HALF, MODE_SECOND_HALF };

/* the lift's settings, as hover_lift.h lists them */
#define PHASE(work) ((work)->ang.x)
#define RIDE_STEP(work) ((Uint16)(work)->ang.z)
#define REST(work) ((Uint8)(work)->scl.x)
#define Y_SCALE(work) ((work)->scl.y + 1.0F)
#define RISE(work) ((work)->scl.z)
/* a RISE nearer 0 than this puts the copy in the world without
 * WORLD_MOVING */
#define STILL_RISE 0.001F

/* how high above pos the lift lies, kept in task->extra in place of a
 * pointer. task_destroy frees a non-null extra, so the destructor clears extra
 * first */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define HEIGHT(task) (*(Float *)&(task)->extra)

/* a player within a lift's radius plus NEAR_MARGIN is near it */
#define NEAR_MARGIN 32.0F

static NJS_TEXNAME hover_lift_textures_0[3] = {
#include "shared/hover_lift/hover_lift_textures_0.inc"
};

NJS_TEXLIST hover_lift_texlist_0 = {
#include "shared/hover_lift/hover_lift_texlist_0.inc"
};

static Sint16 hover_lift_polygons_0[217] = {
#include "shared/hover_lift/hover_lift_polygons_0.inc"
};

static Sint32 hover_lift_vertices_0[83] = {
#include "shared/hover_lift/hover_lift_vertices_0.inc"
};

static NJS_CNK_MODEL hover_lift_model_0 = {
#include "shared/hover_lift/hover_lift_model_0.inc"
};

static NJS_CNK_OBJECT hover_lift_object_0 = {
#include "shared/hover_lift/hover_lift_object_0.inc"
};

static NJS_MATERIAL hover_lift_collision_0_materials[1] = {
#include "shared/hover_lift/hover_lift_collision_0_materials.inc"
};

static Sint16 hover_lift_collision_0_indices_0[24] = {
#include "shared/hover_lift/hover_lift_collision_0_indices_0.inc"
};

static NJS_MESHSET hover_lift_collision_0_meshsets[1] = {
#include "shared/hover_lift/hover_lift_collision_0_meshsets.inc"
};

static NJS_POINT3 hover_lift_collision_0_points[8] = {
#include "shared/hover_lift/hover_lift_collision_0_points.inc"
};

static NJS_VECTOR hover_lift_collision_0_normals[8] = {
#include "shared/hover_lift/hover_lift_collision_0_normals.inc"
};

static NJS_MODEL hover_lift_collision_0_model = {
#include "shared/hover_lift/hover_lift_collision_0_model.inc"
};

NJS_OBJECT hover_lift_collision_0 = {
#include "shared/hover_lift/hover_lift_collision_0.inc"
};

static NJS_TEXNAME hover_lift_textures_1[3] = {
#include "shared/hover_lift/hover_lift_textures_1.inc"
};

NJS_TEXLIST hover_lift_texlist_1 = {
#include "shared/hover_lift/hover_lift_texlist_1.inc"
};

static Sint16 hover_lift_polygons_1[225] = {
#include "shared/hover_lift/hover_lift_polygons_1.inc"
};

static Sint32 hover_lift_vertices_1[83] = {
#include "shared/hover_lift/hover_lift_vertices_1.inc"
};

static NJS_CNK_MODEL hover_lift_model_1 = {
#include "shared/hover_lift/hover_lift_model_1.inc"
};

static NJS_CNK_OBJECT hover_lift_object_1 = {
#include "shared/hover_lift/hover_lift_object_1.inc"
};

static NJS_MATERIAL hover_lift_collision_1_materials[1] = {
#include "shared/hover_lift/hover_lift_collision_1_materials.inc"
};

static Sint16 hover_lift_collision_1_indices_0[21] = {
#include "shared/hover_lift/hover_lift_collision_1_indices_0.inc"
};

static NJS_MESHSET hover_lift_collision_1_meshsets[1] = {
#include "shared/hover_lift/hover_lift_collision_1_meshsets.inc"
};

static NJS_POINT3 hover_lift_collision_1_points[8] = {
#include "shared/hover_lift/hover_lift_collision_1_points.inc"
};

static NJS_VECTOR hover_lift_collision_1_normals[8] = {
#include "shared/hover_lift/hover_lift_collision_1_normals.inc"
};

static NJS_MODEL hover_lift_collision_1_model = {
#include "shared/hover_lift/hover_lift_collision_1_model.inc"
};

NJS_OBJECT hover_lift_collision_1 = {
#include "shared/hover_lift/hover_lift_collision_1.inc"
};

static NJS_TEXNAME hover_lift_textures_2[4] = {
#include "shared/hover_lift/hover_lift_textures_2.inc"
};

static NJS_TEXLIST hover_lift_texlist_2 = {
#include "shared/hover_lift/hover_lift_texlist_2.inc"
};

static Sint16 hover_lift_polygons_2[543] = {
#include "shared/hover_lift/hover_lift_polygons_2.inc"
};

static Sint32 hover_lift_vertices_2[331] = {
#include "shared/hover_lift/hover_lift_vertices_2.inc"
};

static NJS_CNK_MODEL hover_lift_model_2 = {
#include "shared/hover_lift/hover_lift_model_2.inc"
};

static NJS_CNK_OBJECT hover_lift_object_2 = {
#include "shared/hover_lift/hover_lift_object_2.inc"
};

static NJS_MATERIAL hover_lift_collision_2_materials[1] = {
#include "shared/hover_lift/hover_lift_collision_2_materials.inc"
};

static Sint16 hover_lift_collision_2_indices_0[29] = {
#include "shared/hover_lift/hover_lift_collision_2_indices_0.inc"
};

static NJS_MESHSET hover_lift_collision_2_meshsets[1] = {
#include "shared/hover_lift/hover_lift_collision_2_meshsets.inc"
};

static NJS_POINT3 hover_lift_collision_2_points[12] = {
#include "shared/hover_lift/hover_lift_collision_2_points.inc"
};

static NJS_VECTOR hover_lift_collision_2_normals[12] = {
#include "shared/hover_lift/hover_lift_collision_2_normals.inc"
};

static NJS_MODEL hover_lift_collision_2_model = {
#include "shared/hover_lift/hover_lift_collision_2_model.inc"
};

static NJS_OBJECT hover_lift_collision_2 = {
#include "shared/hover_lift/hover_lift_collision_2.inc"
};

/* each shape's collision object and model, by enum hover_lift_shape */
KEPT static NJS_OBJECT *collisions[3] = {
#include "shared/hover_lift/collisions.inc"
};
KEPT static NJS_CNK_MODEL *models[3] = {
#include "shared/hover_lift/models.inc"
};
NJS_TEXLIST *hover_lift_texlists[4] = {
#include "shared/hover_lift/hover_lift_texlists.inc"
};

static void hover_lift_start(struct task *task);
static void hover_lift_destroy(struct task *task);
static void hover_lift_step(struct task *task);
static void hover_lift_draw(struct task *task);

void hover_lift_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->work->submode = SHAPE_SHOUKOU;
    hover_lift_start(task);
}

void hover_lift_k_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->work->submode = SHAPE_SHOUKOUK;
    hover_lift_start(task);
}

void hover_lift_box_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->work->submode = SHAPE_BOXSHOUKOU;
    hover_lift_start(task);
}

/* set up a lift of the shape in its work's submode. the work's mode, timer
 * and tick take its place in the cycle from game_state.steps. timer counts the
 * steps of a ride, and tick the steps left of a rest */
static void hover_lift_start(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Uint32 ride;
    Uint32 half;
    Uint32 cycle;
    Uint32 place;

    task->draw = hover_lift_draw;
    task->step = hover_lift_step;
    task->destructor = hover_lift_destroy;
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- REST POSITION";
        (void)"<- ANGLE Y";
        (void)"<- SHOUKOU SPEED";
        (void)"<- REST TIME";
        (void)"<- HEIGHT";
        (void)"<- HOVER HEIGHT";
    }
    ride = HALF_TURN / RIDE_STEP(work);
    half = ride + REST(work);
    cycle = half * 2;
    place = game_state.steps % cycle;
    work->tick = 0;
    if (place >= half) {
        work->timer = place - half;
        work->mode = MODE_SECOND_HALF;
        if (work->timer > ride) {
            work->tick = half - work->timer;
            work->timer = 0;
            work->mode = MODE_FIRST_HALF;
        }
    } else {
        work->timer = place;
        work->mode = MODE_FIRST_HALF;
        if (work->timer > ride) {
            work->tick = half - work->timer;
            work->timer = 0;
            work->mode = MODE_SECOND_HALF;
        }
    }
    HEIGHT(task) = 0.0F;
    /* @hack spelling: a jump to the statement after it, where falling through
     * would do */
    goto alloc;
alloc:
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: writes the copy of the shape's collision object through
     * null where world_pool_alloc has none to lend (the MOV.L at 0x13a of the
     * object's code). the source expects that null: hover_lift_destroy tests
     * task->motion for it (the TST at 0x206 of the object's code). yet
     * hover_lift_step reads through task->motion untested (the FMOV.S at 0x332
     * of the object's code) */
#ifdef NONMATCHING
    /* this side repairs the tag above: without an object, the lift ends on
     * its next step, and its placement makes it again while in range */
    if (object == NULL) {
        task_kill(task);
        return;
    }
#endif
    *object = *collisions[work->submode];
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = 1.0F;
    object->scl[1] = Y_SCALE(work);
    object->scl[2] = 1.0F;
    object->ang[1] = work->ang.y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (njAbs(RISE(work)) < STILL_RISE) {
        flags = WORLD_SOLID;
    } else {
        flags = WORLD_SOLID | WORLD_MOVING;
    }
    /* @hack spelling: the first test's body left empty, where the other tests
     * nested under its negation would do */
    if (object->model->r > WORLD_SMALL_SIZE_CLASS_1_MAX) {
    } else if (object->model->r > WORLD_SMALL_SIZE_CLASS_2_MAX) {
        flags |= WORLD_SIZE_CLASS(1);
    } else if (object->model->r > WORLD_SMALL_SIZE_CLASS_3_MAX) {
        flags |= WORLD_SIZE_CLASS(2);
    } else {
        flags |= WORLD_SIZE_CLASS(3);
    }
    world_add(flags, task, object);
}

static void hover_lift_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->extra.p = NULL;
    task->motion.p = NULL;
}

static void hover_lift_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Angle turn;
    Float cosine;
    Float radius;
    Float scaled;

    if (placement_try_leave(task)) {
        return;
    }
    if (!game_state.time_stopped) {
        turn = (Uint16)(work->timer * work->ang.z);
        /* @hack spelling: the end of the ride tested in each mode's arm, where
         * one test that flips the mode would do */
        if (work->mode != MODE_FIRST_HALF) {
            if (turn > HALF_TURN) {
                turn = 0;
                work->mode = MODE_FIRST_HALF;
                work->timer = 0;
                work->tick = REST(work);
            }
        } else {
            if (turn > HALF_TURN) {
                turn = 0;
                work->mode = MODE_SECOND_HALF;
                work->timer = 0;
                work->tick = REST(work);
            }
        }
        turn += work->mode != MODE_FIRST_HALF ? HALF_TURN : 0;
        cosine = njSin(PHASE(work) + turn + QUARTER_TURN);
        HEIGHT(task) = ((cosine * 0.5F) + 0.5F) * RISE(work);
        if (work->tick == 0) {
            work->timer++;
        } else {
            work->tick--;
        }
        /* @hack redundant: an empty else */
    } else {
    }
    object = task->motion.p;
    TASK_FORCES_OF(task)
    [0].speed.y = work->pos.y + HEIGHT(task) - object->pos[1];
    TASK_FORCES_OF(task)
    [1].speed.y = work->pos.y + HEIGHT(task) - object->pos[1];
    object->pos[1] = work->pos.y + HEIGHT(task);
    radius = object->model->r;
    scaled = radius * Y_SCALE(work);
    if (player_near((NJS_POINT3 *)object->pos,
                    (radius > scaled ? radius : scaled) + NEAR_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void hover_lift_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(hover_lift_texlists[work->submode]);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + HEIGHT(task), work->pos.z);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, 1.0F, Y_SCALE(work), 1.0F);
    njCnkEasyDrawModel(models[work->submode]);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
