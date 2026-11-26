/* the springs a flute stand calls up, KDSPRING and KDSPRINGB in a stage's
 * object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/knudai.h"
#include "shared/knudai_spring.h"
#include "shared/spring.h"
#include "system/task.h"

#include "shared/knudai_spring/text.inc"

/* @hack type: scl.z truncated to a Sint16, where a cast to int would do */
#define KNUDAI_ID(work, mask) (((Sint16)(work)->scl.z & (mask)) % KNUDAI_IDS)

/* the object's modes, in its work's submode */
enum knudai_spring_mode { MODE_WAIT, MODE_APPEAR, MODE_SHOWN };
/* the placement's state once the object's stand is played */
#define STATE_PLAYED 1

/* the work's tick: TRUE for KDSPRINGB, and FALSE for KDSPRING */
#define SPRING_B(work) ((work)->tick)

/* the work's mode: TRUE ends the object once its spring is made. the object
 * only sets it FALSE */
#define DONE(work) ((work)->mode)

/* a new spring's timer. the spring grows from 1/GROW_TIMER of its size */
#define GROW_TIMER 40

static void knudai_spring_destroy(struct task *task);
static void knudai_spring_step(struct task *task);

void knudai_spring_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @bug logic: the init loses the placement's state. MODE_WAIT overwrites
     * the MODE_APPEAR that a state other than 0 sets. the placement's later
     * tasks then wait for knudai_played again. the store at 0x34 of the
     * object's code overwrites the one at 0x1e */
    if (task->placement != NULL && placement_get_state(task)) {
        work->submode = MODE_APPEAR;
    }
    if (!placement_try_leave(task)) {
        task->step = knudai_spring_step;
        task->destructor = knudai_spring_destroy;
        work->submode = MODE_WAIT;
    }
}

void knudai_spring_b_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @bug logic: the init loses the placement's state, as
     * knudai_spring_init does. the store at 0x72 of the object's code
     * overwrites the one at 0x5c */
    if (task->placement != NULL && placement_get_state(task)) {
        work->submode = MODE_APPEAR;
    }
    if (!placement_try_leave(task)) {
        task->step = knudai_spring_step;
        task->destructor = knudai_spring_destroy;
        work->submode = MODE_WAIT;
        SPRING_B(work) = TRUE;
    }
}

static void knudai_spring_destroy(struct task *task)
{
}

static void knudai_spring_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *spring;
    NJS_POINT3 knudai_pos;
    /* @hack spelling: mask, a local for KNUDAI_ID_MASK, where the constant in
     * place would do */
    int mask;
    /* @hack spelling: create_child, a local for task_create_child, where the
     * function called by name would do */
    struct task *(*create_child)(Uint16 flags, void (*step)(struct task *task),
                                 struct task *parent);

    if (placement_try_leave(task)) {
        return;
    }
    mask = KNUDAI_ID_MASK;
    create_child = task_create_child;
    /* @hack redundant: a loop that never runs */
    while (0) {
        DONE(work) = FALSE;
    }
    switch (work->submode) {
    case MODE_WAIT:
        /* @hack redundant: a test of knudai_get_pos, which returns 1 for
         * every id knudai_played returns 1 for. nothing reads the pos it
         * writes */
        if (knudai_played(KNUDAI_ID(work, mask)) == 1 &&
            knudai_get_pos(KNUDAI_ID(work, mask), &knudai_pos)) {
            work->submode = MODE_APPEAR;
            if (task->placement != NULL) {
                placement_set_state(task, STATE_PLAYED);
            }
        } else if (task->child != NULL) {
            task_destroy_children(task);
        }
        break;
    case MODE_APPEAR:
        task_destroy_children(task);
        if (task->child == NULL) {
            /* @hack redundant: a block no path reaches, kept for its test of
             * create_child and its string. SHC still places the string in
             * this source's constants */
            if (0) {
                (void)KNUDAI_SPRING_DEBUG_ID;
                if (create_child) {
                }
            }
            spring = create_child(
                TASK_WORK, SPRING_B(work) ? spring_b_step : spring_a_step,
                task);
            if (spring != NULL) {
                spring->work->timer = GROW_TIMER;
            }
        }
        if (task->child != NULL) {
            spring = task->child;
            spring->work->pos = work->pos;
            spring->work->ang = work->ang;
            spring->work->scl = work->scl;
            work->submode = MODE_SHOWN;
            DONE(work) = FALSE;
        }
        break;
    case MODE_SHOWN:
        if (DONE(work)) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
        break;
    }
}
