/* STG23.PRS's texture lists, chunk models, motions and texture names, from
 * the disc. its C lies between block.c and oil_derrick.c, and its D lies
 * between block.c and oil_derrick.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/oil_derrick.h"

#include "stg23/hidden_base_derrick/text.inc"

static NJS_TEXNAME textures[6] = {
#include "stg23/hidden_base_derrick/textures.inc"
};
NJS_TEXLIST oil_derrick_texlist = {
#include "stg23/hidden_base_derrick/oil_derrick_texlist.inc"
};
static Sint16 oil_derrick_object_child_sibling_sibling_polygons[289] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_sibling_polygons.inc"
};
static Sint32 oil_derrick_object_child_sibling_sibling_vertices[163] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_sibling_vertices.inc"
};
static NJS_CNK_MODEL oil_derrick_object_child_sibling_sibling_model = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_sibling_model.inc"
};
static NJS_CNK_OBJECT oil_derrick_object_child_sibling_sibling = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_sibling.inc"
};
static Sint16 oil_derrick_object_child_sibling_child_sibling_polygons[381] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child_sibling_polygons.inc"
};
static Sint32 oil_derrick_object_child_sibling_child_sibling_vertices[219] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child_sibling_vertices.inc"
};
static NJS_CNK_MODEL oil_derrick_object_child_sibling_child_sibling_model = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child_sibling_model.inc"
};
static NJS_CNK_OBJECT oil_derrick_object_child_sibling_child_sibling = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child_sibling.inc"
};
static Sint16 oil_derrick_object_child_sibling_child_polygons[135] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child_polygons.inc"
};
static Sint32 oil_derrick_object_child_sibling_child_vertices[51] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child_vertices.inc"
};
static NJS_CNK_MODEL oil_derrick_object_child_sibling_child_model = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child_model.inc"
};
static NJS_CNK_OBJECT oil_derrick_object_child_sibling_child = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_child.inc"
};
static Sint16 oil_derrick_object_child_sibling_polygons[131] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_polygons.inc"
};
static Sint32 oil_derrick_object_child_sibling_vertices[99] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_vertices.inc"
};
static NJS_CNK_MODEL oil_derrick_object_child_sibling_model = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling_model.inc"
};
static NJS_CNK_OBJECT oil_derrick_object_child_sibling = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_sibling.inc"
};
static Sint16 oil_derrick_object_child_polygons[73] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_polygons.inc"
};
static Sint32 oil_derrick_object_child_vertices[31] = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_vertices.inc"
};
static NJS_CNK_MODEL oil_derrick_object_child_model = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child_model.inc"
};
static NJS_CNK_OBJECT oil_derrick_object_child = {
#include "stg23/hidden_base_derrick/oil_derrick_object_child.inc"
};
static Sint16 oil_derrick_polygons[399] = {
#include "stg23/hidden_base_derrick/oil_derrick_polygons.inc"
};
static Sint32 oil_derrick_vertices[235] = {
#include "stg23/hidden_base_derrick/oil_derrick_vertices.inc"
};
static NJS_CNK_MODEL oil_derrick_model = {
#include "stg23/hidden_base_derrick/oil_derrick_model.inc"
};
NJS_CNK_OBJECT oil_derrick_object = {
#include "stg23/hidden_base_derrick/oil_derrick_object.inc"
};
static NJS_MKEY_A oil_derrick_motion_joint_2_angles[98] = {
#include "stg23/hidden_base_derrick/oil_derrick_motion_joint_2_angles.inc"
};
static NJS_MKEY_A oil_derrick_motion_joint_3_angles[100] = {
#include "stg23/hidden_base_derrick/oil_derrick_motion_joint_3_angles.inc"
};
static NJS_MKEY_A oil_derrick_motion_joint_4_angles[100] = {
#include "stg23/hidden_base_derrick/oil_derrick_motion_joint_4_angles.inc"
};
static NJS_MDATA2 oil_derrick_motion_tracks[6] = {
#include "stg23/hidden_base_derrick/oil_derrick_motion_tracks.inc"
};
NJS_MOTION oil_derrick_motion = {
#include "stg23/hidden_base_derrick/oil_derrick_motion.inc"
};
static NJS_MDATA2 oil_derrick_shape_tracks[6] = {
#include "stg23/hidden_base_derrick/oil_derrick_shape_tracks.inc"
};
NJS_MOTION oil_derrick_shape = {
#include "stg23/hidden_base_derrick/oil_derrick_shape.inc"
};
