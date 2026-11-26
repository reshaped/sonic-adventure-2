#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_line.h>
#include <sg_xpt.h>

/* sum + a * b, rounded once, as SHC's FMAC rounds it. GCC rounds the product
 * and the sum apart unless told to fuse them */
#ifdef NONMATCHING
/* this side keeps the console's bits by fusing them */
#define LINE_ADD_PRODUCT(sum, a, b) __builtin_fmaf((a), (b), (sum))
#else
#define LINE_ADD_PRODUCT(sum, a, b) ((sum) + ((a) * (b)))
#endif

/* the near plane's z, and 1/z for a point on it */
#define NEAR_Z 1.0F
#define NEAR_INVERSE_Z 1.0F
/* a store queue's words, and the words of a line's sprite the call writes */
#define SQ_WORDS 8
#define WRITTEN_WORDS 12

/* the call cuts the first point where its depth is at or below 1, and the
 * second otherwise */
void njCalcLine3DNearClip(const Float *coordinates)
{
    /* @hack redundant: register on every local, declared in this order,
     * where plain locals would do */
    register Float scale_x;
    register Float scale_y;
    register Float inverse_scale_x;
    register Float inverse_scale_y;
    register Float start_x;
    register Float start_y;
    register Float start_z;
    register Float start_inv_z;
    register Float end_x;
    register Float end_y;
    register Float end_z;
    register Float end_inv_z;
    register Float view_start_x;
    register Float view_start_y;
    register Float view_end_x;
    register Float view_end_y;
    register Float fraction;
    register Float delta_x;
    register Float delta_y;
    register Float inverse_length;
    register Float projected_start_x;
    register Float projected_start_y;
    register Float projected_end_x;
    register Float offset_x;
    register Float offset_y;
    register Float projected_end_y;
    register Float *sq;

    sq = (Float *)line_cursor;
    scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inverse_scale_x = 1.0F / scale_x;
    inverse_scale_y = 1.0F / scale_y;
    start_x = *coordinates++;
    start_y = *coordinates++;
    start_z = *coordinates++;
    start_inv_z = *coordinates++;
    end_x = *coordinates++;
    end_y = *coordinates++;
    end_z = *coordinates++;
    end_inv_z = *coordinates;
    view_start_x = (start_x - _nj_screen_.cx) * start_z * inverse_scale_x;
    view_start_y = (start_y - _nj_screen_.cy) * start_z * inverse_scale_y;
    view_end_x = (end_x - _nj_screen_.cx) * end_z * inverse_scale_x;
    view_end_y = (end_y - _nj_screen_.cy) * end_z * inverse_scale_y;
    /* @bug logic: cuts the first point, not the second, where the first's z
     * is exactly 1 and the second's is below it. njCalcLine3DClip's FCMP/GT
     * FR2,FR10 at 0x8c12e326 sends such a line here, since the first isn't
     * below 1. FCMP/GT FR1,FR10 at 0x8c12e426 takes the first for the point
     * to cut, and the second goes out uncut, behind the near plane */
    if (start_z <= NEAR_Z) {
        fraction = (NEAR_Z - start_z) / (end_z - start_z);
        view_start_x =
            LINE_ADD_PRODUCT(view_start_x, fraction, view_end_x - view_start_x);
        view_start_y =
            LINE_ADD_PRODUCT(view_start_y, fraction, view_end_y - view_start_y);
        start_inv_z = NEAR_INVERSE_Z;
    } else {
        fraction = (NEAR_Z - end_z) / (start_z - end_z);
        view_end_x =
            LINE_ADD_PRODUCT(view_end_x, fraction, view_start_x - view_end_x);
        view_end_y =
            LINE_ADD_PRODUCT(view_end_y, fraction, view_start_y - view_end_y);
        end_inv_z = NEAR_INVERSE_Z;
    }

    view_start_x *= start_inv_z;
    view_start_y *= start_inv_z;
    view_end_x *= end_inv_z;
    view_end_y *= end_inv_z;
    projected_start_x = LINE_ADD_PRODUCT(_nj_screen_.cx, scale_x, view_start_x);
    projected_start_y = LINE_ADD_PRODUCT(_nj_screen_.cy, scale_y, view_start_y);
    projected_end_x = LINE_ADD_PRODUCT(_nj_screen_.cx, scale_x, view_end_x);
    projected_end_y = LINE_ADD_PRODUCT(_nj_screen_.cy, scale_y, view_end_y);
    delta_x = projected_end_x - projected_start_x;
    delta_y = projected_end_y - projected_start_y;
    /* SHC rounds delta_y's square, then adds delta_x's to it by FMAC */
#ifdef NONMATCHING
    /* this side keeps the console's bits: it rounds the same square, and fuses
     * the other into the sum */
    inverse_length = fsrra(__builtin_fmaf(delta_x, delta_x, delta_y * delta_y));
#else
    inverse_length = fsrra((delta_x * delta_x) + (delta_y * delta_y));
#endif
    offset_x = line_half_width * delta_x * inverse_length;
    offset_y = line_half_width * delta_y * inverse_length;

    sq += SQ_WORDS;
    *--sq = projected_end_x - offset_y;
    *--sq = end_inv_z;
    *--sq = projected_end_y - offset_x;
    *--sq = projected_end_x + offset_y;
    *--sq = start_inv_z;
    *--sq = projected_start_y - offset_x;
    *--sq = projected_start_x + offset_y;
    *(Uint32 *)--sq = KM_VERTEXPARAM_ENDOFSTRIP;
    prefetch(sq);

    sq += WRITTEN_WORDS;
    *--sq = projected_start_y + offset_x;
    *--sq = projected_start_x - offset_y;
    *--sq = end_inv_z;
    *--sq = projected_end_y + offset_x;
    prefetch(sq);
    sq += SQ_WORDS;
    line_cursor = (Uint32 *)sq;
}

void njDrawLine3DExSetStrip(NJS_POINT3 *points, Sint32 count)
{
    struct line_point *projected;
    Int clipped;
    Int segments;

    projected = (struct line_point *)_nj_vertex_buf_;
    clipped = njCalcPoint3DLine(points, projected, count);
    if (clipped == 0) {
        njCalcLine3D(projected, count - 1, sizeof(*projected));
    } else if (clipped < count) {
        /* @hack spelling: segments set to count, then stepped down, where
         * segments = count - 1 would do */
        segments = count;
        --segments;
        /* @hack spelling: a goto loop, where a do loop would do */
    next_segment:
        njCalcLine3DClip(projected);
        ++projected;
        if (--segments) {
            goto next_segment;
        }
    }
}

void njDrawLine3DExSetList(NJS_POINT3 *points, Sint32 count)
{
    struct line_point *projected;
    Int clipped;
    Int segments;

    projected = (struct line_point *)_nj_vertex_buf_;
    clipped = njCalcPoint3DLine(points, projected, count);
    if (clipped == 0) {
        njCalcLine3D(projected, count >> 1, 2 * sizeof(*projected));
    } else if (clipped < count) {
        /* @hack spelling: segments set to count, then halved, where
         * segments = count >> 1 would do */
        segments = count;
        segments >>= 1;
        /* @hack spelling: a goto loop, where a do loop would do */
    next_segment:
        njCalcLine3DClip(projected);
        projected += 2;
        if (--segments) {
            goto next_segment;
        }
    }
}
