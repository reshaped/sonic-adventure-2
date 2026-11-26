#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "object/set.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "stg21/box_textures.h"
#include "stg21/stone_box.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

enum box_mode { BOX_WHOLE, BOX_BROKEN };
#define NO_NEXT_MODE (-1)
#define NEXT_MODE(task) ((task)->unk_3c.l)
#define BOX_PARTS 4
#define PART_LIFT 16.0F
/* the floor probe starts PROBE_LIFT above the parts' start. a floor up to
 * that far above them still counts */
#define PROBE_LIFT 1.0F
/* the whole box's center, above its pos */
#define WHOLE_CENTER_Y 15.0F
#define BREAK_SOUND 5

static void box_destroy(struct task *task);
static void box_step(struct task *task);
static void box_draw(struct task *task);
static void box_take_mode(struct task *task);
/* @hack type: external linkage, where static would do. SHC leaves out a
 * static function whose every call it inlines */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void stone_box_set_next_mode(struct task *task, int mode);
static void box_whole_step(struct task *task);
static void box_break(struct task *task);
static void box_base_step(struct task *task);

/* clang-format off */
static struct debris_part parts[BOX_PARTS] = {
    {&box_part_0_texlist, &box_part_0_model, {-4.5F, 10.0F, -4.5F}, {0, 0, 0}},
    {&box_part_1_texlist, &box_part_1_model, {4.5F, 10.0F, 4.5F}, {0, 0, 0}},
    {&box_part_2_texlist, &box_part_2_model, {-6.0F, 2.0F, -6.0F}, {0, 0, 0}},
    {&box_part_3_texlist, &box_part_3_model, {6.0F, 2.0F, 6.0F}, {0, 0, 0}},
};
/* clang-format on */
NJS_TEXLIST *stone_box_texlists[] = {
    &stone_box_texlist,
    &box_part_0_texlist,
    &box_part_1_texlist,
    &box_part_2_texlist,
    &box_part_3_texlist,
    &box_base_texlist,
    NULL,
};
/* the record's one shape, by box_mode: the whole box's, and its base's */
/* clang-format off */
static struct collision_shape box_shapes[] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, WHOLE_CENTER_Y, 0.0F}, 10.0F, 15.0F, 10.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 7.5F, 0.0F}, 10.0F, 7.5F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 break_volume = 50;
static void (*box_steps[])(struct task *task) = {box_whole_step, box_base_step};
static Float part_speed = 0.7F;
static Float part_spread = 0.35F;
static Float part_rise = 0.25F;

void stone_box_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 center;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = box_draw;
    task->step = box_step;
    task->destructor = box_destroy;
    /* @hack spelling: the mode set through task in each arm, where work would
     * do */
    if (placement_get_state(task) != BOX_WHOLE) {
        collision_init(task, &box_shapes[BOX_BROKEN], 1, COLLISION_OBJECT);
        task->work->mode = BOX_BROKEN;
    } else {
        /* @hack redundant: the whole box's center, which nothing reads */
        center = work->pos;
        center.y += WHOLE_CENTER_Y;
        collision_init(task, &box_shapes[BOX_WHOLE], 1, COLLISION_OBJECT);
        task->work->mode = BOX_WHOLE;
    }
    NEXT_MODE(task) = NO_NEXT_MODE;
}

static void box_destroy(struct task *task)
{
}

static void box_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        box_take_mode(task);
        box_steps[task->work->mode](task);
    }
}

static void box_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the mode read through task, where work would do */
    switch (task->work->mode) {
    case BOX_WHOLE:
        njSetTexture(&stone_box_texlist);
        njCnkEasyDrawModel(&stone_box_model);
        break;
    case BOX_BROKEN:
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&box_base_texlist);
        njCnkEasyDrawModel(&box_base_model);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
    njPopMatrixEx();
}

static void box_take_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

void stone_box_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task) = mode;
}

static void box_whole_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;

    /* @hack spelling: hit assigned inside the test, where a statement of its
     * own would do */
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (hit = work->collision->hit->owner) != NULL && hit->work != NULL &&
        hit->work->id == WORK_ID_ATTACK) {
        box_break(task);
        stone_box_set_next_mode(task, BOX_BROKEN);
        hit->work->flags |= WORK_HURT;
        return;
    }
    collision_register(task);
}

static void box_break(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles angles;
    NJS_VECTOR speed;
    Float floor;

    pos = work->pos;
    pos.y += PART_LIFT;
    speed.x = 0.0F;
    speed.y = part_rise;
    speed.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + PROBE_LIFT, pos.z, &angles);
    debris_break_flags(&pos, work->ang.y, parts, BOX_PARTS, part_speed,
                       part_spread, floor, &speed,
                       DEBRIS_TRAIL | DEBRIS_VANISH);
    sound_play_at(BREAK_SOUND, NULL, 0, break_volume, &work->pos);
    collision_free(task);
    collision_init(task, &box_shapes[BOX_BROKEN], 1, COLLISION_OBJECT);
    placement_set_state(task, BOX_BROKEN);
}

static void box_base_step(struct task *task)
{
    collision_register(task);
}
