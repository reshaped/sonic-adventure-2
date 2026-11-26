/* the distant land and the water's land, which the background draws, with their
 * file names. each surface holds a model of the distant land. the background
 * scrolls that model's texture by the surface's uv */
#ifndef STG31_SCENERY_LAND_H
#define STG31_SCENERY_LAND_H

#include <ninja.h>

#include "collision/world.h"
#include "draw/texture_anim.h"

extern struct land_table green_hill_distant_land;
extern const char green_hill_distant_land_filenames[26];
extern NJS_TEXLIST green_hill_distant_texlist;

extern struct land_table green_hill_water_land;
extern const char green_hill_water_land_filenames[113];
extern NJS_TEXLIST green_hill_water_texlist;

extern NJS_CNK_OBJECT green_hill_surface_0;
extern NJS_CNK_OBJECT green_hill_surface_1;
extern NJS_CNK_OBJECT green_hill_surface_2;
extern NJS_CNK_OBJECT green_hill_surface_3;
extern NJS_CNK_OBJECT green_hill_surface_4;
extern NJS_CNK_OBJECT green_hill_surface_5;
extern NJS_CNK_OBJECT green_hill_surface_6;
extern NJS_CNK_OBJECT green_hill_surface_7;
extern NJS_CNK_OBJECT green_hill_surface_8;
extern NJS_CNK_OBJECT green_hill_surface_9;
extern NJS_CNK_OBJECT green_hill_surface_10;
extern NJS_CNK_OBJECT green_hill_surface_11;
extern NJS_CNK_OBJECT green_hill_surface_12;
extern NJS_CNK_OBJECT green_hill_surface_13;
extern NJS_CNK_OBJECT green_hill_surface_14;
extern NJS_CNK_OBJECT green_hill_surface_15;

extern struct texture_anim green_hill_surface_uv_0;
extern struct texture_anim green_hill_surface_uv_1;
extern struct texture_anim green_hill_surface_uv_2;
extern struct texture_anim green_hill_surface_uv_3;
extern struct texture_anim green_hill_surface_uv_4;
extern struct texture_anim green_hill_surface_uv_5;
extern struct texture_anim green_hill_surface_uv_6;
extern struct texture_anim green_hill_surface_uv_7;
extern struct texture_anim green_hill_surface_uv_8;
extern struct texture_anim green_hill_surface_uv_9;
extern struct texture_anim green_hill_surface_uv_10;
extern struct texture_anim green_hill_surface_uv_11;
extern struct texture_anim green_hill_surface_uv_12;
extern struct texture_anim green_hill_surface_uv_13;
extern struct texture_anim green_hill_surface_uv_14;
extern struct texture_anim green_hill_surface_uv_15;

#endif
