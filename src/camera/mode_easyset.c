/* the camera's EASYSET mode, and the state it gives each camera */
#include "camera/camera.h"
#include "camera/modes.h"

struct camera_state camera_easyset_states[CAMERAS];

void camera_mode_easyset(struct camera_stack *stack, struct camera_slot *slot)
{
    camera_state = camera_easyset_states[camera_current_index];
}
