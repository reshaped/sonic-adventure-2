#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stone_box.h"

static Sint16 box_part_2_polygons[63] = {
#include "stg21/ocean_box_part_2_art/box_part_2_polygons.inc"
};

static Sint32 box_part_2_vertices[35] = {
#include "stg21/ocean_box_part_2_art/box_part_2_vertices.inc"
};

NJS_CNK_MODEL box_part_2_model = {
#include "stg21/ocean_box_part_2_art/box_part_2_model.inc"
};

static NJS_CNK_OBJECT box_part_2_object = {
#include "stg21/ocean_box_part_2_art/box_part_2_object.inc"
};
