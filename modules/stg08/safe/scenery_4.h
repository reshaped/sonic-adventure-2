#ifndef STG08_SAFE_SCENERY_4_H
#define STG08_SAFE_SCENERY_4_H

/* Scenery 4 geometry. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME scenery_4_names[4] = {
#include "stg08/safe/scenery_4/scenery_4_names.inc"
};

static NJS_TEXLIST security_scenery_4_texlist = {
#include "stg08/safe/scenery_4/security_scenery_4_texlist.inc"
};

static Sint16 scenery_4_polygons[215] = {
#include "stg08/safe/scenery_4/scenery_4_polygons.inc"
};

static Sint32 scenery_4_vertices[179] = {
#include "stg08/safe/scenery_4/scenery_4_vertices.inc"
};

static NJS_CNK_MODEL scenery_4_model = {
#include "stg08/safe/scenery_4/scenery_4_model.inc"
};

static NJS_CNK_OBJECT security_scenery_4_object = {
#include "stg08/safe/scenery_4/security_scenery_4_object.inc"
};

#endif
