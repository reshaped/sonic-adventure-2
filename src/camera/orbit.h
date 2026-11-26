/* the pad's orbit of the camera about a point, in the modes EDITOR, FOLLOW
 * and SNAPSHOT. while the pad holds Start, its stick turns the camera. L then
 * moves the camera out, and R in */
#ifndef CAMERA_ORBIT_H
#define CAMERA_ORBIT_H

/* the turn a step takes for a CAMERA_STICK_READING, the stick's axis times 256.
 * that is 4 Angle units for each unit the axis leans */
#define ORBIT_TURN(stick) ((stick) / 64)
/* how far a step moves the camera out or in. FOLLOW and EDITOR move it
 * ORBIT_MOVE_FAST while X is held */
#define ORBIT_MOVE 2.0F
#define ORBIT_MOVE_FAST 10.0F
/* the modes keep the camera at least ORBIT_DIST_MIN from the point */
#define ORBIT_DIST_MIN 5.0F

#endif
