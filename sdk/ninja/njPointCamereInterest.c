#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>

void njPointCameraInterest(NJS_CAMERA *c, Float x, Float y, Float z)
{
    /* @hack spelling: sin_roll and cos_roll declared ahead of the other
     * locals, not after them */
    Float sin_roll;
    Float cos_roll;
    Float inv_distance;
    Float xz_length2;
    Float xz_length;
    Float inv_length;
    /* the direction's pitch is its turn from the xz plane toward -y, and its
     * yaw its turn about y from +z toward +x */
    Float cos_pitch;
    Float sin_pitch;
    Float inv_xz_length;
    Float cos_yaw;
    Float sin_yaw;

    c->vx = x - c->px;
    c->vy = y - c->py;
    c->vz = z - c->pz;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code adds x's square to y's
     * by an FMAC, then z's square by another */
    inv_distance = fsrra(__builtin_fmaf(
        c->vz, c->vz, __builtin_fmaf(c->vx, c->vx, c->vy * c->vy)));
#else
    inv_distance = fsrra((c->vx * c->vx) + (c->vy * c->vy) + (c->vz * c->vz));
#endif
    c->vx *= inv_distance;
    c->vy *= inv_distance;
    c->vz *= inv_distance;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code adds x's square to z's
     * by an FMAC */
    xz_length2 = __builtin_fmaf(c->vx, c->vx, c->vz * c->vz);
#else
    xz_length2 = (c->vx * c->vx) + (c->vz * c->vz);
#endif
    /* @hack spelling: xz_length, a local for the root, where the call in
     * cos_pitch's product would do */
    xz_length = _builtin_sqrtf(xz_length2);
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code adds y's square to
     * xz_length2 by an FMAC */
    inv_length = fsrra(__builtin_fmaf(c->vy, c->vy, xz_length2));
#else
    inv_length = fsrra(xz_length2 + (c->vy * c->vy));
#endif
    cos_pitch = xz_length * inv_length;
    sin_pitch = -c->vy * inv_length;
    if (xz_length2 > NO_YAW_LENGTH2_MAX) {
        inv_xz_length = fsrra(xz_length2);
        cos_yaw = c->vz * inv_xz_length;
        sin_yaw = c->vx * inv_xz_length;
    } else {
        cos_yaw = 1.0F;
        sin_yaw = 0.0F;
    }
    fsca(c->roll, &sin_roll, &cos_roll);
    /* @bug logic: gives the first product of lx.x and of ly.x the wrong sign.
     * lx and ly are meant to be the vectors that the turns of njSetCamera's
     * view of c take to (-1, 0, 0) and (0, 1, 0). the first vector's x
     * subtracts sin_roll * sin_pitch * sin_yaw, but FSUB at 0x8c15d580 takes
     * cos_roll * cos_yaw from that product as it stands. the second vector's
     * x adds cos_roll * sin_pitch * sin_yaw, but FNEG at 0x8c15d5b0 negates
     * it. lx is perpendicular to the direction only where sin_roll,
     * sin_pitch or sin_yaw is 0, and ly only where cos_roll, sin_pitch or
     * sin_yaw is 0 */
    c->lx.x = (sin_roll * sin_pitch * sin_yaw) - (cos_roll * cos_yaw);
    c->lx.y = -sin_roll * cos_pitch;
    c->lx.z = (cos_roll * sin_yaw) - (sin_roll * sin_pitch * cos_yaw);
    c->ly.x = (-cos_roll * sin_pitch * sin_yaw) - (sin_roll * cos_yaw);
    c->ly.y = cos_roll * cos_pitch;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the second
     * product's last multiply and the sum into one FMAC */
    c->ly.z = __builtin_fmaf(cos_roll * sin_pitch, cos_yaw, sin_roll * sin_yaw);
#else
    c->ly.z = (sin_roll * sin_yaw) + (cos_roll * sin_pitch * cos_yaw);
#endif
}
