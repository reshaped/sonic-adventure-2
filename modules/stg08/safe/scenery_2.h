#ifndef STG08_SAFE_SCENERY_2_H
#define STG08_SAFE_SCENERY_2_H

/* Scenery 2 geometry. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME scenery_2_names[4] = {
#include "stg08/safe/scenery_2/scenery_2_names.inc"
};

static NJS_TEXLIST security_scenery_2_texlist = {
#include "stg08/safe/scenery_2/security_scenery_2_texlist.inc"
};

static Sint16 scenery_2_polygons[221] = {
#include "stg08/safe/scenery_2/scenery_2_polygons.inc"
};

static Sint32 scenery_2_vertices[183] = {
#include "stg08/safe/scenery_2/scenery_2_vertices.inc"
};

static NJS_CNK_MODEL scenery_2_model = {
#include "stg08/safe/scenery_2/scenery_2_model.inc"
};

static NJS_CNK_OBJECT security_scenery_2_object = {
#include "stg08/safe/scenery_2/security_scenery_2_object.inc"
};

#endif
