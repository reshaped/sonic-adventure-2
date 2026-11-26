#include <ninja.h>
#include "chao/marker.h"
#include "draw/lines.h"
#include "system/task.h"

#define GROW_STEP 0.3F /* how much the marker's line grows each step */

void garden_height_marker_step(struct task *marker)
{
    struct task_work *work = marker->work;
    work->scl.x += GROW_STEP;
    if (work->scl.x > work->scl.y) {
        task_kill(marker);
    }
}

void garden_height_marker_draw(struct task *marker)
{
    struct task_work *work = marker->work;
    NJS_POINT3 base;
    NJS_POINT3 tip;
    base = work->pos;
    tip.x = work->pos.x;
    tip.y = work->pos.y + work->scl.x;
    tip.z = work->pos.z;
    line_draw_points(&base, &tip);
}

void garden_height_marker_destroy(struct task *marker)
{
}
