/* the torch light, TAIMATSU in a stage's object list: a flickering light on
 * the players near it. it draws nothing */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/light.h"
#include "object/set.h"
#include "shared/object_light.h"
#include "shared/torch_light.h"
#include "system/game_state.h"
#include "system/task.h"

/* the light, orange. object_light_add_near aims each copy it makes from the
 * torch toward its player */
/* @hack type: the light an array of one, where a single record would do */
static struct light torch_light[1] = {
    {{0.3F, 0.1F, 1.0F}, 100.0F, 0.2F, 1.0F, 0.75F, 0.25F},
};
static Float dat_torch_light_0 = 0.25F;
static Float dat_torch_light_1 = 0.333F;
static Float dat_torch_light_2 = 0.125F;
static Float dat_torch_light_3 = 0.1F;

static void torch_light_destroy(struct task *task);
static void torch_light_step(struct task *task);
static void torch_light_draw(struct task *task);

void torch_light_init(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: work read through its own address, where work would
     * do */
    Float range = (*&work)->scl.x;

    if (!placement_try_leave_by(task, range * range)) {
        task->draw = torch_light_draw;
        task->destructor = torch_light_destroy;
        task->step = torch_light_step;
    }
}

static void torch_light_destroy(struct task *task)
{
}

static void torch_light_step(struct task *task)
{
    struct task_work *work = task->work;
    Float range = work->scl.x;
    struct light *light;
    int i;

    if (!placement_try_leave_by(task, range * range)) {
        for (i = 0; i < GAME_PLAYERS; i++) {
            /* @hack spelling: task->work read again, where work would do */
            light = object_light_add_near(torch_light, i, &work->pos,
                                          task->work->scl.x);
            if (light != NULL) {
                object_light_jitter(light);
            }
        }
    }
}

static void torch_light_draw(struct task *task)
{
}
