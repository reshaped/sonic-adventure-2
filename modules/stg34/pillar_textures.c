/* Column for the low electric pillar. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg34/electric_pillar.h"

#include "stg34/pillar_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg34/pillar_textures/textures.inc"
};
NJS_TEXLIST electric_pillar_texlist = {
#include "stg34/pillar_textures/electric_pillar_texlist.inc"
};

static Sint16 electric_pillar_polygons[381] = {
#include "stg34/pillar_textures/electric_pillar_polygons.inc"
};

static Sint32 electric_pillar_vertices[155] = {
#include "stg34/pillar_textures/electric_pillar_vertices.inc"
};

NJS_CNK_MODEL electric_pillar_model = {
#include "stg34/pillar_textures/electric_pillar_model.inc"
};

static NJS_CNK_OBJECT electric_pillar_object = {
#include "stg34/pillar_textures/electric_pillar_object.inc"
};
