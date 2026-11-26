#include <ninja.h>
#include <sg_xpt.h>

#include "chao/activities.h"
#include "chao/animal_training.h"
#include "chao/behavior.h"
#include "chao/held.h"
#include "chao/cocoon.h"
#include "chao/body_collision.h"
#include "chao/face.h"
#include "chao/fruit.h"
#include "chao/icon.h"
#include "chao/parts.h"
#include "chao/planting.h"
#include "chao/idle.h"
#include "chao/intention.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/sleep.h"
#ifdef NONMATCHING
/* chao_release_book, by which this side lets go of a Chao's book */
#include "chao/small_toys.h"
#endif
#include "chao/swim.h"
#include "chao/work.h"
#include "chao/world.h"
#include "system/task.h"

/* the frames past which chao_wait_behavior idles */
#define WAIT_FRAMES 60
#define DEFAULT_RANGE 80.0F

int chao_wait_behavior(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *state = &work->behavior;

    /* @hack spelling: a one-case switch on the mode, and the flags written
     * through chao->work, where an if and work would do */
    switch (state->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_STAND);
        state->mode++;
        CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
        break;
    }
    if (state->timer++ > WAIT_FRAMES) {
        CHAO_WORK(chao)->flags |= CHAO_COLLIDES;
        chao_set_behavior(chao, chao_idle_behavior);
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_wait_held_behavior(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *state = &work->behavior;

    /* @hack spelling: a one-case switch on the mode, and the flags written
     * through chao->work, where an if and work would do */
    switch (state->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_STAND);
        state->mode++;
        CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static void reset_body(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *state = &work->behavior;

    work->body_angle = 0;
    chao_fade_props(chao);
    if (state->queue[0] != state->previous) {
        chao_restore_eyes(chao);
        chao_restore_mouth(chao);
    }
    chao_clear_emotion_mark(chao);
    if (work->field) {
        task_kill(work->field);
        work->field = NULL;
    }
#ifdef NONMATCHING
    /* this side lets go of the book by chao_release_book. that also clears the
     * link the book holds to its reader */
    chao_release_book(chao);
#else
    if (work->book) {
        task_kill(work->book);
        work->book = NULL;
    }
#endif
    /* @hack spelling: the flags written through chao->work, where work would
     * do */
    CHAO_WORK(chao)->flags |= CHAO_COLLIDES;
    chao_enable_push(chao);
    chao_allow_displacement(chao);
    chao_enable_body_shapes(chao);
    CHAO_WORK(chao)->flags |= CHAO_MOVES;
}

chao_behavior_fn chao_current_behavior(struct task *chao)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);

    return state->queue[state->current];
}

static void reset_progress(struct chao_behavior *state)
{
    state->mode = 0;
    state->phase = 0;
    state->timer = 0;
    state->count = 0;
    state->limit = BEHAVIOR_LIMIT;
}

void chao_set_behavior(struct task *chao, chao_behavior_fn action)
{
    chao_set_timed_behavior(chao, action, BEHAVIOR_UNTIMED);
}

void chao_set_timed_behavior(struct task *chao, chao_behavior_fn action,
                             int frames)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);
    int slot;

    reset_progress(state);
    state->previous = state->queue[state->current];
    for (slot = 0; slot < CHAO_BEHAVIORS; slot++) {
        state->queue[slot] = NULL;
    }
    state->queued = 1;
    state->current = 0;
    state->queue[0] = action;
    state->frames[0] = frames;
    state->value = 0;
    garden_command_partner(chao, PARTNER_CANCEL);
    reset_body(chao);
}

void chao_set_behavior_with_value(struct task *chao, chao_behavior_fn action,
                                  int value)
{
    chao_set_behavior(chao, action);
    chao_set_behavior_value(chao, value);
}

void chao_queue_behavior(struct task *chao, chao_behavior_fn action)
{
    chao_queue_timed_behavior(chao, action, BEHAVIOR_UNTIMED);
}

void chao_queue_timed_behavior(struct task *chao, chao_behavior_fn action,
                               int frames)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);

    if (state->queued < CHAO_BEHAVIORS - 1) {
        state->queue[state->queued] = action;
        state->frames[state->queued] = frames;
        state->queued++;
    }
}

Bool chao_advance_behavior(struct task *chao)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);

    reset_progress(state);
    state->previous = state->queue[state->current];
    state->current++;
    state->queued--;
    /* @hack spelling: an if on the slot, where returning the test would do */
    if (state->queue[state->current]) {
        return TRUE;
    }
    return FALSE;
}

void chao_interrupt_behavior(struct task *chao, chao_behavior_fn action)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);

    chao_set_behavior(chao, action);
    garden_separate_partners(chao);
    state->interrupted = TRUE;
}

void chao_resume_behavior(struct task *chao)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);
    chao_behavior_fn interrupted;
    /* @hack redundant: a point no code uses, which sizes the frame */
    NJS_POINT3 unused;

    (void)unused;
    reset_progress(state);
    interrupted = state->queue[state->current];
    state->queue[state->current] = state->previous;
    state->previous = interrupted;
    state->value = 0;
    garden_command_partner(chao, PARTNER_CANCEL);
    reset_body(chao);
    state->interrupted = FALSE;
}

void chao_set_behavior_value(struct task *chao, int value)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);

    state->value = value;
}

int chao_behavior_value(struct task *chao)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);

    return state->value;
}

void chao_behavior_init(struct task *chao)
{
    struct chao_behavior *state = CHAO_BEHAVIOR(chao);
    int i;

    state->mode = 0;
    state->timer = 0;
    state->count = 0;
    state->range = DEFAULT_RANGE;
    state->base = CHAO_WORK(chao)->base.pos;
    chao_set_behavior(chao, chao_idle_behavior);
    for (i = 0; i < CHAO_INTENTIONS; i++) {
        state->cooldown[i] = 0;
    }
}

void chao_behavior_destroy(struct task *chao)
{
#ifdef NONMATCHING
    /* this side lets go of the book as the Chao dies. on this side the book's
     * extra names its reader, and book_destroy reads it */
    chao_release_book(chao);
#else
    /* @hack redundant: a point no code uses, which sizes the frame */
    NJS_POINT3 unused;

    (void)unused;
#endif
}

static void check_interrupts(struct task *chao)
{
    Bool found = FALSE;
    chao_behavior_fn action = chao_current_behavior(chao);

    if (action == chao_evolve_behavior) {
        return;
    }
    if (action == chao_die_behavior) {
        return;
    }
    if (chao_try_swimming(chao)) {
        found = TRUE;
    } else if (action != chao_sleep_behavior &&
               action != chao_seated_sleep_behavior) {
        /* every try runs, so the last that applies sets the behavior */
        if (chao_try_player_call(chao)) {
            found = TRUE;
        }
        if (chao_try_ball(chao)) {
            found = TRUE;
        }
        if (chao_try_take_headgear(chao)) {
            found = TRUE;
        }
        if (chao_try_take_seed(chao)) {
            found = TRUE;
        }
        if (chao_try_take_fruit(chao)) {
            found = TRUE;
        }
        if (chao_try_animal(chao)) {
            found = TRUE;
        }
        if (chao_try_cry(chao)) {
            found = TRUE;
        }
    }
    if (chao_try_player_hold(chao)) {
        found = TRUE;
    } else if (chao_try_petting(chao)) {
        found = TRUE;
    }
    if (chao_try_knockback(chao)) {
        found = TRUE;
    }
    if (found) {
        garden_clear_partner(chao);
        garden_separate_partners(chao);
    }
}

void fun_0c501218(struct task *chao)
{
}

/* @hack spelling: running and finishing the behavior as macros, expanded in
 * both paths of chao_behavior_step, where two functions would do */
#define RUN_CURRENT(result, chao, state)                                       \
    do {                                                                       \
        if ((state)->queue[(state)->current]) {                                \
            (result) = (state)->queue[(state)->current]((chao));               \
        } else {                                                               \
            (result) = BEHAVIOR_FINISH;                                        \
        }                                                                      \
    } while (0)

#define FINISH_BEHAVIOR(chao, result)                                          \
    do {                                                                       \
        if ((result) == BEHAVIOR_FINISH) {                                     \
            if (!chao_advance_behavior((chao))) {                              \
                chao_set_behavior((chao), chao_idle_behavior);                 \
            }                                                                  \
        } else if ((result) == BEHAVIOR_BREAK) {                               \
            chao_set_behavior((chao), chao_idle_behavior);                     \
        }                                                                      \
    } while (0)

void chao_behavior_step(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *state = &work->behavior;
    int result;

    if (!(work->flags & CHAO_RUNS_BEHAVIOR)) {
        return;
    }
    if (work->flags & CHAO_NO_INTENTIONS) {
        check_interrupts(chao);
        RUN_CURRENT(result, chao, state);
        FINISH_BEHAVIOR(chao, result);
        return;
    }
    fun_0c50ee3a(chao);
    chao_tick_intentions(chao);
    check_interrupts(chao);
    RUN_CURRENT(result, chao, state);
    if (state->frames[state->current] > 0) {
        state->frames[state->current]--;
        if (state->frames[state->current] == 0) {
            result = BEHAVIOR_FINISH;
        }
    }
    if (result == BEHAVIOR_CHOOSE) {
        /* @hack type: each intention narrowed to a Uint16, where the int
         * would do */
        if ((Uint16)chao_intention(chao) == 0) {
            chao_choose_intention(chao);
        } else {
            /* @hack redundant: an empty test of interrupted */
            if (state->interrupted) {
            }
        }
        chao_run_intention(chao, (Uint16)chao_intention(chao));
    } else {
        FINISH_BEHAVIOR(chao, result);
    }
}
