/* City Escape's parked car where collision_init gives its setup no record,
 * and where it gives one */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "stg13/car.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    return 0.0F;
}

Float njScalor(NJS_VECTOR *v)
{
    return 0.0F;
}

int main(void)
{
    struct placement placement;
    struct task *car;
    int fails;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* the init gives the car its setup, which runs at the next step */
    for (fails = FALSE; fails <= TRUE; fails++) {
        collision_fails = fails;
        car = object_start(TASK_WORK, car_init, &placement);
        assert(car != NULL && car->motion.p == NULL);
        task_step_level(OBJECT_LEVEL);
        if (fails) {
            /* without a record, the setup destroys the car and frees its
             * record, and releases the placement */
            assert(task_levels[OBJECT_LEVEL] == NULL);
            assert(!(placement.flags & PLACEMENT_LIVE));
        } else {
            assert(car->motion.p != NULL && car->work->collision != NULL);
            task_destroy(car);
        }
        assert(heap_live == 0);
    }
    return 0;
}
