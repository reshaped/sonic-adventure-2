#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "boss_last1/combat_art.h"
#include "boss_last1/arena.h"
#include "boss_last1/body.h"
#include "boss_last1/stage.h"
#include "boss_last1/weak_point.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the life support's modes, in its work's mode */
enum support_mode { SUPPORT_ACTIVE, SUPPORT_HIT };
/* the active life support's attack and guard levels. any attack above its
 * guard hurts it. a hit sets its attack to NO_ATTACK and its guard to
 * SHAPE_TOP_LEVEL, so nothing hurts it. its attack is NO_ATTACK through the
 * lifted wave too */
#define SUPPORT_ATTACK 1
#define SUPPORT_GUARD 0
#define NO_ATTACK 0
/* the radius of the life support's collision record, and the barrier's */
#define RECORD_RADIUS 200.0F
/* the life support glows by the sine of a phase that moves PULSE_STEP each
 * step. once hit, it dims to DIM_BRIGHTNESS or below and stays */
#define PULSE_STEP 0x200
#define DIM_BRIGHTNESS (-0.2F)
/* a hit life support smokes on each step but one of each SMOKE_CYCLE. a
 * puff starts up to SMOKE_SPREAD from it across the ground. its height runs
 * from SMOKE_TOP above it down to SMOKE_DEPTH below that */
#define SMOKE_CYCLE 5
#define SMOKE_SPREAD 30.0F
#define SMOKE_TOP 10.0F
#define SMOKE_DEPTH 15.0F
#define SMOKE_SCALE 8.0F
/* the pump's motion moves PUMP_STEP frames a step, and wraps past
 * PUMP_FRAMES */
#define PUMP_STEP 2.0F
#define PUMP_FRAMES 6.0F
/* the models draw SUPPORT_DROP below the anchor */
#define SUPPORT_DROP 11.0F
/* the barrier's height above the origin */
#define BARRIER_HEIGHT 147.0F

/* the life support's task_work. phase, boss and anchor lie in ang's words,
 * and brightness and frame in scl.x and scl.z */
struct support_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Uint16 timer;
    Angle phase;
    const struct task_work *boss;
    const NJS_POINT3 *anchor;
    NJS_POINT3 pos;
    Float brightness;
    Float unused_24;
    Float frame;
    struct collision_work *collision;
};

static const NJS_VECTOR burst_rise = {0, 0.2F, 0};
/* clang-format off */
static struct collision_shape support_shape = {
    0, SHAPE_SPHERE, PUSH_PLAYERS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(SUPPORT_ATTACK),
    SHAPE_HURTABLE | SHAPE_UNTURNED, {0, -10, 0}, 25, 0, 0, 0, {0, 0, 0}};
/* clang-format on */

/* @hack spelling: the rearm written into each caller by a macro, where an
 * inline function would do */
#define SUPPORT_REARM(task, work)                                              \
    do {                                                                       \
        collision_set_attack((task), 0, SUPPORT_ATTACK);                       \
        collision_set_guard((task), 0, SUPPORT_GUARD);                         \
        (work)->flags &= ~WORK_HURT;                                           \
        (work)->mode = SUPPORT_ACTIVE;                                         \
        (work)->submode = 0;                                                   \
    } while (0)

static void support_active(struct task *task)
{
    struct support_work *work = (struct support_work *)task->work;
    if (collision_hurt(task)) {
        if (collision_next_player(task)) {
            collision_set_attack(task, 0, NO_ATTACK);
            collision_set_guard(task, 0, SHAPE_TOP_LEVEL);
            work->mode = SUPPORT_HIT;
            work->submode = 0;
        } else {
            work->flags &= ~WORK_HURT;
        }
    }
    if (biolizard_attack() == BIOLIZARD_ATTACK_LIFTED_WAVE) {
        if (collision_get_attack(task, 0)) {
            collision_set_attack(task, 0, NO_ATTACK);
        }
    } else if (collision_get_attack(task, 0) != SUPPORT_ATTACK) {
        collision_set_attack(task, 0, SUPPORT_ATTACK);
    }
    work->phase += PULSE_STEP;
    work->brightness = njSin(work->phase);
}

static void support_hit(struct task *task)
{
    struct support_work *work = (struct support_work *)task->work;
    NJS_VECTOR velocity;
    NJS_POINT3 pos;
    const struct task_work *boss = work->boss;
    if (boss->mode != BIOLIZARD_DAMAGE && boss->mode != BIOLIZARD_DEFEAT) {
        SUPPORT_REARM(task, work);
    }
    if (work->brightness > DIM_BRIGHTNESS) {
        work->phase += PULSE_STEP;
        work->brightness = njSin(work->phase);
    }
    if (game_state.steps % SMOKE_CYCLE) {
        velocity = burst_rise;
        pos = work->pos;
        pos.x += SMOKE_SPREAD - (rand() * (2 * SMOKE_SPREAD * RAND_SCALE));
        pos.y += SMOKE_TOP - (rand() * (SMOKE_DEPTH * RAND_SCALE));
        pos.z += SMOKE_SPREAD - (rand() * (2 * SMOKE_SPREAD * RAND_SCALE));
        burst_create_wobbling(&pos, &velocity, SMOKE_SCALE);
    }
}

static void support_step(struct task *task)
{
    struct support_work *work = (struct support_work *)task->work;
    if (!game_state.time_stopped) {
        switch (work->mode) {
        case SUPPORT_ACTIVE:
            support_active(task);
            break;
        case SUPPORT_HIT:
            support_hit(task);
            break;
        }
        {
            /* @hack spelling: anchor, a local copy of work->anchor, where
             * *work->anchor would do */
            const NJS_POINT3 *anchor = work->anchor;
            work->pos = *anchor;
        }
        work->frame += PUMP_STEP;
        if (work->frame > PUMP_FRAMES) {
            work->frame -= PUMP_FRAMES;
        }
        collision_register(task);
    }
}

static void support_draw(struct task *task)
{
    struct support_work *work = (struct support_work *)task->work;
    njSetTexture(&biolizard_effect_texlist);
    draw_attr_save();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_OFFSET_MATERIAL |
                     NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_attr_set(0, NJD_FST_UA);
    constant_material_set(work->brightness, work->brightness, work->brightness,
                          work->brightness);
    light_apply(BIOLIZARD_LIFE_SUPPORT_LIGHT);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njTranslate(NULL, 0.0F, -SUPPORT_DROP, 0.0F);
    njRotateY(NULL, task->parent->work->ang.y);
    njCnkSimpleDrawObject(&biolizard_support_base);
    njCnkSimpleDrawMotion(&biolizard_support_pump, &biolizard_support_motion,
                          work->frame);
    njPopMatrixEx();
    constant_material_set(0, 0, 0, 0);
    draw_attr_restore();
    draw_control_restore();
    light_apply(light_after_draw);
}

static void support_destroy(struct task *task)
{
}

struct task *biolizard_life_support_create(struct task *parent,
                                           const NJS_POINT3 *anchor)
{
    struct task *task;
    struct support_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, support_step, parent);
    if (!task) {
        return NULL;
    }
    work = (struct support_work *)task->work;
    work->pos = *anchor;
    work->anchor = anchor;
    work->boss = parent->work;
    work->frame = 0.0F;
    collision_init(task, &support_shape, 1, COLLISION_ENEMY);
    collision_set_radius(task, RECORD_RADIUS);
    task->destructor = support_destroy;
    task->draw = support_draw;
    return task;
}

Sint8 biolizard_life_support_hit(struct task *task)
{
    struct support_work *work;
    if (!task) {
        return -1;
    }
    work = (struct support_work *)task->work;
    /* @hack spelling: a return of 1 and one of 0, where a return of the test
     * would do */
    if (work->flags & WORK_HURT) {
        return 1;
    }
    return 0;
}

void biolizard_life_support_reset(struct task *task)
{
    struct support_work *work;
    if (task) {
        work = (struct support_work *)task->work;
        SUPPORT_REARM(task, work);
    }
}

static void barrier_step(struct task *task)
{
    if (!game_state.time_stopped && !game_state.unk_09) {
        collision_register(task);
    }
}

static void barrier_destroy(struct task *task)
{
}

struct task *biolizard_barrier_create(struct task *parent)
{
    struct task *task;
    struct task_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, barrier_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    work->pos.x = work->pos.z = 0.0F;
    work->pos.y = BARRIER_HEIGHT;
    collision_init(task, &biolizard_upper_cylinder, 1, COLLISION_OBJECT);
    /* @bug memory: reads through null at 0x8c509746 where collision_init,
     * at 0x8c50973c, can't make the collision record */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. it destroys the task and returns
     * null where collision_init made no record */
    if (!task->work->collision) {
        task_destroy(task);
        return NULL;
    }
#endif
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, RECORD_RADIUS);
    task->destructor = barrier_destroy;
    return task;
}
