#include <ninja.h>
#include <sg_xpt.h>

#include "shared/pillar4.h"

static const char material_hasira1[16] = {
#include "shared/ocean_pillar4_art/material_hasira1.inc"
};

static const char material_kabe2[12] = {
#include "shared/ocean_pillar4_art/material_kabe2.inc"
};

static const char material_kage3[12] = {
#include "shared/ocean_pillar4_art/material_kage3.inc"
};

static const char material_sekihi5[14] = {
#include "shared/ocean_pillar4_art/material_sekihi5.inc"
};

enum { PILLAR4_HASIRA1, PILLAR4_KABE2, PILLAR4_KAGE3, PILLAR4_SEKIHI5 };

static NJS_TEXNAME pillar4_textures[4] = {
#include "shared/ocean_pillar4_art/pillar4_textures.inc"
};

NJS_TEXLIST pillar4_texlist = {
#include "shared/ocean_pillar4_art/pillar4_texlist.inc"
};

static Sint16 pillar4_polygons[267] = {
#include "shared/ocean_pillar4_art/pillar4_polygons.inc"
};

static Sint32 pillar4_vertices[283] = {
#include "shared/ocean_pillar4_art/pillar4_vertices.inc"
};

NJS_CNK_MODEL pillar4_model = {
#include "shared/ocean_pillar4_art/pillar4_model.inc"
};

static NJS_CNK_OBJECT pillar4_display = {
#include "shared/ocean_pillar4_art/pillar4_display.inc"
};

static NJS_MATERIAL pillar4_object_materials[1] = {
#include "shared/ocean_pillar4_art/pillar4_object_materials.inc"
};

static Sint16 pillar4_object_faces[59] = {
#include "shared/ocean_pillar4_art/pillar4_object_faces.inc"
};

static NJS_MESHSET pillar4_object_meshsets[1] = {
#include "shared/ocean_pillar4_art/pillar4_object_meshsets.inc"
};

static NJS_POINT3 pillar4_object_points[32] = {
#include "shared/ocean_pillar4_art/pillar4_object_points.inc"
};

static NJS_VECTOR pillar4_object_normals[32] = {
#include "shared/ocean_pillar4_art/pillar4_object_normals.inc"
};

static NJS_MODEL pillar4_object_model = {
#include "shared/ocean_pillar4_art/pillar4_object_model.inc"
};

NJS_OBJECT pillar4_object = {
#include "shared/ocean_pillar4_art/pillar4_object.inc"
};
