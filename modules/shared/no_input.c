/* the input stop, NOINPCOL in a stage's object list. it is a sphere that takes
 * the jump and action buttons from each player inside it */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/no_input.h"
#include "system/pad.h"
#include "system/task.h"

/* NoconTask is the task the stops share. its marks hold a bit for each player
 * a stop found inside it since NoconTask's last step */
#define MARKS(task) ((task)->unk_3c.l)
#define MARK_0 0x1
#define MARK_1 0x2
/* NoconTask's level, which steps before the stops' */
#define NO_INPUT_LEVEL 0
#define RADIUS(work) ((work)->scl.x)

static void no_input_destroy(struct task *task);
static void no_input_step(struct task *task);
static void no_input_draw(struct task *task);

/* NoconTask while it runs, or null */
static struct task *no_input_task = NULL;
/* the stops started, less those destroyed */
static Sint32 no_input_stops = 0;

/* NoconTask's step */
static void no_input_clear(struct task *task)
{
    if (MARKS(task) & MARK_0) {
        pad_jump_held[0] = 0;
        pad_jump_off[0] = 0;
        pad_action_held[0] = 0;
        pad_action_off[0] = 0;
        pad_jump_pressed[0] = 0;
        pad_jump_released[0] = 0;
        pad_action_pressed[0] = 0;
        pad_action_released[0] = 0;
    }
    if (MARKS(task) & MARK_1) {
        pad_jump_held[1] = 0;
        pad_jump_off[1] = 0;
        pad_action_held[1] = 0;
        pad_action_off[1] = 0;
        pad_jump_pressed[1] = 0;
        pad_jump_released[1] = 0;
        pad_action_pressed[1] = 0;
        pad_action_released[1] = 0;
    }
    MARKS(task) = 0;
}

static void no_input_task_destroy(struct task *task)
{
    if (task == no_input_task) {
        no_input_task = NULL;
    }
}

void no_input_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = no_input_draw;
    task->step = no_input_step;
    task->destructor = no_input_destroy;
    no_input_stops++;
    if (no_input_task == NULL) {
        no_input_task =
            task_create(0, NO_INPUT_LEVEL, no_input_clear, "NoconTask");
        if (no_input_task != NULL) {
            no_input_task->destructor = no_input_task_destroy;
            MARKS(no_input_task) = 0;
        }
    }
}

static void no_input_destroy(struct task *task)
{
    no_input_stops--;
    /* @bug logic: leaves NoconTask running after the last stop's destroy.
     * each destroy follows its stop's start, so the count falls only to 0,
     * and the test waits for a count below 0 (CMP/PZ at 0xc4 of the object's
     * code) */
    if (no_input_task != NULL && no_input_stops < 0) {
        task_destroy(no_input_task);
        no_input_task = NULL;
    }
}

static void no_input_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos_0;
    NJS_POINT3 pos_1;
    int marks;

    if (placement_try_leave(task)) {
        return;
    }
    marks = 0;
    if (player_get_past(0, 0, &pos_0, NULL) &&
        njDistanceP2P(&pos_0, &work->pos) < RADIUS(work)) {
        marks = MARK_0;
    }
    if (player_get_past(1, 0, &pos_1, NULL) &&
        njDistanceP2P(&pos_1, &work->pos) < RADIUS(work)) {
        marks |= MARK_1;
    }
    if (no_input_task != NULL) {
        MARKS(no_input_task) |= marks;
    }
}

static void no_input_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    /* @hack redundant: a return, and a body behind it that no path reaches,
     * where an empty function would do. the body's string still lands in this
     * source's constants. the image holds none of the body's code, which only
     * sizes the function's stack frame */
    if (1) {
        return;
    }
    (void)"<- COL SCL";
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    scale.x = scale.y = scale.z = RADIUS(work);
    njScaleV(NULL, &scale);
    njPopMatrixEx();
}
