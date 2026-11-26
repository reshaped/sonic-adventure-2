/* the goal door's texture names and two SET editor labels */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_goal.h"

#include "shared/core_door_art/text.inc"

static const char door_yaw_caption[12] = "<- ANGLE Y";

const struct core_door_material_names core_door_material_names = {
    TEXT_1, TEXT_2, TEXT_3, TEXT_4, TEXT_5, TEXT_6, TEXT_7, TEXT_8,
};

/* the texture lists, chunk models, motions, chunk object pointers and
 * texture list pointers of STG37.PRS and STG38.PRS, from the disc. its D
 * precedes this file's own D */
static NJS_TEXNAME door_materials[8] = {
#include "shared/core_door_art/door_materials.inc"
};
static NJS_TEXLIST door_texlist = {
#include "shared/core_door_art/door_texlist.inc"
};
static Sint16 frame_polygons[117] = {
#include "shared/core_door_art/frame_polygons.inc"
};
static Sint32 frame_vertices[99] = {
#include "shared/core_door_art/frame_vertices.inc"
};
static NJS_CNK_MODEL frame_model = {
#include "shared/core_door_art/frame_model.inc"
};
static NJS_CNK_OBJECT frame = {
#include "shared/core_door_art/frame.inc"
};
static Sint16 left_upper_shaft_polygons[77] = {
#include "shared/core_door_art/left_upper_shaft_polygons.inc"
};
static Sint32 left_upper_shaft_vertices[87] = {
#include "shared/core_door_art/left_upper_shaft_vertices.inc"
};
static NJS_CNK_MODEL left_upper_shaft_model = {
#include "shared/core_door_art/left_upper_shaft_model.inc"
};
static NJS_CNK_OBJECT left_upper_shaft = {
#include "shared/core_door_art/left_upper_shaft.inc"
};
static Sint16 left_lower_shaft_polygons[77] = {
#include "shared/core_door_art/left_lower_shaft_polygons.inc"
};
static Sint32 left_lower_shaft_vertices[87] = {
#include "shared/core_door_art/left_lower_shaft_vertices.inc"
};
static NJS_CNK_MODEL left_lower_shaft_model = {
#include "shared/core_door_art/left_lower_shaft_model.inc"
};
static NJS_CNK_OBJECT left_lower_shaft = {
#include "shared/core_door_art/left_lower_shaft.inc"
};
static NJS_CNK_OBJECT left_shafts = {
#include "shared/core_door_art/left_shafts.inc"
};
static Sint16 left_upper_glass_polygons[185] = {
#include "shared/core_door_art/left_upper_glass_polygons.inc"
};
static Sint32 left_upper_glass_vertices[143] = {
#include "shared/core_door_art/left_upper_glass_vertices.inc"
};
static NJS_CNK_MODEL left_upper_glass_model = {
#include "shared/core_door_art/left_upper_glass_model.inc"
};
static NJS_CNK_OBJECT left_upper_glass = {
#include "shared/core_door_art/left_upper_glass.inc"
};
static Sint16 left_lower_glass_polygons[185] = {
#include "shared/core_door_art/left_lower_glass_polygons.inc"
};
static Sint32 left_lower_glass_vertices[143] = {
#include "shared/core_door_art/left_lower_glass_vertices.inc"
};
static NJS_CNK_MODEL left_lower_glass_model = {
#include "shared/core_door_art/left_lower_glass_model.inc"
};
static NJS_CNK_OBJECT left_lower_glass = {
#include "shared/core_door_art/left_lower_glass.inc"
};
static NJS_CNK_OBJECT left_glass = {
#include "shared/core_door_art/left_glass.inc"
};
static Sint16 left_leaf_polygons[605] = {
#include "shared/core_door_art/left_leaf_polygons.inc"
};
static Sint32 left_leaf_vertices[571] = {
#include "shared/core_door_art/left_leaf_vertices.inc"
};
static NJS_CNK_MODEL left_leaf_model = {
#include "shared/core_door_art/left_leaf_model.inc"
};
static NJS_CNK_OBJECT left_leaf = {
#include "shared/core_door_art/left_leaf.inc"
};
static Sint16 right_upper_shaft_polygons[77] = {
#include "shared/core_door_art/right_upper_shaft_polygons.inc"
};
static Sint32 right_upper_shaft_vertices[87] = {
#include "shared/core_door_art/right_upper_shaft_vertices.inc"
};
static NJS_CNK_MODEL right_upper_shaft_model = {
#include "shared/core_door_art/right_upper_shaft_model.inc"
};
static NJS_CNK_OBJECT right_upper_shaft = {
#include "shared/core_door_art/right_upper_shaft.inc"
};
static Sint16 right_lower_shaft_polygons[77] = {
#include "shared/core_door_art/right_lower_shaft_polygons.inc"
};
static Sint32 right_lower_shaft_vertices[87] = {
#include "shared/core_door_art/right_lower_shaft_vertices.inc"
};
static NJS_CNK_MODEL right_lower_shaft_model = {
#include "shared/core_door_art/right_lower_shaft_model.inc"
};
static NJS_CNK_OBJECT right_lower_shaft = {
#include "shared/core_door_art/right_lower_shaft.inc"
};
static NJS_CNK_OBJECT right_shafts = {
#include "shared/core_door_art/right_shafts.inc"
};
static Sint16 right_upper_glass_polygons[185] = {
#include "shared/core_door_art/right_upper_glass_polygons.inc"
};
static Sint32 right_upper_glass_vertices[143] = {
#include "shared/core_door_art/right_upper_glass_vertices.inc"
};
static NJS_CNK_MODEL right_upper_glass_model = {
#include "shared/core_door_art/right_upper_glass_model.inc"
};
static NJS_CNK_OBJECT right_upper_glass = {
#include "shared/core_door_art/right_upper_glass.inc"
};
static Sint16 right_lower_glass_polygons[185] = {
#include "shared/core_door_art/right_lower_glass_polygons.inc"
};
static Sint32 right_lower_glass_vertices[143] = {
#include "shared/core_door_art/right_lower_glass_vertices.inc"
};
static NJS_CNK_MODEL right_lower_glass_model = {
#include "shared/core_door_art/right_lower_glass_model.inc"
};
static NJS_CNK_OBJECT right_lower_glass = {
#include "shared/core_door_art/right_lower_glass.inc"
};
static NJS_CNK_OBJECT right_glass = {
#include "shared/core_door_art/right_glass.inc"
};
static Sint16 right_leaf_polygons[519] = {
#include "shared/core_door_art/right_leaf_polygons.inc"
};
static Sint32 right_leaf_vertices[511] = {
#include "shared/core_door_art/right_leaf_vertices.inc"
};
static NJS_CNK_MODEL right_leaf_model = {
#include "shared/core_door_art/right_leaf_model.inc"
};
static NJS_CNK_OBJECT right_leaf = {
#include "shared/core_door_art/right_leaf.inc"
};
static NJS_CNK_OBJECT door_root = {
#include "shared/core_door_art/door_root.inc"
};
static NJS_MKEY_F core_door_motion_joint_1_position[172] = {
#include "shared/core_door_art/core_door_motion_joint_1_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_3_position[86] = {
#include "shared/core_door_art/core_door_motion_joint_3_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_4_position[91] = {
#include "shared/core_door_art/core_door_motion_joint_4_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_6_position[19] = {
#include "shared/core_door_art/core_door_motion_joint_6_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_7_position[19] = {
#include "shared/core_door_art/core_door_motion_joint_7_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_8_position[178] = {
#include "shared/core_door_art/core_door_motion_joint_8_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_10_position[86] = {
#include "shared/core_door_art/core_door_motion_joint_10_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_11_position[91] = {
#include "shared/core_door_art/core_door_motion_joint_11_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_13_position[19] = {
#include "shared/core_door_art/core_door_motion_joint_13_position.inc"
};
static NJS_MKEY_F core_door_motion_joint_14_position[19] = {
#include "shared/core_door_art/core_door_motion_joint_14_position.inc"
};
static NJS_MDATA3 core_door_motion_tracks[16] = {
#include "shared/core_door_art/core_door_motion_tracks.inc"
};
NJS_MOTION core_door_motion[1] = {
#include "shared/core_door_art/core_door_motion.inc"
};
static NJS_MDATA2 shape_motion_tracks[16] = {
#include "shared/core_door_art/shape_motion_tracks.inc"
};
static NJS_MOTION shape_motion = {
#include "shared/core_door_art/shape_motion.inc"
};
NJS_CNK_OBJECT *core_door_models[1] = {
#include "shared/core_door_art/core_door_models.inc"
};
NJS_TEXLIST *core_door_texlists[2] = {
#include "shared/core_door_art/core_door_texlists.inc"
};

static const char *editor_label = " ";
