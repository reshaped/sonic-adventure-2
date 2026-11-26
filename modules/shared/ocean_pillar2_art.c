#include <ninja.h>
#include <sg_xpt.h>

#include "shared/pillar2.h"

static const char material_hasira1[16] = {
#include "shared/ocean_pillar2_art/material_hasira1.inc"
};

static const char material_hasira2[16] = {
#include "shared/ocean_pillar2_art/material_hasira2.inc"
};

static const char material_kage3[12] = {
#include "shared/ocean_pillar2_art/material_kage3.inc"
};

enum { PILLAR2_HASIRA1, PILLAR2_HASIRA2, PILLAR2_KAGE3 };

static NJS_TEXNAME pillar2_textures[3] = {
#include "shared/ocean_pillar2_art/pillar2_textures.inc"
};

NJS_TEXLIST pillar2_texlist = {
#include "shared/ocean_pillar2_art/pillar2_texlist.inc"
};

static Sint16 pillar2_polygons[157] = {
#include "shared/ocean_pillar2_art/pillar2_polygons.inc"
};

static Sint32 pillar2_vertices[107] = {
#include "shared/ocean_pillar2_art/pillar2_vertices.inc"
};

NJS_CNK_MODEL pillar2_model = {
#include "shared/ocean_pillar2_art/pillar2_model.inc"
};

static NJS_CNK_OBJECT pillar2_object = {
#include "shared/ocean_pillar2_art/pillar2_object.inc"
};
