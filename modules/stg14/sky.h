/* The sky model follows the current camera in Radical Highway. */
#ifndef STG14_SKY_H
#define STG14_SKY_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME sky_names[3] = {
#include "stg14/sky/sky_names.inc"
};

static NJS_TEXLIST sky_texlist = {
#include "stg14/sky/sky_texlist.inc"
};

static Sint16 sky_polygons[907] = {
#include "stg14/sky/sky_polygons.inc"
};

static Sint32 sky_vertices[459] = {
#include "stg14/sky/sky_vertices.inc"
};

static NJS_CNK_MODEL sky_model = {
#include "stg14/sky/sky_model.inc"
};

static NJS_CNK_OBJECT sky_object = {
#include "stg14/sky/sky_object.inc"
};

#endif
