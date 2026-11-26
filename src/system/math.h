/* rotation angles from a matrix or a direction, a point's distance from a
 * segment and from a plane, a step toward a point, and an integer power */
#ifndef SYSTEM_MATH_H
#define SYSTEM_MATH_H
#include <ninja.h>
#include <sg_xpt.h>

/* the angles of the rotation that matrix holds. as with Ninja's matrices, a row
 * vector v turns to v times matrix. matrix_angles_xyz takes it for a turn
 * about x by *ax, then about y by *ay, then about z by *az. it reads only the
 * first three rows and columns, and a positive scale of all of them changes
 * nothing. each angle comes out within half a turn of 0 */
void matrix_angles_xyz(const NJS_MATRIX matrix, Angle *ax, Angle *ay,
                       Angle *az);
/* the same angles of the current matrix, each for the order of the axes its
 * name gives. current_matrix_angles_xzy takes a turn about x, then z, then y */
void current_matrix_angles_xyz(Angle *ax, Angle *ay, Angle *az);
void current_matrix_angles_xzy(Angle *ax, Angle *ay, Angle *az);
void current_matrix_angles_yxz(Angle *ax, Angle *ay, Angle *az);

/* the angles that turn an axis toward the direction (x, y, z), whatever its
 * length. direction_angles_xy turns the z axis: about x by *ax, then about y
 * by *ay. direction_angles_xz turns the y axis: about x by *ax, then about z
 * by *az */
void direction_angles_xy(Float x, Float y, Float z, Angle *ax, Angle *ay);
void direction_angles_xz(Float x, Float y, Float z, Angle *ax, Angle *az);

/* the square of the distance from point to the segment that starts at line's
 * (px, py, pz) and runs the length of its vector. where nearest isn't null,
 * the segment's point nearest to point goes there */
Float point_segment_distance2(const NJS_POINT3 *point, const NJS_LINE *line,
                              NJS_POINT3 *nearest);
/* the distance from point to plane: positive on the side plane's vector
 * points to, and negative on the other. where foot isn't null, the plane's
 * point nearest to point goes there. plane's vector need not be a unit
 * vector, and must not be 0 */
Float point_plane_distance(const NJS_POINT3 *point, const NJS_PLANE *plane,
                           NJS_POINT3 *foot);

/* set out to the point distance along the way from start toward target, or
 * to start itself where target is the same point */
void point_toward(const NJS_POINT3 *start, const NJS_POINT3 *target,
                  Float distance, NJS_POINT3 *out);

/* base to the power exponent, or 1 for an exponent of 0 or less */
int int_power(int base, int exponent);

#endif
