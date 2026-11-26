/* the camera's KLAMATH mode */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "player/player.h"
#include "system/task.h"

struct klamath_work {
    Float height; /* how high the camera stands above the player */
    /* @hack redundant: drop, which nothing sets but to 0, cut by a tenth and
     * taken off pos.y each step */
    Float drop;
};

void camera_mode_klamath(struct camera_stack *stack, struct camera_slot *slot)
{
    struct klamath_work *work = (struct klamath_work *)slot->work;
    struct camera_zone *zone = slot->zone;
    NJS_VECTOR dir;
    NJS_POINT3 center;
    Float length;

    if (slot->steps == 0) {
        if (zone->param[ZONE_HEIGHT] != 0.0F) {
            work->height = zone->param[ZONE_HEIGHT];
        } else {
            work->height = zone->param[ZONE_DIST] * 0.5F;
        }
        work->drop = 0.0F;
    }
    /* @hack spelling: center, a copy of the x and z of the zone's point set
     * a member at a time, where zone->point read in place would do */
    center.x = zone->point.x;
    /* @hack redundant: center's y, which nothing reads */
    center.y = player_work[0]->pos.y;
    center.z = zone->point.z;
    /* @hack spelling: target set a member at a time, where one copy of the
     * player's pos would do */
    camera_state.target.x = player_work[camera_current_index]->pos.x;
    camera_state.target.y = player_work[camera_current_index]->pos.y;
    camera_state.target.z = player_work[camera_current_index]->pos.z;
    dir.x = player_work[camera_current_index]->pos.x - center.x;
    dir.y = 0.0F;
    dir.z = player_work[camera_current_index]->pos.z - center.z;
    njUnitVector(&dir);
    camera_state.pos.x =
        camera_state.target.x + (zone->param[ZONE_DIST] * dir.x);
    camera_state.pos.y = work->height + camera_state.target.y;
    camera_state.pos.z =
        camera_state.target.z + (zone->param[ZONE_DIST] * dir.z);
    work->drop *= 0.9F;
    camera_state.pos.y -= work->drop;
    dir.x = camera_state.pos.x - center.x;
    dir.y = 0.0F;
    dir.z = camera_state.pos.z - center.z;
    length = njUnitVector(&dir);
    camera_state.target.y +=
        work->height * (length - zone->param[ZONE_DIST]) / length;
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 1;
}
