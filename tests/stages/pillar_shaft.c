/* Sand Ocean's pillar where the collision world's pool has no object for it,
 * and where it has one. the init makes the shaft, a child, before it asks */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "stg21/pillar.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

NJS_CNK_MODEL pillar_rubble_0, pillar_rubble_1;
NJS_CNK_MODEL pillar_stone_0, pillar_stone_1, pillar_stone_2;
NJS_OBJECT pillar_fallen_collision[1];
NJS_MOTION pillar_motion[1];
NJS_OBJECT pillar_upright_collision[1];

int main(void)
{
    struct placement placement;
    struct task *pillar;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* without an object, the init destroys the pillar and the shaft with it,
     * and releases the placement */
    pool_left = 0;
    assert(object_start(TASK_WORK, ocean_pillar_init, &placement) == NULL);
    assert(!(placement.flags & PLACEMENT_LIVE));
    assert(heap_live == 0 && world_live == 0);
    /* with one, the shaft steps in the same walk and takes its record */
    pool_left = 1;
    pillar = object_start(TASK_WORK, ocean_pillar_init, &placement);
    assert(pillar != NULL && pillar->child != NULL);
    assert(pillar->child->work->collision != NULL);
    assert(pool_lent == 1 && world_live == 1);
    task_destroy(pillar);
    assert(heap_live == 0 && pool_lent == 0 && world_live == 0);
    return 0;
}
