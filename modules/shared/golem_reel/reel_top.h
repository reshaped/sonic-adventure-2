#ifndef SHARED_GOLEM_REEL_REEL_TOP_H
#define SHARED_GOLEM_REEL_REEL_TOP_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 reel_top_polygons[861] = {
#include "shared/golem_reel/reel_top/reel_top_polygons.inc"
};

static Sint32 reel_top_vertices[519] = {
#include "shared/golem_reel/reel_top/reel_top_vertices.inc"
};

static NJS_CNK_MODEL reel_top_model = {
#include "shared/golem_reel/reel_top/reel_top_model.inc"
};

static NJS_CNK_OBJECT reel_top_object = {
#include "shared/golem_reel/reel_top/reel_top_object.inc"
};

#endif
