#ifndef STG26_ROOM_SCENERY_H
#define STG26_ROOM_SCENERY_H

#include <ninja.h>

#include "system/texture.h"

extern NJS_CNK_OBJECT quarters_south_modifiers;
extern NJS_CNK_OBJECT quarters_east_modifiers;
extern NJS_CNK_OBJECT quarters_west_modifiers;
extern NJS_CNK_OBJECT quarters_north_band_object;
extern NJS_CNK_OBJECT quarters_side_band_object;
extern NJS_CNK_OBJECT quarters_pillar_0_object;
extern NJS_CNK_OBJECT quarters_pillar_1_object;
extern NJS_CNK_OBJECT quarters_pillar_2_object;
extern NJS_CNK_OBJECT quarters_pillar_3_object;
extern NJS_CNK_OBJECT quarters_water_object;
extern NJS_TEXLIST quarters_water_surface_texlist;
extern NJS_CNK_MODEL quarters_north_band;
extern NJS_CNK_MODEL quarters_side_band;
extern NJS_CNK_MODEL quarters_pillar_0_model;
extern NJS_CNK_MODEL quarters_pillar_1_model;
extern NJS_CNK_MODEL quarters_pillar_2_model;
extern NJS_CNK_MODEL quarters_pillar_3_model;
/* @hack type: the water's model declared as an array of one, where a single
 * model would do */
extern NJS_CNK_MODEL quarters_water_model[1];
extern NJS_TEXLIST quarters_band_texlist;
extern NJS_TEXLIST quarters_pillar_0_texlist;
extern NJS_TEXLIST quarters_pillar_1_texlist;
extern NJS_TEXLIST quarters_pillar_2_texlist;
extern NJS_TEXLIST quarters_pillar_3_texlist;
extern NJS_TEXLIST **quarters_background_groups[];
extern struct texture_pvm quarters_background_pvms[];
extern NJS_OBJECT quarters_kill_area;

#endif
