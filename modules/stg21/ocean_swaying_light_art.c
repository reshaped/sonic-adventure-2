#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/swaying_light.h"

static Sint16 swaying_light_polygons[55] = {
#include "stg21/ocean_swaying_light_art/swaying_light_polygons.inc"
};

static Sint32 swaying_light_vertices[51] = {
#include "stg21/ocean_swaying_light_art/swaying_light_vertices.inc"
};

NJS_CNK_MODEL swaying_light_model = {
#include "stg21/ocean_swaying_light_art/swaying_light_model.inc"
};

static NJS_CNK_OBJECT swaying_light_object = {
#include "stg21/ocean_swaying_light_art/swaying_light_object.inc"
};
