#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/corner.h"

static Sint16 corner_polygons[1725] = {
#include "shared/ocean_corner_art/corner_polygons.inc"
};

static Sint32 corner_vertices[1951] = {
#include "shared/ocean_corner_art/corner_vertices.inc"
};

NJS_CNK_MODEL corner_model = {
#include "shared/ocean_corner_art/corner_model.inc"
};

static NJS_CNK_OBJECT corner_object = {
#include "shared/ocean_corner_art/corner_object.inc"
};
