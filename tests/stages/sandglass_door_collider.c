/* Death Chamber's sandglass door where the collision world's pool has no
 * object for it, and where it has one. the init makes the collider, a child,
 * before it asks */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "stg25/chamber_util.h"
#include "stg25/sandglass_door.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

void chamber_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = 0.0F;
    *z = radius;
}

int main(void)
{
    struct placement placement;
    struct task *door;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* without an object, the init destroys the door and the collider with
     * it, and releases the placement */
    pool_left = 0;
    assert(object_start(TASK_WORK | TASK_MOTION | TASK_FORCE,
                        chamber_sandglass_door_init, &placement) == NULL);
    assert(!(placement.flags & PLACEMENT_LIVE));
    assert(heap_live == 0 && world_live == 0);
    /* with one, the collider holds the door's collision record */
    pool_left = 1;
    door = object_start(TASK_WORK | TASK_MOTION | TASK_FORCE,
                        chamber_sandglass_door_init, &placement);
    assert(door != NULL && door->child != NULL);
    assert(door->child->work->collision != NULL);
    assert(pool_lent == 1 && world_live == 1);
    task_destroy(door);
    assert(heap_live == 0 && pool_lent == 0 && world_live == 0);
    return 0;
}
