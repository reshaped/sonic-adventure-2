#include <ninja.h>
#include <sg_xpt.h>

#include "shared/pickup_object_2.h"
#include "stg22/pickup_art/text.inc"

static NJS_TEXNAME materials[2] = {
#include "stg22/pickup_art/materials.inc"
};
NJS_TEXLIST pickup_object_2_texlist = {
#include "stg22/pickup_art/pickup_object_2_texlist.inc"
};

static Sint16 pickup_polygons[219] = {
#include "stg22/pickup_art/pickup_polygons.inc"
};

static Sint32 pickup_vertices[55] = {
#include "stg22/pickup_art/pickup_vertices.inc"
};

NJS_CNK_MODEL pickup_object_2_model = {
#include "stg22/pickup_art/pickup_object_2_model.inc"
};

static NJS_CNK_OBJECT pickup_object_node = {
#include "stg22/pickup_art/pickup_object_node.inc"
};
