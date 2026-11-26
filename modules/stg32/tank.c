/* the tank, TANK in Meteor Herd's object list: a tank that a player's attack
 * breaks, leaving its base */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/tank_textures.h"
#include "stg32/meteors.h"
#include "stg32/scenery_shapes.h"
#include "stg32/tank.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the tank's shapes: BODY_SHAPE, the cylinder an attack breaks, and a capsule
 * beside it */
#define TANK_SHAPES 2
#define BODY_SHAPE 0
/* in the work's submode: whether the tank is broken. the destructor stores it
 * in the placement's state, and the init restores it from there */
#define BROKEN(work) ((work)->submode)
#define PIECES 10
#define BREAK_SPEED 1.2F
#define BREAK_SPREAD 1.3F
#define BREAK_LIFT 0.15F
#define FLOOR_DROP 0.1F
#define SPIN_RADIUS 0.0F
#define BREAK_SCORE 20
#define BREAK_SOUND 7

/* the indexes of models[] */
enum tank_model { WHOLE_MODEL, BASE_MODEL, TANK_MODELS };

NJS_TEXLIST *meteor_tank_textures[] = {&meteor_tank_texlist, &tank_base_texlist,
                                       &tank_top_texlist,    &tank_side_texlist,
                                       &tank_center_texlist, NULL};
/* the pieces the tank breaks into: three of its top, six of its side, and its
 * center */
#define TOP_HEIGHT 42.0F
#define SIDE_HEIGHT 33.0F
#define TOP(x, z, yaw)                                                         \
    {&tank_top_texlist, &tank_top_piece, {x, TOP_HEIGHT, z}, {0, yaw, 0}, NULL}
#define SIDE(x, z, yaw)                                                        \
    {&tank_side_texlist,                                                       \
     &tank_side_piece,                                                         \
     {x, SIDE_HEIGHT, z},                                                      \
     {0, yaw, 0},                                                              \
     NULL}
/* clang-format off */
static struct debris_built_part pieces[PIECES] = {
    TOP(0.0F, 11.0F, 0),
    TOP(9.199999F, -5.4F, THIRD_TURN),
    TOP(-9.200001F, -5.4F, -THIRD_TURN),
    SIDE(0.0F, -7.6F, 0),
    SIDE(-5.6F, -4.8F, SIXTH_TURN),
    SIDE(-6.400001F, 2.800001F, THIRD_TURN),
    SIDE(-0.400002F, 6.800002F, HALF_TURN),
    SIDE(5.999999F, 4.000002F, -THIRD_TURN),
    SIDE(5.999999F, -3.599998F, -SIXTH_TURN),
    {&tank_center_texlist, &tank_center_piece, {0.0F, 23.0F, 0.0F},
     {0, -SIXTH_TURN, 0}, NULL},
};

static struct collision_shape tank_shapes[TANK_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_HURT_NO_PUSH,
     {0.0F, 33.0F, 0.0F}, 10.0F, 13.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 10.0F, -20.0F}, 5.0F, 10.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* meteor_tank_build built both models */
static Bool models_built = FALSE;
static struct model_build models[] = {
    {MODEL_BUILD_ALONE, &meteor_tank_texlist, meteor_tank_object, NULL},
    {MODEL_BUILD_ALONE, &tank_base_texlist, tank_base, NULL},
};

static void tank_destroy(struct task *task);
static void tank_step(struct task *task);
static void tank_draw(struct task *task);

Uint8 *meteor_tank_build(Uint8 *buffer)
{
    debris_build(&buffer, pieces, PIECES);
    if (model_builds_compile(&buffer, models, TANK_MODELS) == TANK_MODELS) {
        models_built = TRUE;
    }
    return buffer;
}

void meteor_tank_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = tank_draw;
    task->step = tank_step;
    task->destructor = tank_destroy;
    work->mode = 0;
    collision_init(task, tank_shapes, TANK_SHAPES, COLLISION_OBJECT);
    if (task->placement != NULL) {
        BROKEN(work) = placement_get_state(task);
    }
}

static void tank_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL) {
        placement_set_state(task, BROKEN(work));
    }
}

#include "shared/breakable_hit.h"

static void tank_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    NJS_VECTOR velocity;

    if (placement_try_leave(task)) {
        return;
    }
    if (BROKEN(work) == FALSE &&
        (hit = collision_next_player_by_shape(task, BODY_SHAPE)) != NULL &&
        breakable_player_hit(task, hit)) {
        velocity.x = 0.0F;
        velocity.y = BREAK_LIFT;
        velocity.z = 0.0F;
        debris_break_built(&work->pos, work->ang.y, pieces, PIECES, BREAK_SPEED,
                           BREAK_SPREAD, work->pos.y - FLOOR_DROP, &velocity,
                           DEBRIS_FADE_COLORS | DEBRIS_TRAIL, SPIN_RADIUS);
        BROKEN(work) = TRUE;
        score_add(BREAK_SCORE);
        object_sound_play(BREAK_SOUND, &work->pos);
    }
    work->flags &= ~WORK_HURT;
    if (work->collision != NULL && BROKEN(work)) {
        work->collision->shapes->flags |= SHAPE_OFF;
    }
    collision_register(task);
}

static void tank_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for the drawn node, where tank_base and
     * meteor_tank_object themselves would do */
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (BROKEN(work)) {
        object = tank_base;
        njSetTexture(&tank_base_texlist);
        if (models_built) {
            njDirectDrawObject(models[BASE_MODEL].built);
        } else {
            njCnkEasyDrawModel(object->model);
        }
    } else {
        njCnkModDrawModel(tank_shadow->model);
        njSetTexture(&meteor_tank_texlist);
        if (models_built) {
            njDirectDrawObject(models[WHOLE_MODEL].built);
        } else {
            object = meteor_tank_object;
            njCnkEasyDrawModel(object->model);
        }
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
