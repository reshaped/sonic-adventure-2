/* King Boom Boo's arena: the ground about bigbogy_arena_center, and the boss's
 * circle of radius bigbogy_circle_radius about it. a heading h is the
 * direction (njSin(h), 0, njCos(h)). a point's bearing is the heading from the
 * center to it. a point a call sets lies at the center's height unless its
 * comment says otherwise. no call reads its task parameter */
#ifndef BOSS_BIGBOGY_ARENA_H
#define BOSS_BIGBOGY_ARENA_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* position's distance from the center in x and z. a non-null offset takes
 * position less the center, with y 0 */
Float bigbogy_arena_distance(const NJS_POINT3 *position, NJS_VECTOR *offset);
/* set *out to the center moved by offset's x and z */
void bigbogy_arena_point(const NJS_VECTOR *offset, NJS_POINT3 *out);
/* set *out to the point radius from the center along direction. direction's
 * y counts in its length, so a y other than 0 brings the point nearer */
void bigbogy_arena_along(const NJS_VECTOR *direction, Float radius,
                         NJS_POINT3 *out);
/* set *out to the point radius from the center at bearing angle.
 * bigbogy_arena_at_radius sets it at position's bearing */
void bigbogy_arena_at_angle(Angle angle, Float radius, NJS_POINT3 *out);
void bigbogy_arena_at_radius(const NJS_POINT3 *position, Float radius,
                             NJS_POINT3 *out);

/* the heading along the boss's circle at position, from 0 to 0xffff. it is a
 * quarter turn past position's bearing, or three quarters while
 * bigbogy_reversed() returns 1 */
Angle bigbogy_arena_tangent(struct task *task, const NJS_POINT3 *position);
/* set *out to position moved along its bearing onto the boss's circle, at
 * position's height */
void bigbogy_arena_project(struct task *task, const NJS_POINT3 *position,
                           NJS_POINT3 *out);
/* set *out to position's point on the boss's circle, turned about the center
 * by angle against bigbogy_arena_tangent's heading. the call returns the
 * tangent's heading at *out, which can pass 0xffff or fall below 0.
 * bigbogy_arena_turn turns position at its own distance from the center.
 * bigbogy_task() must not be null */
Angle bigbogy_arena_advance(struct task *task, const NJS_POINT3 *position,
                            Angle angle, NJS_POINT3 *out);
Angle bigbogy_arena_turn(const NJS_POINT3 *position, Angle angle,
                         NJS_POINT3 *out);
/* the short turn between angle and bigbogy_arena_tangent's heading at
 * position, from 0 to 0x8000 */
Angle bigbogy_tangent_distance(Angle angle, const NJS_POINT3 *position);
/* set *step to (to - from) / frames, only as precise as njInvertSqrt. a
 * frames of 0 makes each component infinite or NaN */
void bigbogy_step_between(const NJS_POINT3 *from, const NJS_POINT3 *to,
                          Uint16 frames, NJS_VECTOR *step);
/* the short turn between from's bearing and to's, from 0 to 0x8000 */
Angle bigbogy_arena_angle_between(const NJS_POINT3 *from, const NJS_POINT3 *to);
/* add a speed along heading to velocity's x and z. bigbogy_set_speed sets
 * them to it. y stays as it is */
void bigbogy_add_speed(NJS_VECTOR *velocity, Angle heading, Float speed);
void bigbogy_set_speed(NJS_VECTOR *velocity, Angle heading, Float speed);
#endif
