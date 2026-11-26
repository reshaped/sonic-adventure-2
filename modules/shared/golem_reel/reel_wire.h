#ifndef SHARED_GOLEM_REEL_REEL_WIRE_H
#define SHARED_GOLEM_REEL_REEL_WIRE_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 reel_wire_polygons[51] = {
#include "shared/golem_reel/reel_wire/reel_wire_polygons.inc"
};

static Sint32 reel_wire_vertices[39] = {
#include "shared/golem_reel/reel_wire/reel_wire_vertices.inc"
};

static NJS_CNK_MODEL reel_wire_model = {
#include "shared/golem_reel/reel_wire/reel_wire_model.inc"
};

static NJS_CNK_OBJECT reel_wire_object = {
#include "shared/golem_reel/reel_wire/reel_wire_object.inc"
};

#endif
