/* drawing helpers: two spheres' overlap, a step toward a point, a line along
 * a turned axis, and a sphere's test against the camera's view */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "draw/helpers.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/util.h"
#include "system/vector.h"

#ifdef NONMATCHING
/* port_wide, which widens this side's cull under 16:9 */
#include "engine/dc/port.h"
#endif

/* how far off the camera's direction a sphere must lie to be out of view */
#define VIEW_ANGLE ((Angle)(50 * ANGLE_PER_DEGREE))
/* what the port's 16:9 adds to the cone: the angle the picture's corners
 * gain at the camera's 70 degree view, from 41 degrees off its middle to
 * 47 */
#define WIDE_VIEW_GAIN ((Angle)(6 * ANGLE_PER_DEGREE))
/* the squared distance from the camera, plus the radius squared, below which
 * a sphere is in view */
#define VIEW_NEAR_DISTANCE2 2500.0F

/* nothing calls these. none is static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* TRUE when spheres of these radii at these centers overlap, else FALSE */
Bool spheres_overlap(const NJS_POINT3 *center1, const NJS_POINT3 *center2,
                     Float radius1, Float radius2);
/* draw a line from white at pos to pale blue at its end. the end is pos plus
 * (0, 0, length) turned as njRotateXYZ turns by ang's x, y and z */
void draw_line_facing(const struct angles *ang, const NJS_POINT3 *pos,
                      Float length);
/* NOLINTEND(misc-use-internal-linkage) */

Bool spheres_overlap(const NJS_POINT3 *center1, const NJS_POINT3 *center2,
                     Float radius1, Float radius2)
{
    NJS_SPHERE spheres[2];

    /* @hack spelling: the centers copied member by member, where
     * spheres[0].c = *center1 and spheres[1].c = *center2 would do */
    spheres[0].c.x = center1->x;
    spheres[0].c.y = center1->y;
    spheres[0].c.z = center1->z;
    spheres[0].r = radius1;
    spheres[1].c.x = center2->x;
    spheres[1].c.y = center2->y;
    spheres[1].c.z = center2->z;
    spheres[1].r = radius2;
    return njCollisionCheckSS(&spheres[0], &spheres[1]);
}

void point_toward(const NJS_POINT3 *start, const NJS_POINT3 *target,
                  Float distance, NJS_POINT3 *out)
{
    NJS_VECTOR way;

    way.x = target->x - start->x;
    way.y = target->y - start->y;
    way.z = target->z - start->z;
    if (njUnitVector(&way) == 0.0F) {
        *out = *start;
    } else {
        way.x *= distance;
        way.y *= distance;
        way.z *= distance;
        way.x += start->x;
        way.y += start->y;
        way.z += start->z;
        *out = way;
    }
}

void draw_line_facing(const struct angles *ang, const NJS_POINT3 *pos,
                      Float length)
{
    NJS_VECTOR ahead;
    NJS_POINT3 end;

    ahead.x = 0.0F;
    ahead.y = 0.0F;
    ahead.z = length;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateXYZ(NULL, ang->x, ang->y, ang->z);
    njCalcVector(NULL, &ahead, &end);
    end.x += pos->x;
    end.y += pos->y;
    end.z += pos->z;
    njPopMatrix(1);
    line_draw(pos->x, pos->y, pos->z, end.x, end.y, end.z);
}

Bool sphere_out_of_view(const NJS_POINT3 *center, Float radius)
{
    NJS_VECTOR to_center;
    NJS_VECTOR view;
    /* @hack redundant: 12 bytes nothing reads or writes */
    Uint8 unused[12];

    view = camera_views[camera_current_index]->dir;
    njUnitVector(&view);
    vector_diff(center, &camera_views[camera_current_index]->pos, &to_center);
    njUnitVector(&to_center);
#ifdef NONMATCHING
    /* this side's cone gains what the picture's corners gain under 16:9 */
    if (vector_angle(&to_center, &view, NULL) <
        VIEW_ANGLE + (port_wide ? WIDE_VIEW_GAIN : 0)) {
#else
    if (vector_angle(&to_center, &view, NULL) < VIEW_ANGLE) {
#endif
        return FALSE;
    }
    vector_diff(center, &camera_views[camera_current_index]->pos, &to_center);
    if (njScalor2(&to_center) + (radius * radius) < VIEW_NEAR_DISTANCE2) {
        return FALSE;
    }
    return TRUE;
}
