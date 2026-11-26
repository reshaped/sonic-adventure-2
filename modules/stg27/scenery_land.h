/* the poison's, the lamps' and the sky's land tables, which the background
 * draws */
#ifndef STG27_SCENERY_LAND_H
#define STG27_SCENERY_LAND_H

#include <ninja.h>

#include "collision/world.h"

extern struct land_table colony_poison_land;
extern const char colony_poison_land_filenames[221];
extern NJS_TEXLIST colony_poison_texlist;

extern struct land_table colony_lamp_land;
extern const char colony_lamp_land_filenames[25];
extern NJS_TEXLIST colony_lamp_texlist;
extern NJS_CNK_OBJECT colony_scenery_0;
extern NJS_CNK_OBJECT colony_scenery_1;
extern NJS_CNK_OBJECT colony_scenery_2;
extern NJS_CNK_OBJECT colony_scenery_3;
extern NJS_CNK_OBJECT colony_scenery_4;
extern NJS_CNK_OBJECT colony_scenery_5;
extern NJS_CNK_OBJECT colony_scenery_6;
extern NJS_CNK_OBJECT colony_scenery_7;
extern NJS_CNK_OBJECT colony_scenery_8;

extern struct land_table colony_sky_land;
extern const char colony_sky_land_filenames[25];
extern NJS_TEXLIST colony_sky_land_texlist;
extern NJS_CNK_OBJECT colony_pulse_0;
extern NJS_CNK_OBJECT colony_pulse_1;
extern NJS_CNK_OBJECT colony_pulse_2;

#endif
