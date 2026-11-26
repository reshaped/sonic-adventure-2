#ifndef STG08_SAFE_SCENERY_3_H
#define STG08_SAFE_SCENERY_3_H

/* Scenery 3 geometry. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME scenery_3_names[4] = {
#include "stg08/safe/scenery_3/scenery_3_names.inc"
};

static NJS_TEXLIST security_scenery_3_texlist = {
#include "stg08/safe/scenery_3/security_scenery_3_texlist.inc"
};

static Sint16 scenery_3_polygons[215] = {
#include "stg08/safe/scenery_3/scenery_3_polygons.inc"
};

static Sint32 scenery_3_vertices[179] = {
#include "stg08/safe/scenery_3/scenery_3_vertices.inc"
};

static NJS_CNK_MODEL scenery_3_model = {
#include "stg08/safe/scenery_3/scenery_3_model.inc"
};

static NJS_CNK_OBJECT security_scenery_3_object = {
#include "stg08/safe/scenery_3/security_scenery_3_object.inc"
};

#endif
