/* Sky Rail flying scenery. */
#ifndef STG06_SCENERY_ART_KAZANI_H
#define STG06_SCENERY_ART_KAZANI_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME kazani_texlist_names[1] = {
#include "stg06/scenery_art/kazani/kazani_texlist_names.inc"
};

static NJS_TEXLIST kazani_texlist = {
#include "stg06/scenery_art/kazani/kazani_texlist.inc"
};

static Sint16 kazani_polygons[167] = {
#include "stg06/scenery_art/kazani/kazani_polygons.inc"
};

static Sint32 kazani_vertices[123] = {
#include "stg06/scenery_art/kazani/kazani_vertices.inc"
};

static NJS_CNK_MODEL kazani_model = {
#include "stg06/scenery_art/kazani/kazani_model.inc"
};

static NJS_CNK_OBJECT kazani_object = {
#include "stg06/scenery_art/kazani/kazani_object.inc"
};

#endif
