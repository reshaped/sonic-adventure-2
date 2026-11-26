#include <ninja.h>
#include <sg_xpt.h>

#include "shared/orbital/platform.h"

#include "shared/frstage.h"

#include "shared/aircont_textures/text.inc"

static NJS_TEXNAME textures[9] = {
#include "shared/aircont_textures/textures.inc"
};
NJS_TEXLIST frstage_texlist = {
#include "shared/aircont_textures/frstage_texlist.inc"
};

/* clang-format off */
static Sint16 frstage_light_polygons[143] = {
#include "shared/aircont_textures/frstage_light_polygons.inc"
};

static Sint32 frstage_light_vertices[163] = {
#include "shared/aircont_textures/frstage_light_vertices.inc"
};

static NJS_CNK_MODEL frstage_light_model = {
#include "shared/aircont_textures/frstage_light_model.inc"
};

NJS_CNK_OBJECT frstage_light[1] = {
#include "shared/aircont_textures/frstage_light.inc"
};
static Sint16 frstage_floor_polygons[837] = {
#include "shared/aircont_textures/frstage_floor_polygons.inc"
};

static Sint32 frstage_floor_vertices[731] = {
#include "shared/aircont_textures/frstage_floor_vertices.inc"
};

static NJS_CNK_MODEL frstage_floor_model = {
#include "shared/aircont_textures/frstage_floor_model.inc"
};

NJS_CNK_OBJECT frstage_floor[1] = {
#include "shared/aircont_textures/frstage_floor.inc"
};
static NJS_MATERIAL frstage_collision_materials[1] = {
#include "shared/aircont_textures/frstage_collision_materials.inc"
};

static Sint16 frstage_collision_faces[130] = {
#include "shared/aircont_textures/frstage_collision_faces.inc"
};

static NJS_MESHSET frstage_collision_meshsets[1] = {
#include "shared/aircont_textures/frstage_collision_meshsets.inc"
};

static NJS_POINT3 frstage_collision_points[52] = {
#include "shared/aircont_textures/frstage_collision_points.inc"
};

static NJS_VECTOR frstage_collision_normals[52] = {
#include "shared/aircont_textures/frstage_collision_normals.inc"
};

static NJS_MODEL frstage_collision_model = {
#include "shared/aircont_textures/frstage_collision_model.inc"
};

NJS_OBJECT frstage_collision = {
#include "shared/aircont_textures/frstage_collision.inc"
};
/* clang-format on */
