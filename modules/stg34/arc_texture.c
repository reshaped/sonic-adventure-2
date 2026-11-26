/* Camera-facing quad for the animated arc. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg34/electric_pillar.h"

#include "stg34/arc_texture/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg34/arc_texture/textures.inc"
};
NJS_TEXLIST electric_arc_texlist = {
#include "stg34/arc_texture/electric_arc_texlist.inc"
};

static Sint16 electric_arc_polygons[25] = {
#include "stg34/arc_texture/electric_arc_polygons.inc"
};

static Sint32 electric_arc_vertices[19] = {
#include "stg34/arc_texture/electric_arc_vertices.inc"
};

NJS_CNK_MODEL electric_arc_model[1] = {
#include "stg34/arc_texture/electric_arc_model.inc"
};

static NJS_CNK_OBJECT electric_arc_object = {
#include "stg34/arc_texture/electric_arc_object.inc"
};
