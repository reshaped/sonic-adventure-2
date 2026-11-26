/* the camera's EDITOR2 mode */
#include "camera/camera.h"
#include "camera/editor.h"
#include "camera/modes.h"

void camera_mode_editor2(struct camera_stack *stack, struct camera_slot *slot)
{
    camera_state.pos = camera_editor.camera_pos;
    camera_state.target = camera_editor.cursor;
    camera_state.ang.z = camera_editor.roll;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
}
