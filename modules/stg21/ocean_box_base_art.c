#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stone_box.h"

static Sint16 box_base_polygons[171] = {
#include "stg21/ocean_box_base_art/box_base_polygons.inc"
};

static Sint32 box_base_vertices[175] = {
#include "stg21/ocean_box_base_art/box_base_vertices.inc"
};

NJS_CNK_MODEL box_base_model = {
#include "stg21/ocean_box_base_art/box_base_model.inc"
};

static NJS_CNK_OBJECT box_base_object = {
#include "stg21/ocean_box_base_art/box_base_object.inc"
};
