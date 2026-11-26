/* the camera's FIX mode */
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "camera/zones.h"

void camera_mode_fix(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_zone *zone = slot->zone;

    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
    camera_state.pos = zone->camera_pos;
    camera_state.target = zone->point;
    camera_state.ang.z = zone->camera_ang.z;
}
