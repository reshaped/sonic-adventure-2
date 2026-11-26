/* the camera's SS mode: the camera looks at a point between two that the
 * caller keeps, and closes in on a place beyond it */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/ss.h"
#include "camera/stack.h"
#include "system/vector.h"

/* the camera stands back from the target along a unit line. RISE is that line's
 * y before it is made a unit again. it keeps the camera above the target */
#define RISE 0.5F
/* the camera moves CLOSE of the way to its goal in a step */
#define CLOSE 0.2F

/* each camera's SS work, as camera_ss_start last set it */
static struct camera_ss *ss_works[CAMERAS];

int camera_ss_start(int camera, struct camera_ss *param)
{
    struct camera_slot *slot;

    slot = camera_push_mode(camera, CAMERA_MODE_SS);
    if (slot == NULL) {
        return 0;
    }
    camera_set_adjust(camera, camera_top(camera), param->adjust);
    ss_works[camera] = (struct camera_ss *)slot->work;
    camera_ss_set(camera, param);
    return 1;
}

void camera_ss_set(int camera, struct camera_ss *param)
{
    *ss_works[camera] = *param;
}

void camera_mode_ss(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_ss *work = (struct camera_ss *)slot->work;
    /* @hack spelling: ends, a copy of from and to read through an array,
     * where work's from and to read in place would do */
    NJS_POINT3 *ends[2];
    NJS_VECTOR back;
    NJS_VECTOR span;
    /* @hack redundant: spare, a local that only sizes the frame */
    NJS_VECTOR spare; /* NOLINT(clang-diagnostic-unused-variable) */
    Float dist;
    /* @hack spelling: close, a local for CLOSE, where CLOSE in place would
     * do */
    Float close;

    ends[0] = work->from;
    ends[1] = work->to;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
    camera_state.target.x =
        ((ends[1]->x - ends[0]->x) * work->part) + ends[0]->x;
    camera_state.target.y =
        ((ends[1]->y - ends[0]->y) * work->part) + ends[0]->y;
    camera_state.target.z =
        ((ends[1]->z - ends[0]->z) * work->part) + ends[0]->z;
    back.x = camera_views[camera_current_index]->pos.x -
             camera_views[camera_current_index]->target.x;
    back.y = camera_views[camera_current_index]->pos.y -
             camera_views[camera_current_index]->target.y;
    back.z = camera_views[camera_current_index]->pos.z -
             camera_views[camera_current_index]->target.z;
    vector_subtract(ends[0], ends[1], &span);
    /* @hack redundant: the length times 1 */
    dist = njScalor(&span) * 1.0F;
    njUnitVector(&back);
    back.y = RISE;
    njUnitVector(&back);
    camera_state.pos.x = camera_state.target.x + (dist * back.x);
    camera_state.pos.y = camera_state.target.y + (dist * back.y);
    camera_state.pos.z = camera_state.target.z + (dist * back.z);
    close = CLOSE;
    camera_state.pos.x =
        ((camera_state.pos.x - camera_views[camera_current_index]->pos.x) *
         close) +
        camera_views[camera_current_index]->pos.x;
    camera_state.pos.y =
        ((camera_state.pos.y - camera_views[camera_current_index]->pos.y) *
         close) +
        camera_views[camera_current_index]->pos.y;
    camera_state.pos.z =
        ((camera_state.pos.z - camera_views[camera_current_index]->pos.z) *
         close) +
        camera_views[camera_current_index]->pos.z;
    camera_state.ang.z = 0;
}
