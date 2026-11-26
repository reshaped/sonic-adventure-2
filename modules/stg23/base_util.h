#ifndef STG23_BASE_UTIL_H
#define STG23_BASE_UTIL_H
#include <ninja.h>

/* ocean_model_draw_tinted_scaled, with color a Float whose value converts
 * to the packed color. no code calls it */
void base_model_draw_float_tint(NJS_CNK_MODEL *model, Float color, Float scale);

/* draw the debug cylinder scaled by scale, its base on the current origin.
 * no code calls it */
void base_cylinder_draw(NJS_VECTOR *scale);

/* to - from as the shorter turn, -0x8000 to 0x7fff. no code calls it */
Angle base_angle_delta(Angle from, Angle to);

#endif
