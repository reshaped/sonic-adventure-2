/* Death Chamber's lion head where body_init finds no room for its body */
#include <assert.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "object/placement.h"
#include "shared/common_object.h"
#include "stg25/lion_head.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

static Bool body_fails;

void body_init(struct task *task)
{
    if (!body_fails) {
        task->force.p = syCalloc(1, sizeof(struct body));
    }
}

void body_destroy(struct task *task)
{
    syFree(task->force.p);
    task->force.p = NULL;
}

int main(void)
{
    struct placement placement;
    struct task *head;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* a head without a body ends at its next step, before body_step */
    body_fails = TRUE;
    head = object_start(TASK_WORK | TASK_MOTION, chamber_lion_head_init,
                        &placement);
    assert(head != NULL && head->step == task_destroy);
    task_step_level(OBJECT_LEVEL);
    assert(task_levels[OBJECT_LEVEL] == NULL);
    assert(!(placement.flags & PLACEMENT_LIVE) && heap_live == 0);
    body_fails = FALSE;
    head = object_start(TASK_WORK | TASK_MOTION, chamber_lion_head_init,
                        &placement);
    assert(head != NULL && head->step != task_destroy);
    task_destroy(head);
    assert(heap_live == 0);
    return 0;
}
