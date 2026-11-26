/* grind_rail_path_place, for grind_rail.c's GLINDRB and stg30/curved_rail.c's
 * curved rails. curved_rail.c defines CURVED_RAIL, which adds the fix of the
 * slopes below */
#ifndef SHARED_GRIND_PATH_H
#define SHARED_GRIND_PATH_H
#include <ninja.h>
#include <shinobi.h>

#include "stage/path.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/math.h"

#ifdef CURVED_RAIL
/* a slope that turns more than SLOPE_JUMP from the point before's counts as a
 * half turn's flip. SLOPE_JUMP is 67.5 degrees */
#define SLOPE_JUMP 0x3000
#endif

/* copy src to the Shinobi heap, turned by ang and moved to pos, and return the
 * copy. each point's slopes turn with it. where the heap has no room, the call
 * frees what it took and returns null. src must be a PATH_LAYOUT_SLOPES path */
static struct path *grind_rail_path_place(NJS_POINT3 *pos, struct angles *ang,
                                          struct path *src)
{
    struct path_point *from = src->points;
    struct path *path;
    struct path_point *points;
    Angle ang_x;
    Angle ang_y;
    Angle ang_z;
#ifdef CURVED_RAIL
    Sint16 delta_x;
    Sint16 delta_z;
    Bool flip_x;
    Bool flip_z;
#endif
    int i;

    path = syCalloc(1, sizeof(struct path));
    points = syCalloc(src->count, sizeof(struct path_point));
    if (path != NULL && points != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(pos);
        njRotateEx(&ang->x, 0);
        njPushMatrixEx();
#ifdef CURVED_RAIL
        flip_x = FALSE;
        flip_z = FALSE;
#endif
        for (i = 0; i < src->count; i++) {
            njRotateZ(NULL, from[i].slope_z);
            njRotateX(NULL, from[i].slope_x);
            current_matrix_angles_yxz(&ang_x, &ang_y, &ang_z);
#ifdef CURVED_RAIL
            /* current_matrix_angles_yxz can give a point's slopes a half turn
             * from the point before's. where x or z jumps by more than
             * SLOPE_JUMP, the copy turns that slope back a half turn. it sets
             * the other to a half turn less itself. the points after take the
             * same change */
            if (flip_x) {
                ang_x += -HALF_TURN;
                ang_z = HALF_TURN - ang_z;
            } else if (flip_z) {
                ang_z += -HALF_TURN;
                ang_x = HALF_TURN - ang_x;
            }
            if (i > 0) {
                delta_x = ang_x - points[i - 1].slope_x;
                delta_z = ang_z - points[i - 1].slope_z;
            } else {
                delta_x = 0;
                delta_z = 0;
            }
            if (delta_x > SLOPE_JUMP || delta_x < -SLOPE_JUMP) {
                ang_x += -HALF_TURN;
                ang_z = HALF_TURN - ang_z;
                flip_x = TRUE;
                flip_z = FALSE;
            } else if (delta_z > SLOPE_JUMP || delta_z < -SLOPE_JUMP) {
                ang_z += -HALF_TURN;
                ang_x = HALF_TURN - ang_x;
                flip_x = FALSE;
                flip_z = TRUE;
            }
#endif
            points[i].slope_x = ang_x;
            points[i].slope_z = ang_z;
            points[i].length = from[i].length;
            matrix_load_parent();
            njCalcPoint(NULL, &from[i].pos, &points[i].pos);
        }
        njPopMatrix(2);
        *path = *src;
        /* @hack redundant: the length copied again, after the copy of the
         * whole record */
        path->length = src->length;
        path->points = points;
        return path;
    } else {
        if (path != NULL) {
            syFree(path);
        }
        if (points != NULL) {
            syFree(points);
        }
        return NULL;
    }
}

#endif
