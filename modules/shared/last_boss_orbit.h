/* shared/last_boss_arena.h's orbit calls, for each fight's arena.c to include
 * once. the includer names the arena's center, ARENA_CENTER, and the call
 * that returns the boss's task, ARENA_BOSS. the Biolizard's also names
 * ARENA_REVERSED, the call that returns 1 while its orbit runs the other
 * way */
#include <mathf.h>
#include "shared/last_boss_arena.h"
#include "system/angle.h"
#include "system/matrix.h"
#include "system/vector.h"
#ifdef NONMATCHING
/* zam_vec3_turn_y, by which this side turns a vector. for a finite vector it
 * gives the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

Float last_boss_radius(const NJS_POINT3 *position, NJS_VECTOR *offset)
{
    NJS_VECTOR radial;
    Float radius;
    vector_diff(position, &ARENA_CENTER, &radial);
    radius = njSqrt((radial.x * radial.x) + (radial.z * radial.z));
    if (offset) {
        *offset = radial;
    }
    return radius;
}

void last_boss_world_point(const NJS_VECTOR *offset, NJS_POINT3 *out)
{
    NJS_POINT3 point;
    vector_sum(&ARENA_CENTER, offset, &point);
    *out = point;
}

void last_boss_along(const NJS_VECTOR *direction, Float distance,
                     NJS_POINT3 *out)
{
    NJS_POINT3 point;
    NJS_VECTOR radial;
    radial = *direction;
    njUnitVector(&radial);
    radial.x *= distance;
    radial.y *= distance;
    radial.z *= distance;
    vector_sum(&ARENA_CENTER, &radial, &point);
    *out = point;
}

void last_boss_at_angle(Angle angle, Float radius, NJS_POINT3 *out)
{
    NJS_POINT3 point;
    point.x = ARENA_CENTER.x + (radius * njSin(angle));
    point.y = ARENA_CENTER.y;
    point.z = ARENA_CENTER.z + (radius * njCos(angle));
    *out = point;
}

void last_boss_at_radius(const NJS_POINT3 *position, Float distance,
                         NJS_POINT3 *out)
{
    NJS_POINT3 point;
    NJS_VECTOR radial;
    last_boss_radius(position, &radial);
    njUnitVector(&radial);
    radial.x *= distance;
    radial.y *= distance;
    radial.z *= distance;
    vector_sum(&ARENA_CENTER, &radial, &point);
    *out = point;
}

Float last_boss_distance(const NJS_POINT3 *first, const NJS_POINT3 *second,
                         NJS_VECTOR *offset)
{
    NJS_VECTOR delta;
    vector_diff(first, second, &delta);
    if (offset) {
        *offset = delta;
    }
    return njScalor(&delta);
}

#define ARENA_BEARING(point)                                                   \
    ((Angle)(atan2f((point)->x - ARENA_CENTER.x,                               \
                    (point)->z - ARENA_CENTER.z) *                             \
             ANGLE_PER_RADIAN))

Angle last_boss_tangent(const NJS_POINT3 *position)
{
    /* @hack redundant: unused, a vector nothing reads, which sizes the frame */
    NJS_VECTOR unused;
#ifdef ARENA_REVERSED
    Angle tangent;
#else
    /* @hack redundant: tangent declared register, where a plain local would
     * do */
    register Angle tangent;
#endif
    (void)unused;
    tangent = ARENA_BEARING(position) + QUARTER_TURN;
    /* @hack type: tangent cut to a Uint16 here, where the return's cut alone
     * would do */
    tangent = (Uint16)tangent;
    /* @hack redundant: a call of ARENA_BOSS whose result nothing reads */
    ARENA_BOSS();
#ifdef ARENA_REVERSED
    if (ARENA_REVERSED() == 1) {
        tangent += HALF_TURN;
    }
#endif
    return (Uint16)tangent;
}

void last_boss_tangent_vector(const NJS_POINT3 *position, NJS_VECTOR *out)
{
    Angle tangent = last_boss_tangent(position);
    out->x = njSin(tangent);
    out->y = 0.0F;
    out->z = njCos(tangent);
}

Angle last_boss_angle_distance(const NJS_POINT3 *first,
                               const NJS_POINT3 *second)
{
    Angle from = last_boss_bearing(first);
    Angle to = last_boss_bearing(second);
    /* @hack spelling: difference, a local the result passes through, where
     * returning the call's result would do */
    Angle difference = angle_distance(from, to);
    return difference;
}

Angle last_boss_angle_difference(const NJS_POINT3 *first,
                                 const NJS_POINT3 *second)
{
    Angle from = last_boss_bearing(first);
    Angle to = last_boss_bearing(second);
    /* @hack spelling: difference, a local the result passes through, where
     * returning the call's result would do */
    Angle difference = angle_difference(from, to);
    return difference;
}

Angle last_boss_angle_magnitude(Angle angle)
{
    angle = (Uint16)angle;
    if (angle > HALF_TURN) {
        angle = -angle;
    }
    return (Uint16)angle;
}

Angle last_boss_advance(const NJS_POINT3 *position, Angle step, NJS_POINT3 *out)
{
    /* @hack redundant: unused, a vector nothing reads, which sizes the frame */
    NJS_VECTOR unused;
    NJS_VECTOR offset;
    NJS_POINT3 point;
    Angle tangent = last_boss_tangent(position);
#ifdef ARENA_REVERSED
    Angle rotation;
    if (ARENA_REVERSED() == 1) {
        rotation = step;
    } else {
        rotation = -step;
    }
#else
    /* @hack redundant: a test of step and out that no path reaches */
    if (0) {
        if (step && out) {
        }
    }
    step = -step;
#endif
    (void)unused;
    last_boss_radius(position, &offset);
#ifdef ARENA_REVERSED
#define ORBIT_STEP rotation
#else
#define ORBIT_STEP step
#endif
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_y(ORBIT_STEP, &offset.x, &offset.x);
#else
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, ORBIT_STEP);
    njCalcVector(NULL, &offset, &offset);
    njPopMatrix(1);
#endif
    last_boss_world_point(&offset, &point);
    *out = point;
    return tangent + ORBIT_STEP;
#undef ORBIT_STEP
}

Angle last_boss_turn(const NJS_POINT3 *position, Angle rotation,
                     NJS_POINT3 *out)
{
    /* @hack redundant: unused, a vector nothing reads, which sizes the frame */
    NJS_VECTOR unused;
    NJS_VECTOR offset;
    NJS_POINT3 point;
    /* @hack spelling: copy, a copy of rotation, where rotation would do */
    Angle copy = rotation;
    Angle step;
    (void)unused;
    /* @bug uninitialized: position turns by step, which no path sets, and the
     * returned angle adds it. the Biolizard reads it at 0x8c503a18 and
     * 0x8c503a6c, and Finalhazard at 0x8c505e3e and 0x8c505e92 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: step is 0 */
    step = 0;
#endif
    last_boss_radius(position, &offset);
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_y(step, &offset.x, &offset.x);
#else
    njPushMatrix(&_nj_unit_matrix_);
    /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
    njRotateY(NULL, step);
    njCalcVector(NULL, &offset, &offset);
    njPopMatrix(1);
#endif
    last_boss_world_point(&offset, &point);
    *out = point;
    return copy + step;
}

Angle last_boss_bearing(const NJS_POINT3 *position)
{
    return ARENA_BEARING(position);
}

Angle last_boss_step_angle(Angle angle, Angle step)
{
#ifdef ARENA_REVERSED
    /* @hack spelling: heading set to ARENA_REVERSED's answer and tested,
     * where a test of the call would do */
    Angle heading = ARENA_REVERSED();
    if (heading) {
        heading = angle - step;
    } else {
        heading = angle + step;
    }
#else
    Angle heading = angle + step;
    /* @hack redundant: a test of angle and step that no path reaches */
    if (0) {
        if (angle == step) {
        }
    }
#endif
    return heading;
}

void last_boss_vector_length(NJS_VECTOR *direction, Float length,
                             NJS_VECTOR *out)
{
    NJS_VECTOR scaled;
    scaled = *direction;
    if (length == 0.0F) {
        *out = scaled;
        return;
    }
    if (njScalor(&scaled) == 0.0F) {
        *out = scaled;
        return;
    }
    njUnitVector(&scaled);
    scaled.x *= length;
    scaled.y *= length;
    scaled.z *= length;
    *out = scaled;
}
