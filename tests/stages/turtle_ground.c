/* Dry Lagoon's swimming turtle where the collision world's pool has no object
 * for its ground, and where it has one */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "camera/camset.h"
#include "camera/zones.h"
#include "object/placement.h"
#include "stg18/turtle.h"
#include "stg18/turtle_art.h"
#include "system/matrix.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

NJS_CNK_OBJECT turtle_eyes[1];
NJS_OBJECT turtle_ground[1];
NJS_CNK_MODEL turtle_eye_model_0, turtle_eye_model_1, turtle_eye_model_2,
    turtle_eye_model_3, turtle_eye_model_4;
NJS_MOTION turtle_swim_motion, turtle_stop_motion, turtle_wait_motion;
NJS_POINT3 turtle_path_points[141];

static struct camera_zone zone;
static Bool zone_taken;

struct camera_zone *camset_add_marked(const struct zone_shape *shape)
{
    assert(!zone_taken);
    zone_taken = TRUE;
    return &zone;
}

/* take back the zone camset_add_marked gave. any other fails the test, as
 * the heap's bytes in a record whose zone the init hasn't set would */
void camset_free_zone(struct camera_zone *zone_freed)
{
    assert(zone_freed == &zone && zone_taken);
    zone_taken = FALSE;
}

void matrix_get(NJS_MATRIX *matrix)
{
}

int main(void)
{
    struct placement placement;
    struct task *task;

    memset(&placement, 0, sizeof(placement));
    task_init();
    /* without a ground, the init destroys the swimmer before it takes a
     * zone, and frees its record */
    pool_left = 0;
    assert(object_start(TASK_WORK | TASK_FORCE, turtle_init, &placement) ==
           NULL);
    assert(!(placement.flags & PLACEMENT_LIVE));
    assert(heap_live == 0 && !zone_taken && world_live == 0);
    /* with one, the swimmer holds its ground and a zone until its destroy */
    pool_left = 1;
    task = object_start(TASK_WORK | TASK_FORCE, turtle_init, &placement);
    assert(task != NULL && zone_taken && world_live == 1 && pool_lent == 1);
    task_destroy(task);
    assert(!(placement.flags & PLACEMENT_LIVE));
    assert(heap_live == 0 && !zone_taken && world_live == 0 && pool_lent == 0);
    return 0;
}
