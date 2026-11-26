#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigbogy/boss.h"
#include "boss_bigbogy/arena.h"
#include "boss_bigbogy/stage.h"
#include "system/angle.h"
#include "system/matrix.h"
#ifdef NONMATCHING
/* zam_vec3_turn_y, by which this side turns a vector. for a finite vector it
 * gives the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

/* @hack type: bigbogy_reversed declared again as bigbogy_reversed_for, which
 * takes the boss task and ignores it, where bigbogy_reversed() would do */
#ifdef NONMATCHING
/* bigbogy_reversed by its own name: this side's objects don't define
 * bigbogy_reversed_for, which only symbols.txt places */
#define bigbogy_reversed_for(boss) ((void)(boss), bigbogy_reversed())
#else
Sint8 bigbogy_reversed_for(struct task *boss);
#endif

Float bigbogy_arena_distance(const NJS_POINT3 *position, NJS_VECTOR *offset)
{
    NJS_VECTOR radial;
    Float radius;

    radial.x = position->x - bigbogy_arena_center.x;
    radial.y = 0.0F;
    radial.z = position->z - bigbogy_arena_center.z;
    radius = njSqrt((radial.x * radial.x) + (radial.z * radial.z));
    if (offset) {
        *offset = radial;
    }
    return radius;
}

void bigbogy_arena_point(const NJS_VECTOR *offset, NJS_POINT3 *out)
{
    NJS_POINT3 point;
    point.x = bigbogy_arena_center.x + offset->x;
    point.y = bigbogy_arena_center.y;
    point.z = bigbogy_arena_center.z + offset->z;
    *out = point;
}

void bigbogy_arena_along(const NJS_VECTOR *direction, Float radius,
                         NJS_POINT3 *out)
{
    NJS_POINT3 point;
    NJS_VECTOR radial;

    radial = *direction;
    njUnitVector(&radial);
    point.x = bigbogy_arena_center.x + (radial.x * radius);
    point.y = bigbogy_arena_center.y;
    point.z = bigbogy_arena_center.z + (radial.z * radius);
    *out = point;
}

void bigbogy_arena_at_angle(Angle angle, Float radius, NJS_POINT3 *out)
{
    NJS_POINT3 point;
    point.x = bigbogy_arena_center.x + (njSin(angle) * radius);
    point.y = bigbogy_arena_center.y;
    point.z = bigbogy_arena_center.z + (njCos(angle) * radius);
    *out = point;
}

void bigbogy_arena_at_radius(const NJS_POINT3 *position, Float radius,
                             NJS_POINT3 *out)
{
    NJS_POINT3 point;
    NJS_VECTOR radial;
    bigbogy_arena_distance(position, &radial);
    njUnitVector(&radial);
    point.x = bigbogy_arena_center.x + (radial.x * radius);
    point.y = bigbogy_arena_center.y;
    point.z = bigbogy_arena_center.z + (radial.z * radius);
    *out = point;
}

#define BEARING(point)                                                         \
    ((Angle)(atan2f((point)->x - bigbogy_arena_center.x,                       \
                    (point)->z - bigbogy_arena_center.z) *                     \
             ANGLE_PER_RADIAN))

Angle bigbogy_arena_tangent(struct task *task, const NJS_POINT3 *position)
{
    /* @hack redundant: the vector unused, which only sizes the frame */
    NJS_VECTOR unused;
    Angle heading;
    Angle tangent;
    (void)unused;
    tangent = QUARTER_TURN + BEARING(position);
    /* @hack type: the cast of tangent to Uint16, where the cast on return
     * alone would do */
    heading = (Uint16)tangent;
    if (bigbogy_reversed_for(bigbogy_task()) == 1) {
        heading += HALF_TURN;
    }
    return (Uint16)heading;
}

void bigbogy_arena_project(struct task *task, const NJS_POINT3 *position,
                           NJS_POINT3 *out)
{
    NJS_POINT3 point;
    NJS_VECTOR radial;
    point = *position;
    if (bigbogy_arena_distance(&point, &radial) == bigbogy_circle_radius) {
        *out = point;
        return;
    }
    bigbogy_arena_along(&radial, bigbogy_circle_radius, &point);
    point.y = position->y;
    *out = point;
}

Angle bigbogy_arena_advance(struct task *task, const NJS_POINT3 *position,
                            Angle angle, NJS_POINT3 *out)
{
    NJS_POINT3 projected;
    NJS_VECTOR radial;
    NJS_POINT3 point;
    Angle heading;
    Angle turn;
    struct task *boss;

    heading = bigbogy_arena_tangent(NULL, position);
    bigbogy_arena_project(task, position, &projected);
    boss = bigbogy_task();
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (boss) {
        if (bigbogy_reversed() == 1) {
            turn = angle;
        } else {
            turn = -angle;
        }
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: without the boss, turn is 0 */
    else {
        turn = 0;
    }
#endif
    bigbogy_arena_distance(&projected, &radial);
    /* @bug uninitialized: turn is read unset where bigbogy_task() is null.
     * the branch at 0x8c5098e2 skips both its assignments. the rotation at
     * 0x8c50990c and the sum returned at 0x8c509944 read it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above by the else above, where turn is 0
     * without the boss. for a finite vector, the kernel gives the bits of the
     * matrix calls below, as engine/dc/turn.h says */
    zam_vec3_turn_y(turn, &radial.x, &radial.x);
#else
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, turn);
    njCalcVector(NULL, &radial, &radial);
    njPopMatrix(1);
#endif
    bigbogy_arena_point(&radial, &point);
    *out = point;
    return heading + turn;
}

Angle bigbogy_arena_turn(const NJS_POINT3 *position, Angle angle,
                         NJS_POINT3 *out)
{
    /* @hack redundant: the point unused, which only sizes the frame */
    NJS_POINT3 unused;
    NJS_VECTOR radial;
    NJS_POINT3 point;
    Angle heading;
    Angle turn;
    struct task *boss;
    (void)unused;

    heading = bigbogy_arena_tangent(NULL, position);
    boss = bigbogy_task();
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (boss) {
        if (bigbogy_reversed() == 1) {
            turn = angle;
        } else {
            turn = -angle;
        }
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: without the boss, turn is 0 */
    else {
        turn = 0;
    }
#endif
    bigbogy_arena_distance(position, &radial);
    /* @bug uninitialized: turn is read unset where bigbogy_task() is null.
     * the branch at 0x8c50996a skips both its assignments. the rotation at
     * 0x8c509990 and the sum returned at 0x8c5099c8 read it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above by the else above, where turn is 0
     * without the boss. for a finite vector, the kernel gives the bits of the
     * matrix calls below, as engine/dc/turn.h says */
    zam_vec3_turn_y(turn, &radial.x, &radial.x);
#else
    matrix_push_unit();
    njRotateY(NULL, turn);
    njCalcVector(NULL, &radial, &radial);
    njPopMatrixEx();
#endif
    bigbogy_arena_point(&radial, &point);
    *out = point;
    return heading + turn;
}

Angle bigbogy_tangent_distance(Angle angle, const NJS_POINT3 *position)
{
    Angle heading = bigbogy_arena_tangent(NULL, position);
    return angle_distance(heading, angle);
}

void bigbogy_step_between(const NJS_POINT3 *from, const NJS_POINT3 *to,
                          Uint16 frames, NJS_VECTOR *step)
{
    NJS_VECTOR delta;
    Float inverse;
    /* @hack redundant: frames times 1.0F, where (Float)frames would do */
    inverse = njInvertSqrt(frames * 1.0F) * njInvertSqrt(frames * 1.0F);
    delta.x = to->x - from->x;
    delta.y = to->y - from->y;
    delta.z = to->z - from->z;
    delta.x *= inverse;
    delta.y *= inverse;
    delta.z *= inverse;
    *step = delta;
}

Angle bigbogy_arena_angle_between(const NJS_POINT3 *from, const NJS_POINT3 *to)
{
    Angle first;
    Angle second;
    first = BEARING(from);
    second = BEARING(to);
    return angle_distance(first, second);
}

void bigbogy_add_speed(NJS_VECTOR *velocity, Angle heading, Float speed)
{
    velocity->x += njSin(heading) * speed;
    velocity->z += njCos(heading) * speed;
}

void bigbogy_set_speed(NJS_VECTOR *velocity, Angle heading, Float speed)
{
    velocity->x = njSin(heading) * speed;
    velocity->z = njCos(heading) * speed;
}
