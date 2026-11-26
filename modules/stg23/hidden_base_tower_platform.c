/* STG23.PRS's texture lists, chunk models, Basic models and motions, from
 * the disc. its D lies between pillar.c and tower.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/tower_models.h"
#include "stg23/tower_textures.h"

static NJS_TEXNAME hidden_base_tower_platform_textures[4] = {
#include "stg23/hidden_base_tower_platform/hidden_base_tower_platform_textures.inc"
};
NJS_TEXLIST tower_platform_texlist = {
#include "stg23/hidden_base_tower_platform/tower_platform_texlist.inc"
};
static Sint16 tower_platform_polygons[739] = {
#include "stg23/hidden_base_tower_platform/tower_platform_polygons.inc"
};
static Sint32 tower_platform_vertices[519] = {
#include "stg23/hidden_base_tower_platform/tower_platform_vertices.inc"
};
NJS_CNK_MODEL tower_platform_model = {
#include "stg23/hidden_base_tower_platform/tower_platform_model.inc"
};
NJS_CNK_OBJECT tower_platform = {
#include "stg23/hidden_base_tower_platform/tower_platform.inc"
};
static NJS_TEXNAME hidden_base_tower_shards_textures[2] = {
#include "stg23/hidden_base_tower_platform/hidden_base_tower_shards_textures.inc"
};
NJS_TEXLIST tower_shards_texlist = {
#include "stg23/hidden_base_tower_platform/tower_shards_texlist.inc"
};
static Sint16 tower_fragment_5_polygons[95] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_5_polygons.inc"
};
static Sint32 tower_fragment_5_vertices[71] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_5_vertices.inc"
};
NJS_CNK_MODEL tower_fragment_5 = {
#include "stg23/hidden_base_tower_platform/tower_fragment_5.inc"
};
static NJS_CNK_OBJECT tower_fragment_5_object = {
#include "stg23/hidden_base_tower_platform/tower_fragment_5_object.inc"
};
static Sint16 tower_fragment_4_polygons[67] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_4_polygons.inc"
};
static Sint32 tower_fragment_4_vertices[51] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_4_vertices.inc"
};
NJS_CNK_MODEL tower_fragment_4 = {
#include "stg23/hidden_base_tower_platform/tower_fragment_4.inc"
};
static NJS_CNK_OBJECT tower_fragment_4_object = {
#include "stg23/hidden_base_tower_platform/tower_fragment_4_object.inc"
};
static Sint16 tower_fragment_3_polygons[87] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_3_polygons.inc"
};
static Sint32 tower_fragment_3_vertices[63] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_3_vertices.inc"
};
NJS_CNK_MODEL tower_fragment_3 = {
#include "stg23/hidden_base_tower_platform/tower_fragment_3.inc"
};
static NJS_CNK_OBJECT tower_fragment_3_object = {
#include "stg23/hidden_base_tower_platform/tower_fragment_3_object.inc"
};
static Sint16 tower_fragment_2_polygons[93] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_2_polygons.inc"
};
static Sint32 tower_fragment_2_vertices[75] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_2_vertices.inc"
};
NJS_CNK_MODEL tower_fragment_2 = {
#include "stg23/hidden_base_tower_platform/tower_fragment_2.inc"
};
static NJS_CNK_OBJECT tower_fragment_2_object = {
#include "stg23/hidden_base_tower_platform/tower_fragment_2_object.inc"
};
static Sint16 tower_fragment_1_polygons[89] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_1_polygons.inc"
};
static Sint32 tower_fragment_1_vertices[67] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_1_vertices.inc"
};
NJS_CNK_MODEL tower_fragment_1 = {
#include "stg23/hidden_base_tower_platform/tower_fragment_1.inc"
};
static NJS_CNK_OBJECT tower_fragment_1_object = {
#include "stg23/hidden_base_tower_platform/tower_fragment_1_object.inc"
};
static Sint16 tower_fragment_0_polygons[81] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_0_polygons.inc"
};
static Sint32 tower_fragment_0_vertices[51] = {
#include "stg23/hidden_base_tower_platform/tower_fragment_0_vertices.inc"
};
NJS_CNK_MODEL tower_fragment_0 = {
#include "stg23/hidden_base_tower_platform/tower_fragment_0.inc"
};
static NJS_CNK_OBJECT tower_fragment_0_object = {
#include "stg23/hidden_base_tower_platform/tower_fragment_0_object.inc"
};
static NJS_CNK_OBJECT tower_shards_root = {
#include "stg23/hidden_base_tower_platform/tower_shards_root.inc"
};
static NJS_TEXNAME hidden_base_tower_support_textures[1] = {
#include "stg23/hidden_base_tower_platform/hidden_base_tower_support_textures.inc"
};
NJS_TEXLIST tower_support_texlist = {
#include "stg23/hidden_base_tower_platform/tower_support_texlist.inc"
};
static Sint16 tower_support_polygons[91] = {
#include "stg23/hidden_base_tower_platform/tower_support_polygons.inc"
};
static Sint32 tower_support_vertices[83] = {
#include "stg23/hidden_base_tower_platform/tower_support_vertices.inc"
};
NJS_CNK_MODEL tower_support_model = {
#include "stg23/hidden_base_tower_platform/tower_support_model.inc"
};
static NJS_CNK_OBJECT tower_support_object = {
#include "stg23/hidden_base_tower_platform/tower_support_object.inc"
};
static NJS_MATERIAL tower_collision_materials[1] = {
#include "stg23/hidden_base_tower_platform/tower_collision_materials.inc"
};
static Sint16 tower_collision_faces[29] = {
#include "stg23/hidden_base_tower_platform/tower_collision_faces.inc"
};
static NJS_MESHSET tower_collision_meshsets[1] = {
#include "stg23/hidden_base_tower_platform/tower_collision_meshsets.inc"
};
static NJS_POINT3 tower_collision_points[12] = {
#include "stg23/hidden_base_tower_platform/tower_collision_points.inc"
};
static NJS_POINT3 tower_collision_normals[12] = {
#include "stg23/hidden_base_tower_platform/tower_collision_normals.inc"
};
static NJS_MODEL tower_collision_model = {
#include "stg23/hidden_base_tower_platform/tower_collision_model.inc"
};
NJS_OBJECT tower_collision = {
#include "stg23/hidden_base_tower_platform/tower_collision.inc"
};
static NJS_MKEY_A tower_motion_joint_0_angles[53] = {
#include "stg23/hidden_base_tower_platform/tower_motion_joint_0_angles.inc"
};
static NJS_MDATA2 tower_motion_tracks[1] = {
#include "stg23/hidden_base_tower_platform/tower_motion_tracks.inc"
};
NJS_MOTION tower_motion[1] = {
#include "stg23/hidden_base_tower_platform/tower_motion.inc"
};
