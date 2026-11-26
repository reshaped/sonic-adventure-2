/* STG23.PRS's texture lists, chunk models and Basic models, from the disc.
 * its D lies between plinth.c and lift.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/platform_textures.h"
#include "stg23/platforms.h"

static NJS_TEXNAME hidden_base_decks_lift_3m_textures[3] = {
#include "stg23/hidden_base_decks_lift_3m/hidden_base_decks_lift_3m_textures.inc"
};
NJS_TEXLIST lift_3m_texlist = {
#include "stg23/hidden_base_decks_lift_3m/lift_3m_texlist.inc"
};
static Sint16 lift_3m_polygons[141] = {
#include "stg23/hidden_base_decks_lift_3m/lift_3m_polygons.inc"
};
static Sint32 lift_3m_vertices[111] = {
#include "stg23/hidden_base_decks_lift_3m/lift_3m_vertices.inc"
};
NJS_CNK_MODEL lift_3m = {
#include "stg23/hidden_base_decks_lift_3m/lift_3m.inc"
};
static NJS_CNK_OBJECT lift_3m_object = {
#include "stg23/hidden_base_decks_lift_3m/lift_3m_object.inc"
};
static NJS_TEXNAME hidden_base_decks_lift_5m_textures[3] = {
#include "stg23/hidden_base_decks_lift_3m/hidden_base_decks_lift_5m_textures.inc"
};
NJS_TEXLIST lift_5m_texlist = {
#include "stg23/hidden_base_decks_lift_3m/lift_5m_texlist.inc"
};
static Sint16 lift_5m_polygons[141] = {
#include "stg23/hidden_base_decks_lift_3m/lift_5m_polygons.inc"
};
static Sint32 lift_5m_vertices[111] = {
#include "stg23/hidden_base_decks_lift_3m/lift_5m_vertices.inc"
};
NJS_CNK_MODEL lift_5m = {
#include "stg23/hidden_base_decks_lift_3m/lift_5m.inc"
};
static NJS_CNK_OBJECT lift_5m_object = {
#include "stg23/hidden_base_decks_lift_3m/lift_5m_object.inc"
};
static NJS_TEXNAME hidden_base_decks_lift_7m_textures[3] = {
#include "stg23/hidden_base_decks_lift_3m/hidden_base_decks_lift_7m_textures.inc"
};
NJS_TEXLIST lift_7m_texlist = {
#include "stg23/hidden_base_decks_lift_3m/lift_7m_texlist.inc"
};
static Sint16 lift_7m_polygons[141] = {
#include "stg23/hidden_base_decks_lift_3m/lift_7m_polygons.inc"
};
static Sint32 lift_7m_vertices[111] = {
#include "stg23/hidden_base_decks_lift_3m/lift_7m_vertices.inc"
};
NJS_CNK_MODEL lift_7m = {
#include "stg23/hidden_base_decks_lift_3m/lift_7m.inc"
};
static NJS_CNK_OBJECT lift_7m_object = {
#include "stg23/hidden_base_decks_lift_3m/lift_7m_object.inc"
};
static NJS_TEXNAME hidden_base_decks_lift_dark_3m_textures[3] = {
#include "stg23/hidden_base_decks_lift_3m/hidden_base_decks_lift_dark_3m_textures.inc"
};
NJS_TEXLIST lift_dark_3m_texlist = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_3m_texlist.inc"
};
static Sint16 lift_dark_3m_polygons[141] = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_3m_polygons.inc"
};
static Sint32 lift_dark_3m_vertices[103] = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_3m_vertices.inc"
};
NJS_CNK_MODEL lift_dark_3m = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_3m.inc"
};
static NJS_CNK_OBJECT lift_dark_3m_object = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_3m_object.inc"
};
static NJS_TEXNAME hidden_base_decks_lift_dark_5m_textures[3] = {
#include "stg23/hidden_base_decks_lift_3m/hidden_base_decks_lift_dark_5m_textures.inc"
};
NJS_TEXLIST lift_dark_5m_texlist = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_5m_texlist.inc"
};
static Sint16 lift_dark_5m_polygons[141] = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_5m_polygons.inc"
};
static Sint32 lift_dark_5m_vertices[103] = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_5m_vertices.inc"
};
NJS_CNK_MODEL lift_dark_5m = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_5m.inc"
};
static NJS_CNK_OBJECT lift_dark_5m_object = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_5m_object.inc"
};
static NJS_TEXNAME hidden_base_decks_lift_dark_7m_textures[3] = {
#include "stg23/hidden_base_decks_lift_3m/hidden_base_decks_lift_dark_7m_textures.inc"
};
NJS_TEXLIST lift_dark_7m_texlist = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_7m_texlist.inc"
};
static Sint16 lift_dark_7m_polygons[141] = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_7m_polygons.inc"
};
static Sint32 lift_dark_7m_vertices[107] = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_7m_vertices.inc"
};
NJS_CNK_MODEL lift_dark_7m = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_7m.inc"
};
static NJS_CNK_OBJECT lift_dark_7m_object = {
#include "stg23/hidden_base_decks_lift_3m/lift_dark_7m_object.inc"
};
static NJS_TEXNAME hidden_base_decks_lift_shadow_textures[1] = {
#include "stg23/hidden_base_decks_lift_3m/hidden_base_decks_lift_shadow_textures.inc"
};
NJS_TEXLIST lift_shadow_texlist = {
#include "stg23/hidden_base_decks_lift_3m/lift_shadow_texlist.inc"
};
static Sint16 lift_shadow_polygons[51] = {
#include "stg23/hidden_base_decks_lift_3m/lift_shadow_polygons.inc"
};
static Sint32 lift_shadow_vertices[39] = {
#include "stg23/hidden_base_decks_lift_3m/lift_shadow_vertices.inc"
};
NJS_CNK_MODEL lift_shadow = {
#include "stg23/hidden_base_decks_lift_3m/lift_shadow.inc"
};
static NJS_CNK_OBJECT lift_shadow_object = {
#include "stg23/hidden_base_decks_lift_3m/lift_shadow_object.inc"
};
static NJS_MATERIAL lift_collision_3m_materials[1] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_3m_materials.inc"
};
static Sint16 lift_collision_3m_faces[16] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_3m_faces.inc"
};
static NJS_MESHSET lift_collision_3m_meshsets[1] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_3m_meshsets.inc"
};
static NJS_POINT3 lift_collision_3m_points[8] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_3m_points.inc"
};
static NJS_POINT3 lift_collision_3m_normals[8] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_3m_normals.inc"
};
static NJS_MODEL lift_collision_3m_model = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_3m_model.inc"
};
NJS_OBJECT lift_collision_3m = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_3m.inc"
};
static NJS_MATERIAL lift_collision_5m_materials[1] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_5m_materials.inc"
};
static Sint16 lift_collision_5m_faces[16] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_5m_faces.inc"
};
static NJS_MESHSET lift_collision_5m_meshsets[1] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_5m_meshsets.inc"
};
static NJS_POINT3 lift_collision_5m_points[8] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_5m_points.inc"
};
static NJS_POINT3 lift_collision_5m_normals[8] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_5m_normals.inc"
};
static NJS_MODEL lift_collision_5m_model = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_5m_model.inc"
};
NJS_OBJECT lift_collision_5m = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_5m.inc"
};
static NJS_MATERIAL lift_collision_7m_materials[1] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_7m_materials.inc"
};
static Sint16 lift_collision_7m_faces[16] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_7m_faces.inc"
};
static NJS_MESHSET lift_collision_7m_meshsets[1] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_7m_meshsets.inc"
};
static NJS_POINT3 lift_collision_7m_points[8] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_7m_points.inc"
};
static NJS_POINT3 lift_collision_7m_normals[8] = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_7m_normals.inc"
};
static NJS_MODEL lift_collision_7m_model = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_7m_model.inc"
};
NJS_OBJECT lift_collision_7m = {
#include "stg23/hidden_base_decks_lift_3m/lift_collision_7m.inc"
};
