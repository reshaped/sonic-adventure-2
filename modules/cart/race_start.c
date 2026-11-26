/* cart_race, and cart_release_start, whose task clears cart_clock_held as it
 * ends */
#include "cart/race.h"
#include "system/task.h"

struct cart_race cart_race;

static void start_step(struct task *task);
static void start_draw(struct task *task);
static void start_destroy(struct task *task);

void cart_release_start(void)
{
    struct task *task;
    task = task_create(TASK_WORK, 2, start_step, "startExec");
    task->draw = start_draw;
    task->destructor = start_destroy;
}
static void start_step(struct task *task)
{
    task_destroy(task);
}
static void start_draw(struct task *task)
{
}
static void start_destroy(struct task *task)
{
    cart_clock_held = 0;
}
