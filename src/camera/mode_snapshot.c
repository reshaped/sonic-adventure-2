/* the camera's SNAPSHOT mode */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/helpers.h"
#include "camera/modes.h"
#include "camera/orbit.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/pad.h"
#include "system/task.h"

/* how far a step of the pad moves the point */
#define POINT_SPEED 1.0F

void camera_mode_snapshot(struct camera_stack *stack, struct camera_slot *slot)
{
    NJS_POINT3 *point = (NJS_POINT3 *)slot->work;
    Sint16 x;
    Sint16 y;
    Uint16 right; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Uint16 left; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    if (slot->steps == 0) {
        *point = player_work[camera_current_index]->pos;
    } else {
        point_move_by_pad(point, POINT_SPEED, camera_current_index, FALSE);
    }
    camera_state.target = *point;
    camera_state.ang.z = 0;
    if (player_pads[camera_current_index]->on & PDD_DGT_ST) {
        x = CAMERA_STICK_READING(player_pads[camera_current_index]->x1);
        y = CAMERA_STICK_READING(player_pads[camera_current_index]->y1);
        if (x > -CAMERA_STICK_DEAD_ZONE && x < CAMERA_STICK_DEAD_ZONE) {
            x = 0;
        }
        if (y > -CAMERA_STICK_DEAD_ZONE && y < CAMERA_STICK_DEAD_ZONE) {
            y = 0;
        }
        /* @hack redundant: the reads of the pad's r and l, which nothing
         * uses */
        right = player_pads[camera_current_index]->r;
        left = player_pads[camera_current_index]->l;
        camera_state.ang.y = stack->place.ang.y + ORBIT_TURN(x);
        camera_state.ang.x = stack->place.ang.x + ORBIT_TURN(y);
        if (player_pads[camera_current_index]->on & PDD_DGT_TL) {
            camera_state.dist = stack->place.dist + ORBIT_MOVE;
        } else if (player_pads[camera_current_index]->on & PDD_DGT_TR) {
            camera_state.dist = stack->place.dist - ORBIT_MOVE;
        }
    }
    if (camera_state.dist < ORBIT_DIST_MIN) {
        camera_state.dist = ORBIT_DIST_MIN;
    }
    camera_state.type = CAMERA_TARGET_ANG_DIST;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
}
