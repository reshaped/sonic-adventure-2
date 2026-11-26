/* a path walked round as a ring, for Mad Space's carriages and FB2's rock */
#ifndef STG44_LOOP_PATH_IMPL_H
#define STG44_LOOP_PATH_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>

#include "stage/path.h"

/* a place on the ring. segment is the index of the point it lies past.
 * fraction is the share of the way to the next point that it has come. it runs
 * from 0 to 1 */
struct loop_cursor {
    Float fraction;
    int segment;
};

/* scale the turn delta by fraction, the shorter way round. SCALE_TURN reads
 * delta's low 16 bits as a signed turn of at most half a FULL_TURN. it scales
 * that turn, and keeps delta's other bits, which leave a turn the same modulo
 * FULL_TURN */
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define SCALE_TURN(delta, fraction)                                            \
    do {                                                                       \
        Sint16 low = (Sint16)(delta);                                          \
        low = (Sint16)(low * (fraction));                                      \
        (delta) = ((delta) & ~0xffff) | (Uint16)low;                           \
    } while (0)
#else
/* @hack type: delta's low half written through a Sint16 pointer, which holds
 * on the SH-4's little-endian layout, where the cast and the mask above would
 * do */
#define SCALE_TURN(delta, fraction)                                            \
    do {                                                                       \
        Sint16 *low = (Sint16 *)&(delta);                                      \
        Float value = *low;                                                    \
        value *= (fraction);                                                   \
        *low = value;                                                          \
    } while (0)
#endif

#pragma inline(segment_direction)
/* @hack spelling: the difference as an inline walk of three Floats, with
 * end++ and out++ statements of their own, where three statements on the
 * fields would do */
static void segment_direction(const Float *start, const Float *end, Float *out)
{
    *out = *end - *start++;
    end++;
    out++;
    *out++ = *end - *start++;
    end++;
    *out = *end - *start;
}

static int loop_point(struct path_point *points, int count, int index,
                      NJS_POINT3 *position, Angle *slope_x, Angle *slope_z)
{
    while (index < 0) {
        index += count;
    }
    while (index >= count) {
        index -= count;
    }
    *position = points[index].pos;
    *slope_x = points[index].slope_x;
    *slope_z = points[index].slope_z;
    return index;
}

#define BLEND_POINT(out, from, to, a, b)                                       \
    do {                                                                       \
        (out)->x = ((from).x * (a)) + ((to).x * (b));                          \
        (out)->y = ((from).y * (a)) + ((to).y * (b));                          \
        (out)->z = ((from).z * (a)) + ((to).z * (b));                          \
    } while (0)

/* move the cursor, which *segment and *fraction hold, by travel along the
 * ring of path's points. a travel below 0 moves it back. *position gets the
 * cursor's point, between the point at the segment and the next one. the
 * slopes get the same blend of those points' slopes. *direction, unless
 * direction is null, gets the next point less the one at the segment. returns
 * 0.
 *
 * where path isn't PATH_LAYOUT_SLOPES, the call writes nothing. path must hold
 * two points or more, none at the place of the next round the ring */
static int loop_advance(struct path *path, int *segment, Float *fraction,
                        Float travel, NJS_POINT3 *position,
                        NJS_VECTOR *direction, Angle *slope_x, Angle *slope_z)
{
    /* @hack spelling: out, a copy of position, where position itself would do
     */
    NJS_POINT3 *out;
    Angle start_x;
    Angle start_z;
    Angle end_x;
    Angle end_z;
    NJS_POINT3 start;
    NJS_POINT3 end;
    /* @hack redundant: frame_pad, a local that only sizes the frame */
    NJS_VECTOR frame_pad;
    int count;
    struct path_point *points;
    /* @hack spelling: path_points, a second copy of path->points, where points
     * alone would do */
    struct path_point *path_points;
    int index;
    int next;
    Float length;
    Float remaining;
    Float distance;
    Float to;
    Float from;

    /* @bug uninitialized: where path isn't PATH_LAYOUT_SLOPES, the call
     * returns without writing *position or the slopes (CMP/EQ at 0x8c501302
     * in the train's copy). carriage_advance and orbit_advance read them all
     * the same */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns 0, with the position,
     * the direction and the slopes zero. it also returns for a path of no
     * points, for which the matching code never returns */
    if (path->flags != PATH_LAYOUT_SLOPES || path->count <= 0) {
        position->x = position->y = position->z = 0.0F;
        if (direction != NULL) {
            *direction = *position;
        }
        *slope_x = *slope_z = 0;
        return 0;
    }
#endif
    distance = travel;
    out = position;
    if (path->flags == PATH_LAYOUT_SLOPES) {
        path_points = path->points;
        points = path_points;
        count = path->count;
        if (distance > 0.0F) {
            index = *segment;
            next = index + 1;
            to = *fraction;
            for (;;) {
                index = loop_point(points, count, index, &start, &start_x,
                                   &start_z);
                next = loop_point(points, count, next, &end, &end_x, &end_z);
                length = njDistanceP2P(&start, &end);
                /* @hack spelling: remaining computed in two statements, where
                 * one would do */
                remaining = length * to;
                remaining = length - remaining;
                if (remaining < 0.0F) {
                    remaining = 0.0F;
                }
                if (distance > remaining) {
                    distance -= remaining;
                } else {
                    Angle dz;
                    Angle dx;

                    from = (remaining - distance) / length;
                    to = 1.0F - from;
                    if (to < 0.0F) {
                        to = 0.0F;
                        from = 1.0F;
                    }
                    *segment = index;
                    *fraction = to;
                    BLEND_POINT(out, start, end, from, to);
                    dx = end_x - start_x;
                    dz = end_z - start_z;
                    SCALE_TURN(dx, to);
                    SCALE_TURN(dz, to);
                    *slope_x = start_x + dx;
                    *slope_z = start_z + dz;
                    if (direction) {
                        segment_direction(&start.x, &end.x, &direction->x);
                    }
                    return 0;
                }
                to = 0.0F;
                index++;
                next++;
            }
        } else {
            index = *segment;
            next = index + 1;
            to = *fraction;
            for (;;) {
                index = loop_point(points, count, index, &start, &start_x,
                                   &start_z);
                next = loop_point(points, count, next, &end, &end_x, &end_z);
                length = njDistanceP2P(&start, &end);
                remaining = length * to;
                from = length - remaining;
                if (from < 0.0F) {
                    remaining = length;
                }
                if (-distance > remaining) {
                    distance += remaining;
                } else {
                    Angle dz;
                    Angle dx;

                    to = (remaining + distance) / length;
                    from = 1.0F - to;
                    if (from < 0.0F) {
                        to = 1.0F;
                        from = 0.0F;
                    }
                    *segment = index;
                    *fraction = to;
                    BLEND_POINT(out, start, end, from, to);
                    dx = end_x - start_x;
                    dz = end_z - start_z;
                    SCALE_TURN(dx, to);
                    SCALE_TURN(dz, to);
                    *slope_x = start_x + dx;
                    *slope_z = start_z + dz;
                    if (direction) {
                        segment_direction(&start.x, &end.x, &direction->x);
                    }
                    return 0;
                }
                to = 1.0F;
                index--;
                next--;
            }
        }
    }
    return 0;
}

#endif
