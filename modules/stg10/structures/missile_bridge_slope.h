/* Missile bridge slope geometry. */
#ifndef STG10_STRUCTURES_MISSILE_BRIDGE_SLOPE_H
#define STG10_STRUCTURES_MISSILE_BRIDGE_SLOPE_H
#include <ninja.h>
#include <sg_xpt.h>

enum missile_bridge_slope_texture_slot {
    MISSILE_BRIDGE_SLOPE_TEX_MIU128_MH001,
    MISSILE_BRIDGE_SLOPE_TEX_MIU64_MH002,
    MISSILE_BRIDGE_SLOPE_TEX_MIU64_MH003,
    MISSILE_BRIDGE_SLOPE_TEX_MIU64_MH006,
    MISSILE_BRIDGE_SLOPE_TEX_PR128_8,
    MISSILE_BRIDGE_SLOPE_TEX_PR32_1,
    MISSILE_BRIDGE_SLOPE_TEX_PR32_3,
    MISSILE_BRIDGE_SLOPE_TEX_PR64_12,
    MISSILE_BRIDGE_SLOPE_TEX_PR_3KAGE32,
    MISSILE_BRIDGE_SLOPE_TEXTURE_COUNT
};

static NJS_TEXNAME
    missile_bridge_slope_names[MISSILE_BRIDGE_SLOPE_TEXTURE_COUNT] = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_names.inc"
};

static NJS_TEXLIST missile_bridge_slope_texlist = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_texlist.inc"
};

static Sint16 missile_bridge_slope_polygons[1055] = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_polygons.inc"
};

static Sint32 missile_bridge_slope_vertices[1107] = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_vertices.inc"
};

static NJS_CNK_MODEL missile_bridge_slope_model = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_model.inc"
};

static NJS_CNK_OBJECT missile_bridge_slope_object = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_object.inc"
};

/* SHC initializes the two packed colors without braces. */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL missile_bridge_slope_solid_materials = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_solid_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Uint16 missile_bridge_slope_solid_strips[75] = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_solid_strips.inc"
};

static NJS_MESHSET missile_bridge_slope_solid_faces = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_solid_faces.inc"
};

static NJS_POINT3 missile_bridge_slope_solid_points[34] = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_solid_points.inc"
};

static NJS_VECTOR missile_bridge_slope_solid_normals[34] = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_solid_normals.inc"
};

static NJS_MODEL missile_bridge_slope_solid_model = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_solid_model.inc"
};

static NJS_OBJECT missile_bridge_slope_collision = {
#include "stg10/structures/missile_bridge_slope/missile_bridge_slope_collision.inc"
};

#endif
