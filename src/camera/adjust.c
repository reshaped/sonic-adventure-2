/* each camera's step of the adjust paired with the mode on top of its
 * stack */
#include "camera/stack.h"

void camera_run_adjust(int camera)
{
    struct camera_slot *adjust =
        &camera_stacks[camera].adjusts[camera_stacks[camera].top];

    if (adjust->step != NULL) {
        adjust->step(&camera_stacks[camera], adjust);
        adjust->steps++;
    }
}
