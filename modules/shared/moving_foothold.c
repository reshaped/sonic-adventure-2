/* the moving foothold, ASHIBA in a stage's object list. it swings up and down
 * and to and fro, and carries the players on it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/moving_foothold.h"
#include "shared/ocean_util.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* kinds 3 to 5 take the collision objects of kinds 0 to 2, under models of
 * their own. SHADOW indexes the shadow's model and texture list, after the
 * kinds' */
#define FOOTHOLD_KINDS 6
#define SHADOW FOOTHOLD_KINDS
#define KIND(work) ((Uint8)(work)->ang.z % FOOTHOLD_KINDS)
/* an angle's high byte. each of the work's ang.x, ang.y and ang.z holds a
 * setting there. a byte of 256ths of a turn moves HIGH_SHIFT bits up to be an
 * Angle */
#define HIGH_BYTE 0xff00
#define HIGH_SHIFT 8
/* in the high byte of the work's ang.z: the drawn models turn a further quarter
 * turn */
#define TURN_MODELS 0x1
/* each swing follows the cosine of an angle. a speed of n advances the angle
 * by n << SPEED_SHIFT a step. both angles add the phase. high is HIGH_BYTE,
 * which the callers hold in a local */
#define SPEED_SHIFT 4
#define SWAY_SPEED(work) ((Uint8)(work)->ang.x << SPEED_SHIFT)
#define RISE_SPEED(work, high)                                                 \
    (((work)->ang.x & (high)) >> (HIGH_SHIFT - SPEED_SHIFT))
#define PHASE(work) ((Uint8)(work)->ang.y << HIGH_SHIFT)
/* the scale of the models' width and depth, which the work's scl.x holds
 * less 1 */
#define WIDTH_SCALE(work) ((work)->scl.x + 1.0F)
/* the placed pos, which the foothold swings about */
#define ORIGIN(task) ((NJS_POINT3 *)(task)->extra.p)

/* the evalflags that would keep an object's pos, ang and scl from applying */
#define EVAL_UNITS (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
#define FOOTHOLD_WORLD_FLAGS (WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID)
/* world_collect_near_players takes the copy only while a player lies within
 * NEAR_RANGE of it */
#define NEAR_RANGE 86.0F

#include "shared/moving_foothold/text.inc"
#include "shared/moving_foothold_art.h"

/* each kind's model, and the shadow's at SHADOW */
static NJS_CNK_MODEL *moving_foothold_models[FOOTHOLD_KINDS + 1] = {
#include "shared/moving_foothold/moving_foothold_models.inc"
};
NJS_TEXLIST *moving_foothold_texlists[FOOTHOLD_KINDS + 2] = {
#include "shared/moving_foothold/moving_foothold_texlists.inc"
};
static NJS_OBJECT *moving_foothold_objects[FOOTHOLD_KINDS] = {
#include "shared/moving_foothold/moving_foothold_objects.inc"
};
static char *moving_foothold_names[FOOTHOLD_KINDS] = {
#include "shared/moving_foothold/moving_foothold_names.inc"
};

static void moving_foothold_destroy(struct task *task);
static void moving_foothold_step(struct task *task);
static void moving_foothold_draw(struct task *task);

void moving_foothold_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float sway;
    Float rise;
    Float x;
    Float z;
    /* @hack spelling: mask, a local for HIGH_BYTE, where HIGH_BYTE in place
     * would do */
    Angle mask;
    /* @hack spelling: zero, a local for 0, where 0 in place would do */
    Angle zero;

    ORIGIN(task)->x = work->pos.x;
    ORIGIN(task)->y = work->pos.y;
    ORIGIN(task)->z = work->pos.z;
    /* @hack spelling: task->work read again, where work would do */
    sway =
        njCos((game_state.steps * SWAY_SPEED(task->work)) + PHASE(task->work)) *
        task->work->scl.z;
    mask = HIGH_BYTE;
    /* @bug logic: puts the foothold scl.y higher than a step would at the same
     * game_state.steps, so the first step drops it scl.y further than the
     * swing moves it. a step scales the cosine less 1 by scl.y (FADD at 0x254
     * of the object's code), and the init the cosine itself (FMUL at 0x8e) */
    rise = njCos((game_state.steps * RISE_SPEED(task->work, mask)) +
                 PHASE(task->work)) *
           task->work->scl.y;
    ocean_polar_xz(task->work->ang.y & mask, sway, &x, &z);
    work->pos.x += x;
    work->pos.y += rise;
    work->pos.z += z;
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = moving_foothold_draw;
    task->step = moving_foothold_step;
    task->destructor = moving_foothold_destroy;
    zero = 0;
    object = world_pool_alloc();
    /* @bug memory: writes the copy through null where world_pool_alloc has
     * no object to lend (MOV.L at 0xec of the object's code).
     * moving_foothold_destroy tests task->motion for null (TST at 0x184).
     * moving_foothold_step writes through it untested (FMOV.S at 0x2e4) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the foothold, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = *moving_foothold_objects[KIND(task->work)];
    task->motion.p = object;
    object->evalflags &= ~EVAL_UNITS;
    object->scl[0] = object->scl[2] = WIDTH_SCALE(task->work);
    object->scl[1] = 1.0F;
    object->ang[0] = zero;
    object->ang[1] = task->work->ang.y & mask;
    object->ang[2] = zero;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(FOOTHOLD_WORLD_FLAGS, task, object);
}

static void moving_foothold_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    /* task_destroy would free the pool's copy with syFree */
    task->motion.p = NULL;
}

static void moving_foothold_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float sway;
    Float rise;
    Float x;
    Float z;
    NJS_POINT3 pos;
    /* @hack spelling: mask, a local for HIGH_BYTE, where HIGH_BYTE in place
     * would do */
    Angle mask;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. they are the labels of a
     * debug display of the work's settings */
    if (0) {
        (void)"<- SPEED UD / FB";
        (void)"<- ANG Y / RUG(  %)";
        (void)"<- ASHIBA(    /    )";
        (void)"YOKO";
        (void)"TATE";
        (void)"<- SCALE(1.0+x)";
        (void)"<- UD RANGE";
        (void)"<- FB RANGE";
    }

    if (placement_try_leave(task)) {
        return;
    }
    mask = HIGH_BYTE;
    object = task->motion.p;
    /* @hack spelling: task->work read again, where work would do */
    sway =
        njCos((game_state.steps * SWAY_SPEED(task->work)) + PHASE(task->work)) *
        task->work->scl.z;
    rise = (njCos((game_state.steps * RISE_SPEED(task->work, mask)) +
                  PHASE(task->work)) -
            1.0F) *
           task->work->scl.y;
    ocean_polar_xz(task->work->ang.y & mask, sway, &x, &z);
    pos.x = ORIGIN(task)->x + x;
    pos.y = ORIGIN(task)->y + rise;
    pos.z = ORIGIN(task)->z + z;
    TASK_FORCES_OF(task)
    [0].speed.x = TASK_FORCES_OF(task)[1].speed.x = pos.x - work->pos.x;
    TASK_FORCES_OF(task)
    [0].speed.y = TASK_FORCES_OF(task)[1].speed.y = pos.y - work->pos.y;
    TASK_FORCES_OF(task)
    [0].speed.z = TASK_FORCES_OF(task)[1].speed.z = pos.z - work->pos.z;
    work->pos = pos;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (player_near((NJS_POINT3 *)object->pos, NEAR_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

/* @hack redundant: task, mask, push and pop register variables, where plain
 * ones would do */
static void moving_foothold_draw(register struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    /* @hack redundant: unused, eight bytes nothing reads, which size the
     * frame */
    Uint8 unused[8];
    /* @hack spelling: mask, kinds, models, push and pop, locals for
     * HIGH_BYTE, FOOTHOLD_KINDS, the models, njPushMatrixEx and
     * njPopMatrixEx, where each in place would do */
    register Angle mask;
    int kinds;
    NJS_CNK_MODEL **models;
    register Bool (*push)(void);
    register Bool (*pop)(void);

    /* @hack spelling: task->work read again, where work would do */
    scale.x = scale.z = WIDTH_SCALE(task->work);
    scale.y = 1.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    mask = HIGH_BYTE;
    models = moving_foothold_models;
    kinds = FOOTHOLD_KINDS;
    pop = njPopMatrixEx;
    push = njPushMatrixEx;
    /* @hack redundant: a loop no path enters */
    while (0) {
        ORIGIN(task)->x = 0.0F;
    }
    push();
    njTranslateEx(&work->pos);
    njRotateY(NULL, task->work->ang.y & mask);
    if (((task->work->ang.z & mask) >> HIGH_SHIFT) & TURN_MODELS) {
        njRotateY(NULL, QUARTER_TURN);
    }
    push();
    njSetTexture(moving_foothold_texlists[(Uint8)task->work->ang.z % kinds]);
    ocean_model_draw_scaled_v(models[(Uint8)task->work->ang.z % kinds], &scale);
    pop();
    njTranslate(NULL, 0.0F, ORIGIN(task)->y - work->pos.y, 0.0F);
    /* @hack redundant: a block no path reaches */
    if (0) {
        /* @hack redundant: scl_x, scl_y and scl_z register variables, where
         * plain ones would do */
        register Float scl_x = task->work->scl.x;
        register Float scl_y = task->work->scl.y;
        register Float scl_z = task->work->scl.z;

        push();
        push();
        njTranslate(NULL, scl_x, scl_y, scl_z);
        pop();
    }
    njSetTexture(moving_foothold_texlists[SHADOW]);
    ocean_model_draw_scaled_v(models[SHADOW], &scale);
    pop();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
