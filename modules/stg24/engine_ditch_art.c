/* DITCH's model: its base, and the panel it draws twice */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg24/panel_art.h"

static Sint16 engine_ditch_object_child_polygons[51] = {
#include "stg24/engine_ditch_art/engine_ditch_object_child_polygons.inc"
};

static Sint32 engine_ditch_object_child_vertices[51] = {
#include "stg24/engine_ditch_art/engine_ditch_object_child_vertices.inc"
};

static NJS_CNK_MODEL engine_ditch_object_child_model = {
#include "stg24/engine_ditch_art/engine_ditch_object_child_model.inc"
};

static NJS_CNK_OBJECT engine_ditch_object_child = {
#include "stg24/engine_ditch_art/engine_ditch_object_child.inc"
};

static Sint16 engine_ditch_polygons[83] = {
#include "stg24/engine_ditch_art/engine_ditch_polygons.inc"
};

static Sint32 engine_ditch_vertices[51] = {
#include "stg24/engine_ditch_art/engine_ditch_vertices.inc"
};

static NJS_CNK_MODEL engine_ditch_model = {
#include "stg24/engine_ditch_art/engine_ditch_model.inc"
};

NJS_CNK_OBJECT engine_ditch_object = {
#include "stg24/engine_ditch_art/engine_ditch_object.inc"
};
