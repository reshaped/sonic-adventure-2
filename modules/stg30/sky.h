#ifndef STG30_SKY_H
#define STG30_SKY_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "system/texture.h"

extern Bool sky_built;
extern struct model_build stars_builds[3];
extern struct model_build orbit_builds[3];
extern NJS_TEXLIST **rush_sky_groups[];
extern struct texture_pvm rush_sky_pvms[];
extern NJS_TEXLIST rush_sky_pvm_texlist;
extern NJS_TEXLIST rush_sky_land_texlist;
extern NJS_CNK_OBJECT rush_stars_object;
extern NJS_CNK_OBJECT rush_earth_object;
extern NJS_CNK_OBJECT rush_orbit_128_object[];
extern NJS_CNK_OBJECT rush_orbit_64_object[];
extern const char rush_orbit_128_texture[12];
extern const char sky_orbit_texture[12];

#endif
