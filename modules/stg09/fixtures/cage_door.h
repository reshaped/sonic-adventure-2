#ifndef STG09_FIXTURES_CAGE_DOOR_H
#define STG09_FIXTURES_CAGE_DOOR_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 cage_door_panel_polygons[303] = {
#include "stg09/fixtures/cage_door/cage_door_panel_polygons.inc"
};

static Sint32 cage_door_panel_vertices[307] = {
#include "stg09/fixtures/cage_door/cage_door_panel_vertices.inc"
};

static NJS_CNK_MODEL cage_door_panel_model = {
#include "stg09/fixtures/cage_door/cage_door_panel_model.inc"
};

static NJS_CNK_OBJECT cage_door_panel_object = {
#include "stg09/fixtures/cage_door/cage_door_panel_object.inc"
};

#endif
