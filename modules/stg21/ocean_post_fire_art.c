#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/fire_fixture.h"

static Sint16 post_fire_polygons[301] = {
#include "stg21/ocean_post_fire_art/post_fire_polygons.inc"
};

static Sint32 post_fire_vertices[231] = {
#include "stg21/ocean_post_fire_art/post_fire_vertices.inc"
};

NJS_CNK_MODEL post_fire_model = {
#include "stg21/ocean_post_fire_art/post_fire_model.inc"
};

static NJS_CNK_OBJECT post_fire_object = {
#include "stg21/ocean_post_fire_art/post_fire_object.inc"
};
