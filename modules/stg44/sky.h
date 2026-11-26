#ifndef STG44_SKY_H
#define STG44_SKY_H
#include <ninja.h>
#include "collision/world.h"
#include "system/texture.h"

/* the background's land and its file names. each object below holds a model of
 * mad_space_sky_land. the background pulses those models' color */
extern struct land_table mad_space_sky_land;
extern const char mad_space_sky_land_filenames[67];
extern NJS_CNK_OBJECT mad_space_land_0;
extern NJS_CNK_OBJECT mad_space_land_1;
extern NJS_CNK_OBJECT mad_space_land_2;
extern NJS_CNK_OBJECT mad_space_land_3;
extern NJS_CNK_OBJECT mad_space_land_4;
extern NJS_CNK_OBJECT mad_space_land_5;
extern NJS_CNK_OBJECT mad_space_land_6;
extern NJS_CNK_OBJECT mad_space_land_7;
extern NJS_CNK_OBJECT mad_space_land_8;
extern NJS_CNK_OBJECT mad_space_land_9;
extern NJS_CNK_OBJECT mad_space_land_11;
extern NJS_CNK_OBJECT mad_space_land_12;
extern NJS_CNK_OBJECT mad_space_land_13;
extern NJS_CNK_OBJECT mad_space_land_14;
extern NJS_CNK_OBJECT mad_space_land_15;
extern NJS_CNK_OBJECT mad_space_land_16;

extern NJS_TEXLIST mad_space_sky_texlist;
extern NJS_TEXLIST background_4_texlist;
/* @hack type: the five objects below declared as arrays of one, where single
 * objects would do */
extern NJS_CNK_OBJECT background_0_object[];
extern NJS_CNK_OBJECT background_1_object[];
extern NJS_CNK_OBJECT background_2_object[];
extern NJS_CNK_OBJECT background_3_object[];
extern NJS_CNK_OBJECT background_4_object[];
extern NJS_TEXLIST **background_texture_groups[];
extern struct texture_pvm background_pvms[];

extern NJS_TEXLIST mad_space_animated_surface_texlist;

#endif
