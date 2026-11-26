/* Dry Lagoon's pool where the collision world's pool has no object for the
 * water, and where it has one */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "stg18/lagoon_util.h"
#include "stg18/pool.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

Sint32 lagoon_size_class_flags(Float size)
{
    return 0;
}

int main(void)
{
    struct placement placement;
    struct task *pool;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* the child the pool makes steps in the same walk. without an object, it
     * destroys the pool and itself, and releases the placement */
    pool_left = 0;
    assert(object_start(TASK_WORK, pool_init, &placement) == NULL);
    assert(!(placement.flags & PLACEMENT_LIVE));
    assert(heap_live == 0 && world_live == 0);
    pool_left = 1;
    pool = object_start(TASK_WORK, pool_init, &placement);
    assert(pool != NULL && pool->child != NULL);
    assert(pool_lent == 1 && world_live == 1);
    task_destroy(pool);
    assert(heap_live == 0 && pool_lent == 0 && world_live == 0);
    return 0;
}
