/* Green Hill's scenery, the objects of stg31/scenery.h. the flowers time their
 * moves by game_state.steps. every bobbing flower bobs as one, and every
 * turning flower turns in the same steps */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg31/prop_art.h"
#include "stg31/scenery.h"
#include "system/game_state.h"
#include "system/task.h"

/* the SET editor's label for an ang.y, which no code reads */
static const char editor_angle_y[12] = "<- ANGLE Y";

#define LAND_SCENERY (OBJECT_AS_LAND | OBJECT_MAY_LEAVE)
/* a turning flower's head turns flower_turn_speed a step, in the first
 * TURN_STEPS of every TURN_PERIOD steps. HEAD_TURN keeps its turn */
#define HEAD_TURN(work) ((work)->timer)
#define TURN_PERIOD 20
#define TURN_STEPS 10
/* a bobbing flower's head, FLOWER_HEIGHT above the stem's origin, rises
 * BOB_HEIGHT over RISE_STEPS and falls back over FALL_STEPS. it rests for the
 * rest of every BOB_PERIOD steps. HEAD_LIFT keeps its lift */
#define HEAD_LIFT(work) ((work)->scl.z)
#define BOB_PERIOD 40
#define RISE_STEPS 20
#define FALL_STEPS 15
#define FLOWER_HEIGHT 15.0F
#define BOB_HEIGHT 1.5F

NJS_TEXLIST *green_hill_scenery_texlists[] = {
    &green_hill_palm_texlist, &green_hill_rock_texlist,
    &green_hill_tall_totem_texlist, &green_hill_short_totem_texlist, NULL};

/* TRUE once palm_builds holds the palm's build, which the palm then draws in
 * place of its chunk model */
static Bool palm_built = FALSE;
#define PALM_BUILDS 1
static struct model_build palm_builds[PALM_BUILDS] = {
    {MODEL_BUILD_ALONE, &green_hill_palm_texlist, green_hill_palm_object,
     NULL}};

/* clang-format off */
static struct collision_shape palm_shape = {
    0, SHAPE_CAPSULE,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 36.0F, 0.0F}, 4.0F, 36.0F, 0.0F, 0, {0, 0, 0}};
static struct collision_shape flower_shape = {
    0, SHAPE_CAPSULE,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 11.0F, 0.0F}, 3.5F, 11.0F, 0.0F, 0, {0, 0, 0}};
static Angle flower_turn_speed = 0x500;
static struct collision_shape grass_shape = {
    0, SHAPE_BOX,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 4.845F, 0.0F}, 8.0F, 9.844999F, 1.5F, 0, {0, 0, 0}};
static struct collision_shape tall_totem_shape = {
    0, SHAPE_CYLINDER,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 11.0F, 0.0F}, 2.59F, 11.0F, 0.0F, 0, {0, 0, 0}};
static struct collision_shape short_totem_shape = {
    0, SHAPE_CYLINDER,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 8.25F, 0.0F}, 2.59F, 8.25F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */

static void palm_destroy(struct task *task);
static void palm_step(struct task *task);
static void palm_draw(struct task *task);
static void turning_flower_destroy(struct task *task);
static void turning_flower_step(struct task *task);
static void turning_flower_draw(struct task *task);
static void bobbing_flower_destroy(struct task *task);
static void bobbing_flower_step(struct task *task);
static void bobbing_flower_draw(struct task *task);

Uint8 *green_hill_palm_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, palm_builds, PALM_BUILDS) ==
        PALM_BUILDS) {
        palm_built = TRUE;
    }
    return buffer;
}

void green_hill_palm_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = palm_draw;
        task->step = palm_step;
        task->destructor = palm_destroy;
        work->mode = 0;
        work->ang.x = 0;
        work->ang.z = 0;
        collision_init(task, &palm_shape, 1, COLLISION_OBJECT);
    }
}

static void palm_destroy(struct task *task)
{
}

static void palm_step(struct task *task)
{
    /* @hack redundant: work, a local that nothing reads */
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void palm_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&green_hill_palm_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (palm_built) {
        njDirectDrawObject(palm_builds[0].built);
    } else {
        njCnkEasyDrawModel(green_hill_palm_object->model);
    }
    njPopMatrixEx();
}

void green_hill_turning_flower_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: statements kept for their strings, the SET editor's
     * labels, which SHC places in this source's constants */
    (void)"<- ANGLE Y";
    (void)"<- MODEL(0-1)";

    if (!placement_try_leave(task)) {
        task->draw = turning_flower_draw;
        task->step = turning_flower_step;
        task->destructor = turning_flower_destroy;
        work->mode = 0;
        collision_init(task, &flower_shape, 1, COLLISION_OBJECT);
    }
}

static void turning_flower_destroy(struct task *task)
{
}

static void turning_flower_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        /* @hack redundant: a jump to the label right below it */
        goto register_flower;
    register_flower:
        collision_register(task);
        if (game_state.steps % TURN_PERIOD < TURN_STEPS) {
            HEAD_TURN(work) += flower_turn_speed;
        }
    }
}

static void turning_flower_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *flower;
    NJS_CNK_OBJECT *head;

    if (work->scl.x >= 1.0F) {
        flower = &green_hill_turning_flower_a;
    } else {
        flower = &green_hill_turning_flower_b;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(flower->model);
    head = flower->child;
    njTranslateEx((NJS_VECTOR *)head->pos);
    njRotateZ(NULL, HEAD_TURN(work));
    njCnkEasyDrawModel(head->model);
    njPopMatrixEx();
}

void green_hill_bobbing_flower_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = bobbing_flower_draw;
        task->step = bobbing_flower_step;
        task->destructor = bobbing_flower_destroy;
        work->mode = 0;
        collision_init(task, &flower_shape, 1, COLLISION_OBJECT);
    }
}

static void bobbing_flower_destroy(struct task *task)
{
}

static void bobbing_flower_step(struct task *task)
{
    struct task_work *work = task->work;
    int frame;

    if (!placement_try_leave(task)) {
        collision_register(task);
        frame = game_state.steps % BOB_PERIOD;
        if (frame < RISE_STEPS) {
            HEAD_LIFT(work) = frame * (BOB_HEIGHT / RISE_STEPS);
        } else if (frame < RISE_STEPS + FALL_STEPS) {
            HEAD_LIFT(work) =
                (1.0F - ((frame - RISE_STEPS) * (1.0F / FALL_STEPS))) *
                BOB_HEIGHT;
        } else {
            HEAD_LIFT(work) = 0.0F;
        }
    }
}

static void bobbing_flower_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *flower = &green_hill_bobbing_flower;
    NJS_CNK_OBJECT *head;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(flower->model);
    head = flower->child;
    njTranslate(NULL, 0.0F, HEAD_LIFT(work) + FLOWER_HEIGHT, 0.0F);
    njCnkEasyDrawModel(head->model);
    njPopMatrixEx();
}

void green_hill_grass_init(struct task *task)
{
    model_object_init(task, &green_hill_rock_texlist,
                      green_hill_grass_object->model, &grass_shape, 1,
                      LAND_SCENERY);
}

void green_hill_rock_init(struct task *task)
{
    solid_object_init(task, &green_hill_rock_texlist,
                      green_hill_rock_object->model, NULL, 0,
                      &green_hill_rock_collision, LAND_SCENERY);
}

void green_hill_shadow_init(struct task *task)
{
    model_object_init(task, &green_hill_rock_texlist,
                      green_hill_shadow_object->model, NULL, 0,
                      OBJECT_TURNED | OBJECT_MAY_LEAVE);
}

void green_hill_tall_totem_init(struct task *task)
{
    model_object_init(task, &green_hill_tall_totem_texlist,
                      green_hill_tall_totem_object->model, &tall_totem_shape, 1,
                      LAND_SCENERY);
}

void green_hill_short_totem_init(struct task *task)
{
    model_object_init(task, &green_hill_short_totem_texlist,
                      green_hill_short_totem_object->model, &short_totem_shape,
                      1, LAND_SCENERY);
}
