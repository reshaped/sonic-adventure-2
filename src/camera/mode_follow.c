/* the camera's FOLLOW mode */
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/avoid.h"
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/orbit.h"
#include "camera/stack.h"
#include "system/pad.h"

/* the player the camera looks at, and the player whose pad turns it */
#define FOLLOWED 0
#define TURNING 1

void camera_mode_follow(struct camera_stack *stack, struct camera_slot *slot)
{
    Sint16 x;
    Sint16 y;
    Uint16 right; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Uint16 left; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    camera_player_target(FOLLOWED, &camera_state.target);
    camera_state.ang.z = 0;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
    if (player_pads[TURNING]->on & PDD_DGT_ST) {
        x = CAMERA_STICK_READING(player_pads[TURNING]->x1);
        y = CAMERA_STICK_READING(player_pads[TURNING]->y1);
        if (x > -CAMERA_STICK_DEAD_ZONE && x < CAMERA_STICK_DEAD_ZONE) {
            x = 0;
        }
        if (y > -CAMERA_STICK_DEAD_ZONE && y < CAMERA_STICK_DEAD_ZONE) {
            y = 0;
        }
        /* @hack redundant: the reads of the pad's r and l, which nothing
         * uses */
        right = player_pads[TURNING]->r;
        left = player_pads[TURNING]->l;
        camera_state.ang.y = stack->place.ang.y + ORBIT_TURN(x);
        camera_state.ang.x = stack->place.ang.x + ORBIT_TURN(y);
        if (player_pads[TURNING]->on & PDD_DGT_TX) {
            if (player_pads[TURNING]->on & PDD_DGT_TL) {
                camera_state.dist = stack->place.dist + ORBIT_MOVE_FAST;
            } else if (player_pads[TURNING]->on & PDD_DGT_TR) {
                camera_state.dist = stack->place.dist - ORBIT_MOVE_FAST;
            }
        } else {
            if (player_pads[TURNING]->on & PDD_DGT_TL) {
                camera_state.dist = stack->place.dist + ORBIT_MOVE;
            } else if (player_pads[TURNING]->on & PDD_DGT_TR) {
                camera_state.dist = stack->place.dist - ORBIT_MOVE;
            }
        }
    }
    if (camera_state.dist < ORBIT_DIST_MIN) {
        camera_state.dist = ORBIT_DIST_MIN;
    }
    camera_state.type = CAMERA_TARGET_ANG_DIST;
}
