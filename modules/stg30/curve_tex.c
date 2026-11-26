#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/curved_rail_art.h"

const char curve_et64_3[8] = {
#include "stg30/curve_tex/curve_et64_3.inc"
};
const char curve_miu128_fr003[16] = {
#include "stg30/curve_tex/curve_miu128_fr003.inc"
};
const char curve_miu256_fr002[16] = {
#include "stg30/curve_tex/curve_miu256_fr002.inc"
};
const char curve_miu32_fr001[12] = {
#include "stg30/curve_tex/curve_miu32_fr001.inc"
};
const char curve_miu64_fr004[12] = {
#include "stg30/curve_tex/curve_miu64_fr004.inc"
};
const char curve_miu64_fr007[12] = {
#include "stg30/curve_tex/curve_miu64_fr007.inc"
};
const char curve_space64_1[10] = {
#include "stg30/curve_tex/curve_space64_1.inc"
};

static NJS_TEXNAME textures[7] = {
#include "stg30/curve_tex/textures.inc"
};
NJS_TEXLIST curved_rail_texlist = {
#include "stg30/curve_tex/curved_rail_texlist.inc"
};

static Sint16 curved_rail_scroll_polygons[217] = {
#include "stg30/curve_tex/curved_rail_scroll_polygons.inc"
};

static Sint32 curved_rail_scroll_vertices[175] = {
#include "stg30/curve_tex/curved_rail_scroll_vertices.inc"
};

static NJS_CNK_MODEL curved_rail_scroll_mesh = {
#include "stg30/curve_tex/curved_rail_scroll_mesh.inc"
};

static NJS_CNK_OBJECT curved_rail_scroll = {
#include "stg30/curve_tex/curved_rail_scroll.inc"
};

static Sint16 curved_rail_polygons[803] = {
#include "stg30/curve_tex/curved_rail_polygons.inc"
};

static Sint32 curved_rail_vertices[743] = {
#include "stg30/curve_tex/curved_rail_vertices.inc"
};

static NJS_CNK_MODEL curved_rail_mesh = {
#include "stg30/curve_tex/curved_rail_mesh.inc"
};

NJS_CNK_OBJECT curved_rail_object = {
#include "stg30/curve_tex/curved_rail_object.inc"
};

static NJS_MATERIAL curved_rail_collision_materials[1] = {
#include "stg30/curve_tex/curved_rail_collision_materials.inc"
};

static Sint16 curved_rail_collision_indices_0[47] = {
#include "stg30/curve_tex/curved_rail_collision_indices_0.inc"
};

static NJS_MESHSET curved_rail_collision_meshsets[1] = {
#include "stg30/curve_tex/curved_rail_collision_meshsets.inc"
};

static NJS_POINT3 curved_rail_collision_points[18] = {
#include "stg30/curve_tex/curved_rail_collision_points.inc"
};

static NJS_VECTOR curved_rail_collision_normals[18] = {
#include "stg30/curve_tex/curved_rail_collision_normals.inc"
};

static NJS_MODEL curved_rail_collision_model = {
#include "stg30/curve_tex/curved_rail_collision_model.inc"
};

NJS_OBJECT curved_rail_collision = {
#include "stg30/curve_tex/curved_rail_collision.inc"
};
