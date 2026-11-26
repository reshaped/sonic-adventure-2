/* STG23.PRS's texture lists, chunk models and motions, from the disc. its D
 * lies between stacked_platform.c and staff.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/staff_models.h"
#include "stg23/staff_textures.h"

static NJS_TEXNAME hidden_base_staff_bright_textures[1] = {
#include "stg23/hidden_base_staff_bright/hidden_base_staff_bright_textures.inc"
};
NJS_TEXLIST staff_bright_texlist = {
#include "stg23/hidden_base_staff_bright/staff_bright_texlist.inc"
};
static Sint16 staff_bright_polygons[385] = {
#include "stg23/hidden_base_staff_bright/staff_bright_polygons.inc"
};
static Sint32 staff_bright_vertices[207] = {
#include "stg23/hidden_base_staff_bright/staff_bright_vertices.inc"
};
NJS_CNK_MODEL staff_bright_model = {
#include "stg23/hidden_base_staff_bright/staff_bright_model.inc"
};
static NJS_CNK_OBJECT staff_bright_object = {
#include "stg23/hidden_base_staff_bright/staff_bright_object.inc"
};
static NJS_TEXNAME hidden_base_staff_bright_broken_textures[1] = {
#include "stg23/hidden_base_staff_bright/hidden_base_staff_bright_broken_textures.inc"
};
NJS_TEXLIST staff_bright_broken_texlist = {
#include "stg23/hidden_base_staff_bright/staff_bright_broken_texlist.inc"
};
static Sint16 staff_bright_broken_polygons[181] = {
#include "stg23/hidden_base_staff_bright/staff_bright_broken_polygons.inc"
};
static Sint32 staff_bright_broken_vertices[67] = {
#include "stg23/hidden_base_staff_bright/staff_bright_broken_vertices.inc"
};
static NJS_CNK_MODEL staff_bright_broken_model = {
#include "stg23/hidden_base_staff_bright/staff_bright_broken_model.inc"
};
NJS_CNK_OBJECT staff_bright_broken = {
#include "stg23/hidden_base_staff_bright/staff_bright_broken.inc"
};
static NJS_TEXNAME hidden_base_staff_bright_shards_textures[1] = {
#include "stg23/hidden_base_staff_bright/hidden_base_staff_bright_shards_textures.inc"
};
NJS_TEXLIST staff_bright_shards_texlist = {
#include "stg23/hidden_base_staff_bright/staff_bright_shards_texlist.inc"
};
static Sint16 staff_bright_shard_1_polygons[101] = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_1_polygons.inc"
};
static Sint32 staff_bright_shard_1_vertices[75] = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_1_vertices.inc"
};
NJS_CNK_MODEL staff_bright_shard_1 = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_1.inc"
};
static NJS_CNK_OBJECT staff_bright_shard_1_object = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_1_object.inc"
};
static Sint16 staff_bright_shard_0_polygons[101] = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_0_polygons.inc"
};
static Sint32 staff_bright_shard_0_vertices[75] = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_0_vertices.inc"
};
NJS_CNK_MODEL staff_bright_shard_0 = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_0.inc"
};
static NJS_CNK_OBJECT staff_bright_shard_0_object = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_0_object.inc"
};
static Sint16 staff_bright_shard_2_polygons[101] = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_2_polygons.inc"
};
static Sint32 staff_bright_shard_2_vertices[71] = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_2_vertices.inc"
};
NJS_CNK_MODEL staff_bright_shard_2 = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_2.inc"
};
static NJS_CNK_OBJECT staff_bright_shard_2_object = {
#include "stg23/hidden_base_staff_bright/staff_bright_shard_2_object.inc"
};
static NJS_CNK_OBJECT staff_bright_shards_root = {
#include "stg23/hidden_base_staff_bright/staff_bright_shards_root.inc"
};
static NJS_TEXNAME hidden_base_staff_dark_textures[1] = {
#include "stg23/hidden_base_staff_bright/hidden_base_staff_dark_textures.inc"
};
NJS_TEXLIST staff_dark_texlist = {
#include "stg23/hidden_base_staff_bright/staff_dark_texlist.inc"
};
static Sint16 staff_dark_polygons[385] = {
#include "stg23/hidden_base_staff_bright/staff_dark_polygons.inc"
};
static Sint32 staff_dark_vertices[279] = {
#include "stg23/hidden_base_staff_bright/staff_dark_vertices.inc"
};
NJS_CNK_MODEL staff_dark_model = {
#include "stg23/hidden_base_staff_bright/staff_dark_model.inc"
};
static NJS_CNK_OBJECT staff_dark_object = {
#include "stg23/hidden_base_staff_bright/staff_dark_object.inc"
};
static NJS_TEXNAME hidden_base_staff_dark_broken_textures[1] = {
#include "stg23/hidden_base_staff_bright/hidden_base_staff_dark_broken_textures.inc"
};
NJS_TEXLIST staff_dark_broken_texlist = {
#include "stg23/hidden_base_staff_bright/staff_dark_broken_texlist.inc"
};
static Sint16 staff_dark_broken_polygons[181] = {
#include "stg23/hidden_base_staff_bright/staff_dark_broken_polygons.inc"
};
static Sint32 staff_dark_broken_vertices[83] = {
#include "stg23/hidden_base_staff_bright/staff_dark_broken_vertices.inc"
};
static NJS_CNK_MODEL staff_dark_broken_model = {
#include "stg23/hidden_base_staff_bright/staff_dark_broken_model.inc"
};
NJS_CNK_OBJECT staff_dark_broken = {
#include "stg23/hidden_base_staff_bright/staff_dark_broken.inc"
};
static NJS_TEXNAME hidden_base_staff_dark_shards_textures[1] = {
#include "stg23/hidden_base_staff_bright/hidden_base_staff_dark_shards_textures.inc"
};
NJS_TEXLIST staff_dark_shards_texlist = {
#include "stg23/hidden_base_staff_bright/staff_dark_shards_texlist.inc"
};
static Sint16 staff_dark_shard_1_polygons[101] = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_1_polygons.inc"
};
static Sint32 staff_dark_shard_1_vertices[107] = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_1_vertices.inc"
};
NJS_CNK_MODEL staff_dark_shard_1 = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_1.inc"
};
static NJS_CNK_OBJECT staff_dark_shard_1_object = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_1_object.inc"
};
static Sint16 staff_dark_shard_2_polygons[109] = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_2_polygons.inc"
};
static Sint32 staff_dark_shard_2_vertices[71] = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_2_vertices.inc"
};
NJS_CNK_MODEL staff_dark_shard_2 = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_2.inc"
};
static NJS_CNK_OBJECT staff_dark_shard_2_object = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_2_object.inc"
};
static Sint16 staff_dark_shard_0_polygons[101] = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_0_polygons.inc"
};
static Sint32 staff_dark_shard_0_vertices[107] = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_0_vertices.inc"
};
NJS_CNK_MODEL staff_dark_shard_0 = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_0.inc"
};
static NJS_CNK_OBJECT staff_dark_shard_0_object = {
#include "stg23/hidden_base_staff_bright/staff_dark_shard_0_object.inc"
};
static NJS_CNK_OBJECT staff_dark_shards_root = {
#include "stg23/hidden_base_staff_bright/staff_dark_shards_root.inc"
};
static NJS_MKEY_A staff_break_motion_joint_0_angles[74] = {
#include "stg23/hidden_base_staff_bright/staff_break_motion_joint_0_angles.inc"
};
static NJS_MDATA2 staff_break_motion_tracks[1] = {
#include "stg23/hidden_base_staff_bright/staff_break_motion_tracks.inc"
};
NJS_MOTION staff_break_motion = {
#include "stg23/hidden_base_staff_bright/staff_break_motion.inc"
};
