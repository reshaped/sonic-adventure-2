/* The pickup uses this course's material on the shared colony model. */

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/pickup_object_2.h"
#include "stg30/sign_textures.h"

const char pickup_object_2_miu64_fr005[12] = {
#include "stg30/signs/pickup/pickup_object_2_miu64_fr005.inc"
};

static NJS_TEXNAME textures[1] = {
#include "stg30/signs/pickup/textures.inc"
};

NJS_TEXLIST pickup_object_2_texlist = {
#include "stg30/signs/pickup/pickup_object_2_texlist.inc"
};

static Sint16 pickup_polygons[143] = {
#include "stg30/signs/pickup/pickup_polygons.inc"
};

static Sint32 pickup_vertices[71] = {
#include "stg30/signs/pickup/pickup_vertices.inc"
};

NJS_CNK_MODEL pickup_object_2_model = {
#include "stg30/signs/pickup/pickup_object_2_model.inc"
};

/* SHC retains the pickup node alongside the model passed to holdable_init. */
static NJS_CNK_OBJECT pickup_object_node = {
#include "stg30/signs/pickup/pickup_object_node.inc"
};
