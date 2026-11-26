#ifndef NJ_LINE_H
#define NJ_LINE_H
#include <ninja.h>
#include <sg_xpt.h>

/* njDrawLineExStartOpaque and njDrawLineExStartTrans set these for the line
 * drawers. line_projection holds the screen's scale and center, in pixels. a
 * line reaches line_half_width pixels to each side of its middle.
 * line_cursor is the store queue address the next line's sprite goes to */
struct line_projection {
    Float scale_x;
    Float scale_y;
    Float center_x;
    Float center_y;
};
extern Float line_half_width;
extern Uint32 *line_cursor;
extern struct line_projection line_projection;

/* a point as njCalcPoint3DLine projects it. it holds the point's place on the
 * screen in pixels, its depth and the depth's reciprocal. the near plane lies
 * at a depth of 1 */
struct line_point {
    Float screen_x;
    Float screen_y;
    Float depth;
    Float inverse_depth;
};
/* project count points, 1 or more, by the current matrix into result, and
 * return how many have a depth of 1 or less. the matrix must be in XMTRX.
 * result must be 32-byte aligned. for an odd count, the call writes over the
 * 16 bytes past the last point */
Int njCalcPoint3DLine(const NJS_POINT3 *points, struct line_point *result,
                      Int count);
/* send count lines, 1 or more. a line runs from a point to the one after it,
 * and the next line's first point lies stride bytes on */
void njCalcLine3D(const struct line_point *points, Int count, Int stride);
/* send the line from points[0] to points[1] as njCalcLine3D sends one. where
 * exactly one point's depth is below 1, njCalcLine3DNearClip sends the line
 * cut at a depth of 1 instead. where both are, nothing goes */
void njCalcLine3DClip(const struct line_point *points);
/* send the line between the two struct line_point records at coordinates as
 * njCalcLine3D sends one, cut at a depth of 1. one point's depth must be 1 or
 * less, and the other's more than 1 */
void njCalcLine3DNearClip(const Float *coordinates);

#endif
