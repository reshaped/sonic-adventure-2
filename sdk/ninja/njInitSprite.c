#include <nj_global.h>
#include <ninja.h>
#include <sg_xpt.h>

const NJS_ARGB sprite_white = {1.0F, 1.0F, 1.0F, 1.0F};

/* where a corner lies: OUTSIDE where its z is below _nj_near_clip_, and
 * INSIDE where it isn't */
enum { INSIDE, OUTSIDE };

/* an order, an entry of sprite_clip_orders, tells sprite_clip_near what to
 * send of a triangle. it holds two bits for each side, from bit 0. with
 * SEND_CROSSING, the clipper sends the point where the side crosses the near
 * clip. with SEND_END, it sends the corner the side ends at. FOUR_POINTS marks
 * a triangle the near clip cuts to four points, whose third point goes last */
#define SEND_CROSSING 1U
#define SEND_END 2U
#define FOUR_POINTS 0x80U

#define SIDE_BITS(start, end)                                                  \
    (((start) != (end) ? SEND_CROSSING : 0U) |                                 \
     ((end) == INSIDE ? SEND_END : 0U))

/* the order of a triangle whose corners a, b and c lie inside or outside. its
 * sides run c to a, a to b and b to c */
#define CLIP_ORDER(a, b, c)                                                    \
    (SIDE_BITS(c, a) | (SIDE_BITS(a, b) << 2) | (SIDE_BITS(b, c) << 4) |       \
     ((a) + (b) + (c) == 1 ? FOUR_POINTS : 0U))

/* an index's bits 2, 1 and 0 are set where the first, second and third
 * corners lie outside */
const Uint8 sprite_clip_orders[8] = {
    CLIP_ORDER(INSIDE, INSIDE, INSIDE),   CLIP_ORDER(INSIDE, INSIDE, OUTSIDE),
    CLIP_ORDER(INSIDE, OUTSIDE, INSIDE),  CLIP_ORDER(INSIDE, OUTSIDE, OUTSIDE),
    CLIP_ORDER(OUTSIDE, INSIDE, INSIDE),  CLIP_ORDER(OUTSIDE, INSIDE, OUTSIDE),
    CLIP_ORDER(OUTSIDE, OUTSIDE, INSIDE), CLIP_ORDER(OUTSIDE, OUTSIDE, OUTSIDE),
};

void njInitSprite(void)
{
}
