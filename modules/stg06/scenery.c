/* Sky Rail's scenery: six kinds of object, each of which stands still and
 * draws one model. the stage's object list names them RCROSS, TIM, KUSA,
 * KAKASI, FENCE and WOODEAD */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "stg06/scenery.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the set editor's labels for an object's placement */
static const char scenery_edit_angle_x[12] = "<- ANGLE X";
static const char scenery_edit_angle_y[12] = "<- ANGLE Y";
static const char scenery_edit_angle_z[12] = "<- ANGLE Z";

#include "stg06/scenery/text.inc"
#include "stg06/scenery_art/scenery.h"

/* an object's model and its texture list, in task->motion and task->force */
#define MODEL(task) ((NJS_CNK_MODEL *)(task)->motion.p)
#define TEXLIST(task) ((NJS_TEXLIST *)(task)->force.p)

/* in the work's mode: the object stands still, or pulses by pulse 1 or pulse
 * 2. no init here sets a mode, so every object stands still */
enum scenery_mode { MODE_STILL, MODE_PULSE_1, MODE_PULSE_2 };
/* in the work's submode: the object turns by njRotateZ, njRotateX and
 * njRotateY, in that order. without it the object turns about y alone */
#define TURNED 3

#define RCROSS_SHAPES 2
#define TIM_SHAPES 2
#define SCARECROW_SHAPES 1
#define FENCE_SHAPES 1
#define DEAD_TREE_SHAPES 1

NJS_TEXLIST *scenery_texlists[7] = {
#include "stg06/scenery/scenery_texlists.inc"
};
/* the pulses. a pulse's angle is the game's steps plus the work's timer, times
 * the pulse's rate. along each axis, the object's scale is 1 plus the sine
 * times the axis's gain. no code reads the phases */
static Angle pulse_1_rate = 0x300;
static Float pulse_1_gain_y = 0.04F;
static Float pulse_1_gain_xz = -0.02F;
static Sint32 pulse_1_phases = 60;
static Angle pulse_2_rate = 0x400;
static Float pulse_2_gain_y = 0.05F;
static Float pulse_2_gain_x = -0.05F;
static Float pulse_2_gain_z = -0.0F;
static Sint32 pulse_2_phases = 60;
/* clang-format off */
static struct collision_shape rcross_shapes[RCROSS_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 18.0F, 0.0F}, 9.0F, 18.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {16.5F, 4.5F, 0.0F}, 6.0F, 12.5F, 0.0F, 0, {0, 0, QUARTER_TURN}},
};
static struct collision_shape tim_shapes[TIM_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {-20.0F, 13.0F, 0.0F}, 7.0F, 19.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {-9.0F, 0.0F, -1.0F}, 7.0F, 9.0F, 0.0F, 0, {0, 0, QUARTER_TURN}},
};
static struct collision_shape scarecrow_shapes[SCARECROW_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 7.0F, 0.0F}, 8.0F, 7.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape fence_shapes[FENCE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 10.0F, 0.0F}, 17.0F, 12.5F, 1.0F, 0, {0, 0, 0}},
};
static struct collision_shape dead_tree_shapes[DEAD_TREE_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {2.0F, 23.0F, 0.0F}, 10.0F, 25.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void scenery_start(struct task *task, struct collision_shape *shapes,
                          int count);
static void scenery_destroy(struct task *task);
static void scenery_step(register struct task *task);
static void scenery_draw(struct task *task);

void rcross_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    work->submode = TURNED;
    task->motion.p = &sky_rail_rcross_model;
    task->force.p = &sky_rail_rcross_texlist;
    scenery_start(task, rcross_shapes, RCROSS_SHAPES);
}

void tim_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = &sky_rail_tim_model;
    task->force.p = &sky_rail_tim_texlist;
    scenery_start(task, tim_shapes, TIM_SHAPES);
}

void grass_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    work->submode = TURNED;
    task->motion.p = &sky_rail_grass_model;
    task->force.p = &sky_rail_grass_texlist;
    scenery_start(task, NULL, 0);
}

void scarecrow_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = &sky_rail_scarecrow_model;
    task->force.p = &sky_rail_scarecrow_texlist;
    scenery_start(task, scarecrow_shapes, SCARECROW_SHAPES);
}

void fence_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = &sky_rail_fence_model;
    task->force.p = &sky_rail_fence_texlist;
    scenery_start(task, fence_shapes, FENCE_SHAPES);
}

void dead_tree_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = &sky_rail_dead_tree_model;
    task->force.p = &sky_rail_dead_tree_texlist;
    scenery_start(task, dead_tree_shapes, DEAD_TREE_SHAPES);
}

static void scenery_start(struct task *task, struct collision_shape *shapes,
                          int count)
{
    task->draw = scenery_draw;
    task->step = scenery_step;
    task->destructor = scenery_destroy;
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
}

/* task_destroy frees task->motion and task->force once the destructor
 * returns. an object's two point at a static model and texture list, so the
 * destructor clears them */
static void scenery_destroy(struct task *task)
{
    task->force.p = NULL;
    task->motion.p = NULL;
}

/* @hack redundant: task and work declared register, where a plain parameter and
 * a plain local would do */
static void scenery_step(register struct task *task)
{
    register struct task_work *work = task->work;

    if (!placement_try_leave(task) && work->collision != NULL) {
        collision_register(task);
    }
}

static void scenery_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float sine;
    Float scale_x;
    Float scale_y;
    Float scale_z;

    njSetTexture(TEXLIST(task));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (work->submode) {
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
    }
    njRotateY(NULL, work->ang.y);
    switch (work->mode) {
    case MODE_PULSE_1:
        sine = njSin((game_state.steps + work->timer) * pulse_1_rate);
        scale_y = 1.0F + (sine * pulse_1_gain_y);
        scale_x = 1.0F + (sine * pulse_1_gain_xz);
        njScale(NULL, scale_x, scale_y, scale_x);
        break;
    case MODE_PULSE_2:
        sine = njSin((game_state.steps + work->timer) * pulse_2_rate);
        scale_y = 1.0F + (sine * pulse_2_gain_y);
        scale_x = 1.0F + (sine * pulse_2_gain_x);
        scale_z = 1.0F + (sine * pulse_2_gain_z);
        njScale(NULL, scale_x, scale_y, scale_z);
        break;
    /* @hack redundant: an empty default */
    default:
        break;
    }
    njCnkEasyDrawModel(MODEL(task));
    njPopMatrixEx();
}
