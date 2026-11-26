/* the camera's LEAVE mode. SHC builds this source unoptimized, as
 * config/compilers.mk routes it */
#include <ninja.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/task.h"

/* the pos and roll of the camera's place at the mode's first step */
struct leave_work {
    NJS_POINT3 pos;
    Angle roll;
};

void camera_mode_leave(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_place *place = &stack->place;
    struct leave_work *work = (struct leave_work *)slot->work;

    if (slot->steps == 0) {
        work->pos = place->pos;
        work->roll = place->ang.z;
    }
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
    camera_state.pos = work->pos;
    camera_state.target = player_work[camera_current_index]->pos;
    camera_state.target.y +=
        player_records[camera_current_index]->consts.target_height;
    camera_state.ang.z = work->roll;
}
