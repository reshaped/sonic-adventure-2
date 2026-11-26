/* rotation angles from a matrix or a direction, an angle part of the way to
 * another, and a point's distance from a segment and from a plane */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"
#include "system/math.h"

/* the angle of the direction (x, y), as atan2f gives it */
#define ATAN2_ANGLE(y, x) ((Angle)(atan2f(y, x) * ANGLE_PER_RADIAN))

/* whether ang lies within an eighth of a turn of 0 or of half a turn, where
 * its cosine's magnitude is at least its sine's */
#define COSINE_LEADS(ang) ((((ang) + EIGHTH_TURN) & QUARTER_TURN) == 0)

/* nothing calls these two, and no header declares them. they stay external,
 * since SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the angles of the current matrix as current_matrix_angles_xyz gives them,
 * for a turn about z, then x, then y */
void current_matrix_angles_zxy(Angle *ax, Angle *ay, Angle *az);
/* set a point of its own as the current motion. then read two translations into
 * its two points, and use neither */
void fun_8c012960(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* each matrix_angles_ function reads two of its angles, a and then b, from one
 * row of the matrix. that row holds cos b cos a, cos b sin a and sin b. they
 * come in some order, each perhaps negated.
 *
 * a is the direction of the two entries that hold cos b. b is the direction of
 * sin b against the larger of those two entries. sin b is first multiplied by
 * that entry's cos a or sin a. where that factor is negative, b takes half a
 * turn more. the third angle comes from the matrix turned back by a */

/* the matrix's first row is (cos ay cos az, cos ay sin az, -sin ay) */
void matrix_angles_xyz(const NJS_MATRIX matrix, Angle *ax, Angle *ay, Angle *az)
{
    Angle ang;
    Float sine;
    Float cosine;

    /* @hack spelling: az and ang set in one statement, where ang's
     * assignment and then *az = ang would do */
    *az = ang = ATAN2_ANGLE(matrix[1], matrix[0]);
    sine = njSin(ang);
    cosine = njCos(ang);
    if (COSINE_LEADS(ang)) {
        ang = ATAN2_ANGLE(-(matrix[2] * cosine), matrix[0]);
        if (cosine < 0.0F) {
            ang += HALF_TURN;
        }
    } else {
        ang = ATAN2_ANGLE(-(matrix[2] * sine), matrix[1]);
        if (sine < 0.0F) {
            ang += HALF_TURN;
        }
    }
    *ay = (Sint16)ang;
    *ax = ATAN2_ANGLE((matrix[8] * sine) - (matrix[9] * cosine),
                      (matrix[5] * cosine) - (matrix[4] * sine));
}

void current_matrix_angles_xyz(Angle *ax, Angle *ay, Angle *az)
{
    NJS_MATRIX matrix;

    njGetMatrix(&matrix);
    matrix_angles_xyz(matrix, ax, ay, az);
}

/* the matrix's first row is (cos az cos ay, sin az, -cos az sin ay) */
static void matrix_angles_xzy(const NJS_MATRIX matrix, Angle *ax, Angle *ay,
                              Angle *az)
{
    Angle ang;
    Float sine;
    Float cosine;

    /* @hack spelling: ay and ang set in one statement, as in
     * matrix_angles_xyz, where two statements would do */
    *ay = ang = ATAN2_ANGLE(-matrix[2], matrix[0]);
    sine = njSin(ang);
    cosine = njCos(ang);
    if (COSINE_LEADS(ang)) {
        ang = ATAN2_ANGLE(matrix[1] * cosine, matrix[0]);
        if (cosine < 0.0F) {
            ang += HALF_TURN;
        }
    } else {
        ang = ATAN2_ANGLE(matrix[1] * sine, -matrix[2]);
        if (sine < 0.0F) {
            ang += HALF_TURN;
        }
    }
    *az = (Sint16)ang;
    *ax = ATAN2_ANGLE((matrix[6] * cosine) + (matrix[4] * sine),
                      (matrix[10] * cosine) + (matrix[8] * sine));
}

void current_matrix_angles_xzy(Angle *ax, Angle *ay, Angle *az)
{
    NJS_MATRIX matrix;

    njGetMatrix(&matrix);
    matrix_angles_xzy(matrix, ax, ay, az);
}

/* the matrix's second row is (-cos ax sin az, cos ax cos az, sin ax) */
static void matrix_angles_yxz(const NJS_MATRIX matrix, Angle *ax, Angle *ay,
                              Angle *az)
{
    Angle ang;
    Float sine;
    Float cosine;

    /* @hack spelling: az and ang set in one statement, as in
     * matrix_angles_xyz, where two statements would do */
    *az = ang = ATAN2_ANGLE(-matrix[4], matrix[5]);
    sine = njSin(ang);
    cosine = njCos(ang);
    if (COSINE_LEADS(ang)) {
        ang = ATAN2_ANGLE(matrix[6] * cosine, matrix[5]);
        if (cosine < 0.0F) {
            ang += HALF_TURN;
        }
    } else {
        ang = ATAN2_ANGLE(matrix[6] * sine, -matrix[4]);
        if (sine < 0.0F) {
            ang += HALF_TURN;
        }
    }
    *ax = (Sint16)ang;
    *ay = ATAN2_ANGLE((matrix[8] * cosine) + (matrix[9] * sine),
                      (matrix[0] * cosine) + (matrix[1] * sine));
}

void current_matrix_angles_yxz(Angle *ax, Angle *ay, Angle *az)
{
    NJS_MATRIX matrix;

    njGetMatrix(&matrix);
    matrix_angles_yxz(matrix, ax, ay, az);
}

/* the matrix's third row is (cos ax sin ay, -sin ax, cos ax cos ay) */
static void matrix_angles_zxy(const NJS_MATRIX matrix, Angle *ax, Angle *ay,
                              Angle *az)
{
    Angle ang;
    Float sine;
    Float cosine;

    /* @hack spelling: ay and ang set in one statement, as in
     * matrix_angles_xyz, where two statements would do */
    *ay = ang = ATAN2_ANGLE(matrix[8], matrix[10]);
    sine = njSin(ang);
    cosine = njCos(ang);
    if (COSINE_LEADS(ang)) {
        ang = ATAN2_ANGLE(-(matrix[9] * cosine), matrix[10]);
        if (cosine < 0.0F) {
            ang += HALF_TURN;
        }
    } else {
        ang = ATAN2_ANGLE(-(matrix[9] * sine), matrix[8]);
        if (sine < 0.0F) {
            ang += HALF_TURN;
        }
    }
    *ax = (Sint16)ang;
    *az = ATAN2_ANGLE((matrix[6] * sine) - (matrix[4] * cosine),
                      (matrix[0] * cosine) - (matrix[2] * sine));
}

void current_matrix_angles_zxy(Angle *ax, Angle *ay, Angle *az)
{
    NJS_MATRIX matrix;

    njGetMatrix(&matrix);
    matrix_angles_zxy(matrix, ax, ay, az);
}

/* the z axis, turned about x and then about y, is (cos ax sin ay, -sin ax,
 * cos ax cos ay) */
void direction_angles_xy(Float x, Float y, Float z, Angle *ax, Angle *ay)
{
    Angle ang;
    Float factor;

    /* @hack spelling: ay and ang set in one statement, as in
     * matrix_angles_xyz, where two statements would do */
    *ay = ang = ATAN2_ANGLE(x, z);
    if (COSINE_LEADS(ang)) {
        factor = njCos(ang);
        ang = ATAN2_ANGLE(-(y * factor), z);
    } else {
        factor = njSin(ang);
        ang = ATAN2_ANGLE(-(y * factor), x);
    }
    if (factor < 0.0F) {
        ang = (Sint16)(ang + HALF_TURN);
    }
    *ax = ang;
}

/* the y axis, turned about x and then about z, is (-cos ax sin az, cos ax
 * cos az, sin ax) */
void direction_angles_xz(Float x, Float y, Float z, Angle *ax, Angle *az)
{
    Angle ang;
    Float factor;

    /* @hack spelling: az and ang set in one statement, as in
     * matrix_angles_xyz, where two statements would do */
    *az = ang = ATAN2_ANGLE(-x, y);
    if (COSINE_LEADS(ang)) {
        factor = njCos(ang);
        ang = ATAN2_ANGLE(z * factor, y);
    } else {
        factor = njSin(ang);
        ang = ATAN2_ANGLE(z * factor, -x);
    }
    if (factor < 0.0F) {
        ang = (Sint16)(ang + HALF_TURN);
    }
    *ax = ang;
}

Angle angle_interpolate(Angle from, Angle to, Float part, Float rest)
{
    int turn;

    turn = (Uint16)(to - from);
    /* @hack spelling: the test as turn <= HALF_TURN - 1, where turn <
     * HALF_TURN would do */
    if (turn <= HALF_TURN - 1) {
        return (Angle)(((Float)turn * part / (part + rest)) + (Float)from);
    }
    return (Angle)((-((Float)(FULL_TURN - turn) * part) / (part + rest)) +
                   (Float)from);
}

Float point_segment_distance2(const NJS_POINT3 *point, const NJS_LINE *line,
                              NJS_POINT3 *nearest)
{
    NJS_VECTOR to_point;
    NJS_POINT3 end;
    NJS_POINT3 near;
    Float along;
    Float length2;

    to_point.x = point->x - line->px;
    to_point.y = point->y - line->py;
    to_point.z = point->z - line->pz;
    /* how far along the segment point lies, times the vector's length: 0 at
     * the segment's start, and the length's square at its end */
    along = (to_point.x * line->vx) + (to_point.y * line->vy) +
            (to_point.z * line->vz);
    /* @hack redundant: the else after the arms that return */
    if (along > 0.0F) {
        length2 = (line->vx * line->vx) + (line->vy * line->vy) +
                  (line->vz * line->vz);
        /* @hack redundant: the else after the arm that returns */
        if (along >= length2) {
            /* @hack spelling: a from of its own in each arm, where one for
             * the function would do */
            NJS_VECTOR from;

            end.x = line->px + line->vx;
            end.y = line->py + line->vy;
            end.z = line->pz + line->vz;
            if (nearest != NULL) {
                *nearest = end;
            }
            from.x = point->x - end.x;
            from.y = point->y - end.y;
            from.z = point->z - end.z;
            return (from.x * from.x) + (from.y * from.y) + (from.z * from.z);
        } else {
            NJS_VECTOR from;
            Float part;

            part = along / length2;
            near.x = line->px + (part * line->vx);
            near.y = line->py + (part * line->vy);
            near.z = line->pz + (part * line->vz);
            if (nearest != NULL) {
                *nearest = near;
            }
            from.x = point->x - near.x;
            from.y = point->y - near.y;
            from.z = point->z - near.z;
            return (from.x * from.x) + (from.y * from.y) + (from.z * from.z);
        }
    } else {
        if (nearest != NULL) {
            nearest->x = line->px;
            nearest->y = line->py;
            nearest->z = line->pz;
        }
        return (to_point.x * to_point.x) + (to_point.y * to_point.y) +
               (to_point.z * to_point.z);
    }
}

Float point_plane_distance(const NJS_POINT3 *point, const NJS_PLANE *plane,
                           NJS_POINT3 *foot)
{
    NJS_VECTOR to_point;
    Float along;
    Float length2;
    Float part;

    to_point.x = point->x - plane->px;
    to_point.y = point->y - plane->py;
    to_point.z = point->z - plane->pz;
    along = (to_point.x * plane->vx) + (to_point.y * plane->vy) +
            (to_point.z * plane->vz);
    length2 = (plane->vx * plane->vx) + (plane->vy * plane->vy) +
              (plane->vz * plane->vz);
    if (foot != NULL) {
        part = along / length2;
        foot->x = point->x - (part * plane->vx);
        foot->y = point->y - (part * plane->vy);
        foot->z = point->z - (part * plane->vz);
    }
    return along * njInvertSqrt(length2);
}

void fun_8c012960(void)
{
    /* @hack spelling: second declared ahead of first, not after it */
    NJS_POINT3 second;
    NJS_POINT3 first;

    /* @bug mismatched-call: passes first, a point nothing has set, where
     * njSetCnkCurrentMotion takes an NJS_MOTION. the call at 0x8c012968
     * reads first's y as the motion's nbFrame (MOV.L @(4,R4),R2 at
     * 0x8c11d996) */
    njSetCnkCurrentMotion((NJS_MOTION *)&first, 0.0F);
    njGetCnkMotionTranslate(&second.x, 0);
    njGetTranslation(NULL, &first);
}
