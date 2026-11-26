/* njDirectCalcPointLoop and njDirectCalcPointLoopD8 for GCC. make dc compiles
 * this in place of sdk/ninja/njDirectPointLoops.src, whose loops call each near
 * clipper by BSR. a BSR reaches 4 KB either way, and GCC's link may place a
 * clipper further off. the shipped njDirectCalcPointLoopD8 also writes R8
 * without saving it. a caller GCC compiles may keep a value in R8 across the
 * call. each float these loops hand a clipper has the shipped bits. it comes
 * from one FTRV of the same point, w included */
#include <ninja.h>
#include <nj_direct_draw.h>
#include <sg_xpt.h>

#include "draw_transform.h"

#define ALL_BEHIND                                                             \
    (DIRECT_FIRST_BEHIND | DIRECT_SECOND_BEHIND | DIRECT_THIRD_BEHIND)

#ifdef DREAMCAST
#include <stddef.h>

/* The near clippers receive the original eight-word strip records. */
typedef char direct_vertex_layout[(sizeof(NJS_DIRECT_VERTEX) == 32 &&
                                   offsetof(NJS_DIRECT_VERTEX, px) == 0 &&
                                   offsetof(NJS_DIRECT_VERTEX, py) == 4 &&
                                   offsetof(NJS_DIRECT_VERTEX, pz) == 8 &&
                                   offsetof(NJS_DIRECT_VERTEX, nz) == 12 &&
                                   offsetof(NJS_DIRECT_VERTEX, nx) == 16 &&
                                   offsetof(NJS_DIRECT_VERTEX, ny) == 20 &&
                                   offsetof(NJS_DIRECT_VERTEX, uv) == 24 &&
                                   offsetof(NJS_DIRECT_VERTEX, param) == 28)
                                      ? 1
                                      : -1];
typedef char
    direct_d8_vertex_layout[(sizeof(NJS_DIRECT_VERTEX_D8) == 32 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, px) == 0 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, py) == 4 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, pz) == 8 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, one) == 12 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, base) == 16 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, off) == 20 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, param) == 24 &&
                             offsetof(NJS_DIRECT_VERTEX_D8, uv) == 28)
                                ? 1
                                : -1];
#endif

enum direct_format { DIRECT_PLAIN, DIRECT_ENVIRONMENT, DIRECT_D8 };

/* a triangle's corner: its record's point in view space, and its param */
struct corner {
    struct vector4 view;
    Uint32 param;
};

/* without always_inline, GCC compiles each use as a call, at -O2 as at -Os.
 * the call returns each corner through clip_direct_strip's frame */
static inline __attribute__((always_inline)) struct corner
transform_corner(const void *record, enum direct_format format)
{
    struct corner corner;

    if (format == DIRECT_D8) {
        const NJS_DIRECT_VERTEX_D8 *vertex = record;

        corner.view =
            xmtrx_transform(vertex->px, vertex->py, vertex->pz, vertex->one);
        corner.param = vertex->param;
    } else {
        const NJS_DIRECT_VERTEX *vertex = record;

        corner.view = xmtrx_transform(vertex->px, vertex->py, vertex->pz, 1.0F);
        corner.param = vertex->param;
    }
    return corner;
}

static void clip_direct_strip(void *records, Int count, NJS_POINT3 *third_point,
                              enum direct_format format)
{
    Uint32 stride = format == DIRECT_D8 ? sizeof(NJS_DIRECT_VERTEX_D8)
                                        : sizeof(NJS_DIRECT_VERTEX);
    Uint8 *record = records;
    struct corner first;
    struct corner second;
    struct corner third;
    Uint32 mask;

    /* the shipped loops make no such test. below DIRECT_TRIANGLE_CORNERS
     * records their triangle count wraps, and they walk about 2^32 triangles
     * past the strip */
    if (count < DIRECT_TRIANGLE_CORNERS) {
        return;
    }
    first = transform_corner(record, format);
    second = transform_corner(record + stride, format);
    do {
        third = transform_corner(record + (2 * stride), format);
        mask = (first.view.z < NEAR_Z ? DIRECT_FIRST_BEHIND : 0) |
               (second.view.z < NEAR_Z ? DIRECT_SECOND_BEHIND : 0) |
               (third.view.z < NEAR_Z ? DIRECT_THIRD_BEHIND : 0);
        /* only a strip's last record has the param KM_VERTEXPARAM_ENDOFSTRIP.
         * where the first two corners' params differ, the three records lie
         * on two strips */
        if (first.param == second.param && mask != 0 && mask != ALL_BEHIND) {
            third_point->x = third.view.x;
            third_point->y = third.view.y;
            third_point->z = third.view.z;
            switch (format) {
            case DIRECT_D8:
                njDirectNearClipSubD8((NJS_DIRECT_VERTEX_D8 *)record, mask,
                                      first.view.x, first.view.y, first.view.z,
                                      first.view.w, second.view.x,
                                      second.view.y, second.view.z);
                break;
            case DIRECT_PLAIN:
                njDirectNearClipSub((NJS_DIRECT_VERTEX *)record, mask,
                                    first.view.x, first.view.y, first.view.z,
                                    first.view.w, second.view.x, second.view.y,
                                    second.view.z);
                break;
            case DIRECT_ENVIRONMENT:
                njDirectEnvNearClipSub((NJS_DIRECT_VERTEX *)record, mask,
                                       first.view.x, first.view.y, first.view.z,
                                       first.view.w, second.view.x,
                                       second.view.y, second.view.z);
                break;
            }
        }
        first = second;
        second = third;
        record += stride;
    } while (--count >= DIRECT_TRIANGLE_CORNERS);
}

void njDirectCalcPointLoopD8(NJS_DIRECT_VERTEX_D8 *vertices, Int count,
                             NJS_POINT3 *third)
{
    clip_direct_strip(vertices, count, third, DIRECT_D8);
}

void njDirectCalcPointLoop(NJS_DIRECT_VERTEX *vertices, Int count,
                           NJS_POINT3 *third, Int environment)
{
    clip_direct_strip(vertices, count, third,
                      environment > 0 ? DIRECT_ENVIRONMENT : DIRECT_PLAIN);
}
