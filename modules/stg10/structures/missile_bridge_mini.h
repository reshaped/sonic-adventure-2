/* Missile bridge mini geometry. */
#ifndef STG10_STRUCTURES_MISSILE_BRIDGE_MINI_H
#define STG10_STRUCTURES_MISSILE_BRIDGE_MINI_H
#include <ninja.h>
#include <sg_xpt.h>

enum missile_bridge_mini_texture_slot {
    MISSILE_BRIDGE_MINI_TEX_MIU64_IG019,
    MISSILE_BRIDGE_MINI_TEX_MIU64_MH001,
    MISSILE_BRIDGE_MINI_TEX_MIU64_MH003,
    MISSILE_BRIDGE_MINI_TEX_MIU64_MH005,
    MISSILE_BRIDGE_MINI_TEX_PR64_21,
    MISSILE_BRIDGE_MINI_TEX_PR_3KAGE32,
    MISSILE_BRIDGE_MINI_TEXTURE_COUNT
};

static NJS_TEXNAME
    missile_bridge_mini_names[MISSILE_BRIDGE_MINI_TEXTURE_COUNT] = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_names.inc"
};

static NJS_TEXLIST missile_bridge_mini_texlist = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_texlist.inc"
};

static Sint16 missile_bridge_mini_polygons[377] = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_polygons.inc"
};

static Sint32 missile_bridge_mini_vertices[303] = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_vertices.inc"
};

static NJS_CNK_MODEL missile_bridge_mini_model = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_model.inc"
};

static NJS_CNK_OBJECT missile_bridge_mini_object = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_object.inc"
};

/* SHC initializes the two packed colors without braces. */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL missile_bridge_mini_solid_materials = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_solid_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Uint16 missile_bridge_mini_solid_strips[33] = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_solid_strips.inc"
};

static NJS_MESHSET missile_bridge_mini_solid_faces = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_solid_faces.inc"
};

static NJS_POINT3 missile_bridge_mini_solid_points[16] = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_solid_points.inc"
};

static NJS_VECTOR missile_bridge_mini_solid_normals[16] = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_solid_normals.inc"
};

static NJS_MODEL missile_bridge_mini_solid_model = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_solid_model.inc"
};

static NJS_OBJECT missile_bridge_mini_collision = {
#include "stg10/structures/missile_bridge_mini/missile_bridge_mini_collision.inc"
};

#endif
