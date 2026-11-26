/* Green Hill's background: its PVMs, its sky and its horizon. the two water
 * areas are the objects of the stage's two kill areas */
#ifndef STG31_SKY_H
#define STG31_SKY_H

#include <ninja.h>

#include "draw/texture_anim.h"
#include "system/texture.h"

extern struct texture_pvm green_hill_background_pvms[];
extern NJS_TEXLIST **green_hill_background_groups[];
extern NJS_OBJECT green_hill_water_area_a;
extern NJS_OBJECT green_hill_water_area_b;

/* @hack type: the sky's objects below declared as arrays of one, where single
 * objects would do */
extern NJS_TEXLIST green_hill_horizon_texlist;
extern NJS_CNK_OBJECT green_hill_horizon[];
extern NJS_TEXLIST green_hill_horizon_scroll_texlist;
extern NJS_CNK_OBJECT green_hill_horizon_scroll[];
extern struct texture_anim green_hill_horizon_scroll_uv;
extern NJS_TEXLIST green_hill_sky_north_texlist;
extern NJS_CNK_OBJECT green_hill_sky_north[];
extern NJS_TEXLIST green_hill_sky_east_texlist;
extern NJS_CNK_OBJECT green_hill_sky_east[];
extern NJS_TEXLIST green_hill_sky_south_texlist;
extern NJS_CNK_OBJECT green_hill_sky_south[];
extern NJS_TEXLIST green_hill_sky_west_texlist;
extern NJS_CNK_OBJECT green_hill_sky_west[];
extern NJS_TEXLIST green_hill_lower_1_texlist;
extern NJS_CNK_OBJECT green_hill_lower_1[];
extern struct texture_anim green_hill_lower_1_uv;
extern NJS_TEXLIST green_hill_lower_2_texlist;
extern NJS_CNK_OBJECT green_hill_lower_2[];
extern struct texture_anim green_hill_lower_2_uv;
extern NJS_TEXLIST green_hill_lower_3_texlist;
extern NJS_CNK_OBJECT green_hill_lower_3[];
extern struct texture_anim green_hill_lower_3_uv;
extern NJS_TEXLIST green_hill_lower_4_texlist;
extern NJS_CNK_OBJECT green_hill_lower_4[];
extern struct texture_anim green_hill_lower_4_uv;
extern NJS_TEXLIST green_hill_lower_5_texlist;
extern NJS_CNK_OBJECT green_hill_lower_5[];
extern struct texture_anim green_hill_lower_5_uv;
extern NJS_TEXLIST green_hill_lower_6_texlist;
extern NJS_CNK_OBJECT green_hill_lower_6[];
extern struct texture_anim green_hill_lower_6_uv;
extern NJS_TEXLIST green_hill_lower_7_texlist;
extern NJS_CNK_OBJECT green_hill_lower_7[];
extern struct texture_anim green_hill_lower_7_uv;

#endif
