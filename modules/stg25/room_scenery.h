#ifndef STG25_ROOM_SCENERY_H
#define STG25_ROOM_SCENERY_H

#include <ninja.h>

#include "shared/fog.h"
#include "system/texture.h"

/* the rooms' models and their texture lists. each model's name says where it
 * draws. the far east draws the hall's model again, turned a quarter turn
 * about y */
extern NJS_TEXLIST death_chamber_center_texlist;
extern NJS_CNK_MODEL death_chamber_center_top_model;
extern NJS_CNK_MODEL death_chamber_center_middle_model;
extern NJS_CNK_MODEL death_chamber_center_base_model;
extern NJS_TEXLIST death_chamber_hall_texlist;
extern NJS_CNK_MODEL death_chamber_hall_model;
extern NJS_TEXLIST death_chamber_wall_texlist;
extern NJS_CNK_MODEL death_chamber_wall_model;
extern NJS_TEXLIST death_chamber_north_texlist;
extern NJS_CNK_MODEL death_chamber_north_model;
extern NJS_TEXLIST death_chamber_east_north_texlist;
extern NJS_CNK_MODEL death_chamber_east_north_model;
extern NJS_TEXLIST death_chamber_east_south_texlist;
extern NJS_CNK_MODEL death_chamber_east_south_model;
/* the center's moving object and its motion, the spinner the background
 * turns about y, and the three pools */
extern NJS_TEXLIST death_chamber_center_object_texlist;
extern NJS_CNK_OBJECT death_chamber_center_object;
extern NJS_MOTION death_chamber_center_motion[1];
extern NJS_CNK_MODEL death_chamber_spinner_model;
extern NJS_CNK_MODEL death_chamber_center_water[1];
extern NJS_CNK_MODEL death_chamber_south_west_water[1];
extern NJS_CNK_MODEL death_chamber_south_east_water[1];
/* the background's texture lists, and the PVM table that names bgtex25 */
extern NJS_TEXLIST **death_chamber_background_texture_groups[];
extern struct texture_pvm death_chamber_background_pvms[];
extern NJS_OBJECT death_chamber_kill_area;

extern struct fog_setting death_chamber_under_water_fog;
extern struct fog_setting death_chamber_above_water_fog;

#endif
