/* the short and long fences, FENCES and FENCEL in Iron Gate's object list, and
 * the panel, PANEL. SGOAL, a shut door, runs as a fence. stg09/fence.c holds
 * Prison Lane's fences and panel */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg11/fence.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "stg11/fence/text.inc"

/* a fence's model and its texture list, in task->motion and task->force */
#define MODEL(task) ((NJS_CNK_MODEL *)(task)->motion.p)
#define TEXLIST(task) ((NJS_TEXLIST *)(task)->force.p)

/* in the work's mode: the fence stands still, or pulses by pulse 1 or pulse
 * 2. a MODE_SHADOWED fence draws under DRAW_CONTROL_SHADOW */
enum fence_mode { MODE_STILL, MODE_PULSE_1, MODE_PULSE_2, MODE_SHADOWED };
/* in the work's submode: the fence turns by njRotateZ, njRotateX and
 * njRotateY, in that order. with any other submode it turns about y alone */
#define TURNED_ZXY 4

#define SGOAL_SHAPES 5
#define FENCE_SHORT_SHAPES 1
#define FENCE_LONG_SHAPES 3
#define PANEL_MODE (OBJECT_AS_LAND | OBJECT_MAY_LEAVE)

static NJS_TEXLIST fence_long_texlist;
static NJS_CNK_MODEL fence_long_model;
static NJS_TEXLIST fence_short_texlist;
static NJS_CNK_MODEL fence_short_model;
static NJS_TEXLIST sgoal_texlist;
static NJS_CNK_MODEL sgoal_model;
static NJS_TEXLIST panel_texlist;
/* @hack type: the panel's node declared as an array, where the node alone
 * would do */
static NJS_CNK_OBJECT panel_object[1];
static NJS_OBJECT panel_collision;

static Angle pulse_1_rate;
static Float pulse_1_gain_y;
static Float pulse_1_gain_xz;
static Sint32 pulse_1_phases;
static Angle pulse_2_rate;
static Float pulse_2_gain_y;
static Float pulse_2_gain_x;
static Float pulse_2_gain_z;
static Sint32 pulse_2_phases;
static struct collision_shape sgoal_shapes[SGOAL_SHAPES];
static struct collision_shape fence_short_shapes[FENCE_SHORT_SHAPES];
static struct collision_shape fence_long_shapes[FENCE_LONG_SHAPES];

static void fence_start(struct task *task, struct collision_shape *shapes,
                        int count);
static void fence_destroy(struct task *task);
static void fence_step(struct task *task);
static void fence_draw_posed(struct task *task);
static void fence_draw(struct task *task);

void sgoal_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &sgoal_model;
        task->force.p = &sgoal_texlist;
        fence_start(task, sgoal_shapes, SGOAL_SHAPES);
    }
}

void iron_gate_fence_short_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &fence_short_model;
        task->force.p = &fence_short_texlist;
        fence_start(task, fence_short_shapes, FENCE_SHORT_SHAPES);
    }
}

void iron_gate_fence_long_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &fence_long_model;
        task->force.p = &fence_long_texlist;
        fence_start(task, fence_long_shapes, FENCE_LONG_SHAPES);
    }
}

static void fence_start(struct task *task, struct collision_shape *shapes,
                        int count)
{
    struct task_work *work = task->work;

    /* @hack spelling: the mode and the submode tested as truth values, where
     * tests against MODE_STILL and 0 would do */
    if (!work->mode && !work->submode) {
        task->draw = fence_draw;
    } else {
        task->draw = fence_draw_posed;
    }
    task->step = fence_step;
    task->destructor = fence_destroy;
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
}

/* task_destroy frees task->motion and task->force once the destructor
 * returns. a fence's two point at its model and texture list, not the heap's,
 * so the destructor clears them */
static void fence_destroy(struct task *task)
{
    task->force.p = NULL;
    task->motion.p = NULL;
}

/* @hack redundant: task and work declared register, where a plain parameter and
 * a plain local would do */
static void fence_step(register struct task *task)
{
    register struct task_work *work = task->work;

    if (!placement_try_leave(task) && work->collision != NULL) {
        collision_register(task);
    }
}

static void fence_draw_posed(struct task *task)
{
    struct task_work *work = task->work;
    Float sine;
    Float scale_x;
    Float scale_y;
    Float scale_z;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(TEXLIST(task));
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    if (work->submode == TURNED_ZXY) {
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
    }
    if (work->mode == MODE_SHADOWED) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkEasyDrawModel(MODEL(task));
        draw_control_clear(DRAW_CONTROL_SHADOW);
    } else {
        njCnkEasyDrawModel(MODEL(task));
    }
    njPopMatrix(1);
}

static void fence_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(TEXLIST(task));
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(MODEL(task));
    njPopMatrix(1);
}

void iron_gate_panel_init(struct task *task)
{
    solid_object_init(task, &panel_texlist, panel_object->model, NULL, 0,
                      &panel_collision, PANEL_MODE);
}

#include "shared/prison_fixtures/fence_long.h"
#include "shared/prison_fixtures/fence_short.h"
#include "stg11/fixtures/sgoal.h"
#include "shared/prison_fixtures/panel.h"

NJS_TEXLIST *iron_gate_fence_texlists[5] = {
#include "stg11/fence/iron_gate_fence_texlists.inc"
};
/* the pulses. a pulse's angle is the game's steps plus the work's timer, times
 * the pulse's rate. along each axis, the fence's scale is 1 plus the sine
 * times the axis's gain. each pulse's phases word, which no code here reads,
 * is the range in steps of a random timer */
static Angle pulse_1_rate = 0x300;
static Float pulse_1_gain_y = 0.04F;
static Float pulse_1_gain_xz = -0.02F;
static Sint32 pulse_1_phases = 60;
static Angle pulse_2_rate = 0x400;
static Float pulse_2_gain_y = 0.05F;
static Float pulse_2_gain_x = -0.05F;
static Float pulse_2_gain_z = -0.0F;
static Sint32 pulse_2_phases = 60;
/* SGOAL's boxes: a sill, the door, two posts and a lintel, as stg11/shutter.c
 * lays out a shutter's */
/* clang-format off */
static struct collision_shape sgoal_shapes[SGOAL_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, -7.0F, 0.0F}, 35.0F, 5.5F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 25.0F, 0.0F}, 35.0F, 30.0F, 5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {37.0F, 25.0F, 0.0F}, 5.0F, 30.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-37.0F, 25.0F, 0.0F}, 5.0F, 30.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 57.0F, 0.0F}, 35.0F, 7.5F, 10.0F, 0, {0, 0, 0}},
};
/* the short fence's box, and the long fence's box and two bars */
static struct collision_shape fence_short_shapes[FENCE_SHORT_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 7.5F, 0.0F}, 0.5F, 7.5F, 30.0F, 0, {0, 0, 0}},
};
static struct collision_shape fence_long_shapes[FENCE_LONG_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 20.0F, 0.0F}, 0.5F, 20.0F, 60.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 42.0F, 0.0F}, 3.0F, 60.0F, 1.0F, 0,
     {QUARTER_TURN, 0, QUARTER_TURN}},
    {0, SHAPE_CYLINDER2,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 50.0F, 0.0F}, 6.0F, 60.0F, 1.0F, 0,
     {QUARTER_TURN, 0, QUARTER_TURN}},
};
/* clang-format on */
