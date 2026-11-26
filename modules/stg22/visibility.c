#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "stg22/crazy_gadget.h"

static Bool inside_room(NJS_POINT3 *point)
{
    return point->x < -8546.0F && point->x > -10410.0F && point->y < -475.0F &&
           point->y > -1442.0F && point->z > -3517.0F && point->z < -2292.0F;
}

Bool gadget_visible_from_camera(NJS_POINT3 *point)
{
    return inside_room(point) ==
           inside_room(&camera_views[camera_current_index]->pos);
}
