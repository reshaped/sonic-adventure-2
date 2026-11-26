/* an Angle's turns and units, the angles about three axes, and turns from one
 * angle toward another */
#ifndef SYSTEM_ANGLE_H
#define SYSTEM_ANGLE_H
#include <ninja.h>
#include <sg_xpt.h>

/* a turn and its parts, in Angle units */
#define FULL_TURN 0x10000
#define HALF_TURN 0x8000
#define QUARTER_TURN 0x4000
#define EIGHTH_TURN 0x2000
/* a third of a turn and a sixth, each rounded down */
#define THIRD_TURN 0x5555
#define SIXTH_TURN 0x2aaa
/* the bits of an Angle that hold it within one turn */
#define ANGLE_MASK (FULL_TURN - 1)

/* the Angle units in a radian and in a degree, and the degrees in a unit */
#define ANGLE_PER_RADIAN (FULL_TURN / (2 * NJD_PI))
#define ANGLE_PER_DEGREE (FULL_TURN / 360.0F)
#define DEGREES_PER_ANGLE (360.0F / FULL_TURN)
/* the Angle units in a turn, as a Float */
#define ANGLE_PER_TURN ((Float)FULL_TURN)
/* the Angle units in a radian, as a Float. ANGLE_PER_RADIAN is a double, so a
 * product with it is one */
#define ANGLE_PER_RADIAN_F ((Float)ANGLE_PER_RADIAN)
/* n degrees, and n radians, as an Angle, truncated toward 0 */
#define DEGREES(n) ((Angle)((n) * ANGLE_PER_DEGREE))
#define RADIANS(n) ((Angle)((n) * ANGLE_PER_RADIAN))

/* njRotateEx's lv for a turn about x, then y, then z, and for one about z,
 * then x, then y */
#define XYZ_TURN 0
#define ZXY_TURN 1

/* an angle about each axis */
struct angles {
    Angle x, y, z;
};

/* turn from by step toward to, the short way round. the result is from 0 to
 * 0xffff, and is to where to lies within step of from */
Angle angle_approach(Angle from, Angle to, Angle step);
/* the short turn from from to to, from -0x8000 to 0x7fff */
Angle angle_difference(Angle from, Angle to);
/* the size of the short turn between from and to, from 0 to 0x8000 */
Angle angle_distance(Angle from, Angle to);
/* to, moved by whole turns. where the short turn from from to to is positive,
 * the result is at or above from. where it is negative, the result is at or
 * below from. where it is 0, the result is from */
Angle angle_unwrap(Angle from, Angle to);
/* the angle part / (part + rest) of the short way from from to to. part +
 * rest must not be 0 */
Angle angle_interpolate(Angle from, Angle to, Float part, Float rest);

#endif
