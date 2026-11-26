#ifndef STG21_BACKGROUND_ART_H
#define STG21_BACKGROUND_ART_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "system/texture.h"

struct path;

extern struct path *sand_ocean_paths[];
extern NJS_TEXLIST ocean_first_pyramid_texlist;
extern NJS_CNK_MODEL ocean_first_pyramid;
extern NJS_CNK_OBJECT ocean_first_pyramid_object;
extern NJS_TEXLIST ocean_second_pyramid_texlist;
extern NJS_CNK_MODEL ocean_second_pyramid;
extern NJS_CNK_OBJECT ocean_second_pyramid_object;
extern NJS_TEXLIST ocean_first_stars_texlist;
extern NJS_CNK_MODEL ocean_first_stars;
extern NJS_CNK_OBJECT ocean_first_stars_object;
extern NJS_TEXLIST ocean_second_stars_texlist;
extern NJS_CNK_MODEL ocean_second_stars;
extern NJS_CNK_OBJECT ocean_second_stars_object;
extern NJS_TEXLIST ocean_moon_texlist;
extern NJS_CNK_MODEL ocean_moon;
extern NJS_CNK_OBJECT ocean_moon_object;
extern NJS_TEXLIST **ocean_background_groups[];
extern struct texture_pvm ocean_background_pvms[];
extern NJS_CNK_OBJECT ocean_sand_start[1];
extern struct texture_anim ocean_sand_start_anim;
extern NJS_CNK_OBJECT ocean_sand_2200[1];
extern struct texture_anim ocean_sand_2200_anim;
extern NJS_CNK_OBJECT ocean_sand_3970[1];
extern struct texture_anim ocean_sand_3970_anim;
extern NJS_CNK_OBJECT ocean_sand_6830[1];
extern struct texture_anim ocean_sand_6830_anim;
extern NJS_CNK_OBJECT ocean_sand_4250[1];
extern struct texture_anim ocean_sand_4250_anim;
extern NJS_CNK_OBJECT ocean_sand_3500[1];
extern struct texture_anim ocean_sand_3500_anim;
extern NJS_CNK_OBJECT ocean_sand_end[1];
extern struct texture_anim ocean_sand_end_anim;
extern NJS_TEXLIST ocean_quicksand_texlist;
extern NJS_OBJECT sand_ocean_kill_0;
extern NJS_OBJECT sand_ocean_kill_1;
extern NJS_OBJECT sand_ocean_kill_2;
extern NJS_OBJECT sand_ocean_kill_3;

#endif
