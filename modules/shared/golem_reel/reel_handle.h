#ifndef SHARED_GOLEM_REEL_REEL_HANDLE_H
#define SHARED_GOLEM_REEL_REEL_HANDLE_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 reel_handle_polygons[267] = {
#include "shared/golem_reel/reel_handle/reel_handle_polygons.inc"
};

static Sint32 reel_handle_vertices[159] = {
#include "shared/golem_reel/reel_handle/reel_handle_vertices.inc"
};

static NJS_CNK_MODEL reel_handle_model = {
#include "shared/golem_reel/reel_handle/reel_handle_model.inc"
};

static NJS_CNK_OBJECT reel_handle_object = {
#include "shared/golem_reel/reel_handle/reel_handle_object.inc"
};

#endif
