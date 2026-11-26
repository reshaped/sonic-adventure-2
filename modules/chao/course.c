/* a Chao race's course. each segment's floor is two triangles between the
 * edges */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/course.h"
#include "chao/course_keys.h"
#include "chao/work.h"
#include "draw/lines.h"
#include "object/mover.h"
#include "system/task.h"

#define POSITION(mkey) ((NJS_POINT3 *)(mkey)->key)
/* a floor whose normal's y is below WALL_NORMAL_Y is a wall to the Chao */
#define WALL_NORMAL_Y 0.6F
/* a landing on a segment's first triangle turns FIRST_REBOUND of a fall's speed
 * back up. one on its second turns SECOND_REBOUND */
#define FIRST_REBOUND 0.5F
#define SECOND_REBOUND 0.1F
/* the farthest a segment can lie from the point that seeks it */
#define SEARCH_LIMIT 1000000.0F

static Sint32 current_track = 0;

void garden_select_track(Sint32 track)
{
    current_track = track;
}

void garden_draw_track(void)
{
    NJS_MKEY_F *left;
    NJS_MKEY_F *right;
    Uint32 i;
    Uint32 count;
    /* @hack redundant: an array no code uses, which sizes the frame */
    Uint32 unused[3];
    (void)unused;

    count = track_lengths[current_track]->nbFrame;
    left = track_edges[current_track].left;
    for (i = 0; i < count - 1; i++, left++) {
        line_draw_points(POSITION(left), POSITION(left + 1));
    }
    right = track_edges[current_track].right;
    for (i = 0; i < count - 1; i++, right++) {
        line_draw_points(POSITION(right), POSITION(right + 1));
    }
    left = track_edges[current_track].left;
    right = track_edges[current_track].right;
    for (i = 0; i < count - 1; i++, left++, right++) {
        line_draw_points(POSITION(left), POSITION(right));
    }
}

static void track_normal(NJS_POINT3 *first, NJS_POINT3 *second,
                         NJS_POINT3 *third, NJS_VECTOR *normal)
{
    NJS_VECTOR along;
    NJS_VECTOR across;

    along.x = second->x - first->x;
    along.y = second->y - first->y;
    along.z = second->z - first->z;
    across.x = third->x - first->x;
    across.y = third->y - first->y;
    across.z = third->z - first->z;
    njOuterProduct(&along, &across, normal);
}

#define HORIZONTAL_LINE(arg_line, arg_start, arg_end)                          \
    {                                                                          \
        (arg_line).px = (arg_start)->x;                                        \
        (arg_line).py = 0.0F;                                                  \
        (arg_line).pz = (arg_start)->z;                                        \
        (arg_line).vx = (arg_end)->x - (arg_start)->x;                         \
        (arg_line).vy = 0.0F;                                                  \
        (arg_line).vz = (arg_end)->z - (arg_start)->z;                         \
    }

#define VERTICAL_RAY(arg_ray, arg_position)                                    \
    {                                                                          \
        (arg_ray).px = (arg_position)->x;                                      \
        (arg_ray).py = (arg_position)->y;                                      \
        (arg_ray).pz = (arg_position)->z;                                      \
        (arg_ray).vx = 0.0F;                                                   \
        (arg_ray).vy = -1.0F;                                                  \
        (arg_ray).vz = 0.0F;                                                   \
    }

#pragma inline(apply_course_friction)
static void apply_course_friction(struct mover *mover)
{
    mover->spd.x *= mover->friction;
    mover->spd.y *= mover->friction;
    mover->spd.z *= mover->friction;
}

Bool chao_follow_course_floor(struct task *chao)
{
    Uint32 count;
    NJS_MKEY_F *left;
    NJS_MKEY_F *right;
    Uint16 segment; /* no track has more than 83 keys, so it can't wrap */
    Uint16 previous;
    NJS_VECTOR normal;
    struct chao_work *work;
    struct mover *mover;
    NJS_POINT3 *position;
    NJS_POINT3 *left0;
    NJS_POINT3 *left1;
    NJS_POINT3 *right0;
    NJS_POINT3 *right1;
    Float start_distance;
    Float end_distance;
    Float fraction;

    work = CHAO_WORK(chao);
    left = track_edges[current_track].left;
    right = track_edges[current_track].right;
    position = &work->base.pos;
    count = track_lengths[current_track]->nbFrame;
    mover = chao->motion.p;
    previous = segment = work->race_progress;
    if (segment > 0) {
        segment--;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below. it returns FALSE for a segment
     * past the track's last, count - 2 */
    if (segment >= count - 1) {
        return FALSE;
    }
#endif
    left += segment;
    right += segment;

    /* @bug memory: the loop reads keys past the track's end, since it runs
     * segment to count, where the last segment is count - 2 (0x0c51513a) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the loop ends at the last
     * segment */
    for (; segment < count - 1; segment++) {
#else
    /* @hack spelling: segment = segment + 1, where segment++ would do */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (; segment <= count; segment = segment + 1) {
#endif
        left0 = POSITION(left);
        right0 = POSITION(right);
        left++;
        right++;
        left1 = POSITION(left);
        right1 = POSITION(right);
        /* normal.y is above 0 until the Chao passes the segment's far pair of
         * keys. the first such segment is the Chao's */
        track_normal(left1, position, right1, &normal);
        if (normal.y > 0.0F) {
            /* normal.y is below 0 past the left edge. a constrained Chao
             * moves back onto the edge */
            track_normal(left0, position, left1, &normal);
            if (normal.y < 0.0F) {
                NJS_POINT3 flat;
                NJS_LINE edge;
                NJS_POINT3 closest;

                flat.x = position->x;
                flat.y = 0.0F;
                flat.z = position->z;
                HORIZONTAL_LINE(edge, left0, left1);
                njDistanceP2L(&flat, &edge, &closest);
                if (work->flags & CHAO_COURSE_CONSTRAIN) {
                    position->x = closest.x;
                    position->z = closest.z;
                }
            }
            /* both tests hold over the floor's first triangle, of left0,
             * right1 and left1 */
            track_normal(position, left0, right1, &normal);
            if (normal.y >= 0.0F) {
                track_normal(position, right1, left1, &normal);
                if (normal.y >= 0.0F) {
                    NJS_LINE down;
                    NJS_PLANE triangle;
                    NJS_POINT3 floor;
                    NJS_PLANE *ground;

                    ground = &work->race_floor;
                    VERTICAL_RAY(down, position);
                    triangle.px = left0->x;
                    triangle.py = left0->y;
                    triangle.pz = left0->z;
                    track_normal(left0, right1, left1, &normal);
                    triangle.vx = normal.x;
                    triangle.vy = normal.y;
                    triangle.vz = normal.z;
                    njDistanceL2PL(&down, &triangle, &floor);
                    ground->px = left0->x;
                    ground->py = left0->y;
                    ground->pz = left0->z;
                    ground->vx = normal.x;
                    ground->vy = normal.y;
                    ground->vz = normal.z;
                    if ((work->flags & CHAO_COURSE_CONSTRAIN) &&
                        position->y < floor.y) {
                        mover->flags |= MOVER_ON_FLOOR;
                        if (!(mover->flags & MOVER_SLOPE_BOUNCE)) {
                            apply_course_friction(mover);
                        }
                        mover->flags &= ~MOVER_SLOPE_BOUNCE;
                        if (mover->spd.y < 0.0F) {
                            mover->spd.y *= -FIRST_REBOUND;
                        }
                        position->y = floor.y;
                        /* @bug logic: tests normal, a cross product as
                         * long as twice the triangle's area, against
                         * WALL_NORMAL_Y, a unit normal's y. a small flat
                         * triangle then counts as a wall, and a large steep
                         * one doesn't. the test is at 0x0c51532c */
                        if (normal.y < WALL_NORMAL_Y) {
                            mover->flags |= MOVER_AT_WALL;
                            mover->flags &= ~MOVER_ON_FLOOR;
                        }
                    }
                }
            }
            /* normal.y is above 0 past the right edge, which holds a
             * constrained Chao as the left edge does */
            track_normal(right0, position, right1, &normal);
            if (normal.y > 0.0F) {
                NJS_POINT3 flat;
                NJS_LINE edge;
                NJS_POINT3 closest;

                flat.x = position->x;
                flat.y = 0.0F;
                flat.z = position->z;
                HORIZONTAL_LINE(edge, right0, right1);
                njDistanceP2L(&flat, &edge, &closest);
                if (work->flags & CHAO_COURSE_CONSTRAIN) {
                    position->x = closest.x;
                    position->z = closest.z;
                }
            }
            /* both tests hold over the second triangle, of right1, left0 and
             * right0. unlike the first, it gives race_floor a unit normal and
             * the floor's point under the Chao */
            track_normal(position, right1, left0, &normal);
            if (normal.y >= 0.0F) {
                track_normal(position, left0, right0, &normal);
                if (normal.y >= 0.0F) {
                    NJS_LINE down;
                    NJS_PLANE triangle;
                    NJS_POINT3 floor;
                    NJS_PLANE *ground;

                    ground = &work->race_floor;
                    VERTICAL_RAY(down, position);
                    triangle.px = left0->x;
                    triangle.py = left0->y;
                    triangle.pz = left0->z;
                    track_normal(right1, left0, right0, &normal);
                    triangle.vx = normal.x;
                    triangle.vy = normal.y;
                    triangle.vz = normal.z;
                    njDistanceL2PL(&down, &triangle, &floor);
                    if (njScalor(&normal) > 0.0F) {
                        njUnitVector(&normal);
                    }
                    ground->px = floor.x;
                    ground->py = floor.y;
                    ground->pz = floor.z;
                    ground->vx = normal.x;
                    ground->vy = normal.y;
                    ground->vz = normal.z;
                    if ((work->flags & CHAO_COURSE_CONSTRAIN) &&
                        position->y < floor.y) {
                        mover->flags |= MOVER_ON_FLOOR;
                        if (!(mover->flags & MOVER_SLOPE_BOUNCE)) {
                            apply_course_friction(mover);
                        }
                        mover->flags &= ~MOVER_SLOPE_BOUNCE;
                        if (mover->spd.y < 0.0F) {
                            mover->spd.y *= -SECOND_REBOUND;
                        }
                        position->y = floor.y;
                        if (normal.y < WALL_NORMAL_Y) {
                            mover->flags |= MOVER_AT_WALL;
                            mover->flags &= ~MOVER_ON_FLOOR;
                        }
                    }
                }
            }
            if (segment == previous) {
                work->flags &= ~CHAO_COURSE_NEW_SEGMENT;
            } else {
                work->flags |= CHAO_COURSE_NEW_SEGMENT;
            }
            {
                NJS_POINT3 flat;
                NJS_LINE cross;
                NJS_POINT3 closest;

                flat.x = position->x;
                flat.y = 0.0F;
                flat.z = position->z;
                HORIZONTAL_LINE(cross, left0, right0);
                start_distance = njDistanceP2L(&flat, &cross, &closest);
                HORIZONTAL_LINE(cross, left1, right1);
                end_distance = njDistanceP2L(&flat, &cross, &closest);
                fraction = start_distance / (start_distance + end_distance);
            }
            if (fraction < 0.0F) {
                fraction = 0.0F;
            }
            if (fraction >= 1.0F) {
                fraction = 1.0F;
            }
            work->race_progress = segment + fraction;
            /* @hack spelling: the flags read through chao->work, where work
             * would do */
            if (CHAO_WORK(chao)->flags & CHAO_COURSE_AIM) {
                Float lane;
                Float other;
                NJS_POINT3 *target;
                NJS_POINT3 *near_left;
                NJS_POINT3 *near_right;

                target = &mover->target;
                lane = work->lane;
                other = 1.0F - lane;
                if (CHAO_WORK(chao)->flags & CHAO_COURSE_AIM_NEAR) {
                    left--;
                    right--;
                }
                /* the aim is at the far keys of the next segment, or of this
                 * one under CHAO_COURSE_AIM_NEAR */
                /* @bug memory: reads one key past each edge's last from
                 * 0x0c5156fe, on the last segment, count - 2, without
                 * CHAO_COURSE_AIM_NEAR */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above: it stops the aim at
                 * the track's last keys */
                if (right < track_edges[current_track].right + count - 1) {
                    left++;
                    right++;
                }
#else
                left++;
                right++;
#endif
                near_left = POSITION(left);
                near_right = POSITION(right);
                target->x = (near_left->x * other) + (near_right->x * lane);
                target->y = (near_left->y * other) + (near_right->y * lane);
                target->z = (near_left->z * other) + (near_right->z * lane);
            }
            return TRUE;
        }
    }
    return FALSE;
}

Bool chao_entered_course_segment(struct task *chao, Uint16 segment)
{
    struct chao_work *work = CHAO_WORK(chao);
    if ((work->flags & CHAO_COURSE_NEW_SEGMENT) &&
        (Uint16)work->race_progress == segment) {
        return TRUE;
    }
    return FALSE;
}

Uint32 garden_track_point_count(void)
{
    return track_lengths[current_track]->nbFrame;
}

void garden_track_quad(Uint16 segment, struct garden_track_quad *quad)
{
    NJS_MKEY_F *left;
    NJS_MKEY_F *right;
    Uint32 count;

    count = track_lengths[current_track]->nbFrame;
    /* @bug memory: the next keys are read past the track's end where segment
     * is count - 1, which the clamp allows (0x0c515814) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the clamp stops at count - 2 */
    if (segment > count - 2) {
        segment = count - 2;
#else
    if (segment > count - 1) {
        segment = count - 1;
#endif
    }
    left = track_edges[current_track].left + segment;
    right = track_edges[current_track].right + segment;
    quad->points[TRACK_LEFT] = *POSITION(left);
    quad->points[TRACK_RIGHT] = *POSITION(right);
    left++;
    right++;
    quad->points[TRACK_NEXT_LEFT] = *POSITION(left);
    quad->points[TRACK_NEXT_RIGHT] = *POSITION(right);
}

int garden_nearest_track_segment(NJS_POINT3 *position)
{
    NJS_MKEY_F *left;
    NJS_MKEY_F *right;
    Uint32 i;
    Uint32 count;
    NJS_POINT3 *first;
    NJS_POINT3 *second;
    int nearest;
    Float minimum;
    Float distance;
    NJS_POINT3 middle;

    nearest = -1;
    left = track_edges[current_track].left;
    right = track_edges[current_track].right;
    count = track_lengths[current_track]->nbFrame;
    minimum = SEARCH_LIMIT;
    /* @hack spelling: left stepped in the loop's body, where the for would
     * step it with right */
    for (i = 0; i < count - 1; i++, right++) {
        first = POSITION(left);
        second = POSITION(right);
        middle.x = (first->x + second->x) * 0.5F;
        middle.y = (first->y + second->y) * 0.5F;
        middle.z = (first->z + second->z) * 0.5F;
        distance = njDistanceP2P(position, &middle);
        left++;
        if (distance < minimum) {
            minimum = distance;
            nearest = i;
        }
    }
    return nearest;
}
