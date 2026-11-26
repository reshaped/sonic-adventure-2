/* Geometry and texture slots for forked. */
#ifndef STG14_ROAD_FORKED_ART_H
#define STG14_ROAD_FORKED_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME forked_names[5] = {
#include "stg14/road/forked_art/forked_names.inc"
};

static NJS_TEXLIST forked_texlist = {
#include "stg14/road/forked_art/forked_texlist.inc"
};

static Sint16 forked_polygons[817] = {
#include "stg14/road/forked_art/forked_polygons.inc"
};

static Sint32 forked_vertices[559] = {
#include "stg14/road/forked_art/forked_vertices.inc"
};

static NJS_CNK_MODEL forked_model = {
#include "stg14/road/forked_art/forked_model.inc"
};

static NJS_CNK_OBJECT forked_object = {
#include "stg14/road/forked_art/forked_object.inc"
};

#endif
