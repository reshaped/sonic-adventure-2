#include "stg31/prop_art.h"
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 tall_totem_polygons[219] = {
#include "stg31/tall_totem_art/tall_totem_polygons.inc"
};

static Sint32 tall_totem_vertices[175] = {
#include "stg31/tall_totem_art/tall_totem_vertices.inc"
};

static NJS_CNK_MODEL tall_totem_model = {
#include "stg31/tall_totem_art/tall_totem_model.inc"
};

NJS_CNK_OBJECT green_hill_tall_totem_object[1] = {
#include "stg31/tall_totem_art/green_hill_tall_totem_object.inc"
};
