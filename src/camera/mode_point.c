/* the camera's POINT mode */
#include <ninja.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "player/player.h"
#include "system/task.h"

void camera_mode_point(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_zone *zone = slot->zone;
    NJS_VECTOR dir;
    NJS_POINT3 point;

    /* @hack spelling: point, a copy of the zone's point set a member at a
     * time, where zone->point read in place would do */
    point.x = zone->point.x;
    point.y = zone->point.y;
    point.z = zone->point.z;
    camera_state.target.x = player_work[camera_current_index]->pos.x;
    camera_state.target.y =
        player_work[camera_current_index]->pos.y +
        player_records[camera_current_index]->consts.target_height +
        zone->param[ZONE_HEIGHT];
    camera_state.target.z = player_work[camera_current_index]->pos.z;
    dir.x = player_work[camera_current_index]->pos.x - point.x;
    dir.y = player_work[camera_current_index]->pos.y - point.y;
    dir.z = player_work[camera_current_index]->pos.z - point.z;
    njUnitVector(&dir);
    camera_state.pos.x =
        camera_state.target.x + (zone->param[ZONE_DIST] * dir.x);
    camera_state.pos.y =
        camera_state.target.y + (zone->param[ZONE_DIST] * dir.y);
    camera_state.pos.z =
        camera_state.target.z + (zone->param[ZONE_DIST] * dir.z);
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 1;
}
