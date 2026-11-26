#ifndef STG32_SKY_H
#define STG32_SKY_H
#include <ninja.h>
#include "collision/world.h"
#include "system/texture.h"

/* the background's three lands and their file names. each object below holds a
 * model of meteor_herd_background_land. the background pulses those models'
 * color */
extern struct land_table meteor_herd_changing_land;
extern const char meteor_herd_changing_land_filenames[16];
extern struct land_table meteor_herd_background_land;
extern const char meteor_herd_background_land_filenames[48];
extern struct land_table meteor_herd_warning_land;
extern const char meteor_herd_warning_land_filenames[222];
extern NJS_CNK_OBJECT meteor_herd_surface_0;
extern NJS_CNK_OBJECT meteor_herd_surface_1;
extern NJS_CNK_OBJECT meteor_herd_surface_2;
extern NJS_CNK_OBJECT meteor_herd_surface_3;
extern NJS_CNK_OBJECT meteor_herd_surface_4;

extern NJS_TEXLIST meteor_herd_sky_texlist;
extern NJS_TEXLIST meteor_warning_texlist;
extern NJS_TEXLIST meteor_sky_meteor_texlist;
/* @hack type: the four objects below declared as arrays of one, where single
 * objects would do */
extern NJS_CNK_OBJECT meteor_sky_stars[];
extern NJS_CNK_OBJECT meteor_sky_first_orbit[];
extern NJS_CNK_OBJECT meteor_sky_second_orbit[];
extern NJS_CNK_OBJECT meteor_sky_meteor[];

extern NJS_TEXLIST **background_texture_groups[];
extern struct texture_pvm background_pvms[];

extern NJS_TEXLIST meteor_herd_changing_surface_texlist;

extern NJS_TEXLIST meteor_herd_animated_surface_texlist;

#endif
