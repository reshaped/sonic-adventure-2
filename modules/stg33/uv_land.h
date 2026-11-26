/* uv_land, the background's land, whose entries draw two models between them.
 * the background pulses the models' color through uv_land_lower_object and
 * uv_land_upper_object */
#ifndef STG33_UV_LAND_H
#define STG33_UV_LAND_H
#include <ninja.h>

#include "collision/world.h"

extern struct land_table uv_land;
extern const char uv_land_filenames[35];
extern NJS_CNK_OBJECT uv_land_lower_object;
extern NJS_CNK_OBJECT uv_land_upper_object;

extern NJS_TEXLIST knuckles_rouge_animated_surface_texlist;

#endif
