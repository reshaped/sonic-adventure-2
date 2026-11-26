/* cart_finish, which sets cart_finished through a task that ends at its
 * first step */
#include "cart/race.h"
#include "system/task.h"

static void goal_step(struct task *task);
static void goal_draw(struct task *task);
static void goal_destroy(struct task *task);

void cart_finish(void)
{
    struct task *task;
    if (!cart_finished) {
        task = task_create(TASK_WORK, 2, goal_step, "goalExec");
        task->draw = goal_draw;
        task->destructor = goal_destroy;
        cart_finished = 1;
    }
}
static void goal_step(struct task *task)
{
    task_destroy(task);
}
static void goal_draw(struct task *task)
{
}
static void goal_destroy(struct task *task)
{
}
