#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "boss_bigbogy/arena.h"
#include "boss_bigbogy/camera.h"
#include "boss_bigbogy/stage.h"
#include "camera/bossinit.h"
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "system/task.h"

/* a path task's modes and a shots task's, in its work's mode */
enum path_mode { PATH_INIT, PATH_WAIT, PATH_MOVE, PATH_DONE };
enum shot_mode {
    SHOT_INIT = 0,
    SHOT_HOLD = 2,
    SHOT_CUTAWAY = 3,
    SHOT_DONE = 4
};
/* a shots task's cutaways left, and its copies of hold_frames and
 * shot_frames */
#define CUTAWAYS_LEFT(work) ((work)->tick)
#define HOLD_FRAMES(work) ((work)->ang.x)
#define SHOT_FRAMES(work) ((work)->ang.y)

/* a path task's motion record. the view's pos is the task's work's pos. a
 * shots task allocates one too, and leaves it unread */
struct camera_move {
    NJS_POINT3 target;
    NJS_VECTOR target_step;
    NJS_VECTOR pos_step;
    struct bigbogy_camera_path *path;
};
/* a look mode's state, in its slot's work */
struct camera_look {
    NJS_POINT3 pos;
    NJS_POINT3 *target;
};

void bigbogy_camera_move(struct task *task, Uint16 frames)
{
    struct camera_move *move = task->motion.p;
    struct bigbogy_camera_path *path = move->path;
    struct task_work *work = task->work;
    bigbogy_step_between(&path->pos_from, &path->pos_to, frames,
                         &move->pos_step);
    bigbogy_step_between(&path->target_from, &path->target_to, frames,
                         &move->target_step);
    work->timer = frames;
    work->mode = PATH_MOVE;
}

static void move_along_path(struct task *task)
{
    struct camera_move *move = task->motion.p;
    struct task_work *work = task->work;
    struct bigbogy_camera_path *path;
    njAddVector(&work->pos, &move->pos_step);
    njAddVector(&move->target, &move->target_step);
    if (work->timer-- == 0) {
        path = move->path;
        work->pos = path->pos_to;
        move->target = path->target_to;
        work->mode = PATH_DONE;
    }
    camera_easyset_states[0].pos = work->pos;
    camera_easyset_states[0].target = move->target;
}

static void path_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case PATH_INIT:
        work->mode = PATH_WAIT;
        break;
    /* @hack redundant: the empty case of PATH_WAIT */
    case PATH_WAIT:
        break;
    case PATH_MOVE:
        move_along_path(task);
        break;
    /* @hack redundant: the empty case of PATH_DONE */
    case PATH_DONE:
        break;
    }
}

static void path_destroy(struct task *task)
{
    struct camera_move *move = task->motion.p;
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_THREE3);
    camera_set_adjust(0, camera_top(0) - 1, CAMERA_ADJUST_THREE2);
    camera_remove_mode(0, camera_top(0));
    syFree(move->path);
}

struct task *bigbogy_camera_path_create(struct task *parent,
                                        const struct bigbogy_camera_path *spec)
{
    struct task *task;
    struct task_work *work;
    struct camera_move *move;
    struct bigbogy_camera_path *path;
#ifdef NONMATCHING
    /* this side repairs the bug tagged at the copy below. view's ang.x, ang.y,
     * dir and dist start at 0 */
    struct camera_state view = {0};
#else
    struct camera_state view;
#endif

    if (parent) {
        task = task_create_child(TASK_WORK, path_step, parent);
#ifdef NONMATCHING
        /* this side returns null where task_create_child fails. the matching
         * side writes through the null */
        if (!task) {
            return NULL;
        }
#endif
        work = task->work;
        move = syCalloc(1, sizeof(*move));
        if (!move) {
            task_kill(task);
            return NULL;
        }
        task->motion.p = move;
        path = syCalloc(1, sizeof(*path));
        if (!path) {
            /* @bug memory: move is freed twice. the syFree at 0x8c509112
             * leaves task->motion pointing at it. task_free_motion at
             * 0x8c060164 frees it again as the task dies */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above. it takes move out of
             * task->motion before the free, so that the task's death doesn't
             * free it again */
            task->motion.p = NULL;
#endif
            syFree(move);
            task_kill(task);
            /* @bug uninitialized: the call returns no value where the copy
             * of spec can't be allocated, at 0x8c50911c. the caller takes
             * whatever task_kill left in R0 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it returns null */
            return NULL;
#else
            return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
        }
        *path = *spec;
        move->path = path;
        view.type = CAMERA_POS_TARGET;
        view.unk_04 = 0;
        view.trigger_push = 0;
        view.pos = path->pos_from;
        view.target = path->target_from;
        view.ang.z = 0;
        /* @bug uninitialized: the copy at 0x8c509194 reads view's ang.x,
         * ang.y, dir and dist, which nothing set */
        camera_easyset_states[0] = view;
        camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
        camera_push_mode(0, CAMERA_MODE_EASYSET);
        work->pos = path->pos_from;
        move->target = path->target_from;
        task->destructor = path_destroy;
        return task;
    }
    /* @bug uninitialized: the call returns task unset where parent is null.
     * the branch at 0x8c509076 skips every assignment of it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void hold_opening(struct task *task)
{
    struct task_work *work = task->work;
    if (work->timer-- == 0) {
        if (CUTAWAYS_LEFT(work) > 0) {
            work->mode = SHOT_CUTAWAY;
            work->submode = 0;
            work->timer = SHOT_FRAMES(work);
            camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
            camera_push_mode(0, CAMERA_MODE_EASYSET);
            --CUTAWAYS_LEFT(work);
        } else {
            work->mode = SHOT_DONE;
            work->submode = 0;
        }
    }
}

static void cutaway(struct task *task)
{
    struct task_work *work = task->work;
    struct bigbogy_camera_shots *shots = task->extra.p;
    NJS_POINT3 target;
    /* @hack redundant: the copy of shots->target, which nothing reads */
    target = *shots->target;
    (void)target;
    if (work->timer-- == 0) {
        camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
        camera_remove_mode(0, camera_top(0));
        work->timer = HOLD_FRAMES(work);
        work->mode = SHOT_HOLD;
        work->submode = 0;
    }
}

static void shots_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case SHOT_INIT:
        work->timer = BIGBOGY_OPENING_STEPS;
        work->mode = SHOT_HOLD;
        work->submode = 0;
        /* fall through */
    case SHOT_HOLD:
        hold_opening(task);
        break;
    case SHOT_CUTAWAY:
        cutaway(task);
        break;
    /* @hack redundant: the empty case of SHOT_DONE */
    case SHOT_DONE:
        break;
    }
}

static void shots_destroy(struct task *task)
{
    if (camera_mode_at(0, camera_top(0)) == CAMERA_MODE_BOSSINIT) {
        camera_bossinit_end(0);
    }
}

struct task *bigbogy_camera_shots_create(struct task *parent,
                                         struct bigbogy_camera_shots *spec)
{
    struct task *task;
    struct task_work *work;
    struct camera_move *move;
    struct bigbogy_camera_shots *shots;
#ifdef NONMATCHING
    /* this side repairs the bug tagged at the copy below. view's ang.x, ang.y,
     * dir and dist start at 0 */
    struct camera_state view = {0};
#else
    struct camera_state view;
#endif
    NJS_POINT3 pos;

    if (parent) {
        task = task_create_child(TASK_WORK, shots_step, parent);
#ifdef NONMATCHING
        /* this side returns null where task_create_child fails. the matching
         * side writes through the null */
        if (!task) {
            return NULL;
        }
#endif
        work = task->work;
        move = syCalloc(1, sizeof(*move));
        if (!move) {
            task_kill(task);
            /* @bug uninitialized: the call returns no value where either
             * allocation fails. both returns go through 0x8c5093aa, and the
             * caller takes whatever task_kill left in R0 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it returns null */
            return NULL;
#else
            return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
        }
        task->motion.p = move;
        shots = syCalloc(1, sizeof(*shots));
        if (!shots) {
            /* @bug memory: move is freed twice. the syFree at 0x8c5093a0
             * leaves task->motion pointing at it. task_free_motion at
             * 0x8c060164 frees it again as the task dies */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above. it takes move out of
             * task->motion before the free, so that the task's death doesn't
             * free it again */
            task->motion.p = NULL;
#endif
            syFree(move);
            task_kill(task);
#ifdef NONMATCHING
            /* this side repairs the bug tagged at the first failed allocation:
             * it returns null */
            return NULL;
#else
            return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
        }
        *shots = *spec;
        task->extra.p = shots;
        camera_bossinit_start(0, &spec->opening);
        view.type = CAMERA_POS_TARGET;
        view.unk_04 = 0;
        view.trigger_push = 0;
        view.target = *spec->target;
        pos = *spec->target;
        pos.x += BIGBOGY_SHOT_DISTANCE * njSin(spec->heading);
        pos.y = bigbogy_arena_center.y + BIGBOGY_SHOT_RISE;
        pos.z += BIGBOGY_SHOT_DISTANCE * njCos(spec->heading);
        view.pos = pos;
        view.ang.z = 0;
        /* @bug uninitialized: the copy at 0x8c5094a8 reads view's ang.x,
         * ang.y, dir and dist, which nothing set */
        camera_easyset_states[0] = view;
        CUTAWAYS_LEFT(work) = spec->count;
        HOLD_FRAMES(work) = spec->hold_frames;
        SHOT_FRAMES(work) = spec->shot_frames;
        task->destructor = shots_destroy;
        return task;
    }
    /* @bug uninitialized: the call returns task unset where parent is null.
     * the branch at 0x8c50936e skips every assignment of it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void look_step(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_look *look = (struct camera_look *)slot->work;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.pos = look->pos;
    camera_state.target = *look->target;
    camera_state.target.y += BIGBOGY_LOOK_RISE;
    camera_state.ang.z = 0;
    camera_state.trigger_push = 0;
}

void bigbogy_camera_look(NJS_POINT3 *target, const NJS_POINT3 *pos, int adjust)
{
    struct camera_look *look;
    camera_set_adjust(0, camera_top(0), adjust);
    look = (struct camera_look *)camera_set_user_mode(0, camera_top(0) + 1,
                                                      look_step)
               ->work;
    look->target = target;
    look->pos = *pos;
}

void bigbogy_camera_pop(int adjust)
{
    camera_set_adjust(0, camera_top(0), adjust);
    camera_remove_mode(0, camera_top(0));
}
