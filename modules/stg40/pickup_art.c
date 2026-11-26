/* The pickup uses this course's material on the shared colony model. */

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/pickup_object_2.h"

#include "stg40/pickup_art/text.inc"

static NJS_TEXNAME materials[1] = {
#include "stg40/pickup_art/materials.inc"
};

NJS_TEXLIST pickup_object_2_texlist = {
#include "stg40/pickup_art/pickup_object_2_texlist.inc"
};

static Sint16 pickup_polygons[143] = {
#include "stg40/pickup_art/pickup_polygons.inc"
};

static Sint32 pickup_vertices[71] = {
#include "stg40/pickup_art/pickup_vertices.inc"
};

NJS_CNK_MODEL pickup_object_2_model = {
#include "stg40/pickup_art/pickup_object_2_model.inc"
};

/* SHC retains the pickup node alongside the model passed to holdable_init. */
static NJS_CNK_OBJECT pickup_object_node = {
#include "stg40/pickup_art/pickup_object_node.inc"
};
