#include <ninja.h>
#include <sg_xpt.h>

#include "shared/small_pot.h"
#include "stg26/pot_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg26/pot_textures/textures.inc"
};
NJS_TEXLIST small_pot_texlist = {
#include "stg26/pot_textures/small_pot_texlist.inc"
};

/* The small carryable pot from Egg Quarters. */

static Sint16 small_pot_polygons[509] = {
#include "stg26/pot_textures/small_pot_polygons.inc"
};

static Sint32 small_pot_vertices[263] = {
#include "stg26/pot_textures/small_pot_vertices.inc"
};

NJS_CNK_MODEL small_pot_model = {
#include "stg26/pot_textures/small_pot_model.inc"
};

static NJS_CNK_OBJECT small_pot_object = {
#include "stg26/pot_textures/small_pot_object.inc"
};
