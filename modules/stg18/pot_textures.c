#include <ninja.h>
#include <sg_xpt.h>

#include "shared/small_pot.h"
#include "stg18/pot_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg18/pot_textures/textures.inc"
};
NJS_TEXLIST small_pot_texlist = {
#include "stg18/pot_textures/small_pot_texlist.inc"
};

/* Carryable earthenware pot. */

static Sint16 small_pot_polygons[597] = {
#include "stg18/pot_textures/small_pot_polygons.inc"
};

static Sint32 small_pot_vertices[291] = {
#include "stg18/pot_textures/small_pot_vertices.inc"
};

NJS_CNK_MODEL small_pot_model = {
#include "stg18/pot_textures/small_pot_model.inc"
};

static NJS_CNK_OBJECT small_pot_object = {
#include "stg18/pot_textures/small_pot_object.inc"
};
