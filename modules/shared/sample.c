/* the samples, SAMPLE S, SAMPLE M, SAMPLE L and SAMPLESW in a boss stage's
 * object list */
#include <ninja.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/sample.h"
#include "system/angle.h"
#include "system/task.h"

/* a fifth sample, of FALL_KIND_BURIED, which runs fall_step_small and draws as
 * SAMPLE S. no object list names it, and no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do */
void sample_buried_init(struct task *task);
/* NOLINTEND(misc-use-internal-linkage) */

#define SAMPLE_SHAPES 2
/* a held SAMPLE S draws tilted about x and z as its holder is. the draw looks
 * for the holder in the first HOLDER_SLOTS player slots */
#define HOLDER_SLOTS 4
/* the radius and height of each kind's cylinder. debug_draw_model's cylinder
 * has both 1, and is centered on the pos it draws at */
#define SMALL_RADIUS 2.5F
#define SMALL_HEIGHT 5.0F
#define MEDIUM_RADIUS 5.0F
#define MEDIUM_HEIGHT 10.0F
#define LARGE_RADIUS 15.0F
#define LARGE_HEIGHT 30.0F
#define SHAKE_RADIUS 2.5F
#define SHAKE_HEIGHT 40.0F

/* each kind's two upright cylinders, a wide one and a narrow one */
/* clang-format off */
static struct collision_shape sample_s_shapes[SAMPLE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_2000 | SHAPE_HURTABLE, {0.0F, 2.0F, 0.0F}, 15.0F, 4.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ENEMIES), 0,
     SHAPE_UNK_2000, {0.0F, 2.0F, 0.0F}, 0.2F, 4.5F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape sample_m_shapes[SAMPLE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 5.0F, 0.0F}, 10.2F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 4.5F, 0.0F}, 2.6F, 6.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape sample_l_shapes[SAMPLE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 15.0F, 0.0F}, 22.0F, 15.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 15.0F, 0.0F}, 12.6F, 15.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape sample_sw_shapes[SAMPLE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 20.0F, 0.0F}, 1.2F, 20.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 20.0F, 0.0F}, 0.1F, 20.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape sample_buried_shapes[SAMPLE_SHAPES] = {
    {4, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_2000 | SHAPE_HURTABLE, {0.0F, 2.0F, 0.0F}, 15.0F, 4.0F, 0.0F, 0,
     {0, 0, 0}},
    {3, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ENEMIES), 0,
     SHAPE_UNK_2000 | SHAPE_HURTABLE, {0.0F, 2.0F, 0.0F}, 0.2F, 4.5F, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */

static void sample_s_step(struct task *task);
static void sample_s_destroy(struct task *task);
static void sample_m_step(struct task *task);
static void sample_m_destroy(struct task *task);
static void sample_l_step(struct task *task);
static void sample_l_destroy(struct task *task);
static void sample_sw_step(struct task *task);
static void sample_sw_destroy(struct task *task);
static void sample_buried_step(struct task *task);
static void sample_buried_destroy(struct task *task);

static void sample_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;
    struct task_work *holder;
    int i;

    switch (work->id) {
    case FALL_KIND_NORMAL:
    case FALL_KIND_BURIED:
        pos.x = work->pos.x;
        pos.y = work->pos.y + (SMALL_HEIGHT / 2);
        pos.z = work->pos.z;
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        scl.x = SMALL_RADIUS;
        scl.y = SMALL_HEIGHT;
        scl.z = SMALL_RADIUS;
        holder = NULL;
        for (i = 0; i < HOLDER_SLOTS; i++) {
            struct player *record = player_records[i];

            if (record != NULL && record->held == task) {
                holder = player_work[i];
                break;
            }
        }
        if (holder != NULL) {
            ang.x = holder->ang.x;
            ang.z = holder->ang.z;
        }
        break;
    case FALL_KIND_11:
        pos.x = work->pos.x;
        pos.y = work->pos.y + (MEDIUM_HEIGHT / 2);
        pos.z = work->pos.z;
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        scl.x = MEDIUM_RADIUS;
        scl.y = MEDIUM_HEIGHT;
        scl.z = MEDIUM_RADIUS;
        break;
    case FALL_KIND_HEAVY:
        pos.x = work->pos.x;
        pos.y = work->pos.y + (LARGE_HEIGHT / 2);
        pos.z = work->pos.z;
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        scl.x = LARGE_RADIUS;
        scl.y = LARGE_HEIGHT;
        scl.z = LARGE_RADIUS;
        break;
    case FALL_KIND_SHAKE:
        pos.x = work->pos.x + motion->acc.x;
        pos.y = work->pos.y + motion->acc.y + (SHAKE_HEIGHT / 2);
        pos.z = work->pos.z + motion->acc.z;
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        scl.x = SHAKE_RADIUS;
        scl.y = SHAKE_HEIGHT;
        scl.z = SHAKE_RADIUS;
        break;
    }
    debug_draw_model(&pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
}

void sample_s_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    collision_init(task, sample_s_shapes, SAMPLE_SHAPES, COLLISION_OBJECT);
    fall_set_kind(work, motion, FALL_KIND_NORMAL);
    task->destructor = sample_s_destroy;
    task->draw = sample_draw;
    task->step = sample_s_step;
}

static void sample_s_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    fall_step_small(task);
}

static void sample_s_destroy(struct task *task)
{
}

void sample_m_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    collision_init(task, sample_m_shapes, SAMPLE_SHAPES, COLLISION_OBJECT);
    fall_set_kind(work, motion, FALL_KIND_11);
    task->destructor = sample_m_destroy;
    task->step = sample_m_step;
    task->draw = sample_draw;
}

static void sample_m_step(struct task *task)
{
    /* @hack spelling: the task through a copy, where task itself would do */
    struct task *sample = task;

    if (!placement_try_leave(sample)) {
        fall_step_medium(sample);
    }
}

static void sample_m_destroy(struct task *task)
{
}

void sample_l_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    collision_init(task, sample_l_shapes, SAMPLE_SHAPES, COLLISION_OBJECT);
    fall_set_kind(work, motion, FALL_KIND_HEAVY);
    task->destructor = sample_l_destroy;
    task->step = sample_l_step;
    task->draw = sample_draw;
}

static void sample_l_step(struct task *task)
{
    /* @hack spelling: the task through a copy, where task itself would do */
    struct task *sample = task;

    if (!placement_try_leave(sample)) {
        fall_step_large(sample);
    }
}

static void sample_l_destroy(struct task *task)
{
}

void sample_sw_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    collision_init(task, sample_sw_shapes, SAMPLE_SHAPES, COLLISION_OBJECT);
    fall_set_kind(work, motion, FALL_KIND_SHAKE);
    task->destructor = sample_sw_destroy;
    task->step = sample_sw_step;
    task->draw = sample_draw;
}

static void sample_sw_step(struct task *task)
{
    /* @hack spelling: the task through a copy, where task itself would do */
    struct task *sample = task;

    if (!placement_try_leave(sample)) {
        fall_step_fixed(sample);
    }
}

static void sample_sw_destroy(struct task *task)
{
}

void sample_buried_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    collision_init(task, sample_buried_shapes, SAMPLE_SHAPES, COLLISION_OBJECT);
    fall_set_kind(work, motion, FALL_KIND_BURIED);
    task->destructor = sample_buried_destroy;
    task->draw = sample_draw;
    task->step = sample_buried_step;
}

static void sample_buried_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        fall_step_small(task);
    }
}

static void sample_buried_destroy(struct task *task)
{
}
