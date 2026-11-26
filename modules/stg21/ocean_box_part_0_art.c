#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stone_box.h"

static Sint16 box_part_0_polygons[155] = {
#include "stg21/ocean_box_part_0_art/box_part_0_polygons.inc"
};

static Sint32 box_part_0_vertices[115] = {
#include "stg21/ocean_box_part_0_art/box_part_0_vertices.inc"
};

NJS_CNK_MODEL box_part_0_model = {
#include "stg21/ocean_box_part_0_art/box_part_0_model.inc"
};

static NJS_CNK_OBJECT box_part_0_object = {
#include "stg21/ocean_box_part_0_art/box_part_0_object.inc"
};
