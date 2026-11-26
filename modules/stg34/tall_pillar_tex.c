/* Column for the tall electric pillar. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg34/electric_pillar.h"

#include "stg34/tall_pillar_tex/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg34/tall_pillar_tex/textures.inc"
};
NJS_TEXLIST tall_pillar_texlist = {
#include "stg34/tall_pillar_tex/tall_pillar_texlist.inc"
};

static Sint16 tall_pillar_polygons[929] = {
#include "stg34/tall_pillar_tex/tall_pillar_polygons.inc"
};

static Sint32 tall_pillar_vertices[411] = {
#include "stg34/tall_pillar_tex/tall_pillar_vertices.inc"
};

NJS_CNK_MODEL tall_pillar_model = {
#include "stg34/tall_pillar_tex/tall_pillar_model.inc"
};

static NJS_CNK_OBJECT tall_pillar_object = {
#include "stg34/tall_pillar_tex/tall_pillar_object.inc"
};
