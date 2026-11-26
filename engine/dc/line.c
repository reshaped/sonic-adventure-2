/* njCalcPoint3DLine, njCalcLine3D and njCalcLine3DClip for GCC. make dc
 * compiles this in place of sdk/ninja/njLine3DKernels.src, whose
 * njCalcLine3DClip calls njCalcLine3DNearClip by BSR. a BSR reaches 4 KB
 * either way, and GCC's link may place njCalcLine3DNearClip further off.
 * make dc replaces that file's object whole, so its other two kernels are C
 * here too. each float these kernels compute has the shipped bits. it comes
 * from the same operations on the same operands, in the same order */
#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_km.h>
#include <nj_line.h>
#include <sg_xpt.h>

#include "draw_transform.h"

#ifdef DREAMCAST
#include <stddef.h>
#include "kernel_layout.h"

/* The near clipper reads these projected points as consecutive floats. */
typedef char
    line_point_layout[(sizeof(struct line_point) == 16 &&
                       offsetof(struct line_point, screen_x) == 0 &&
                       offsetof(struct line_point, screen_y) == 4 &&
                       offsetof(struct line_point, depth) == 8 &&
                       offsetof(struct line_point, inverse_depth) == 12)
                          ? 1
                          : -1];
/* The line starter supplies the store queue address. send_line commits through
 * fCX before writing fCY at the head of the second queue. */
typedef char
    line_store_queues[(SQ_WORDS * sizeof(Uint32) == SH4_CACHE_LINE_BYTES &&
                       sizeof(KMVERTEX16) == 2 * SH4_CACHE_LINE_BYTES &&
                       offsetof(KMVERTEX16, fCX) == 28 &&
                       offsetof(KMVERTEX16, fCY) == SH4_CACHE_LINE_BYTES)
                          ? 1
                          : -1];
#endif

Int njCalcPoint3DLine(const NJS_POINT3 *points, struct line_point *out,
                      Int count)
{
    Int at_or_behind = 0;
    Int i;

    for (i = 0; i < count; ++i) {
        struct vector4 view =
            xmtrx_transform(points[i].x, points[i].y, points[i].z, 1.0F);
        Float inverse_depth = view.w / view.z;

        /* w is 1 where XMTRX is affine. a point whose z isn't above w is
         * then at or behind the near plane */
        if (!(view.z > view.w)) {
            ++at_or_behind;
        }
        out[i].screen_x = ((view.x * line_projection.scale_x) * inverse_depth) +
                          line_projection.center_x;
        out[i].screen_y = ((view.y * line_projection.scale_y) * inverse_depth) +
                          line_projection.center_y;
        out[i].depth = view.z;
        out[i].inverse_depth = inverse_depth;
    }
    return at_or_behind;
}

/* write the line from start to end as a sprite at line_cursor, a store queue
 * address. the sprite reaches line_half_width to each side of the line.
 * line_cursor moves past the sprite. each prefetch writes out one store queue,
 * half the sprite */
static void send_line(const struct line_point *start,
                      const struct line_point *end)
{
    Float delta_x = end->screen_x - start->screen_x;
    Float delta_y = end->screen_y - start->screen_y;
    Float inverse_length = fsrra((delta_y * delta_y) + (delta_x * delta_x));
    Float offset_x = (delta_x * inverse_length) * line_half_width;
    Float offset_y = (delta_y * inverse_length) * line_half_width;
    KMVERTEX16 *sprite = (KMVERTEX16 *)line_cursor;

    sprite->ParamControlWord = KM_VERTEXPARAM_ENDOFSTRIP;
    sprite->fAX = start->screen_x + offset_y;
    sprite->fAY = start->screen_y - offset_x;
    sprite->fAZ = start->inverse_depth;
    sprite->fBX = end->screen_x + offset_y;
    sprite->fBY = end->screen_y - offset_x;
    sprite->fBZ = end->inverse_depth;
    sprite->fCX = end->screen_x - offset_y;
    prefetch(sprite);
    sprite->fCY = end->screen_y + offset_x;
    sprite->fCZ = end->inverse_depth;
    sprite->fDX = start->screen_x - offset_y;
    sprite->fDY = start->screen_y + offset_x;
    prefetch((Uint32 *)sprite + SQ_WORDS);
    line_cursor = (Uint32 *)(sprite + 1);
}

void njCalcLine3D(const struct line_point *points, Int count, Int stride)
{
    Int i;

    for (i = 0; i < count; ++i) {
        send_line(points, points + 1);
        points = (const struct line_point *)((const Uint8 *)points + stride);
    }
}

void njCalcLine3DClip(const struct line_point *points)
{
    Bool first_behind = points[0].depth < NEAR_Z;
    Bool second_behind = points[1].depth < NEAR_Z;

    if (first_behind != second_behind) {
        njCalcLine3DNearClip((const Float *)points);
    } else if (!first_behind) {
        send_line(points, points + 1);
    }
}
