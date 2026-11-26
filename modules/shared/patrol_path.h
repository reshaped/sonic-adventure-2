/* the closed path a Dry Lagoon turtle or an Egg Quarters guard patrols */
#ifndef SHARED_PATROL_PATH_H
#define SHARED_PATROL_PATH_H
#include <ninja.h>
#include <sg_xpt.h>

#include "stage/path.h"
#include "system/angle.h"
#include "system/math.h"

/* move a place speed along path's ring, and return whether it went round. the
 * ring is the path's points but its last, which the move never reads. the
 * ring's first point follows its last. a place is *point and *along. *point is
 * the index of a point of the ring. *along is how far the place lies from that
 * point toward the next, from 0 to 1.
 *
 * a speed above 0 moves toward higher indexes, and any other speed toward
 * lower ones. *point and *along take the new place, and *pos its position.
 * ang's x and y take the heading at the new place. that heading points toward
 * higher indexes whichever way the move runs. it is the place's segment's,
 * blended by *along toward the next segment's.
 *
 * the ring's last segment runs from its last point back to its first. a move
 * toward higher indexes returns TRUE where it starts on that segment or
 * reaches it. a move toward lower indexes returns TRUE where it passes back
 * over the first point.
 *
 * a path whose flags aren't 0 is left alone, and the call returns FALSE. the
 * path must hold 3 points or more. a move by any speed but 0 never returns
 * where the ring's points all lie at one place */
static Bool patrol_path_move(struct path *path, Sint32 *point, Float *along,
                             Float speed, NJS_POINT3 *pos, struct angles *ang)
{
    Bool wrapped;
    int ring;
    int last;
    int i;
    int i1;
    int i2;
    NJS_POINT3 *start;
    NJS_POINT3 *end;
    NJS_POINT3 *points;
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    NJS_POINT3 *p2;
    Float span;
    Float room;
    Float rest;
    Float fraction;
    Angle ax;
    Angle ay;
    Angle bx;
    Angle by;
    Angle back_ax;
    Angle back_ay;
    Angle back_bx;
    Angle back_by;

    wrapped = FALSE;
    if (path->flags == 0) {
        ring = path->count - 1;
        points = path->points;
        if (speed > 0.0F) {
            i = *point;
            fraction = *along;
            last = ring - 1;
            for (;;) {
                /* @hack spelling: wrapped assigned itself where i lies below
                 * last, where an if that sets it would do */
                /* @hack type: the 1 chosen by the float constant 1.0F, where a
                 * plain 1 would do */
                /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
                /* NOLINTBEGIN(misc-redundant-expression) */
                wrapped = i >= last ? (1.0F ? 1 : 1) : wrapped;
                /* NOLINTEND(misc-redundant-expression) */
                /* NOLINTEND(readability-avoid-nested-conditional-operator) */
                i = i % ring;
                i1 = (i + 1) % ring;
                /* @hack spelling: i2 set from i, then from itself, where
                 * one assignment would do */
                i2 = i;
                i2 = (i2 + 2) % ring;
                /* @hack spelling: start and end assigned in the call's
                 * arguments, where statements of their own would do */
                span = njDistanceP2P(start = &points[i], end = &points[i1]);
                /* @hack spelling: room worked out in three statements, where
                 * span less span times fraction would do */
                room = span;
                room *= fraction;
                room = span - room;
                if (room < 0.0F) {
                    room = 0.0F;
                }
                if (speed > room) {
                    speed -= room;
                } else {
                    rest = (room - speed) / span;
                    fraction = 1.0F - rest;
                    if (fraction < 0.0F) {
                        fraction = 0.0F;
                        rest = 1.0F;
                    }
                    *point = i;
                    *along = fraction;
                    /* @hack type: p0, p1 and p2 cast from their x's
                     * addresses, where start, end and &points[i2] would do */
                    p0 = (NJS_POINT3 *)&start->x;
                    p1 = (NJS_POINT3 *)&end->x;
                    p2 = (NJS_POINT3 *)&points[i2].x;
                    pos->x = (p0->x * rest) + (p1->x * fraction);
                    pos->y = (p0->y * rest) + (p1->y * fraction);
                    pos->z = (p0->z * rest) + (p1->z * fraction);
                    direction_angles_xy(p1->x - p0->x, p1->y - p0->y,
                                        p1->z - p0->z, &ax, &ay);
                    direction_angles_xy(p2->x - p1->x, p2->y - p1->y,
                                        p2->z - p1->z, &bx, &by);
                    ang->x = angle_interpolate(ax, bx, fraction, rest);
                    ang->y = angle_interpolate(ay, by, fraction, rest);
                    return wrapped;
                }
                fraction = 0.0F;
                /* @hack spelling: i = i + 1, where i++ would do */
                i = i + 1;
            }
        } else {
            i = *point;
            fraction = *along;
            last = ring - 1;
            for (;;) {
                i = i % ring;
                i1 = (i + 1) % ring;
                i2 = (i + 2) % ring;
                span = njDistanceP2P(&points[i], &points[i1]);
                /* @hack spelling: room set to span, then scaled by fraction,
                 * where span times fraction would do */
                room = span;
                room *= fraction;
                rest = span - room;
                if (rest < 0.0F) {
                    room = span;
                }
                if (-speed > room) {
                    speed += room;
                } else {
                    room += speed;
                    fraction = room / span;
                    rest = 1.0F - fraction;
                    if (rest < 0.0F) {
                        fraction = 1.0F;
                        rest = 0.0F;
                    }
                    *point = i;
                    *along = fraction;
                    p0 = &points[i];
                    p1 = &points[i1];
                    /* @hack type: p2 cast from its x's address, where
                     * &points[i2] would do */
                    p2 = (NJS_POINT3 *)&points[i2].x;
                    pos->x = (p0->x * rest) + (p1->x * fraction);
                    pos->y = (p0->y * rest) + (p1->y * fraction);
                    pos->z = (p0->z * rest) + (p1->z * fraction);
                    direction_angles_xy(p1->x - p0->x, p1->y - p0->y,
                                        p1->z - p0->z, &back_ax, &back_ay);
                    direction_angles_xy(p2->x - p1->x, p2->y - p1->y,
                                        p2->z - p1->z, &back_bx, &back_by);
                    ang->x =
                        angle_interpolate(back_ax, back_bx, fraction, rest);
                    ang->y =
                        angle_interpolate(back_ay, back_by, fraction, rest);
                    return wrapped;
                }
                fraction = 1.0F;
                /* @hack spelling: the decrement inside the test, where a
                 * statement of its own would do */
                if (--i < 0) {
                    i = last;
                    wrapped = TRUE;
                }
            }
        }
    }
    return FALSE;
}

#endif
