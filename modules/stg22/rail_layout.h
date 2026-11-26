#ifndef STG22_RAIL_LAYOUT_H
#define STG22_RAIL_LAYOUT_H
#include <ninja.h>
#include "stage/path.h"
#include <sg_xpt.h>
#include "collision/collision.h"

/* a transport rail. its rider follows ride, and its markers lie along
 * markers, marker_spacing apart. rail.c places no markers where
 * marker_spacing is below 1 */
struct gadget_rail {
    struct path *ride;
    struct path *markers;
    Float marker_spacing;
    Angle intake_yaw;
    Angle exit_yaw;
    NJS_POINT3 intake_position;
};
extern struct gadget_rail gadget_rails[3];
extern struct collision_shape rail_exit_shape[1];
extern struct collision_shape rail_intake_shapes[3];
extern Float rail_speed_limit;
extern Sint32 rail_reset_requested;
extern Angle rail_marker_roll;
extern Float rail_exit_speed;
extern Angle rail_exit_open_rate;
extern Angle rail_exit_close_rate;
extern Angle rail_marker_spin;

#define RAIL_EXIT_IDLE 0
#define RAIL_EXIT_OPENING 3
#define RAIL_EXIT_CLOSING 4
#endif
