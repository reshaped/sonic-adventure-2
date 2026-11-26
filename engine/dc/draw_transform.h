#ifndef DC_DRAW_TRANSFORM_H
#define DC_DRAW_TRANSFORM_H
#ifdef DREAMCAST
#include <nj_render_types.h>
#include <sh4zam/shz_xmtrx.h>
#include <stddef.h>

/* Ninja passes points to the kernels by the address of their first float. */
typedef char ninja_xyz_layout[(sizeof(Float) == 4 && sizeof(NJS_POINT3) == 12 &&
                               offsetof(NJS_POINT3, x) == 0 &&
                               offsetof(NJS_POINT3, y) == 4 &&
                               offsetof(NJS_POINT3, z) == 8)
                                  ? 1
                                  : -1];
#else
#include "fpu.h"
#endif

/* the view z of the near plane the direct and line drawers clip at. a point
 * whose z is below it is behind the plane */
#define NEAR_Z 1.0F

struct vector4 {
    float x;
    float y;
    float z;
    float w;
};

/* the row (x, y, z, w) times XMTRX. one FTRV computes it, as in the shipped
 * point and line kernels. The caller sets Ninja's current matrix in XMTRX.
 * FTRV reads it without switching banks. Off the console, fpu.h's dc_ftrv
 * models the FTRV on dc_xmtrx */
static struct vector4 xmtrx_transform(float x, float y, float z, float w)
{
    struct vector4 result;
#ifdef DREAMCAST
    result = shz_vec4_to(struct vector4,
                         shz_xmtrx_transform_vec4(shz_vec4_init(x, y, z, w)));
#else
    float point[4];

    point[0] = x;
    point[1] = y;
    point[2] = z;
    point[3] = w;
    dc_ftrv(dc_xmtrx, point);
    result.x = point[0];
    result.y = point[1];
    result.z = point[2];
    result.w = point[3];
#endif
    return result;
}

#endif
