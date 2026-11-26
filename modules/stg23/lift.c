#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "stg23/platform_textures.h"
#include "stg23/platforms.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define LIFT_KINDS 6
#define KIND(work) (((work)->ang.z & 0xff) % LIFT_KINDS)
#define PHASE(work) ((game_state.steps * (work)->ang.x) + START_PHASE(work))
#define SCALE(work) ((work)->scl.x + 1.0F)
#define EVAL_UNITS (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
/* a lift's reach is its kind's radius plus CONTACT_MARGIN. while player 0 or 1
 * is that near the lift's world object, the lift's work keeps WORLD_ADDED.
 * world_collect_near_players takes the object only then */
#define CONTACT_MARGIN 15.0F
/* the index of the shadow's texture list in base_lift_texlists */
#define SHADOW_TEXTURES LIFT_KINDS

static void platform_destroy(struct task *task);
static void lift_step(struct task *task);
static void lift_draw(struct task *task);

KEPT static NJS_CNK_MODEL *models[LIFT_KINDS] = {
#include "stg23/lift/models.inc"
};
static NJS_CNK_MODEL *shadow[] = {&lift_shadow};
/* the lifts' texture lists, then the shadow's, then a null that ends them */
NJS_TEXLIST *base_lift_texlists[LIFT_KINDS + 2] = {
#include "stg23/lift/base_lift_texlists.inc"
};
KEPT static NJS_OBJECT *collision_models[LIFT_KINDS] = {
#include "stg23/lift/collision_models.inc"
};
static const char *kind_names[LIFT_KINDS] = {
    "3.0m", "5.0m", "7.0m", "3.0m(dark)", "5.0m(dark)", "7.0m(dark)",
};
/* each kind's height, as its name gives it. no code reads it */
static Float heights[LIFT_KINDS] = {30.0F, 50.0F, 70.0F, 30.0F, 50.0F, 70.0F};
static Float radii[LIFT_KINDS] = {71.0F, 71.0F, 87.0F, 71.0F, 71.0F, 87.0F};

void base_lift_init(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float offset;

    ORIGIN(task)->x = work->pos.x;
    ORIGIN(task)->y = work->pos.y;
    ORIGIN(task)->z = work->pos.z;
    offset = (njCos(PHASE(task->work)) - 1.0F) * task->work->scl.y;
    work->pos.y += offset;
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = lift_draw;
    task->step = lift_step;
    task->destructor = platform_destroy;
    object = world_pool_alloc();
    *object = *collision_models[KIND(task->work)];
    task->motion.p = object;
    object->evalflags &= ~EVAL_UNITS;
    object->scl[0] = object->scl[2] = SCALE(task->work);
    object->scl[1] = 1.0F;
    object->ang[0] = 0;
    /* @bug logic: the collision model turns by all 16 bits of ang.y, and the
     * drawn lift by YAW, the high byte, alone. so the low byte, which
     * START_PHASE takes, turns the model off the lift by up to 255 of a
     * turn's 65536 units. the store at 0x8c5047ea writes all of ang.y */
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID, task, object);
}

#include "stg23/platform_destroy_impl.h"

static void lift_step(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    Float offset;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->motion.p;
    offset = (njCos(PHASE(task->work)) - 1.0F) * task->work->scl.y;
    pos.x = ORIGIN(task)->x;
    pos.y = ORIGIN(task)->y + offset;
    pos.z = ORIGIN(task)->z;
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
    if (player_near((NJS_POINT3 *)object->pos,
                    radii[KIND(task->work)] + CONTACT_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void lift_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.z = SCALE(work);
    scale.y = 1.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, YAW(task->work));
    njPushMatrixEx();
    njSetTexture(base_lift_texlists[KIND(task->work)]);
    ocean_model_draw_scaled_v(models[KIND(task->work)], &scale);
    njPopMatrixEx();
    njTranslate(NULL, 0.0F, ORIGIN(task)->y - work->pos.y, 0.0F);
    njSetTexture(base_lift_texlists[SHADOW_TEXTURES]);
    ocean_model_draw_scaled_v(shadow[0], &scale);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
