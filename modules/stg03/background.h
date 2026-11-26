/* Green Forest adds two separate near-water surfaces to the jungle background.
 */
#ifndef STG03_BACKGROUND_H
#define STG03_BACKGROUND_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME jungle_background_model_textures[2] = {
#include "stg03/background/jungle_background_model_textures.inc"
};

static NJS_TEXLIST jungle_background_model_texlist = {
#include "stg03/background/jungle_background_model_texlist.inc"
};

#include "shared/jungle_sky_shape.h" /* IWYU pragma: export */

static NJS_TEXNAME background_water_textures[1] = {
#include "stg03/background/background_water_textures.inc"
};

static NJS_TEXLIST background_water_texlist = {
#include "stg03/background/background_water_texlist.inc"
};

#include "shared/jungle_water_shape.h" /* IWYU pragma: export */

static NJS_TEXNAME water_patch_textures[1] = {
#include "stg03/background/water_patch_textures.inc"
};

static NJS_TEXLIST water_patch_texlist = {
#include "stg03/background/water_patch_texlist.inc"
};

static Sint16 west_water_patch_polygons[23] = {
#include "stg03/background/west_water_patch_polygons.inc"
};

static Sint32 west_water_patch_vertices[19] = {
#include "stg03/background/west_water_patch_vertices.inc"
};

static NJS_CNK_MODEL west_water_patch_model = {
#include "stg03/background/west_water_patch_model.inc"
};

static NJS_CNK_OBJECT west_water_patch = {
#include "stg03/background/west_water_patch.inc"
};

static Sint16 east_water_patch_polygons[23] = {
#include "stg03/background/east_water_patch_polygons.inc"
};

static Sint32 east_water_patch_vertices[19] = {
#include "stg03/background/east_water_patch_vertices.inc"
};

static NJS_CNK_MODEL east_water_patch_model = {
#include "stg03/background/east_water_patch_model.inc"
};

static NJS_CNK_OBJECT east_water_patch = {
#include "stg03/background/east_water_patch.inc"
};

static NJS_CNK_OBJECT green_forest_water_object = {
#include "stg03/background/green_forest_water_object.inc"
};

static NJS_TEXNAME green_forest_background_textures[2] = {
#include "stg03/background/green_forest_background_textures.inc"
};

static NJS_TEXLIST green_forest_background_texlist = {
#include "stg03/background/green_forest_background_texlist.inc"
};

#endif
