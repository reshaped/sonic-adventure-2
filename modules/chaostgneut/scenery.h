/* the Neutral Garden's land, its pond and two falls, and its sky's models.
 * the land, the water and the sky all draw with neutral_sky_texlist */
#ifndef CHAOSTGNEUT_SCENERY_H
#define CHAOSTGNEUT_SCENERY_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"

extern struct land_table neutral_land;
/* the buffer the garden compiles neutral_land into, at its address in the
 * image. the garden names it through CHAO_LAND_BUFFER, which GCC's build
 * replaces with end */
extern Uint8 neutral_land_buffer[];
extern NJS_TEXLIST neutral_sky_texlist;
extern const char neutral_texture_filenames[128];
extern NJS_CNK_OBJECT neutral_outer_sky, neutral_inner_sky;
extern NJS_CNK_OBJECT neutral_pond;
extern NJS_CNK_OBJECT neutral_lower_falls;
extern NJS_CNK_OBJECT neutral_upper_falls;
#endif
