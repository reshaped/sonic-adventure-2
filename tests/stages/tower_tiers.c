/* Crazy Gadget's base tower where the heap has no room for its tier table */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "stg23/tower.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

int main(void)
{
    struct placement placement;
    struct task *task;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* the table is the first allocation of the init's step */
    heap_fail_at = 1;
    assert(object_start(TASK_WORK, base_tower_init, &placement) == NULL);
    assert(!(placement.flags & PLACEMENT_LIVE) && heap_live == 0);
    task = object_start(TASK_WORK, base_tower_init, &placement);
    assert(task != NULL && task->extra.p != NULL);
    task_destroy(task);
    assert(heap_live == 0);
    return 0;
}
