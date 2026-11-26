#include "stg23/staff_models.h"
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "stg23/staff.h"
#include "stg23/staff_textures.h"
#include "system/angle.h"
#include "system/task.h"

enum staff_mode { STAFF_WHOLE, STAFF_BROKEN };
enum staff_look { STAFF_BRIGHT, STAFF_DARK, STAFF_LOOKS };
#define NEXT_MODE(task) ((task)->unk_3c.l)
#define NO_NEXT_MODE (-1)
#define LOOK(task) ((int)njAbs((task)->work->scl.z) % STAFF_LOOKS)
/* the steps of the sway so far, which start at a random count */
#define SWAY_STEP(work) ((work)->ang.x)
#define SPIN(work) ((work)->ang.z)
/* the staff's turn about y this step: SPIN, give or take up to SWAY of it. the
 * sway's angle is SWAY_STEP << SWAY_SHIFT, so it runs a cycle in 256 steps */
#define TURN(work)                                                             \
    (SPIN(work) * ((njCos(SWAY_STEP(work) << SWAY_SHIFT) * SWAY) + 1.0F))
#define SWAY 0.2F
#define SWAY_SHIFT 8
/* the bit of the submode that the weak point sets */
#define WEAK_POINT_BIT 0
/* the placement's state once the staff has broken */
#define STATE_BROKEN 1
/* the floor probe starts PROBE_LIFT above the staff's pos */
#define PROBE_LIFT 1.0F
#define WEAK_POINT_HEIGHT 45.0F
#define BREAK_SOUND 5
#define SHARD_SPEED 0.8F
#define SHARD_SPREAD 0.4F
#define SHARD_RISE 0.7F
#define PARTS 3

static void staff_destroy(struct task *task);
static void staff_step(struct task *task);
static void staff_draw(struct task *task);
static void take_mode(struct task *task);
/* @hack type: staff_queue_mode external, where static would do. staff_whole
 * inlines its call, and the image holds its body too */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void staff_queue_mode(struct task *task, int mode);
static void staff_whole(struct task *task);
static void staff_break(struct task *task);
static void staff_broken(struct task *task);

/* clang-format off */
static struct debris_part bright_shards[PARTS] = {
    {&staff_bright_shards_texlist, &staff_bright_shard_0, {4.0252F, 44.7411F, 0.0F}, {0, 0, 0}},
    {&staff_bright_shards_texlist, &staff_bright_shard_1, {-4.7063F, 44.3695F, 0.0F}, {0, 0, 0}},
    {&staff_bright_shards_texlist, &staff_bright_shard_2, {0.0F, 33.077F, 0.0F}, {0, 0, 0}},
};
static struct debris_part dark_shards[PARTS] = {
    {&staff_dark_shards_texlist, &staff_dark_shard_0, {4.0252F, 44.7411F, 0.0F}, {0, 0, 0}},
    {&staff_dark_shards_texlist, &staff_dark_shard_1, {-4.7063F, 44.3695F, 0.0F}, {0, 0, 0}},
    {&staff_dark_shards_texlist, &staff_dark_shard_2, {0.0F, 33.077F, 0.0F}, {0, 0, 0}},
};
/* clang-format on */
static const char *kind_names[] = {"BRIGHT", "DARK"};
NJS_TEXLIST *staff_texlists[] = {
    &staff_bright_texlist,
    &staff_bright_broken_texlist,
    &staff_bright_shards_texlist,
    &staff_dark_texlist,
    &staff_dark_broken_texlist,
    &staff_dark_shards_texlist,
    NULL,
};
/* clang-format off */
static struct collision_shape shape = {
    0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(1),
    SHAPE_UNTURNED | SHAPE_HURT_NO_PUSH | SHAPE_HURTABLE, {0.0F, 25.0F, 0.0F},
    10.0F, 25.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static int break_volume = 50;
static void (*steps[])(struct task *) = {staff_whole, staff_broken};

void staff_init(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (!placement_try_leave(task)) {
        task->draw = staff_draw;
        task->step = staff_step;
        task->destructor = staff_destroy;
        if (placement_get_state(task)) {
            task->work->mode = STAFF_BROKEN;
        } else {
            pos = work->pos;
            pos.y += WEAK_POINT_HEIGHT;
            weak_point_create(task, WEAK_POINT_BIT, &pos);
            collision_init(task, &shape, 1, COLLISION_OBJECT);
            task->work->mode = STAFF_WHOLE;
        }
        NEXT_MODE(task) = NO_NEXT_MODE;
        SWAY_STEP(task->work) = (int)(rand() * 2.0F);
    }
}

static void staff_destroy(struct task *task)
{
}

static void staff_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        take_mode(task);
        steps[task->work->mode](task);
    }
}

static void staff_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    switch (task->work->mode) {
    case STAFF_WHOLE:
        if (LOOK(task) == STAFF_BRIGHT) {
            njSetTexture(&staff_bright_texlist);
            njCnkEasyDrawModel(&staff_bright_model);
        } else {
            njSetTexture(&staff_dark_texlist);
            njCnkEasyDrawModel(&staff_dark_model);
        }
        break;
    case STAFF_BROKEN:
        if (LOOK(task) == STAFF_BRIGHT) {
            njSetTexture(&staff_bright_broken_texlist);
            njCnkEasyDrawMotion(&staff_bright_broken, &staff_break_motion,
                                task->work->timer);
        } else {
            njSetTexture(&staff_dark_broken_texlist);
            njCnkEasyDrawMotion(&staff_dark_broken, &staff_break_motion,
                                task->work->timer);
        }
        break;
    }
    njPopMatrixEx();
}

static void take_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

void staff_queue_mode(struct task *task, int mode)
{
    NEXT_MODE(task) = mode;
    /* @hack redundant: a return as the body's last statement */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void staff_whole(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;

    work->ang.y += TURN(work);
    SWAY_STEP(task->work)++;
    if (task->work->submode) {
        staff_break(task);
        staff_queue_mode(task, STAFF_BROKEN);
    } else {
        collision_register(task);
    }
}

static void staff_break(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR velocity;
    Float floor;

    pos = work->pos;
    velocity.x = 0.0F;
    velocity.y = SHARD_RISE;
    velocity.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + PROBE_LIFT, pos.z, &ang);
    if (LOOK(task) == STAFF_BRIGHT) {
        debris_break_flags(&pos, work->ang.y, bright_shards, PARTS, SHARD_SPEED,
                           SHARD_SPREAD, floor, &velocity,
                           DEBRIS_TRAIL | DEBRIS_VANISH);
    } else {
        debris_break_flags(&pos, work->ang.y, dark_shards, PARTS, SHARD_SPEED,
                           SHARD_SPREAD, floor, &velocity,
                           DEBRIS_TRAIL | DEBRIS_VANISH);
    }
    sound_play_at(BREAK_SOUND, NULL, 0, break_volume, &work->pos);
    task->work->timer = 0;
    placement_set_state(task, STATE_BROKEN);
}

static void staff_broken(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    /* @hack spelling: copy, a second pointer to task->work, where work would
     * do */
    struct task_work *copy;
    Float scale;

    if (task->work->timer < staff_break_motion.nbFrame - 1) {
        copy = task->work;
        /* @bug logic: the slowdown's scale divides as integers. it is 1 on
         * the broken staff's first step and 0 after, so the staff turns once
         * more at full speed and stops. the call at 0x8c5036a6 makes the
         * unsigned division */
        /* NOLINTNEXTLINE(bugprone-integer-division) */
        scale = (Float)((staff_break_motion.nbFrame - copy->timer) /
                        staff_break_motion.nbFrame);
        work->ang.y += TURN(copy) * scale;
        SWAY_STEP(task->work)++;
        task->work->timer++;
    }
}
