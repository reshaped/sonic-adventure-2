#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/grave.h"

static Sint16 grave_polygons[1599] = {
#include "stg21/ocean_grave_art/grave_polygons.inc"
};

static Sint32 grave_vertices[1263] = {
#include "stg21/ocean_grave_art/grave_vertices.inc"
};

NJS_CNK_MODEL grave_model = {
#include "stg21/ocean_grave_art/grave_model.inc"
};

static NJS_CNK_OBJECT grave_object = {
#include "stg21/ocean_grave_art/grave_object.inc"
};
