#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stone_box.h"

static Sint16 stone_box_polygons[365] = {
#include "stg21/ocean_stone_box_art/stone_box_polygons.inc"
};

static Sint32 stone_box_vertices[367] = {
#include "stg21/ocean_stone_box_art/stone_box_vertices.inc"
};

NJS_CNK_MODEL stone_box_model = {
#include "stg21/ocean_stone_box_art/stone_box_model.inc"
};

static NJS_CNK_OBJECT stone_box_object = {
#include "stg21/ocean_stone_box_art/stone_box_object.inc"
};
