/* the camera's INIT mode */
#include <ninja.h>

#include "camera/camera.h"
#include "camera/helpers.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/task.h"

/* the camera stands BACK from the player's pos, against the player's x axis
 * as turn_x_axis turns it. it stands UP higher than that */
#define BACK 60.0F
#define UP 30.0F
/* the mode stays while slot->steps is at most LAST_STAY, and its next step
 * takes it off */
#define LAST_STAY 5

void camera_mode_init(struct camera_stack *stack, struct camera_slot *slot)
{
    NJS_VECTOR dir;

    if (player_work[camera_current_index] == NULL) {
        return;
    }
    camera_set_adjust(camera_current_index, CAMERA_INIT_SLOT,
                      CAMERA_ADJUST_NONE);
    camera_state.target = player_work[camera_current_index]->pos;
    camera_state.target.y +=
        player_records[camera_current_index]->consts.target_height;
    turn_x_axis(&player_work[camera_current_index]->ang, &dir);
    camera_state.pos.x =
        player_work[camera_current_index]->pos.x - (dir.x * BACK);
    camera_state.pos.y =
        player_work[camera_current_index]->pos.y - (dir.y * BACK);
    camera_state.pos.z =
        player_work[camera_current_index]->pos.z - (dir.z * BACK);
    camera_state.pos.y += UP;
    if (slot->steps > LAST_STAY) {
        camera_trail_fill(camera_current_index);
        camera_remove_mode(camera_current_index, CAMERA_INIT_SLOT);
        camera_set_adjust(camera_current_index,
                          camera_top(camera_current_index), CAMERA_ADJUST_NONE);
    }
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
}
