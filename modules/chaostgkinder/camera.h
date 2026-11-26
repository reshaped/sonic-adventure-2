#ifndef CHAOSTGKINDER_CAMERA_H
#define CHAOSTGKINDER_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/stack.h"

/* where the camera starts as rooms.c loads a room, from its entrance. view
 * is the entrance's angle of view. pending's low bit marks a start not yet
 * taken. the next step of kinder_room_camera or kinder_house_camera takes
 * its pos, pitch and yaw, then clears pending */
struct kinder_camera_start {
    Sint32 pending;
    Angle view;
    NJS_POINT3 pos;
    Angle pitch, yaw;
};
extern struct kinder_camera_start kinder_camera_start;
/* after the start, kinder_room_camera leaves the camera as it is, and
 * kinder_house_camera follows player 0 along z */
camera_step kinder_room_camera;
camera_step kinder_house_camera;

#endif
