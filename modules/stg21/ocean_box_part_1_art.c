#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stone_box.h"

static Sint16 box_part_1_polygons[155] = {
#include "stg21/ocean_box_part_1_art/box_part_1_polygons.inc"
};

static Sint32 box_part_1_vertices[83] = {
#include "stg21/ocean_box_part_1_art/box_part_1_vertices.inc"
};

NJS_CNK_MODEL box_part_1_model = {
#include "stg21/ocean_box_part_1_art/box_part_1_model.inc"
};

static NJS_CNK_OBJECT box_part_1_object = {
#include "stg21/ocean_box_part_1_art/box_part_1_object.inc"
};
