/* Radical Highway's spinning hammer where the collision world's pool has no
 * object for its drum, none for its bar, and both */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "stg14/hammer.h"
#include "stg14/spinning_hammer.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

Uint32 highway_hammer_size_class(Float radius)
{
    return 0;
}

int main(void)
{
    struct placement placement;
    struct task *task;
    int lent;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* with no drum, or a drum and no bar, the init destroys the hammer, and
     * the drum leaves the world and goes back to the pool */
    for (lent = 0; lent < 2; lent++) {
        pool_left = lent;
        assert(object_start(TASK_WORK | TASK_FORCE, spinning_hammer_init,
                            &placement) == NULL);
        assert(!(placement.flags & PLACEMENT_LIVE));
        assert(heap_live == 0 && pool_lent == 0 && world_live == 0);
    }
    pool_left = 2;
    task =
        object_start(TASK_WORK | TASK_FORCE, spinning_hammer_init, &placement);
    assert(task != NULL && pool_lent == 2 && world_live == 2);
    task_destroy(task);
    assert(heap_live == 0 && pool_lent == 0 && world_live == 0);
    return 0;
}
