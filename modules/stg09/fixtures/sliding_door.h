#ifndef STG09_FIXTURES_SLIDING_DOOR_H
#define STG09_FIXTURES_SLIDING_DOOR_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME sliding_door_names[3] = {
#include "stg09/fixtures/sliding_door/sliding_door_names.inc"
};

static NJS_TEXLIST sliding_door_texlist = {
#include "stg09/fixtures/sliding_door/sliding_door_texlist.inc"
};

static Sint16 sliding_door_polygons[471] = {
#include "stg09/fixtures/sliding_door/sliding_door_polygons.inc"
};

static Sint32 sliding_door_vertices[487] = {
#include "stg09/fixtures/sliding_door/sliding_door_vertices.inc"
};

static NJS_CNK_MODEL sliding_door_model = {
#include "stg09/fixtures/sliding_door/sliding_door_model.inc"
};

static NJS_CNK_OBJECT sliding_door_object = {
#include "stg09/fixtures/sliding_door/sliding_door_object.inc"
};

#endif
