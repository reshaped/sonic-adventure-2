#include <ninja.h>
#include <sg_xpt.h>

#include "stg10/pickup_art/text.inc"
#include "shared/pickup_object.h"

static NJS_TEXNAME materials[2] = {
#include "stg10/pickup_art/materials.inc"
};
NJS_TEXLIST pickup_object_texlist = {
#include "stg10/pickup_art/pickup_object_texlist.inc"
};

static Sint16 pickup_polygons[219] = {
#include "stg10/pickup_art/pickup_polygons.inc"
};

static Sint32 pickup_vertices[119] = {
#include "stg10/pickup_art/pickup_vertices.inc"
};

NJS_CNK_MODEL pickup_object_model = {
#include "stg10/pickup_art/pickup_object_model.inc"
};

static NJS_CNK_OBJECT pickup_object_node = {
#include "stg10/pickup_art/pickup_object_node.inc"
};
