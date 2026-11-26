#include <ninja.h>
#include <sg_xpt.h>

#include "shared/pillar3.h"

static const char material_hasira1[16] = {
#include "shared/ocean_pillar3_art/material_hasira1.inc"
};

static const char material_kabe2[12] = {
#include "shared/ocean_pillar3_art/material_kabe2.inc"
};

static const char material_kage3[12] = {
#include "shared/ocean_pillar3_art/material_kage3.inc"
};

enum { PILLAR3_HASIRA1, PILLAR3_KABE2, PILLAR3_KAGE3 };

static NJS_TEXNAME pillar3_textures[3] = {
#include "shared/ocean_pillar3_art/pillar3_textures.inc"
};

NJS_TEXLIST pillar3_texlist = {
#include "shared/ocean_pillar3_art/pillar3_texlist.inc"
};

static Sint16 pillar3_polygons[243] = {
#include "shared/ocean_pillar3_art/pillar3_polygons.inc"
};

static Sint32 pillar3_vertices[259] = {
#include "shared/ocean_pillar3_art/pillar3_vertices.inc"
};

NJS_CNK_MODEL pillar3_model = {
#include "shared/ocean_pillar3_art/pillar3_model.inc"
};

static NJS_CNK_OBJECT pillar3_object = {
#include "shared/ocean_pillar3_art/pillar3_object.inc"
};
