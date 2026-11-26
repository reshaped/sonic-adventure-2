#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stone_box.h"

static Sint16 box_part_3_polygons[63] = {
#include "stg21/ocean_box_part_3_art/box_part_3_polygons.inc"
};

static Sint32 box_part_3_vertices[43] = {
#include "stg21/ocean_box_part_3_art/box_part_3_vertices.inc"
};

NJS_CNK_MODEL box_part_3_model = {
#include "stg21/ocean_box_part_3_art/box_part_3_model.inc"
};

static NJS_CNK_OBJECT box_part_3_object = {
#include "stg21/ocean_box_part_3_art/box_part_3_object.inc"
};
