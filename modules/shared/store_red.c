#include <ninja.h>
#include <sg_xpt.h>

#include "shared/store_art.h"
#include "shared/store_mesh.h"
#include "shared/store_textures.h"

static NJS_TEXNAME textures[3] = {
#include "shared/store_red/textures.inc"
};
NJS_TEXLIST store_box_red_texlist = {
#include "shared/store_red/store_box_red_texlist.inc"
};

static struct store_box_red_polygons store_box_red_polygons = {
#include "shared/store_red/store_box_red_polygons.inc"
};

static Sint32 store_box_red_vertices[335] = {
#include "shared/store_red/store_box_red_vertices.inc"
};

NJS_CNK_MODEL store_box_red_model = {
#include "shared/store_red/store_box_red_model.inc"
};

NJS_CNK_OBJECT store_box_red_object = {
#include "shared/store_red/store_box_red_object.inc"
};

static NJS_MATERIAL store_box_collision_materials[1] = {
#include "shared/store_red/store_box_collision_materials.inc"
};

static Sint16 store_box_collision_indices[26] = {
#include "shared/store_red/store_box_collision_indices.inc"
};

static NJS_MESHSET store_box_collision_meshsets[1] = {
#include "shared/store_red/store_box_collision_meshsets.inc"
};

static NJS_POINT3 store_box_collision_points[12] = {
#include "shared/store_red/store_box_collision_points.inc"
};

static NJS_VECTOR store_box_collision_normals[12] = {
#include "shared/store_red/store_box_collision_normals.inc"
};

static NJS_MODEL store_box_collision_model = {
#include "shared/store_red/store_box_collision_model.inc"
};

NJS_OBJECT store_box_collision = {
#include "shared/store_red/store_box_collision.inc"
};
