/* the camera's helpers: vectors through Ninja's calls, angles from a direction
 * and directions from angles, a power, and a point moved by a pad */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/helpers.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/pad.h"
#include "system/vector.h"

#ifdef NONMATCHING
/* this side turns a vector by zam_vec3_turn_yxz and zam_vec3_turn_zxy. for a
 * finite vector they give the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

static void look_vector(struct angles *ang, NJS_VECTOR *out);

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set out to angles that turn -z as ang does, with no turn about z. they are
 * those look_angles gives for look_vector(ang). out->x comes out within a
 * quarter turn of 0. out may be ang */
void angles_unroll(struct angles *ang, struct angles *out);
/* the angles look_angles gives for turn_x_axis(ang), into out's x and y.
 * out->z keeps what it held. out may be ang */
void x_axis_look_angles(struct angles *ang, struct angles *out);
/* NOLINTEND(misc-use-internal-linkage) */

void vector_subtract(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *out)
{
    *out = *first;
    njSubVector(out, second);
}

void vector_add(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *out)
{
    *out = *first;
    njAddVector(out, second);
}

void look_angles(NJS_VECTOR *dir, Angle *ax, Angle *ay)
{
    /* @hack spelling: xz_length, a local for the length the second atan2f
     * takes, where njSqrt's call in atan2f's argument would do */
    Float xz_length;

    *ay = (Angle)(atan2f(-dir->x, -dir->z) * ANGLE_PER_RADIAN);
    xz_length = njSqrt((dir->x * dir->x) + (dir->z * dir->z));
    *ax = (Angle)(atan2f(dir->y, xz_length) * ANGLE_PER_RADIAN);
}

void angles_unroll(struct angles *ang, struct angles *out)
{
    NJS_VECTOR dir;

    look_vector(ang, &dir);
    look_angles(&dir, &out->x, &out->y);
    out->z = 0;
}

void turn_x_axis(struct angles *ang, NJS_VECTOR *out)
{
    out->x = 1.0F;
    out->y = 0.0F;
    out->z = 0.0F;
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls
     * below, as engine/dc/turn.h says */
    zam_vec3_turn_yxz(ang->x, -ang->y, ang->z, &out->x, &out->x);
#else
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateZ(NULL, ang->z);
    njRotateX(NULL, ang->x);
    njRotateY(NULL, -ang->y);
    njCalcVector(NULL, out, out);
    njPopMatrix(1);
#endif
}

void x_axis_look_angles(struct angles *ang, struct angles *out)
{
    NJS_VECTOR dir;

    turn_x_axis(ang, &dir);
    look_angles(&dir, &out->x, &out->y);
}

/* out = -z turned about z by ang->z, then about x by ang->x, then about y by
 * ang->y */
static void look_vector(struct angles *ang, NJS_VECTOR *out)
{
    out->x = 0.0F;
    out->y = 0.0F;
    out->z = -1.0F;
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls
     * below, as engine/dc/turn.h says */
    zam_vec3_turn_zxy(ang->x, ang->y, ang->z, &out->x, &out->x);
#else
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateY(NULL, ang->y);
    njRotateX(NULL, ang->x);
    njRotateZ(NULL, ang->z);
    njCalcVector(NULL, out, out);
    njPopMatrix(1);
#endif
}

void look_vector_xy(struct angles *ang, NJS_VECTOR *out)
{
    out->x = 0.0F;
    out->y = 0.0F;
    out->z = -1.0F;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateY(NULL, ang->y);
    njRotateX(NULL, ang->x);
    njCalcVector(NULL, out, out);
    njPopMatrix(1);
}

void vector_scale(NJS_VECTOR *in, NJS_VECTOR *out, Float scale)
{
    out->x = scale * in->x;
    out->y = scale * in->y;
    out->z = scale * in->z;
}

int int_power(int base, int exponent)
{
    int result = 1;

    /* @hack spelling: the early return for an exponent of 0 or less, then a
     * loop while exponent isn't 0, where one loop while exponent > 0 would
     * do */
    if (exponent <= 0) {
        return 1;
    }
    while (exponent != 0) {
        result *= base;
        exponent--;
    }
    return result;
}

void point_move_by_pad(NJS_POINT3 *pos, Float speed, int camera, Bool stick)
{
    Angle ang;
    Sint16 x;
    Sint16 y;

    ang = camera_views[camera]->ang.y;
    if (player_pads[camera]->on & PDD_DGT_ST) {
        return;
    }
    /* pos moves along +z turned about y by ang. turned by the camera's own
     * ang.y, +z points away from where the camera looks. down moves pos that
     * way, so it adds no turn to ang. each other direction adds its eighths
     * of a turn */
    if (stick == FALSE) {
        if (player_pads[camera]->on & PAD_DIRECTIONS) {
            /* @hack redundant: the last test of this chain, of right, which
             * holds wherever it is reached, where an else would do */
            if (player_pads[camera]->on & PDD_DGT_KU) {
                /* @bug logic: up's test for right reads pad 0's on, not
                 * camera's pad's (MOV.L @R4,R1 at 0x8c0ac468). for a camera
                 * other than 0, up with right moves pos straight ahead unless
                 * pad 0 holds right. up alone moves pos ahead and right while
                 * pad 0 holds right */
                if (player_pads[camera]->on & PDD_DGT_KL) {
                    ang += 5 * EIGHTH_TURN;
                } else if (player_pads[0]->on & PDD_DGT_KR) {
                    ang += 3 * EIGHTH_TURN;
                } else {
                    ang += 4 * EIGHTH_TURN;
                }
            } else if (player_pads[camera]->on & PDD_DGT_KD) {
                if (player_pads[camera]->on & PDD_DGT_KL) {
                    ang += 7 * EIGHTH_TURN;
                } else if (player_pads[camera]->on & PDD_DGT_KR) {
                    ang += EIGHTH_TURN;
                }
            } else if (player_pads[camera]->on & PDD_DGT_KL) {
                ang += 6 * EIGHTH_TURN;
            } else if (player_pads[camera]->on & PDD_DGT_KR) {
                ang += 2 * EIGHTH_TURN;
            }
            pos->z += speed * njCos(ang);
            pos->x += speed * njSin(ang);
        }
    } else {
        x = CAMERA_STICK_READING(player_pads[camera]->x1);
        y = CAMERA_STICK_READING(player_pads[camera]->y1);
        if (x > -CAMERA_STICK_DEAD_ZONE && x < CAMERA_STICK_DEAD_ZONE) {
            x = 0;
        }
        if (y > -CAMERA_STICK_DEAD_ZONE && y < CAMERA_STICK_DEAD_ZONE) {
            y = 0;
        }
        if (x != 0 || y != 0) {
            /* the stick turns ang as the d-pad does. a stick that leans to
             * its +y adds no turn, as down does, and one that leans to its +x
             * adds a quarter turn, as right does */
            ang -= (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN) - QUARTER_TURN;
            pos->z += speed * njCos(ang);
            pos->x += speed * njSin(ang);
        }
    }
    /* @hack redundant: the test for either trigger around the test of
     * each */
    if (player_pads[camera]->on & (PDD_DGT_TL | PDD_DGT_TR)) {
        if (player_pads[camera]->on & PDD_DGT_TL) {
            pos->y += speed;
        } else if (player_pads[camera]->on & PDD_DGT_TR) {
            pos->y -= speed;
        }
    }
}

void z_axis_angles(NJS_VECTOR *dir, Angle *ax, Angle *ay)
{
    *ax = -(Angle)(atan2f(dir->y, dir->z) * ANGLE_PER_RADIAN);
    *ay =
        (Angle)(atan2f(dir->x, njSqrt((dir->y * dir->y) + (dir->z * dir->z))) *
                ANGLE_PER_RADIAN);
}

void y_axis_angles(NJS_VECTOR *dir, Angle *ax, Angle *az)
{
    *az = -(Angle)(atan2f(dir->x, dir->y) * ANGLE_PER_RADIAN);
    *ax =
        (Angle)(atan2f(dir->z, njSqrt((dir->x * dir->x) + (dir->y * dir->y))) *
                ANGLE_PER_RADIAN);
}
