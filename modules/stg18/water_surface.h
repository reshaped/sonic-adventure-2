/* The four animated water surfaces drawn by Dry Lagoon's background task. */
#ifndef STG18_WATER_SURFACE_H
#define STG18_WATER_SURFACE_H
#include <ninja.h>
#include "draw/texture_anim.h"

#define LAGOON_WATER_POSES 16

extern const char dry_lagoon_water_filenames[64];
extern const char dry_lagoon_water_archive[8];
extern NJS_TEXLIST dry_lagoon_water_surface_texlist;
extern NJS_TEXLIST dry_lagoon_water_pvm_texlist;
extern NJS_CNK_MODEL *dry_lagoon_south_water_models[LAGOON_WATER_POSES];
extern struct texture_anim *dry_lagoon_south_water_anims[LAGOON_WATER_POSES];
extern NJS_CNK_MODEL *dry_lagoon_north_water_models[LAGOON_WATER_POSES];
extern struct texture_anim *dry_lagoon_north_water_anims[LAGOON_WATER_POSES];
extern NJS_CNK_MODEL *dry_lagoon_middle_water_models[LAGOON_WATER_POSES];
extern struct texture_anim *dry_lagoon_middle_water_anims[LAGOON_WATER_POSES];
extern NJS_CNK_MODEL *dry_lagoon_middle_water_2_models[LAGOON_WATER_POSES];
extern struct texture_anim *dry_lagoon_middle_water_2_anims[LAGOON_WATER_POSES];

#endif
