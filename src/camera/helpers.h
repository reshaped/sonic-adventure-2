/* the camera's helpers: angles from a direction and directions from angles,
 * as the camera looks, and a point moved by a pad */
#ifndef CAMERA_HELPERS_H
#define CAMERA_HELPERS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* the angles that turn an axis toward dir, which needn't be a unit vector.
 * the first turn comes out within a quarter turn of 0, and the second within
 * half a turn. look_angles turns -z, the way Ninja's camera looks, about x
 * by *ax and then about y by *ay. z_axis_angles turns the z axis about y by
 * *ay and then about x by *ax. y_axis_angles turns the y axis about x by
 * *ax and then about z by *az */
void look_angles(NJS_VECTOR *dir, Angle *ax, Angle *ay);
void z_axis_angles(NJS_VECTOR *dir, Angle *ax, Angle *ay);
void y_axis_angles(NJS_VECTOR *dir, Angle *ax, Angle *az);

/* out = -z turned about x by ang->x, then about y by ang->y. ang->z isn't
 * read, since a turn about z ahead of the others leaves -z where it is */
void look_vector_xy(struct angles *ang, NJS_VECTOR *out);
/* out = the x axis turned about y by -ang->y, then about x by ang->x, then
 * about z by ang->z */
void turn_x_axis(struct angles *ang, NJS_VECTOR *out);

/* move pos by speed the way camera's pad, player_pads[camera], points, as
 * camera sees the ground. up moves pos ahead, along -z turned about y by
 * camera_views[camera]'s ang.y. while stick is FALSE, the d-pad moves pos in
 * eight directions. otherwise the stick moves it the way it leans, read as
 * CAMERA_STICK_READING and CAMERA_STICK_DEAD_ZONE say. L raises pos by
 * speed, or else R lowers it. nothing moves while Start is held */
void point_move_by_pad(NJS_POINT3 *pos, Float speed, int camera, Bool stick);

#endif
