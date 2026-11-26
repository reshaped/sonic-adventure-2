/* Iron Gate's camera path. it glides a player's camera along a fixed path
 * while the player's pad is off */
#ifdef NONMATCHING
/* USHRT_MAX, which bounds this side's check of the spans' steps */
#include <limits.h>
#endif
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "stg11/camera_path.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

/* the work's submode holds the slot of the player whose camera and pad the
 * path takes. its tick holds the index of the path's mode on that camera's
 * stack */
#define SLOT(work) ((work)->submode)
#define MODE_INDEX(work) ((work)->tick)

/* a span of the path, which runs steps steps */
struct camera_path_span {
    Sint32 steps;
    /* the camera's pos goes from pos_from at the span's start to pos_to at its
     * end */
    NJS_POINT3 pos_from;
    NJS_POINT3 pos_to;
    /* the camera's target goes from target_from at the span's start to
     * target_to at its end */
    NJS_POINT3 target_from;
    NJS_POINT3 target_to;
};

static struct camera_path_span camera_path_spans[1] = {
#include "stg11/camera_path/camera_path_spans.inc"
};
#define CAMERA_PATH_SPANS ARRAY_COUNT(camera_path_spans)

static void camera_path_destroy(struct task *task);
static void camera_path_count(struct task *task);

/* the path mode's step. slot->steps, the steps the mode has run, picks the
 * span and how far along it the camera is. past the last span, the camera
 * carries on along that span's line */
static void camera_path_follow(struct camera_stack *stack,
                               struct camera_slot *slot)
{
    NJS_POINT3 pos;
    NJS_POINT3 target;
    struct camera_path_span *span;
    struct camera_path_span *key;
    int step;
    int i;
    Float share;
    Float rest;

    share = 0.0F;
    step = slot->steps;
    span = &camera_path_spans[0];
    /* @hack redundant: a block no path reaches, which reads span before the
     * loop */
    if (0) {
        if (span) {
            if (span) {
                if (span) {
                }
            }
        }
    }
    /* @hack spelling: i cast at the test, where a Uint32 i would do */
    for (i = 0; (Uint32)i < CAMERA_PATH_SPANS; i++) {
        /* @hack spelling: key, a copy of the span that span takes, where
         * span alone would do */
        key = &camera_path_spans[i];
        span = key;
        share = (Float)step / (Float)camera_path_spans[i].steps;
        if (camera_path_spans[i].steps > step) {
            break;
        }
        step -= camera_path_spans[i].steps;
    }
    rest = 1.0F - share;
    pos.x = (span->pos_from.x * rest) + (span->pos_to.x * share);
    pos.y = (span->pos_from.y * rest) + (span->pos_to.y * share);
    pos.z = (span->pos_from.z * rest) + (span->pos_to.z * share);
    target.x = (span->target_from.x * rest) + (span->target_to.x * share);
    target.y = (span->target_from.y * rest) + (span->target_to.y * share);
    target.z = (span->target_from.z * rest) + (span->target_to.z * share);
    camera_state.target = target;
    camera_state.pos = pos;
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
}

void camera_path_init(struct task *task)
{
    struct task_work *work = task->work;
    struct camera_slot *slot;
    int steps;
    int i;

    if (!pad_player_is_enabled(SLOT(work))) {
        task_kill(task);
        return;
    }
#ifdef NONMATCHING
    /* Each span divides by its duration. The task's 16-bit timer also
     * includes one final frame before it releases the pad. */
    steps = 0;
    for (i = 0; (Uint32)i < CAMERA_PATH_SPANS; i++) {
        int duration = camera_path_spans[i].steps;

        if (duration <= 0 || duration > (int)USHRT_MAX - 1 - steps) {
            task_kill(task);
            return;
        }
        steps += duration;
    }
#endif
    MODE_INDEX(work) = camera_top(SLOT(work)) + 1;
    /* @hack spelling: slot, a local for the call's result, where a test of
     * the call would do */
    slot =
        camera_set_user_mode(SLOT(work), MODE_INDEX(work), camera_path_follow);
    if (slot == NULL) {
        task_kill(task);
        return;
    }
    camera_set_adjust(SLOT(work), MODE_INDEX(work), CAMERA_ADJUST_NONE);
    task->step = camera_path_count;
    task->destructor = camera_path_destroy;
#ifdef NONMATCHING
#else
    steps = 0;
    /* @hack spelling: i cast at the test, where a Uint32 i would do */
    for (i = 0; (Uint32)i < CAMERA_PATH_SPANS; i++) {
        steps += camera_path_spans[i].steps;
    }
#endif
    work->timer = steps + 1;
    pad_disable_player(SLOT(work));
}

static void camera_path_destroy(struct task *task)
{
    struct task_work *work = task->work;

    camera_remove_mode(SLOT(work), MODE_INDEX(work));
    pad_enable_player(SLOT(work));
}

static void camera_path_count(struct task *task)
{
    struct task_work *work = task->work;

    if (work->timer != 0) {
        work->timer--;
    } else {
        task_kill(task);
    }
}
