/* the gravestone, GRVSTONE in Pumpkin Hill's object list: a stone the players
 * stand on. a part of it swings to and fro */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg05/gravestone.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for the stone's placement */
static const char gravestone_edit_angle_y[12] = "<- ANGLE Y";
static const char gravestone_edit_mode_a[12] = "<- MODE(A)";
static const char gravestone_edit_mode[12] = "<- MODE( )";

#include "stg05/gravestone/text.inc"
#include "stg05/scenery_art/gravestone.h"

/* the stone's copy of gravestone_collision in the collision world, in
 * task->motion */
#define OBJECT(task) ((NJS_OBJECT *)(task)->motion.p)

/* in ang.x: the stone draws as the A stone */
#define GRAVESTONE_A 0x1
/* the stone takes its size class from its model's radius plus REACH_MARGIN. its
 * copy collides while a player lies within that reach of its pos */
#define REACH_MARGIN 5.0F
#define GRAVESTONE_SHAPES 1

NJS_TEXLIST *gravestone_texlists[] = {&gravestone_texlist,
                                      &gravestone_a_texlist, NULL};
/* the part swings swing_degrees either way about z, by a sine whose angle turns
 * swing_rate a step. each stone starts the sine up to swing_phases steps along,
 * by its work's timer */
static Angle swing_rate = 0xa00;
static Sint32 swing_phases = 60;
static Float swing_degrees = 3.0F;
/* a box that only players meet */
/* clang-format off */
static struct collision_shape gravestone_shapes[GRAVESTONE_SHAPES] = {
    {0, SHAPE_BOX2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {0.0F, 10.0F, 0.0F}, 7.0F, 10.0F, 1.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void gravestone_destroy(struct task *task);
static void gravestone_step(struct task *task);
static void gravestone_draw(struct task *task);

void gravestone_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float reach;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = gravestone_draw;
    task->step = gravestone_step;
    task->destructor = gravestone_destroy;
    work->submode = 0;
    work->timer = (Float)rand() * swing_phases * RAND_SCALE;
    object = task->motion.p;
    *object = gravestone_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_DUSTY;
    reach = object->model->r + REACH_MARGIN;
    if (reach <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    collision_init_in_place(task, gravestone_shapes, GRAVESTONE_SHAPES,
                            COLLISION_OBJECT);
}

static void gravestone_destroy(struct task *task)
{
    if (OBJECT(task)->model != NULL) {
        world_remove(task, OBJECT(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void gravestone_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    /* @hack redundant: the else after the return */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (OBJECT(task)->model != NULL) {
            object = OBJECT(task);
            if (player_near((NJS_POINT3 *)object->pos,
                            object->model->r + REACH_MARGIN)) {
                /* @hack spelling: the flag set as flags = flags |
                 * WORLD_ADDED, where |= would do */
                work->flags = work->flags | WORLD_ADDED;
            } else {
                work->flags &= ~WORLD_ADDED;
            }
        }
        collision_register(task);
    }
}

static void gravestone_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* gravestone_a_texlist names the same three textures, so both stones draw
     * with gravestone_texlist */
    njSetTexture(&gravestone_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (work->ang.x & GRAVESTONE_A) {
        njCnkEasyDrawModel(&gravestone_a_model);
        njTranslateV(NULL, (NJS_VECTOR *)gravestone_a_swing->pos);
        njRotateZ(NULL,
                  (Angle)(njSin((game_state.steps + work->timer) * swing_rate) *
                          swing_degrees * ANGLE_PER_DEGREE));
        njCnkEasyDrawModel(gravestone_a_swing->model);
    } else {
        njCnkEasyDrawModel(&gravestone_model);
        njTranslateV(NULL, (NJS_VECTOR *)gravestone_swing->pos);
        njRotateZ(NULL,
                  (Angle)(njSin((game_state.steps + work->timer) * swing_rate) *
                          swing_degrees * ANGLE_PER_DEGREE));
        njCnkEasyDrawModel(gravestone_swing->model);
    }
    njPopMatrixEx();
}
