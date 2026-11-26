#ifndef STG22_RAIL_SENSOR_H
#define STG22_RAIL_SENSOR_H
#include <ninja.h>
#include <sg_xpt.h>
/* a rail end's extra. object is the end's own copy of its collision object,
 * at the task's pos and ang. center is that model's center in world space.
 * each end's copy collides only while a player is near center. the exit's
 * also needs the exit closed and settled, and the intake's the intake open */
struct gadget_rail_sensor {
    NJS_OBJECT object;
    NJS_POINT3 center;
    Uint8 reserved[8];
};
#define RAIL_SENSOR(task) ((struct gadget_rail_sensor *)(task)->extra.p)
#endif
