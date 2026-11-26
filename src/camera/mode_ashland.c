/* the camera's ASHLAND mode */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"

/* a step sets a zone's angle of view below VIEW_LEAST degrees to
 * VIEW_DEFAULT, in the zone itself */
#define VIEW_LEAST 10
#define VIEW_DEFAULT 70
/* a zone's turn below TURN_LEAST degrees gives way to the default turn,
 * CAMERA_VIEW_TURN_USE_DEFAULT */
#define TURN_LEAST 1

void camera_mode_ashland(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_zone *zone = slot->zone;

    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.pos = zone->camera_pos;
    camera_state.target = player_work[camera_current_index]->pos;
    camera_state.target.y +=
        player_records[camera_current_index]->consts.target_height;
    camera_state.ang.z = zone->camera_ang.z;
    camera_state.trigger_push = 0;
    if (zone->view[ZONE_VIEW_ANGLE] < VIEW_LEAST) {
        zone->view[ZONE_VIEW_ANGLE] = VIEW_DEFAULT;
    }
    if (zone->view[ZONE_VIEW_TURN] < TURN_LEAST) {
        camera_set_view_angle(
            camera_current_index,
            (Angle)((Float)zone->view[ZONE_VIEW_ANGLE] * ANGLE_PER_DEGREE),
            CAMERA_VIEW_TURN_USE_DEFAULT);
    } else {
        camera_set_view_angle(
            camera_current_index,
            (Angle)((Float)zone->view[ZONE_VIEW_ANGLE] * ANGLE_PER_DEGREE),
            (Angle)((Float)zone->view[ZONE_VIEW_TURN] * ANGLE_PER_DEGREE));
    }
}
