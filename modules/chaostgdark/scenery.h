/* the Dark Garden's land, its two water surfaces and its props' models. the
 * land and the water draw with dark_land_texlist, and the props with
 * AL_DARK_OBJ_TEX's textures */
#ifndef CHAOSTGDARK_SCENERY_H
#define CHAOSTGDARK_SCENERY_H
#include <ninja.h>
#include "collision/world.h"

extern struct land_table dark_land;
extern NJS_TEXLIST dark_land_texlist;
extern NJS_CNK_OBJECT dark_water_a;
extern NJS_CNK_OBJECT dark_water_b;
extern NJS_CNK_OBJECT dark_long_fence, dark_medium_fence, dark_short_fence;
extern NJS_CNK_OBJECT dark_grave_a, dark_grave_a_shadow;
extern NJS_CNK_OBJECT dark_grave_b, dark_grave_b_shadow;
extern NJS_CNK_OBJECT dark_stump, dark_kuwa_a, dark_grass_b;

#endif
