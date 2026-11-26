#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>

/* cos_half and sin_half are the cosine and sine of half the turn's angle.
 * scale, 1 / sin_half, makes im a unit axis where qua has unit length. where
 * re * re is 1 or more, sin_half and scale are 0, and the turn is none. m is
 * Rodrigues' matrix of the turn, transposed, since a point multiplies it as a
 * row. cur becomes m times XMTRX, the current matrix, so the turn comes
 * first. ld_ext then loads cur into XMTRX */
void njQuaternionEx(NJS_QUATERNION *qua)
{
    NJS_MATRIX m = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
                    0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F};
    /* @hack type: cur passed to st_ext and ld_ext as its sixteen floats,
     * where its rows would do */
    NJS_MATRIX cur;
    Float scale;
    Float versine;
    /* @hack redundant: register on cos_half, sin_half, cos_sq, cosine, sine,
     * x, y and z */
    register Float cos_half;
    register Float sin_half;
    register Float cos_sq;
    register Float cosine;
    register Float sine;
    register Float x;
    register Float y;
    register Float z;
    Float sine_x;
    Float versine_yz;

#ifdef DREAMCAST
#ifdef NONMATCHING
    /* this side passes cur's rows, the type st_ext takes */
    st_ext((Float(*)[4])cur);
#else
    st_ext(cur);
#endif
#endif
    cos_half = qua->re;
    cos_sq = cos_half * cos_half;
    if (cos_sq > 0.0F) {
        if (cos_sq < 1.0F) {
            sin_half = _builtin_sqrtf(1.0F - cos_sq);
            scale = 1.0F / sin_half;
        } else {
            cos_sq = 1.0F;
            scale = sin_half = 0.0F;
        }
    } else {
        scale = sin_half = 1.0F;
    }
    /* @hack spelling: y and z scaled before x, and x scaled in a statement of
     * its own, where each scaled where it is read would do */
    y = qua->im[1] * scale;
    z = qua->im[2] * scale;
    x = qua->im[0];
    x *= scale;
    cosine = (2.0F * cos_sq) - 1.0F;
    sine = 2.0F * cos_half * sin_half;
    versine = 1.0F - cosine;
    m[0] = (versine * x * x) + cosine;
    m[4] = (versine * x * y) - (sine * z);
    m[8] = (versine * x * z) + (sine * y);
    m[12] = 0.0F;
    m[1] = (versine * x * y) + (sine * z);
    m[5] = (versine * y * y) + cosine;
    /* @hack spelling: sine_x and versine_yz set in one statement with m[9],
     * where their products written out in m[9] and m[6] would do */
    sine_x = sine * x, m[9] = (versine_yz = versine * y * z) - sine_x;
    m[13] = 0.0F;
    m[2] = (versine * x * z) - (sine * y);
    m[6] = versine_yz + sine_x;
    m[10] = (versine * z * z) + cosine;
    m[14] = 0.0F;
    m[11] = m[7] = m[3] = 0.0F;
    m[15] = 1.0F;
    ftrv(&m[0], &cur[0]);
    ftrv(&m[4], &cur[4]);
    ftrv(&m[8], &cur[8]);
    ftrv(&m[12], &cur[12]);
    /* @hack redundant: a test that never runs, of a sum that reads each of
     * cur's floats, cur[4] and cur[5] again, and the constants 0 and 1 */
    if (0) {
        if (cur[0] + cur[1] + cur[2] + cur[3] + cur[4] + cur[5] + cur[6] +
            cur[7] + cur[8] + cur[9] + cur[10] + cur[11] + cur[12] + cur[13] +
            cur[14] + cur[15] + (cur[4] * 0.0F) + (cur[5] * 0.0F) +
            (1.0F - cur[4])) {
        }
    }
#ifdef DREAMCAST
#ifdef NONMATCHING
    /* this side passes cur's rows, the type ld_ext takes */
    ld_ext((Float(*)[4])cur);
#else
    ld_ext(cur);
#endif
#endif
}
