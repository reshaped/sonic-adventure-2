#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/curved_rail_art.h"

const char curve_miu64_ee002[12] = {
#include "stg30/curve_b_tex/curve_miu64_ee002.inc"
};

static NJS_TEXNAME textures[6] = {
#include "stg30/curve_b_tex/textures.inc"
};
NJS_TEXLIST curved_rail_b_texlist = {
#include "stg30/curve_b_tex/curved_rail_b_texlist.inc"
};

static Sint16 curved_rail_b_scroll_polygons[217] = {
#include "stg30/curve_b_tex/curved_rail_b_scroll_polygons.inc"
};

static Sint32 curved_rail_b_scroll_vertices[175] = {
#include "stg30/curve_b_tex/curved_rail_b_scroll_vertices.inc"
};

static NJS_CNK_MODEL curved_rail_b_scroll_mesh = {
#include "stg30/curve_b_tex/curved_rail_b_scroll_mesh.inc"
};

static NJS_CNK_OBJECT curved_rail_b_scroll = {
#include "stg30/curve_b_tex/curved_rail_b_scroll.inc"
};

static Sint16 curved_rail_b_polygons[821] = {
#include "stg30/curve_b_tex/curved_rail_b_polygons.inc"
};

static Sint32 curved_rail_b_vertices[703] = {
#include "stg30/curve_b_tex/curved_rail_b_vertices.inc"
};

static NJS_CNK_MODEL curved_rail_b_mesh = {
#include "stg30/curve_b_tex/curved_rail_b_mesh.inc"
};

NJS_CNK_OBJECT curved_rail_b_object = {
#include "stg30/curve_b_tex/curved_rail_b_object.inc"
};

static NJS_MATERIAL curved_rail_b_collision_child_materials[1] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_child_materials.inc"
};

static Sint16 curved_rail_b_collision_child_indices_0[19] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_child_indices_0.inc"
};

static NJS_MESHSET curved_rail_b_collision_child_meshsets[1] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_child_meshsets.inc"
};

static NJS_POINT3 curved_rail_b_collision_child_points[8] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_child_points.inc"
};

static NJS_VECTOR curved_rail_b_collision_child_normals[8] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_child_normals.inc"
};

static NJS_MODEL curved_rail_b_collision_child_model = {
#include "stg30/curve_b_tex/curved_rail_b_collision_child_model.inc"
};

static NJS_OBJECT curved_rail_b_collision_child = {
#include "stg30/curve_b_tex/curved_rail_b_collision_child.inc"
};

static NJS_MATERIAL curved_rail_b_collision_materials[1] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_materials.inc"
};

static Sint16 curved_rail_b_collision_indices_0[65] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_indices_0.inc"
};

static NJS_MESHSET curved_rail_b_collision_meshsets[1] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_meshsets.inc"
};

static NJS_POINT3 curved_rail_b_collision_points[24] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_points.inc"
};

static NJS_VECTOR curved_rail_b_collision_normals[24] = {
#include "stg30/curve_b_tex/curved_rail_b_collision_normals.inc"
};

static NJS_MODEL curved_rail_b_collision_model = {
#include "stg30/curve_b_tex/curved_rail_b_collision_model.inc"
};

NJS_OBJECT curved_rail_b_collision[1] = {
#include "stg30/curve_b_tex/curved_rail_b_collision.inc"
};
