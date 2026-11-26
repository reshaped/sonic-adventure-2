#ifndef STG28_CAVE_SCENERY_H
#define STG28_CAVE_SCENERY_H

#include <ninja.h>

#include "shared/fog.h"
#include "system/texture.h"

extern struct fog_setting pyramid_cave_fog;
extern NJS_TEXLIST pyramid_cave_wall_texlist, pyramid_cave_lamp_texlist;
extern NJS_CNK_MODEL pyramid_cave_wall_model;
extern NJS_CNK_MODEL pyramid_cave_lamp_model[1];
extern NJS_TEXLIST **pyramid_cave_background_texture_groups[];
extern struct texture_pvm pyramid_cave_background_pvms[];

extern NJS_OBJECT pyramid_cave_kill_area_0;
extern NJS_OBJECT pyramid_cave_kill_area_1;
extern NJS_OBJECT pyramid_cave_kill_area_2;
extern NJS_OBJECT pyramid_cave_kill_area_3;
extern NJS_OBJECT pyramid_cave_kill_area_4;
extern NJS_OBJECT pyramid_cave_kill_area_5;
extern NJS_OBJECT pyramid_cave_kill_area_6;
extern NJS_OBJECT pyramid_cave_kill_area_7;

#endif
