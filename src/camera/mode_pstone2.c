/* the camera's PSTONE2 mode */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/task.h"
#include "system/vector.h"

/* a step takes the mode off the stack once the camera stands farther than FAR
 * from the point it looks at */
#define FAR 150.0F

void camera_mode_pstone2(struct camera_stack *stack, struct camera_slot *slot)
{
    NJS_VECTOR *dir = (NJS_VECTOR *)slot->work;
    NJS_VECTOR apart;
    NJS_VECTOR to_target;
    Float dist;
    struct camera_state *state;

    if (slot->steps == 0) {
        *dir = camera_views[camera_current_index]->dir;
        /* @hack redundant: a call for dir's length, which nothing takes */
        njScalor(dir);
    }
    vector_subtract(&player_work[0]->pos, &player_work[1]->pos, &apart);
    /* @hack spelling: state, a pointer to camera_state taken here, where
     * camera_state written in place would do */
    state = &camera_state;
    vector_add(&player_work[0]->pos, &player_work[1]->pos, &state->target);
    state->target.x *= 0.5F;
    state->target.y *= 0.5F;
    state->target.z *= 0.5F;
    dist = njScalor(&apart);
    state->pos.x = dir->x * dist;
    state->pos.y = dir->y * dist;
    state->pos.z = dir->z * dist;
    vector_add(&state->target, &state->pos, &state->pos);
    state->type = CAMERA_POS_TARGET;
    state->unk_04 = 0;
    state->trigger_push = 0;
    vector_subtract(&state->target, &state->pos, &to_target);
    if (njScalor2(&to_target) > FAR * FAR) {
        camera_remove_mode(camera_current_index,
                           camera_top(camera_current_index));
        camera_set_adjust(camera_current_index,
                          camera_top(camera_current_index),
                          CAMERA_ADJUST_RELATIVE1);
    }
}
