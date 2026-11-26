#include <ninja.h>
#include <sg_xpt.h>

#include "stg13/pickup.h"
#include "stg13/pickup_art/text.inc"

/* clang-format off */

enum { PICKUP_METAL, PICKUP_PANEL };

static NJS_TEXNAME pickup_textures[2] = {
#include "stg13/pickup_art/pickup_textures.inc"
};

NJS_TEXLIST pickup_texlist = {
#include "stg13/pickup_art/pickup_texlist.inc"
};

static Sint16 pickup_polygons[171] = {
#include "stg13/pickup_art/pickup_polygons.inc"
};

static Sint32 pickup_vertices[195] = {
#include "stg13/pickup_art/pickup_vertices.inc"
};

NJS_CNK_MODEL pickup_model = {
#include "stg13/pickup_art/pickup_model.inc"
};

static NJS_CNK_OBJECT pickup_object = {
#include "stg13/pickup_art/pickup_object.inc"
};

/* clang-format on */
