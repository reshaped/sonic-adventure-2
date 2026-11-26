/* the missile's bridges, MSBRILONG, MSBRISHORT, MSBRIMINI and MSBRISLOPE in
 * Metal Harbor's object list. the players walk on them. while the missile
 * lifts off, they draw nothing and bear no player */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg10/harbor_missile.h"
#include "stg10/missile_bridge.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* whether the bridges are hidden: undrawn, with their copies out of the
 * world. their rails' boxes still collide. the bridges hide once the
 * missile's lift-off, with its rider or without, passes BRIDGE_HIDE_STEPS.
 * they are back once it throws the rider or ends */
#define BRIDGE_HIDE_STEPS 44
#define BRIDGES_HIDDEN()                                                       \
    (harbor_missile_task != NULL &&                                            \
     (harbor_missile_task->work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||       \
      harbor_missile_task->work->mode == HARBOR_MISSILE_MODE_EMPTY) &&         \
     harbor_missile_task->work->timer > BRIDGE_HIDE_STEPS)

/* the bridges' mode, in the work's mode */
enum missile_bridge_mode { MODE_STAND };

/* a player is near the world's copy within its model's radius plus
 * REACH_MARGIN of the model's center. the world collects the copy only while
 * one is near. the same sum gives the copy's size class */
#define REACH_MARGIN 50.0F

#define BRIDGE_SHAPES 2
#define SLOPE_SHAPES 6

/* a bridge's record, in task->motion */
struct missile_bridge {
    /* the copy of the kind's collision node that the collision world holds,
     * placed where the work is */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the center of object's model, in world space */
    NJS_TEXLIST *texlist;
    Uint8 unk_44[8];
    NJS_DIRECT_HEAD *built; /* the kind's build from missile_bridges_build */
};

#define BRIDGE(task) ((struct missile_bridge *)(task)->motion.p)
/* the bridge's model, kept in the pointer task->extra in place of a record.
 * task_destroy frees a non-null extra, so the destructor clears the word */
#define MODEL(task) ((NJS_CNK_MODEL *)(task)->extra.p)

static void missile_bridge_start(struct task *task, NJS_CNK_MODEL *model,
                                 NJS_DIRECT_HEAD *built, NJS_TEXLIST *texlist,
                                 NJS_OBJECT *collision,
                                 struct collision_shape *shapes, int count);
static void missile_bridge_destroy(struct task *task);
static void missile_bridge_step(struct task *task);
static void missile_bridge_draw(struct task *task);

/* the SET editor's caption for the work's ang.y */
static const char editor_angle_y[12] = "<- ANGLE Y";

#include "stg10/missile_bridge/text.inc"
#include "stg10/structures/missile_bridge_short.h"
#include "stg10/structures/missile_bridge_mini.h"
#include "stg10/structures/missile_bridge_slope.h"
#include "stg10/structures/missile_bridge_long.h"

NJS_TEXLIST *missile_bridge_texlists[5] = {
#include "stg10/missile_bridge/missile_bridge_texlists.inc"
};
/* each kind's build, or null until missile_bridges_build has made it */
static NJS_DIRECT_HEAD *long_built = NULL;
static NJS_DIRECT_HEAD *short_built = NULL;
static NJS_DIRECT_HEAD *slope_built = NULL;
static NJS_DIRECT_HEAD *mini_built = NULL;
/* each straight bridge's rails, a box along each side */
/* clang-format off */
static struct collision_shape long_shapes[BRIDGE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {16.5F, 14.75F, 120.0F},
     3.5F, 4.75F, 120.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {-16.0F, 14.75F, 120.0F},
     3.0F, 4.75F, 120.0F, 0, {0, 0, 0}},
};
static struct collision_shape short_shapes[BRIDGE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {16.5F, 14.75F, 41.5F},
     3.5F, 4.75F, 41.5F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {-16.0F, 14.75F, 41.5F},
     3.0F, 4.75F, 41.5F, 0, {0, 0, 0}},
};
static struct collision_shape mini_shapes[BRIDGE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {16.5F, 14.75F, 20.0F},
     3.5F, 4.75F, 20.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {-16.0F, 14.75F, 20.0F},
     3.0F, 4.75F, 20.0F, 0, {0, 0, 0}},
};
/* the sloping bridge's rails: a post at each end of each side. a SHAPE_BOX2
 * tilted about z joins each side's posts */
static struct collision_shape slope_shapes[SLOPE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {-16.5F, 165.15F, 233.72F},
     3.75F, 5.2F, 9.25F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {-16.5F, 16.050003F, 5.0F},
     3.75F, 9.05F, 5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {16.5F, 16.050003F, 5.0F},
     3.75F, 9.05F, 5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {-16.5F, 90.1685F, 118.726F},
     138.0F, 8.0F, 3.75F, 0, {0, QUARTER_TURN, -6241}},
    {0, SHAPE_BOX2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {16.5F, 90.1685F, 118.726F},
     138.0F, 8.0F, 3.75F, 0, {0, QUARTER_TURN, -6241}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000, {16.5F, 165.15002F, 233.72502F},
     3.75F, 8.2F, 6.25F, 0, {0, 0, 0}},
};
/* clang-format on */

Uint8 *missile_bridges_build(Uint8 *buf)
{
    njSetTexture(&missile_bridge_long_texlist);
    long_built = object_build(&buf, &missile_bridge_long_object);
    njSetTexture(&missile_bridge_short_texlist);
    short_built = object_build(&buf, &missile_bridge_short_object);
    njSetTexture(&missile_bridge_slope_texlist);
    slope_built = object_build(&buf, &missile_bridge_slope_object);
    njSetTexture(&missile_bridge_mini_texlist);
    mini_built = object_build(&buf, &missile_bridge_mini_object);
    return buf;
}

void missile_bridge_long_init(struct task *task)
{
    missile_bridge_start(task, &missile_bridge_long_model, long_built,
                         &missile_bridge_long_texlist,
                         &missile_bridge_long_collision, long_shapes,
                         BRIDGE_SHAPES);
}

void missile_bridge_short_init(struct task *task)
{
    missile_bridge_start(task, &missile_bridge_short_model, short_built,
                         &missile_bridge_short_texlist,
                         &missile_bridge_short_collision, short_shapes,
                         BRIDGE_SHAPES);
}

void missile_bridge_mini_init(struct task *task)
{
    missile_bridge_start(task, &missile_bridge_mini_model, mini_built,
                         &missile_bridge_mini_texlist,
                         &missile_bridge_mini_collision, mini_shapes,
                         BRIDGE_SHAPES);
}

void missile_bridge_slope_init(struct task *task)
{
    missile_bridge_start(task, &missile_bridge_slope_model, slope_built,
                         &missile_bridge_slope_texlist,
                         &missile_bridge_slope_collision, slope_shapes,
                         SLOPE_SHAPES);
}

static void missile_bridge_start(struct task *task, NJS_CNK_MODEL *model,
                                 NJS_DIRECT_HEAD *built, NJS_TEXLIST *texlist,
                                 NJS_OBJECT *collision,
                                 struct collision_shape *shapes, int count)
{
    struct task_work *work = task->work;
    struct missile_bridge *bridge;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct missile_bridge));
    if (task->motion.p == NULL) {
        return;
    }
    task->extra.p = model;
    /* @hack spelling: the record reached through task->motion for texlist and
     * built, where bridge set first would do */
    BRIDGE(task)->texlist = texlist;
    BRIDGE(task)->built = built;
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = missile_bridge_draw;
    task->step = missile_bridge_step;
    task->destructor = missile_bridge_destroy;
    work->mode = MODE_STAND;
    bridge = task->motion.p;
    bridge->object = *collision;
    bridge->object.evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    /* @hack spelling: the angles set y, z, x, where x, y, z would do */
    bridge->object.ang[1] = work->ang.y;
    bridge->object.ang[2] = 0;
    bridge->object.ang[0] = 0;
    bridge->object.pos[0] = work->pos.x;
    bridge->object.pos[1] = work->pos.y;
    bridge->object.pos[2] = work->pos.z;
    flags = WORLD_SOLID | GROUND_FOOTSTEPS;
    radius = bridge->object.model->r + REACH_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, &bridge->object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the record reached through task->motion for center,
     * where bridge would do */
    njCalcPoint(NULL, &bridge->object.model->center, &BRIDGE(task)->center);
    njPopMatrixEx();
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
}

static void missile_bridge_destroy(struct task *task)
{
    if (BRIDGE(task)->object.model != NULL) {
        world_remove(task, &BRIDGE(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
    task->extra.p = NULL;
}

static void missile_bridge_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    if (placement_try_leave(task)) {
        return;
    }
    if (BRIDGE(task)->object.model != NULL) {
        if (player_near(&BRIDGE(task)->center,
                        BRIDGE(task)->object.model->r + REACH_MARGIN) &&
            !BRIDGES_HIDDEN()) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
}

static void missile_bridge_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(BRIDGE(task)->texlist);
    if (BRIDGES_HIDDEN()) {
        return;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(MODEL(task));
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
