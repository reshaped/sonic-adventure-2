/* Aquatic Mine's scenery: seven kinds of object, each of which stands still
 * and draws one model. the stage's object list names them MITRUCKP, MIKUSA,
 * MILATTI, MIRAIL, MIRING, MIFENCE and NOCOLSKULL. it names four again in
 * lowercase: mitruckp, mikusa, mirail and mifence */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg07/scenery.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#include "stg07/scenery/text.inc"

#include "stg07/scenery/scenery.h"

/* an object's model and its texture list, in task->motion and
 * task->force */
#define MODEL(task) ((NJS_CNK_MODEL *)(task)->motion.p)
#define TEXLIST(task) ((NJS_TEXLIST *)(task)->force.p)

/* in the work's mode: the object stands still, or pulses by pulse 1 or pulse
 * 2. no init here sets a mode, so every object stands still */
enum scenery_mode { MODE_STILL, MODE_PULSE_1, MODE_PULSE_2 };
/* in the work's submode: the object turns by njRotateZ, njRotateY and
 * njRotateX, in that order. with a submode of 0 it turns about y alone */
#define TURNED_ZYX 3

#define MITRUCKP_SHAPES 2
#define MILATTI_SHAPES 1
#define MIFENCE_SHAPES 2

NJS_TEXLIST *scenery_texlists[7] = {
#include "stg07/scenery/scenery_texlists.inc"
};
/* the pulses. a pulse's angle is the game's steps plus the work's timer, times
 * the pulse's rate. along each axis, the object's scale is 1 plus the sine
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
/* clang-format off */
static struct collision_shape mitruckp_shapes[MITRUCKP_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {-10.0F, 4.0F, 12.0F}, 4.0F, 0.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 1.7F, 0.0F}, 12.0F, 1.7F, 5.0F, 0, {0, 0, 0}},
};
static struct collision_shape milatti_shapes[MILATTI_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, 27.0F, 30.0F, 1.0F, 0, {0, 0, 0}},
};
static struct collision_shape mifence_shapes[MIFENCE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 7.0F, 0.0F}, 43.5F, 7.0F, 0.5F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 16.0F, 0.0F}, 3.0F, 43.5F, 1.0F, 0, {0, 0, QUARTER_TURN}},
};
/* clang-format on */

static void scenery_start(struct task *task, struct collision_shape *shapes,
                          int count);
static void scenery_destroy(struct task *task);
static void scenery_step(struct task *task);
static void scenery_draw(struct task *task);

void mitruckp_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &mitruckp_model;
        task->force.p = &mitruckp_texlist;
        scenery_start(task, mitruckp_shapes, MITRUCKP_SHAPES);
    }
}

void mikusa_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->motion.p = &mikusa_model;
        task->force.p = &mikusa_texlist;
        work->submode = TURNED_ZYX;
        scenery_start(task, NULL, 0);
    }
}

void milatti_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &milatti_model;
        task->force.p = &milatti_texlist;
        scenery_start(task, milatti_shapes, MILATTI_SHAPES);
    }
}

void mirail_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->motion.p = &mirail_model;
        task->force.p = &mirail_texlist;
        work->submode = TURNED_ZYX;
        scenery_start(task, NULL, 0);
    }
}

void miring_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &miring_model;
        task->force.p = &miring_texlist;
        scenery_start(task, NULL, 0);
    }
}

void mifence_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &mifence_model;
        task->force.p = &mifence_texlist;
        scenery_start(task, mifence_shapes, MIFENCE_SHAPES);
    }
}

static void scenery_start(struct task *task, struct collision_shape *shapes,
                          int count)
{
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = scenery_draw;
    task->step = scenery_step;
    task->destructor = scenery_destroy;
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
}

/* task_destroy frees task->motion and task->force once the destructor
 * returns. an object's two point at its model and texture list, not the heap's,
 * so the destructor clears them */
static void scenery_destroy(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    task->force.p = NULL;
    task->motion.p = NULL;
}

/* @hack redundant: task and work declared register, where a plain parameter and
 * a plain local would do */
static void scenery_step(register struct task *task)
{
    register struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->collision != NULL) {
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
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    if (work->submode) {
        njRotateZ(NULL, work->ang.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
    } else {
        njRotateY(NULL, work->ang.y);
    }
    switch (work->mode) {
    case MODE_PULSE_1:
        sine = njSin((game_state.steps + work->timer) * pulse_1_rate);
        scale_y = 1.0F + (pulse_1_gain_y * sine);
        scale_x = 1.0F + (pulse_1_gain_xz * sine);
        njScale(NULL, scale_x, scale_y, scale_x);
        break;
    case MODE_PULSE_2:
        sine = njSin((game_state.steps + work->timer) * pulse_2_rate);
        scale_y = 1.0F + (pulse_2_gain_y * sine);
        scale_x = 1.0F + (pulse_2_gain_x * sine);
        scale_z = 1.0F + (pulse_2_gain_z * sine);
        njScale(NULL, scale_x, scale_y, scale_z);
    }
    njCnkEasyDrawModel(MODEL(task));
    njPopMatrix(1);
}

#include "shared/nocolskull.c" /* NOLINT(bugprone-suspicious-include) */
