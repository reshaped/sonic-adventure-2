/* CLEAR SW, a plate that sinks once player 0 stands on it.
 * core_switch_motion.c moves the plate's object in the world and its rider
 * as it sinks */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/core_clear_switch.h"
#include "shared/core_goal.h"
#include "shared/core_switch_parts.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the switch's object is in the world's collision only while a player lies
 * within SWITCH_RANGE of it */
#define SWITCH_RANGE 23.1F
/* the switch's kind by scl.z, an index of core_switch_parts.h's arrays */
#define SWITCH_KINDS 1
#define SWITCH_KIND(task)                                                      \
    ((int)_builtin_fabsf((task)->work->scl.z) % SWITCH_KINDS)

static void switch_destroy(struct task *task);
static void switch_step(struct task *task);
static void switch_draw(struct task *task);
static struct clear_switch *switch_allocate(struct task *task);
static void switch_free(struct task *task);
static void switch_set_mode(struct task *task, int mode);
static void switch_run_mode(struct task *task);
static void switch_enter_wait(struct task *task);
static void switch_wait(struct task *task);
static void switch_enter_sink(struct task *task);

int core_switch_sink_steps = 5;
static int dat_core_clear_switch_0 = 0;
static Float dat_core_clear_switch_1 = 100.0F;
static void (*switch_enter[])(struct task *) = {
    switch_enter_wait, switch_enter_sink, core_switch_enter_down};
static void (*switch_update[])(struct task *) = {switch_wait, core_switch_sink,
                                                 core_switch_hold};

/* @hack spelling: the next mode set by an inlined call, where an assignment
 * would do */
#pragma inline(request_mode)
static void request_mode(struct task *task, int mode)
{
    CLEAR_SWITCH(task)->next_mode = mode;
}

void core_clear_switch_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
    /* @hack spelling: 0 held in a local, where the constant would do */
    Angle zero;
    if (placement_try_leave(task) || !switch_allocate(task)) {
        return;
    }
#ifdef NONMATCHING
#else
    task->draw = switch_draw;
    task->step = switch_step;
    task->destructor = switch_destroy;
#endif
    zero = 0;
    /* @hack redundant: a jump to the next statement */
    goto allocate;
allocate:
    collision = world_pool_alloc();
#ifdef NONMATCHING
    /* this side frees the switch and returns where the pool has no object. the
     * matching side writes through the null */
    if (!collision) {
        switch_free(task);
        return;
    }
#endif
    CLEAR_SWITCH(task)->collision = collision;
    *collision = core_switch_collision;
    collision->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    collision->scl[0] = collision->scl[1] = collision->scl[2] = 1.0F;
    collision->ang[0] = zero;
    collision->ang[1] = task->work->ang.y;
    collision->ang[2] = zero;
    collision->pos[0] = work->pos.x;
    collision->pos[1] = work->pos.y;
    collision->pos[2] = work->pos.z;
    world_add(WORLD_SIZE_CLASS(2) | WORLD_MOVING | WORLD_SOLID, task,
              collision);
#ifdef NONMATCHING
    /* this side frees the switch and its object where world_add couldn't add
     * it. only then does it set the draw, the step and the destructor */
    if (!(work->flags & WORLD_ADDED)) {
        world_pool_free(collision);
        switch_free(task);
        return;
    }
    task->draw = switch_draw;
    task->step = switch_step;
    task->destructor = switch_destroy;
#endif
    task->work->id = WORK_ID_CLEAR_SWITCH;
    core_clear_switch = task;
}

static void switch_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side clears core_clear_switch only where it names this switch */
    if (core_clear_switch == task) {
        core_clear_switch = NULL;
    }
#else
    core_clear_switch = NULL;
#endif
    if (CLEAR_SWITCH(task)->collision) {
        world_remove(task, CLEAR_SWITCH(task)->collision);
        world_pool_free(CLEAR_SWITCH(task)->collision);
    }
    switch_free(task);
}

static void switch_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
    /* @hack redundant: a jump to the next statement */
    goto load_collision;
load_collision:
    collision = CLEAR_SWITCH(task)->collision;
    switch_run_mode(task);
    if (player_near((NJS_POINT3 *)collision->pos, SWITCH_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void switch_draw(struct task *task)
{
    struct task_work *work = task->work;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(core_switch_texlists[SWITCH_KIND(task)]);
    njCnkEasyDrawModel(core_switch_bases[SWITCH_KIND(task)]);
    njTranslate(NULL, 0.0F, CLEAR_SWITCH(task)->height, 0.0F);
    njCnkEasyDrawModel(core_switch_plates[SWITCH_KIND(task)]);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}

static struct clear_switch *switch_allocate(struct task *task)
{
    task->extra.p = syMalloc(sizeof(struct clear_switch));
    if (!task->extra.p) {
        return NULL;
    }
    CLEAR_SWITCH(task)->height = 0.0F;
    CLEAR_SWITCH(task)->collision = NULL;
    switch_set_mode(task, SWITCH_WAITING);
    return task->extra.p;
}

static void switch_free(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void switch_set_mode(struct task *task, int mode)
{
    CLEAR_SWITCH(task)->mode = mode;
    CLEAR_SWITCH(task)->next_mode = SWITCH_NO_CHANGE;
    CLEAR_SWITCH(task)->timer = 0;
    switch_enter[CLEAR_SWITCH(task)->mode](task);
}

static void switch_run_mode(struct task *task)
{
    if (CLEAR_SWITCH(task)->next_mode != SWITCH_NO_CHANGE) {
        CLEAR_SWITCH(task)->mode = CLEAR_SWITCH(task)->next_mode;
        CLEAR_SWITCH(task)->next_mode = SWITCH_NO_CHANGE;
        CLEAR_SWITCH(task)->timer = 0;
        switch_enter[CLEAR_SWITCH(task)->mode](task);
    }
    switch_update[CLEAR_SWITCH(task)->mode](task);
    ++CLEAR_SWITCH(task)->timer;
}

static void switch_enter_wait(struct task *task)
{
}

static void switch_wait(struct task *task)
{
    struct task_work *player;
    FORCE(task)->speed.y = 0.0F;
    player = player_work[0];
    if (player && (player->flags & PLAYER_WORK_ON_GROUND) &&
        player_records[0] && player_records[0]->ground_task &&
        player_records[0]->ground_task->work &&
        player_records[0]->ground_task->work->id == WORK_ID_CLEAR_SWITCH) {
        request_mode(task, SWITCH_SINKING);
    }
}

static void switch_enter_sink(struct task *task){}

/* core_switch_motion.c's code starts at the next 32-byte boundary, in both
 * images */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      32
#pragma endasm
#endif
#endif
/* clang-format on */
