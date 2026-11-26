/* Pumpkin Hill's shed, whose missile target must leave the table with the
 * shed, whether or not collision_init gave it a record */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "shared/missile.h"
#include "stg05/shed.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

/* the one entry of the missiles' target table */
static struct missile_target target = {-1, NULL};

struct missile_target *missile_target_add(Sint32 id, NJS_POINT3 **hit)
{
    assert(target.hit == NULL);
    target.id = id;
    target.hit = hit;
    *hit = NULL;
    return &target;
}

struct missile_target *missile_target_remove(NJS_POINT3 **hit)
{
    if (target.hit != hit) {
        return NULL;
    }
    target.id = -1;
    target.hit = NULL;
    *hit = NULL;
    return &target;
}

int main(void)
{
    struct placement placement;
    struct task *task;
    int fails;

    memset(&placement, 0, sizeof(placement));
    task_init();
    for (fails = FALSE; fails <= TRUE; fails++) {
        collision_fails = fails;
        task = object_start(TASK_WORK, shed_init, &placement);
        assert(task != NULL && target.hit != NULL);
        assert((task->work->collision == NULL) == fails);
        task_destroy(task);
        assert(target.hit == NULL && heap_live == 0);
    }
    return 0;
}
