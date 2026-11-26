/* the camera's HALF adjust */
#include <ninja.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "system/angle.h"

void camera_adjust_half(struct camera_stack *stack, struct camera_slot *adjust)
{
    camera_state.pos.x = (camera_state.pos.x + stack->place.pos.x) * 0.5F;
    camera_state.pos.y = (camera_state.pos.y + stack->place.pos.y) * 0.5F;
    camera_state.pos.z = (camera_state.pos.z + stack->place.pos.z) * 0.5F;
    /* @bug logic: turns ang.x and ang.y by the whole of their distance from
     * the place's, where pos goes half the way. each angle comes out as the
     * mode left it, cut to its low 16 bits. the JSRs to angle_approach at
     * 0x8c0b8b94 and 0x8c0b8bb0 pass angle_distance's result as the step */
    camera_state.ang.x =
        angle_approach(stack->place.ang.x, camera_state.ang.x,
                       angle_distance(camera_state.ang.x, stack->place.ang.x));
    camera_state.ang.y =
        angle_approach(stack->place.ang.y, camera_state.ang.y,
                       angle_distance(camera_state.ang.y, stack->place.ang.y));
    camera_state.type = CAMERA_POS_ANG_DIST;
}
