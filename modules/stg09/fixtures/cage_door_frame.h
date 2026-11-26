#ifndef STG09_FIXTURES_CAGE_DOOR_FRAME_H
#define STG09_FIXTURES_CAGE_DOOR_FRAME_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 cage_door_polygons[51] = {
#include "stg09/fixtures/cage_door_frame/cage_door_polygons.inc"
};

static Sint32 cage_door_vertices[35] = {
#include "stg09/fixtures/cage_door_frame/cage_door_vertices.inc"
};

static NJS_CNK_MODEL cage_door_model = {
#include "stg09/fixtures/cage_door_frame/cage_door_model.inc"
};

static NJS_CNK_OBJECT cage_door_object = {
#include "stg09/fixtures/cage_door_frame/cage_door_object.inc"
};

#endif
