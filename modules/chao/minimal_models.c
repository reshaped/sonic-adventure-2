/* CHAO.PRS's chunk models, motions, chunk object pointers and ninja motion
 * pointers, from the disc. its D lies between minimal.c and movement.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/minimal.h"
#include "object/animal.h"

static Sint16 minimal_raccoon_polygons_9[89] = {
#include "chao/minimal_models/minimal_raccoon_polygons_9.inc"
};
static Sint32 minimal_raccoon_vertices_9[63] = {
#include "chao/minimal_models/minimal_raccoon_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_9 = {
#include "chao/minimal_models/minimal_raccoon_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_9 = {
#include "chao/minimal_models/minimal_raccoon_node_9.inc"
};
static Sint16 minimal_raccoon_polygons_8[59] = {
#include "chao/minimal_models/minimal_raccoon_polygons_8.inc"
};
static Sint32 minimal_raccoon_vertices_8[51] = {
#include "chao/minimal_models/minimal_raccoon_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_8 = {
#include "chao/minimal_models/minimal_raccoon_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_8 = {
#include "chao/minimal_models/minimal_raccoon_node_8.inc"
};
static Sint16 minimal_raccoon_polygons_7[59] = {
#include "chao/minimal_models/minimal_raccoon_polygons_7.inc"
};
static Sint32 minimal_raccoon_vertices_7[51] = {
#include "chao/minimal_models/minimal_raccoon_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_7 = {
#include "chao/minimal_models/minimal_raccoon_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_7 = {
#include "chao/minimal_models/minimal_raccoon_node_7.inc"
};
static Sint16 minimal_raccoon_polygons_6[25] = {
#include "chao/minimal_models/minimal_raccoon_polygons_6.inc"
};
static Sint32 minimal_raccoon_vertices_6[27] = {
#include "chao/minimal_models/minimal_raccoon_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_6 = {
#include "chao/minimal_models/minimal_raccoon_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_6 = {
#include "chao/minimal_models/minimal_raccoon_node_6.inc"
};
static Sint16 minimal_raccoon_polygons_5[25] = {
#include "chao/minimal_models/minimal_raccoon_polygons_5.inc"
};
static Sint32 minimal_raccoon_vertices_5[27] = {
#include "chao/minimal_models/minimal_raccoon_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_5 = {
#include "chao/minimal_models/minimal_raccoon_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_5 = {
#include "chao/minimal_models/minimal_raccoon_node_5.inc"
};
static Sint16 minimal_raccoon_polygons_4[181] = {
#include "chao/minimal_models/minimal_raccoon_polygons_4.inc"
};
static Sint32 minimal_raccoon_vertices_4[99] = {
#include "chao/minimal_models/minimal_raccoon_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_4 = {
#include "chao/minimal_models/minimal_raccoon_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_4 = {
#include "chao/minimal_models/minimal_raccoon_node_4.inc"
};
static Sint16 minimal_raccoon_polygons_3[45] = {
#include "chao/minimal_models/minimal_raccoon_polygons_3.inc"
};
static Sint32 minimal_raccoon_vertices_3[27] = {
#include "chao/minimal_models/minimal_raccoon_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_3 = {
#include "chao/minimal_models/minimal_raccoon_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_3 = {
#include "chao/minimal_models/minimal_raccoon_node_3.inc"
};
static Sint16 minimal_raccoon_polygons_2[45] = {
#include "chao/minimal_models/minimal_raccoon_polygons_2.inc"
};
static Sint32 minimal_raccoon_vertices_2[27] = {
#include "chao/minimal_models/minimal_raccoon_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_2 = {
#include "chao/minimal_models/minimal_raccoon_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_2 = {
#include "chao/minimal_models/minimal_raccoon_node_2.inc"
};
static Sint16 minimal_raccoon_polygons_1[187] = {
#include "chao/minimal_models/minimal_raccoon_polygons_1.inc"
};
static Sint32 minimal_raccoon_vertices_1[87] = {
#include "chao/minimal_models/minimal_raccoon_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_raccoon_mesh_1 = {
#include "chao/minimal_models/minimal_raccoon_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_node_1 = {
#include "chao/minimal_models/minimal_raccoon_node_1.inc"
};
static NJS_CNK_OBJECT minimal_raccoon_model = {
#include "chao/minimal_models/minimal_raccoon_model.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_5_angles[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_6_angles[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_8_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_8_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_held_joint_9_position[2] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_9_position.inc"
};
static NJS_MKEY_A minimal_raccoon_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_held_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_raccoon_held_tracks[10] = {
#include "chao/minimal_models/minimal_raccoon_held_tracks.inc"
};
static NJS_MOTION minimal_raccoon_held = {
#include "chao/minimal_models/minimal_raccoon_held.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_3_position[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_5_position[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_6_position[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_7_position[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_8_position[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_raccoon_walk_joint_9_position[2] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_9_position.inc"
};
static NJS_MKEY_A minimal_raccoon_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_raccoon_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_raccoon_walk_tracks[10] = {
#include "chao/minimal_models/minimal_raccoon_walk_tracks.inc"
};
static NJS_MOTION minimal_raccoon_walk = {
#include "chao/minimal_models/minimal_raccoon_walk.inc"
};
static NJS_MKEY_F minimal_raccoon_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_raccoon_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_raccoon_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_raccoon_idle_tracks[10] = {
#include "chao/minimal_models/minimal_raccoon_idle_tracks.inc"
};
static NJS_MOTION minimal_raccoon_idle = {
#include "chao/minimal_models/minimal_raccoon_idle.inc"
};
static Sint16 minimal_bat_polygons_8[61] = {
#include "chao/minimal_models/minimal_bat_polygons_8.inc"
};
static Sint32 minimal_bat_vertices_8[45] = {
#include "chao/minimal_models/minimal_bat_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_8 = {
#include "chao/minimal_models/minimal_bat_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_8 = {
#include "chao/minimal_models/minimal_bat_node_8.inc"
};
static Sint16 minimal_bat_polygons_7[47] = {
#include "chao/minimal_models/minimal_bat_polygons_7.inc"
};
static Sint32 minimal_bat_vertices_7[51] = {
#include "chao/minimal_models/minimal_bat_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_7 = {
#include "chao/minimal_models/minimal_bat_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_7 = {
#include "chao/minimal_models/minimal_bat_node_7.inc"
};
static Sint16 minimal_bat_polygons_6[55] = {
#include "chao/minimal_models/minimal_bat_polygons_6.inc"
};
static Sint32 minimal_bat_vertices_6[51] = {
#include "chao/minimal_models/minimal_bat_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_6 = {
#include "chao/minimal_models/minimal_bat_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_6 = {
#include "chao/minimal_models/minimal_bat_node_6.inc"
};
static Sint16 minimal_bat_polygons_5[43] = {
#include "chao/minimal_models/minimal_bat_polygons_5.inc"
};
static Sint32 minimal_bat_vertices_5[27] = {
#include "chao/minimal_models/minimal_bat_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_5 = {
#include "chao/minimal_models/minimal_bat_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_5 = {
#include "chao/minimal_models/minimal_bat_node_5.inc"
};
static Sint16 minimal_bat_polygons_4[43] = {
#include "chao/minimal_models/minimal_bat_polygons_4.inc"
};
static Sint32 minimal_bat_vertices_4[27] = {
#include "chao/minimal_models/minimal_bat_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_4 = {
#include "chao/minimal_models/minimal_bat_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_4 = {
#include "chao/minimal_models/minimal_bat_node_4.inc"
};
static Sint16 minimal_bat_polygons_3[45] = {
#include "chao/minimal_models/minimal_bat_polygons_3.inc"
};
static Sint32 minimal_bat_vertices_3[45] = {
#include "chao/minimal_models/minimal_bat_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_3 = {
#include "chao/minimal_models/minimal_bat_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_3 = {
#include "chao/minimal_models/minimal_bat_node_3.inc"
};
static Sint16 minimal_bat_polygons_2[45] = {
#include "chao/minimal_models/minimal_bat_polygons_2.inc"
};
static Sint32 minimal_bat_vertices_2[45] = {
#include "chao/minimal_models/minimal_bat_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_2 = {
#include "chao/minimal_models/minimal_bat_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_2 = {
#include "chao/minimal_models/minimal_bat_node_2.inc"
};
static Sint16 minimal_bat_polygons_1[411] = {
#include "chao/minimal_models/minimal_bat_polygons_1.inc"
};
static Sint32 minimal_bat_vertices_1[189] = {
#include "chao/minimal_models/minimal_bat_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_bat_mesh_1 = {
#include "chao/minimal_models/minimal_bat_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_bat_node_1 = {
#include "chao/minimal_models/minimal_bat_node_1.inc"
};
static NJS_CNK_OBJECT minimal_bat_model = {
#include "chao/minimal_models/minimal_bat_model.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_bat_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_bat_held_joint_8_angles.inc"
};
static NJS_MDATA2 minimal_bat_held_tracks[9] = {
#include "chao/minimal_models/minimal_bat_held_tracks.inc"
};
static NJS_MOTION minimal_bat_held = {
#include "chao/minimal_models/minimal_bat_held.inc"
};
static NJS_MKEY_F minimal_bat_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_bat_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_bat_walk_joint_8_angles.inc"
};
static NJS_MDATA2 minimal_bat_walk_tracks[9] = {
#include "chao/minimal_models/minimal_bat_walk_tracks.inc"
};
static NJS_MOTION minimal_bat_walk = {
#include "chao/minimal_models/minimal_bat_walk.inc"
};
static NJS_MKEY_F minimal_bat_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_bat_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_bat_idle_joint_8_angles.inc"
};
static NJS_MDATA2 minimal_bat_idle_tracks[9] = {
#include "chao/minimal_models/minimal_bat_idle_tracks.inc"
};
static NJS_MOTION minimal_bat_idle = {
#include "chao/minimal_models/minimal_bat_idle.inc"
};
static Sint16 minimal_skeleton_dog_polygons_7[51] = {
#include "chao/minimal_models/minimal_skeleton_dog_polygons_7.inc"
};
static Sint32 minimal_skeleton_dog_vertices_7[33] = {
#include "chao/minimal_models/minimal_skeleton_dog_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_skeleton_dog_mesh_7 = {
#include "chao/minimal_models/minimal_skeleton_dog_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_node_7 = {
#include "chao/minimal_models/minimal_skeleton_dog_node_7.inc"
};
static Sint16 minimal_skeleton_dog_polygons_6[25] = {
#include "chao/minimal_models/minimal_skeleton_dog_polygons_6.inc"
};
static Sint32 minimal_skeleton_dog_vertices_6[27] = {
#include "chao/minimal_models/minimal_skeleton_dog_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_skeleton_dog_mesh_6 = {
#include "chao/minimal_models/minimal_skeleton_dog_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_node_6 = {
#include "chao/minimal_models/minimal_skeleton_dog_node_6.inc"
};
static Sint16 minimal_skeleton_dog_polygons_5[51] = {
#include "chao/minimal_models/minimal_skeleton_dog_polygons_5.inc"
};
static Sint32 minimal_skeleton_dog_vertices_5[33] = {
#include "chao/minimal_models/minimal_skeleton_dog_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_skeleton_dog_mesh_5 = {
#include "chao/minimal_models/minimal_skeleton_dog_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_node_5 = {
#include "chao/minimal_models/minimal_skeleton_dog_node_5.inc"
};
static Sint16 minimal_skeleton_dog_polygons_4[51] = {
#include "chao/minimal_models/minimal_skeleton_dog_polygons_4.inc"
};
static Sint32 minimal_skeleton_dog_vertices_4[33] = {
#include "chao/minimal_models/minimal_skeleton_dog_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_skeleton_dog_mesh_4 = {
#include "chao/minimal_models/minimal_skeleton_dog_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_node_4 = {
#include "chao/minimal_models/minimal_skeleton_dog_node_4.inc"
};
static Sint16 minimal_skeleton_dog_polygons_3[51] = {
#include "chao/minimal_models/minimal_skeleton_dog_polygons_3.inc"
};
static Sint32 minimal_skeleton_dog_vertices_3[33] = {
#include "chao/minimal_models/minimal_skeleton_dog_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_skeleton_dog_mesh_3 = {
#include "chao/minimal_models/minimal_skeleton_dog_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_node_3 = {
#include "chao/minimal_models/minimal_skeleton_dog_node_3.inc"
};
static Sint16 minimal_skeleton_dog_polygons_2[309] = {
#include "chao/minimal_models/minimal_skeleton_dog_polygons_2.inc"
};
static Sint32 minimal_skeleton_dog_vertices_2[195] = {
#include "chao/minimal_models/minimal_skeleton_dog_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_skeleton_dog_mesh_2 = {
#include "chao/minimal_models/minimal_skeleton_dog_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_node_2 = {
#include "chao/minimal_models/minimal_skeleton_dog_node_2.inc"
};
static Sint16 minimal_skeleton_dog_polygons_1[143] = {
#include "chao/minimal_models/minimal_skeleton_dog_polygons_1.inc"
};
static Sint32 minimal_skeleton_dog_vertices_1[99] = {
#include "chao/minimal_models/minimal_skeleton_dog_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_skeleton_dog_mesh_1 = {
#include "chao/minimal_models/minimal_skeleton_dog_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_node_1 = {
#include "chao/minimal_models/minimal_skeleton_dog_node_1.inc"
};
static NJS_CNK_OBJECT minimal_skeleton_dog_model = {
#include "chao/minimal_models/minimal_skeleton_dog_model.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_1_angles[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_2_angles[17] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_3_angles[17] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_4_angles[17] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_5_angles[17] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_6_angles[17] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_held_joint_7_angles[17] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_skeleton_dog_held_tracks[8] = {
#include "chao/minimal_models/minimal_skeleton_dog_held_tracks.inc"
};
static NJS_MOTION minimal_skeleton_dog_held = {
#include "chao/minimal_models/minimal_skeleton_dog_held.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_2_position[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_3_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_5_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_6_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_walk_joint_7_position[2] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_skeleton_dog_walk_tracks[8] = {
#include "chao/minimal_models/minimal_skeleton_dog_walk_tracks.inc"
};
static NJS_MOTION minimal_skeleton_dog_walk = {
#include "chao/minimal_models/minimal_skeleton_dog_walk.inc"
};
static NJS_MKEY_F minimal_skeleton_dog_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_skeleton_dog_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_skeleton_dog_idle_tracks[8] = {
#include "chao/minimal_models/minimal_skeleton_dog_idle_tracks.inc"
};
static NJS_MOTION minimal_skeleton_dog_idle = {
#include "chao/minimal_models/minimal_skeleton_dog_idle.inc"
};
static Sint16 minimal_cheetah_polygons_7[69] = {
#include "chao/minimal_models/minimal_cheetah_polygons_7.inc"
};
static Sint32 minimal_cheetah_vertices_7[51] = {
#include "chao/minimal_models/minimal_cheetah_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_cheetah_mesh_7 = {
#include "chao/minimal_models/minimal_cheetah_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_node_7 = {
#include "chao/minimal_models/minimal_cheetah_node_7.inc"
};
static Sint16 minimal_cheetah_polygons_6[45] = {
#include "chao/minimal_models/minimal_cheetah_polygons_6.inc"
};
static Sint32 minimal_cheetah_vertices_6[33] = {
#include "chao/minimal_models/minimal_cheetah_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_cheetah_mesh_6 = {
#include "chao/minimal_models/minimal_cheetah_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_node_6 = {
#include "chao/minimal_models/minimal_cheetah_node_6.inc"
};
static Sint16 minimal_cheetah_polygons_5[45] = {
#include "chao/minimal_models/minimal_cheetah_polygons_5.inc"
};
static Sint32 minimal_cheetah_vertices_5[33] = {
#include "chao/minimal_models/minimal_cheetah_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_cheetah_mesh_5 = {
#include "chao/minimal_models/minimal_cheetah_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_node_5 = {
#include "chao/minimal_models/minimal_cheetah_node_5.inc"
};
static Sint16 minimal_cheetah_polygons_4[45] = {
#include "chao/minimal_models/minimal_cheetah_polygons_4.inc"
};
static Sint32 minimal_cheetah_vertices_4[33] = {
#include "chao/minimal_models/minimal_cheetah_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_cheetah_mesh_4 = {
#include "chao/minimal_models/minimal_cheetah_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_node_4 = {
#include "chao/minimal_models/minimal_cheetah_node_4.inc"
};
static Sint16 minimal_cheetah_polygons_3[45] = {
#include "chao/minimal_models/minimal_cheetah_polygons_3.inc"
};
static Sint32 minimal_cheetah_vertices_3[33] = {
#include "chao/minimal_models/minimal_cheetah_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_cheetah_mesh_3 = {
#include "chao/minimal_models/minimal_cheetah_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_node_3 = {
#include "chao/minimal_models/minimal_cheetah_node_3.inc"
};
static Sint16 minimal_cheetah_polygons_2[245] = {
#include "chao/minimal_models/minimal_cheetah_polygons_2.inc"
};
static Sint32 minimal_cheetah_vertices_2[159] = {
#include "chao/minimal_models/minimal_cheetah_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_cheetah_mesh_2 = {
#include "chao/minimal_models/minimal_cheetah_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_node_2 = {
#include "chao/minimal_models/minimal_cheetah_node_2.inc"
};
static Sint16 minimal_cheetah_polygons_1[135] = {
#include "chao/minimal_models/minimal_cheetah_polygons_1.inc"
};
static Sint32 minimal_cheetah_vertices_1[99] = {
#include "chao/minimal_models/minimal_cheetah_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_cheetah_mesh_1 = {
#include "chao/minimal_models/minimal_cheetah_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_node_1 = {
#include "chao/minimal_models/minimal_cheetah_node_1.inc"
};
static NJS_CNK_OBJECT minimal_cheetah_model = {
#include "chao/minimal_models/minimal_cheetah_model.inc"
};
static NJS_MKEY_F minimal_cheetah_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_0_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_1_angles[2] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_held_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_cheetah_held_tracks[8] = {
#include "chao/minimal_models/minimal_cheetah_held_tracks.inc"
};
static NJS_MOTION minimal_cheetah_held = {
#include "chao/minimal_models/minimal_cheetah_held.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_3_position[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_4_position[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_5_position[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_6_position[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_cheetah_walk_joint_7_position[2] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_cheetah_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_cheetah_walk_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_cheetah_walk_tracks[8] = {
#include "chao/minimal_models/minimal_cheetah_walk_tracks.inc"
};
static NJS_MOTION minimal_cheetah_walk = {
#include "chao/minimal_models/minimal_cheetah_walk.inc"
};
static NJS_MKEY_F minimal_cheetah_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_cheetah_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_cheetah_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_cheetah_idle_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_cheetah_idle_tracks[8] = {
#include "chao/minimal_models/minimal_cheetah_idle_tracks.inc"
};
static NJS_MOTION minimal_cheetah_idle = {
#include "chao/minimal_models/minimal_cheetah_idle.inc"
};
static Sint16 minimal_dragon_polygons_11[73] = {
#include "chao/minimal_models/minimal_dragon_polygons_11.inc"
};
static Sint32 minimal_dragon_vertices_11[57] = {
#include "chao/minimal_models/minimal_dragon_vertices_11.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_11 = {
#include "chao/minimal_models/minimal_dragon_mesh_11.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_11 = {
#include "chao/minimal_models/minimal_dragon_node_11.inc"
};
static Sint16 minimal_dragon_polygons_10[25] = {
#include "chao/minimal_models/minimal_dragon_polygons_10.inc"
};
static Sint32 minimal_dragon_vertices_10[27] = {
#include "chao/minimal_models/minimal_dragon_vertices_10.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_10 = {
#include "chao/minimal_models/minimal_dragon_mesh_10.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_10 = {
#include "chao/minimal_models/minimal_dragon_node_10.inc"
};
static Sint16 minimal_dragon_polygons_9[25] = {
#include "chao/minimal_models/minimal_dragon_polygons_9.inc"
};
static Sint32 minimal_dragon_vertices_9[27] = {
#include "chao/minimal_models/minimal_dragon_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_9 = {
#include "chao/minimal_models/minimal_dragon_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_9 = {
#include "chao/minimal_models/minimal_dragon_node_9.inc"
};
static Sint16 minimal_dragon_polygons_8[49] = {
#include "chao/minimal_models/minimal_dragon_polygons_8.inc"
};
static Sint32 minimal_dragon_vertices_8[45] = {
#include "chao/minimal_models/minimal_dragon_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_8 = {
#include "chao/minimal_models/minimal_dragon_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_8 = {
#include "chao/minimal_models/minimal_dragon_node_8.inc"
};
static Sint16 minimal_dragon_polygons_7[57] = {
#include "chao/minimal_models/minimal_dragon_polygons_7.inc"
};
static Sint32 minimal_dragon_vertices_7[39] = {
#include "chao/minimal_models/minimal_dragon_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_7 = {
#include "chao/minimal_models/minimal_dragon_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_7 = {
#include "chao/minimal_models/minimal_dragon_node_7.inc"
};
static Sint16 minimal_dragon_polygons_6[49] = {
#include "chao/minimal_models/minimal_dragon_polygons_6.inc"
};
static Sint32 minimal_dragon_vertices_6[45] = {
#include "chao/minimal_models/minimal_dragon_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_6 = {
#include "chao/minimal_models/minimal_dragon_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_6 = {
#include "chao/minimal_models/minimal_dragon_node_6.inc"
};
static Sint16 minimal_dragon_polygons_5[57] = {
#include "chao/minimal_models/minimal_dragon_polygons_5.inc"
};
static Sint32 minimal_dragon_vertices_5[39] = {
#include "chao/minimal_models/minimal_dragon_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_5 = {
#include "chao/minimal_models/minimal_dragon_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_5 = {
#include "chao/minimal_models/minimal_dragon_node_5.inc"
};
static Sint16 minimal_dragon_polygons_4[49] = {
#include "chao/minimal_models/minimal_dragon_polygons_4.inc"
};
static Sint32 minimal_dragon_vertices_4[45] = {
#include "chao/minimal_models/minimal_dragon_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_4 = {
#include "chao/minimal_models/minimal_dragon_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_4 = {
#include "chao/minimal_models/minimal_dragon_node_4.inc"
};
static Sint16 minimal_dragon_polygons_3[49] = {
#include "chao/minimal_models/minimal_dragon_polygons_3.inc"
};
static Sint32 minimal_dragon_vertices_3[45] = {
#include "chao/minimal_models/minimal_dragon_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_3 = {
#include "chao/minimal_models/minimal_dragon_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_3 = {
#include "chao/minimal_models/minimal_dragon_node_3.inc"
};
static Sint16 minimal_dragon_polygons_2[391] = {
#include "chao/minimal_models/minimal_dragon_polygons_2.inc"
};
static Sint32 minimal_dragon_vertices_2[237] = {
#include "chao/minimal_models/minimal_dragon_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_2 = {
#include "chao/minimal_models/minimal_dragon_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_2 = {
#include "chao/minimal_models/minimal_dragon_node_2.inc"
};
static Sint16 minimal_dragon_polygons_1[117] = {
#include "chao/minimal_models/minimal_dragon_polygons_1.inc"
};
static Sint32 minimal_dragon_vertices_1[87] = {
#include "chao/minimal_models/minimal_dragon_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_dragon_mesh_1 = {
#include "chao/minimal_models/minimal_dragon_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_dragon_node_1 = {
#include "chao/minimal_models/minimal_dragon_node_1.inc"
};
static NJS_CNK_OBJECT minimal_dragon_model = {
#include "chao/minimal_models/minimal_dragon_model.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_10_angles.inc"
};
static NJS_MKEY_A minimal_dragon_held_joint_11_angles[16] = {
#include "chao/minimal_models/minimal_dragon_held_joint_11_angles.inc"
};
static NJS_MDATA2 minimal_dragon_held_tracks[12] = {
#include "chao/minimal_models/minimal_dragon_held_tracks.inc"
};
static NJS_MOTION minimal_dragon_held = {
#include "chao/minimal_models/minimal_dragon_held.inc"
};
static NJS_MKEY_F minimal_dragon_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_10_angles.inc"
};
static NJS_MKEY_A minimal_dragon_walk_joint_11_angles[16] = {
#include "chao/minimal_models/minimal_dragon_walk_joint_11_angles.inc"
};
static NJS_MDATA2 minimal_dragon_walk_tracks[12] = {
#include "chao/minimal_models/minimal_dragon_walk_tracks.inc"
};
static NJS_MOTION minimal_dragon_walk = {
#include "chao/minimal_models/minimal_dragon_walk.inc"
};
static NJS_MKEY_F minimal_dragon_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_10_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_10_angles.inc"
};
static NJS_MKEY_A minimal_dragon_idle_joint_11_angles[4] = {
#include "chao/minimal_models/minimal_dragon_idle_joint_11_angles.inc"
};
static NJS_MDATA2 minimal_dragon_idle_tracks[12] = {
#include "chao/minimal_models/minimal_dragon_idle_tracks.inc"
};
static NJS_MOTION minimal_dragon_idle = {
#include "chao/minimal_models/minimal_dragon_idle.inc"
};
static Sint16 minimal_seal_polygons_7[45] = {
#include "chao/minimal_models/minimal_seal_polygons_7.inc"
};
static Sint32 minimal_seal_vertices_7[39] = {
#include "chao/minimal_models/minimal_seal_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_seal_mesh_7 = {
#include "chao/minimal_models/minimal_seal_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_seal_node_7 = {
#include "chao/minimal_models/minimal_seal_node_7.inc"
};
static Sint16 minimal_seal_polygons_6[45] = {
#include "chao/minimal_models/minimal_seal_polygons_6.inc"
};
static Sint32 minimal_seal_vertices_6[39] = {
#include "chao/minimal_models/minimal_seal_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_seal_mesh_6 = {
#include "chao/minimal_models/minimal_seal_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_seal_node_6 = {
#include "chao/minimal_models/minimal_seal_node_6.inc"
};
static Sint16 minimal_seal_polygons_5[39] = {
#include "chao/minimal_models/minimal_seal_polygons_5.inc"
};
static Sint32 minimal_seal_vertices_5[39] = {
#include "chao/minimal_models/minimal_seal_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_seal_mesh_5 = {
#include "chao/minimal_models/minimal_seal_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_seal_node_5 = {
#include "chao/minimal_models/minimal_seal_node_5.inc"
};
static Sint16 minimal_seal_polygons_4[39] = {
#include "chao/minimal_models/minimal_seal_polygons_4.inc"
};
static Sint32 minimal_seal_vertices_4[39] = {
#include "chao/minimal_models/minimal_seal_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_seal_mesh_4 = {
#include "chao/minimal_models/minimal_seal_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_seal_node_4 = {
#include "chao/minimal_models/minimal_seal_node_4.inc"
};
static Sint16 minimal_seal_polygons_3[23] = {
#include "chao/minimal_models/minimal_seal_polygons_3.inc"
};
static Sint32 minimal_seal_vertices_3[21] = {
#include "chao/minimal_models/minimal_seal_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_seal_mesh_3 = {
#include "chao/minimal_models/minimal_seal_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_seal_node_3 = {
#include "chao/minimal_models/minimal_seal_node_3.inc"
};
static Sint16 minimal_seal_polygons_2[23] = {
#include "chao/minimal_models/minimal_seal_polygons_2.inc"
};
static Sint32 minimal_seal_vertices_2[21] = {
#include "chao/minimal_models/minimal_seal_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_seal_mesh_2 = {
#include "chao/minimal_models/minimal_seal_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_seal_node_2 = {
#include "chao/minimal_models/minimal_seal_node_2.inc"
};
static Sint16 minimal_seal_polygons_1[397] = {
#include "chao/minimal_models/minimal_seal_polygons_1.inc"
};
static Sint32 minimal_seal_vertices_1[213] = {
#include "chao/minimal_models/minimal_seal_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_seal_mesh_1 = {
#include "chao/minimal_models/minimal_seal_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_seal_node_1 = {
#include "chao/minimal_models/minimal_seal_node_1.inc"
};
static NJS_CNK_OBJECT minimal_seal_model = {
#include "chao/minimal_models/minimal_seal_model.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_1_angles[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_seal_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_seal_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_seal_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_seal_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_seal_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_seal_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_seal_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_seal_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_seal_held_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_seal_held_tracks[8] = {
#include "chao/minimal_models/minimal_seal_held_tracks.inc"
};
static NJS_MOTION minimal_seal_held = {
#include "chao/minimal_models/minimal_seal_held.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_3_position[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_5_position[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_6_position[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_seal_walk_joint_7_position[2] = {
#include "chao/minimal_models/minimal_seal_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_seal_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_seal_walk_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_seal_walk_tracks[8] = {
#include "chao/minimal_models/minimal_seal_walk_tracks.inc"
};
static NJS_MOTION minimal_seal_walk = {
#include "chao/minimal_models/minimal_seal_walk.inc"
};
static NJS_MKEY_F minimal_seal_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_seal_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_seal_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_seal_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_seal_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_seal_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_seal_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_seal_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_seal_idle_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_seal_idle_tracks[8] = {
#include "chao/minimal_models/minimal_seal_idle_tracks.inc"
};
static NJS_MOTION minimal_seal_idle = {
#include "chao/minimal_models/minimal_seal_idle.inc"
};
static Sint16 minimal_gorilla_polygons_5[71] = {
#include "chao/minimal_models/minimal_gorilla_polygons_5.inc"
};
static Sint32 minimal_gorilla_vertices_5[45] = {
#include "chao/minimal_models/minimal_gorilla_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_gorilla_mesh_5 = {
#include "chao/minimal_models/minimal_gorilla_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_gorilla_node_5 = {
#include "chao/minimal_models/minimal_gorilla_node_5.inc"
};
static Sint16 minimal_gorilla_polygons_4[71] = {
#include "chao/minimal_models/minimal_gorilla_polygons_4.inc"
};
static Sint32 minimal_gorilla_vertices_4[45] = {
#include "chao/minimal_models/minimal_gorilla_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_gorilla_mesh_4 = {
#include "chao/minimal_models/minimal_gorilla_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_gorilla_node_4 = {
#include "chao/minimal_models/minimal_gorilla_node_4.inc"
};
static Sint16 minimal_gorilla_polygons_3[83] = {
#include "chao/minimal_models/minimal_gorilla_polygons_3.inc"
};
static Sint32 minimal_gorilla_vertices_3[57] = {
#include "chao/minimal_models/minimal_gorilla_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_gorilla_mesh_3 = {
#include "chao/minimal_models/minimal_gorilla_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_gorilla_node_3 = {
#include "chao/minimal_models/minimal_gorilla_node_3.inc"
};
static Sint16 minimal_gorilla_polygons_2[83] = {
#include "chao/minimal_models/minimal_gorilla_polygons_2.inc"
};
static Sint32 minimal_gorilla_vertices_2[57] = {
#include "chao/minimal_models/minimal_gorilla_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_gorilla_mesh_2 = {
#include "chao/minimal_models/minimal_gorilla_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_gorilla_node_2 = {
#include "chao/minimal_models/minimal_gorilla_node_2.inc"
};
static Sint16 minimal_gorilla_polygons_1[233] = {
#include "chao/minimal_models/minimal_gorilla_polygons_1.inc"
};
static Sint32 minimal_gorilla_vertices_1[165] = {
#include "chao/minimal_models/minimal_gorilla_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_gorilla_mesh_1 = {
#include "chao/minimal_models/minimal_gorilla_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_gorilla_node_1 = {
#include "chao/minimal_models/minimal_gorilla_node_1.inc"
};
static NJS_CNK_OBJECT minimal_gorilla_model = {
#include "chao/minimal_models/minimal_gorilla_model.inc"
};
static NJS_MKEY_F minimal_gorilla_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_gorilla_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_held_joint_1_position[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_gorilla_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_gorilla_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_gorilla_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_held_joint_4_position[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_gorilla_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_held_joint_5_position[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_gorilla_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_held_joint_5_angles.inc"
};
static NJS_MDATA2 minimal_gorilla_held_tracks[6] = {
#include "chao/minimal_models/minimal_gorilla_held_tracks.inc"
};
static NJS_MOTION minimal_gorilla_held = {
#include "chao/minimal_models/minimal_gorilla_held.inc"
};
static NJS_MKEY_F minimal_gorilla_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_gorilla_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_gorilla_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_walk_joint_2_position[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_gorilla_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_walk_joint_3_position[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_gorilla_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_walk_joint_4_position[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_gorilla_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_gorilla_walk_joint_5_position[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_gorilla_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_gorilla_walk_joint_5_angles.inc"
};
static NJS_MDATA2 minimal_gorilla_walk_tracks[6] = {
#include "chao/minimal_models/minimal_gorilla_walk_tracks.inc"
};
static NJS_MOTION minimal_gorilla_walk = {
#include "chao/minimal_models/minimal_gorilla_walk.inc"
};
static NJS_MKEY_F minimal_gorilla_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_gorilla_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_gorilla_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_gorilla_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_gorilla_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_gorilla_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_gorilla_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_gorilla_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_gorilla_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_gorilla_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_gorilla_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_gorilla_idle_joint_5_angles.inc"
};
static NJS_MDATA2 minimal_gorilla_idle_tracks[6] = {
#include "chao/minimal_models/minimal_gorilla_idle_tracks.inc"
};
static NJS_MOTION minimal_gorilla_idle = {
#include "chao/minimal_models/minimal_gorilla_idle.inc"
};
static Sint16 minimal_half_fish_polygons_10[23] = {
#include "chao/minimal_models/minimal_half_fish_polygons_10.inc"
};
static Sint32 minimal_half_fish_vertices_10[21] = {
#include "chao/minimal_models/minimal_half_fish_vertices_10.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_10 = {
#include "chao/minimal_models/minimal_half_fish_mesh_10.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_10 = {
#include "chao/minimal_models/minimal_half_fish_node_10.inc"
};
static Sint16 minimal_half_fish_polygons_9[105] = {
#include "chao/minimal_models/minimal_half_fish_polygons_9.inc"
};
static Sint32 minimal_half_fish_vertices_9[81] = {
#include "chao/minimal_models/minimal_half_fish_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_9 = {
#include "chao/minimal_models/minimal_half_fish_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_9 = {
#include "chao/minimal_models/minimal_half_fish_node_9.inc"
};
static Sint16 minimal_half_fish_polygons_8[23] = {
#include "chao/minimal_models/minimal_half_fish_polygons_8.inc"
};
static Sint32 minimal_half_fish_vertices_8[21] = {
#include "chao/minimal_models/minimal_half_fish_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_8 = {
#include "chao/minimal_models/minimal_half_fish_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_8 = {
#include "chao/minimal_models/minimal_half_fish_node_8.inc"
};
static Sint16 minimal_half_fish_polygons_7[249] = {
#include "chao/minimal_models/minimal_half_fish_polygons_7.inc"
};
static Sint32 minimal_half_fish_vertices_7[147] = {
#include "chao/minimal_models/minimal_half_fish_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_7 = {
#include "chao/minimal_models/minimal_half_fish_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_7 = {
#include "chao/minimal_models/minimal_half_fish_node_7.inc"
};
static Sint16 minimal_half_fish_polygons_6[23] = {
#include "chao/minimal_models/minimal_half_fish_polygons_6.inc"
};
static Sint32 minimal_half_fish_vertices_6[21] = {
#include "chao/minimal_models/minimal_half_fish_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_6 = {
#include "chao/minimal_models/minimal_half_fish_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_6 = {
#include "chao/minimal_models/minimal_half_fish_node_6.inc"
};
static Sint16 minimal_half_fish_polygons_5[59] = {
#include "chao/minimal_models/minimal_half_fish_polygons_5.inc"
};
static Sint32 minimal_half_fish_vertices_5[51] = {
#include "chao/minimal_models/minimal_half_fish_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_5 = {
#include "chao/minimal_models/minimal_half_fish_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_5 = {
#include "chao/minimal_models/minimal_half_fish_node_5.inc"
};
static Sint16 minimal_half_fish_polygons_4[45] = {
#include "chao/minimal_models/minimal_half_fish_polygons_4.inc"
};
static Sint32 minimal_half_fish_vertices_4[39] = {
#include "chao/minimal_models/minimal_half_fish_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_4 = {
#include "chao/minimal_models/minimal_half_fish_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_4 = {
#include "chao/minimal_models/minimal_half_fish_node_4.inc"
};
static Sint16 minimal_half_fish_polygons_3[59] = {
#include "chao/minimal_models/minimal_half_fish_polygons_3.inc"
};
static Sint32 minimal_half_fish_vertices_3[51] = {
#include "chao/minimal_models/minimal_half_fish_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_3 = {
#include "chao/minimal_models/minimal_half_fish_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_3 = {
#include "chao/minimal_models/minimal_half_fish_node_3.inc"
};
static Sint16 minimal_half_fish_polygons_2[45] = {
#include "chao/minimal_models/minimal_half_fish_polygons_2.inc"
};
static Sint32 minimal_half_fish_vertices_2[39] = {
#include "chao/minimal_models/minimal_half_fish_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_2 = {
#include "chao/minimal_models/minimal_half_fish_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_2 = {
#include "chao/minimal_models/minimal_half_fish_node_2.inc"
};
static Sint16 minimal_half_fish_polygons_1[179] = {
#include "chao/minimal_models/minimal_half_fish_polygons_1.inc"
};
static Sint32 minimal_half_fish_vertices_1[147] = {
#include "chao/minimal_models/minimal_half_fish_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_half_fish_mesh_1 = {
#include "chao/minimal_models/minimal_half_fish_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_node_1 = {
#include "chao/minimal_models/minimal_half_fish_node_1.inc"
};
static NJS_CNK_OBJECT minimal_half_fish_model = {
#include "chao/minimal_models/minimal_half_fish_model.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_8_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_8_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_9_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_9_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_9_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_held_joint_10_position[2] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_10_position.inc"
};
static NJS_MKEY_A minimal_half_fish_held_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_held_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_half_fish_held_tracks[11] = {
#include "chao/minimal_models/minimal_half_fish_held_tracks.inc"
};
static NJS_MOTION minimal_half_fish_held = {
#include "chao/minimal_models/minimal_half_fish_held.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_3_position[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_3_angles[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_5_position[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_5_angles[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_6_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_7_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_8_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_9_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_9_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_9_angles.inc"
};
static NJS_MKEY_F minimal_half_fish_walk_joint_10_position[2] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_10_position.inc"
};
static NJS_MKEY_A minimal_half_fish_walk_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_half_fish_walk_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_half_fish_walk_tracks[11] = {
#include "chao/minimal_models/minimal_half_fish_walk_tracks.inc"
};
static NJS_MOTION minimal_half_fish_walk = {
#include "chao/minimal_models/minimal_half_fish_walk.inc"
};
static NJS_MKEY_F minimal_half_fish_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_half_fish_idle_joint_10_angles[4] = {
#include "chao/minimal_models/minimal_half_fish_idle_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_half_fish_idle_tracks[11] = {
#include "chao/minimal_models/minimal_half_fish_idle_tracks.inc"
};
static NJS_MOTION minimal_half_fish_idle = {
#include "chao/minimal_models/minimal_half_fish_idle.inc"
};
static Sint16 minimal_boar_polygons_8[43] = {
#include "chao/minimal_models/minimal_boar_polygons_8.inc"
};
static Sint32 minimal_boar_vertices_8[27] = {
#include "chao/minimal_models/minimal_boar_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_8 = {
#include "chao/minimal_models/minimal_boar_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_8 = {
#include "chao/minimal_models/minimal_boar_node_8.inc"
};
static Sint16 minimal_boar_polygons_7[51] = {
#include "chao/minimal_models/minimal_boar_polygons_7.inc"
};
static Sint32 minimal_boar_vertices_7[33] = {
#include "chao/minimal_models/minimal_boar_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_7 = {
#include "chao/minimal_models/minimal_boar_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_7 = {
#include "chao/minimal_models/minimal_boar_node_7.inc"
};
static Sint16 minimal_boar_polygons_6[51] = {
#include "chao/minimal_models/minimal_boar_polygons_6.inc"
};
static Sint32 minimal_boar_vertices_6[33] = {
#include "chao/minimal_models/minimal_boar_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_6 = {
#include "chao/minimal_models/minimal_boar_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_6 = {
#include "chao/minimal_models/minimal_boar_node_6.inc"
};
static Sint16 minimal_boar_polygons_5[51] = {
#include "chao/minimal_models/minimal_boar_polygons_5.inc"
};
static Sint32 minimal_boar_vertices_5[33] = {
#include "chao/minimal_models/minimal_boar_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_5 = {
#include "chao/minimal_models/minimal_boar_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_5 = {
#include "chao/minimal_models/minimal_boar_node_5.inc"
};
static Sint16 minimal_boar_polygons_4[51] = {
#include "chao/minimal_models/minimal_boar_polygons_4.inc"
};
static Sint32 minimal_boar_vertices_4[33] = {
#include "chao/minimal_models/minimal_boar_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_4 = {
#include "chao/minimal_models/minimal_boar_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_4 = {
#include "chao/minimal_models/minimal_boar_node_4.inc"
};
static Sint16 minimal_boar_polygons_3[25] = {
#include "chao/minimal_models/minimal_boar_polygons_3.inc"
};
static Sint32 minimal_boar_vertices_3[27] = {
#include "chao/minimal_models/minimal_boar_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_3 = {
#include "chao/minimal_models/minimal_boar_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_3 = {
#include "chao/minimal_models/minimal_boar_node_3.inc"
};
static Sint16 minimal_boar_polygons_2[25] = {
#include "chao/minimal_models/minimal_boar_polygons_2.inc"
};
static Sint32 minimal_boar_vertices_2[27] = {
#include "chao/minimal_models/minimal_boar_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_2 = {
#include "chao/minimal_models/minimal_boar_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_2 = {
#include "chao/minimal_models/minimal_boar_node_2.inc"
};
static Sint16 minimal_boar_polygons_1[373] = {
#include "chao/minimal_models/minimal_boar_polygons_1.inc"
};
static Sint32 minimal_boar_vertices_1[231] = {
#include "chao/minimal_models/minimal_boar_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_boar_mesh_1 = {
#include "chao/minimal_models/minimal_boar_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_boar_node_1 = {
#include "chao/minimal_models/minimal_boar_node_1.inc"
};
static NJS_CNK_OBJECT minimal_boar_model = {
#include "chao/minimal_models/minimal_boar_model.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_boar_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_2_angles[9] = {
#include "chao/minimal_models/minimal_boar_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_3_angles[9] = {
#include "chao/minimal_models/minimal_boar_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_boar_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_boar_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_boar_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_boar_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_boar_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_boar_held_joint_8_angles.inc"
};
static NJS_MDATA2 minimal_boar_held_tracks[9] = {
#include "chao/minimal_models/minimal_boar_held_tracks.inc"
};
static NJS_MOTION minimal_boar_held = {
#include "chao/minimal_models/minimal_boar_held.inc"
};
static NJS_MKEY_F minimal_boar_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_boar_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_boar_walk_joint_8_angles.inc"
};
static NJS_MDATA2 minimal_boar_walk_tracks[9] = {
#include "chao/minimal_models/minimal_boar_walk_tracks.inc"
};
static NJS_MOTION minimal_boar_walk = {
#include "chao/minimal_models/minimal_boar_walk.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_boar_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_boar_idle_joint_8_angles.inc"
};
static NJS_MDATA2 minimal_boar_idle_tracks[9] = {
#include "chao/minimal_models/minimal_boar_idle_tracks.inc"
};
static NJS_MOTION minimal_boar_idle = {
#include "chao/minimal_models/minimal_boar_idle.inc"
};
static Sint16 minimal_condor_polygons_10[45] = {
#include "chao/minimal_models/minimal_condor_polygons_10.inc"
};
static Sint32 minimal_condor_vertices_10[63] = {
#include "chao/minimal_models/minimal_condor_vertices_10.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_10 = {
#include "chao/minimal_models/minimal_condor_mesh_10.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_10 = {
#include "chao/minimal_models/minimal_condor_node_10.inc"
};
static Sint16 minimal_condor_polygons_9[41] = {
#include "chao/minimal_models/minimal_condor_polygons_9.inc"
};
static Sint32 minimal_condor_vertices_9[45] = {
#include "chao/minimal_models/minimal_condor_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_9 = {
#include "chao/minimal_models/minimal_condor_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_9 = {
#include "chao/minimal_models/minimal_condor_node_9.inc"
};
static Sint16 minimal_condor_polygons_8[41] = {
#include "chao/minimal_models/minimal_condor_polygons_8.inc"
};
static Sint32 minimal_condor_vertices_8[45] = {
#include "chao/minimal_models/minimal_condor_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_8 = {
#include "chao/minimal_models/minimal_condor_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_8 = {
#include "chao/minimal_models/minimal_condor_node_8.inc"
};
static Sint16 minimal_condor_polygons_7[25] = {
#include "chao/minimal_models/minimal_condor_polygons_7.inc"
};
static Sint32 minimal_condor_vertices_7[27] = {
#include "chao/minimal_models/minimal_condor_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_7 = {
#include "chao/minimal_models/minimal_condor_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_7 = {
#include "chao/minimal_models/minimal_condor_node_7.inc"
};
static Sint16 minimal_condor_polygons_6[45] = {
#include "chao/minimal_models/minimal_condor_polygons_6.inc"
};
static Sint32 minimal_condor_vertices_6[45] = {
#include "chao/minimal_models/minimal_condor_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_6 = {
#include "chao/minimal_models/minimal_condor_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_6 = {
#include "chao/minimal_models/minimal_condor_node_6.inc"
};
static Sint16 minimal_condor_polygons_5[25] = {
#include "chao/minimal_models/minimal_condor_polygons_5.inc"
};
static Sint32 minimal_condor_vertices_5[27] = {
#include "chao/minimal_models/minimal_condor_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_5 = {
#include "chao/minimal_models/minimal_condor_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_5 = {
#include "chao/minimal_models/minimal_condor_node_5.inc"
};
static Sint16 minimal_condor_polygons_4[45] = {
#include "chao/minimal_models/minimal_condor_polygons_4.inc"
};
static Sint32 minimal_condor_vertices_4[45] = {
#include "chao/minimal_models/minimal_condor_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_4 = {
#include "chao/minimal_models/minimal_condor_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_4 = {
#include "chao/minimal_models/minimal_condor_node_4.inc"
};
static Sint16 minimal_condor_polygons_3[77] = {
#include "chao/minimal_models/minimal_condor_polygons_3.inc"
};
static Sint32 minimal_condor_vertices_3[75] = {
#include "chao/minimal_models/minimal_condor_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_3 = {
#include "chao/minimal_models/minimal_condor_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_3 = {
#include "chao/minimal_models/minimal_condor_node_3.inc"
};
static Sint16 minimal_condor_polygons_2[301] = {
#include "chao/minimal_models/minimal_condor_polygons_2.inc"
};
static Sint32 minimal_condor_vertices_2[177] = {
#include "chao/minimal_models/minimal_condor_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_2 = {
#include "chao/minimal_models/minimal_condor_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_2 = {
#include "chao/minimal_models/minimal_condor_node_2.inc"
};
static Sint16 minimal_condor_polygons_1[137] = {
#include "chao/minimal_models/minimal_condor_polygons_1.inc"
};
static Sint32 minimal_condor_vertices_1[93] = {
#include "chao/minimal_models/minimal_condor_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_condor_mesh_1 = {
#include "chao/minimal_models/minimal_condor_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_condor_node_1 = {
#include "chao/minimal_models/minimal_condor_node_1.inc"
};
static NJS_CNK_OBJECT minimal_condor_model = {
#include "chao/minimal_models/minimal_condor_model.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_condor_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_condor_held_joint_2_position[15] = {
#include "chao/minimal_models/minimal_condor_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_2_angles[15] = {
#include "chao/minimal_models/minimal_condor_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_condor_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_condor_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_condor_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_condor_held_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_condor_held_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_condor_held_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_condor_held_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_condor_held_tracks[11] = {
#include "chao/minimal_models/minimal_condor_held_tracks.inc"
};
static NJS_MOTION minimal_condor_held = {
#include "chao/minimal_models/minimal_condor_held.inc"
};
static NJS_MKEY_F minimal_condor_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_3_angles[15] = {
#include "chao/minimal_models/minimal_condor_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_condor_walk_joint_8_position[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_condor_walk_joint_9_position[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_9_position.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_condor_walk_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_condor_walk_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_condor_walk_tracks[11] = {
#include "chao/minimal_models/minimal_condor_walk_tracks.inc"
};
static NJS_MOTION minimal_condor_walk = {
#include "chao/minimal_models/minimal_condor_walk.inc"
};
static NJS_MKEY_F minimal_condor_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_condor_idle_joint_10_angles[4] = {
#include "chao/minimal_models/minimal_condor_idle_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_condor_idle_tracks[11] = {
#include "chao/minimal_models/minimal_condor_idle_tracks.inc"
};
static NJS_MOTION minimal_condor_idle = {
#include "chao/minimal_models/minimal_condor_idle.inc"
};
static NJS_MKEY_F minimal_condor_fly_joint_1_position[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_1_position.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_condor_fly_joint_4_position[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_4_position.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_condor_fly_joint_5_position[2] = {
#include "chao/minimal_models/minimal_condor_fly_joint_5_position.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_condor_fly_joint_6_position[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_6_position.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_condor_fly_joint_7_position[2] = {
#include "chao/minimal_models/minimal_condor_fly_joint_7_position.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_condor_fly_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_condor_fly_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_condor_fly_tracks[11] = {
#include "chao/minimal_models/minimal_condor_fly_tracks.inc"
};
static NJS_MOTION minimal_condor_fly = {
#include "chao/minimal_models/minimal_condor_fly.inc"
};
static Sint16 minimal_peacock_polygons_9[35] = {
#include "chao/minimal_models/minimal_peacock_polygons_9.inc"
};
static Sint32 minimal_peacock_vertices_9[45] = {
#include "chao/minimal_models/minimal_peacock_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_9 = {
#include "chao/minimal_models/minimal_peacock_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_9 = {
#include "chao/minimal_models/minimal_peacock_node_9.inc"
};
static Sint16 minimal_peacock_polygons_8[49] = {
#include "chao/minimal_models/minimal_peacock_polygons_8.inc"
};
static Sint32 minimal_peacock_vertices_8[45] = {
#include "chao/minimal_models/minimal_peacock_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_8 = {
#include "chao/minimal_models/minimal_peacock_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_8 = {
#include "chao/minimal_models/minimal_peacock_node_8.inc"
};
static Sint16 minimal_peacock_polygons_7[49] = {
#include "chao/minimal_models/minimal_peacock_polygons_7.inc"
};
static Sint32 minimal_peacock_vertices_7[45] = {
#include "chao/minimal_models/minimal_peacock_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_7 = {
#include "chao/minimal_models/minimal_peacock_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_7 = {
#include "chao/minimal_models/minimal_peacock_node_7.inc"
};
static Sint16 minimal_peacock_polygons_6[29] = {
#include "chao/minimal_models/minimal_peacock_polygons_6.inc"
};
static Sint32 minimal_peacock_vertices_6[33] = {
#include "chao/minimal_models/minimal_peacock_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_6 = {
#include "chao/minimal_models/minimal_peacock_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_6 = {
#include "chao/minimal_models/minimal_peacock_node_6.inc"
};
static Sint16 minimal_peacock_polygons_5[61] = {
#include "chao/minimal_models/minimal_peacock_polygons_5.inc"
};
static Sint32 minimal_peacock_vertices_5[57] = {
#include "chao/minimal_models/minimal_peacock_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_5 = {
#include "chao/minimal_models/minimal_peacock_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_5 = {
#include "chao/minimal_models/minimal_peacock_node_5.inc"
};
static Sint16 minimal_peacock_polygons_4[29] = {
#include "chao/minimal_models/minimal_peacock_polygons_4.inc"
};
static Sint32 minimal_peacock_vertices_4[33] = {
#include "chao/minimal_models/minimal_peacock_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_4 = {
#include "chao/minimal_models/minimal_peacock_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_4 = {
#include "chao/minimal_models/minimal_peacock_node_4.inc"
};
static Sint16 minimal_peacock_polygons_3[61] = {
#include "chao/minimal_models/minimal_peacock_polygons_3.inc"
};
static Sint32 minimal_peacock_vertices_3[57] = {
#include "chao/minimal_models/minimal_peacock_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_3 = {
#include "chao/minimal_models/minimal_peacock_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_3 = {
#include "chao/minimal_models/minimal_peacock_node_3.inc"
};
static Sint16 minimal_peacock_polygons_2[195] = {
#include "chao/minimal_models/minimal_peacock_polygons_2.inc"
};
static Sint32 minimal_peacock_vertices_2[123] = {
#include "chao/minimal_models/minimal_peacock_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_2 = {
#include "chao/minimal_models/minimal_peacock_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_2 = {
#include "chao/minimal_models/minimal_peacock_node_2.inc"
};
static Sint16 minimal_peacock_polygons_1[137] = {
#include "chao/minimal_models/minimal_peacock_polygons_1.inc"
};
static Sint32 minimal_peacock_vertices_1[105] = {
#include "chao/minimal_models/minimal_peacock_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_peacock_mesh_1 = {
#include "chao/minimal_models/minimal_peacock_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_peacock_node_1 = {
#include "chao/minimal_models/minimal_peacock_node_1.inc"
};
static NJS_CNK_OBJECT minimal_peacock_model = {
#include "chao/minimal_models/minimal_peacock_model.inc"
};
static NJS_MKEY_A minimal_peacock_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_peacock_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_peacock_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_peacock_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_peacock_held_joint_3_angles[9] = {
#include "chao/minimal_models/minimal_peacock_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_peacock_held_joint_5_angles[11] = {
#include "chao/minimal_models/minimal_peacock_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_peacock_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_peacock_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_peacock_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_peacock_held_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_peacock_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_peacock_held_joint_9_angles.inc"
};
static NJS_MKEY_F minimal_peacock_held_joint_9_scale[16] = {
#include "chao/minimal_models/minimal_peacock_held_joint_9_scale.inc"
};
static NJS_MDATA3 minimal_peacock_held_tracks[10] = {
#include "chao/minimal_models/minimal_peacock_held_tracks.inc"
};
static NJS_MOTION minimal_peacock_held = {
#include "chao/minimal_models/minimal_peacock_held.inc"
};
static NJS_MKEY_F minimal_peacock_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_peacock_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_peacock_walk_joint_2_position[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_peacock_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_peacock_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_peacock_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_peacock_walk_joint_7_position[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_peacock_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_peacock_walk_joint_8_position[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_peacock_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_peacock_walk_joint_9_position[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_9_position.inc"
};
static NJS_MKEY_A minimal_peacock_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_peacock_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_peacock_walk_tracks[10] = {
#include "chao/minimal_models/minimal_peacock_walk_tracks.inc"
};
static NJS_MOTION minimal_peacock_walk = {
#include "chao/minimal_models/minimal_peacock_walk.inc"
};
static NJS_MKEY_F minimal_peacock_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_peacock_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_peacock_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_peacock_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_peacock_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_peacock_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_peacock_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_peacock_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_peacock_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_peacock_idle_tracks[10] = {
#include "chao/minimal_models/minimal_peacock_idle_tracks.inc"
};
static NJS_MOTION minimal_peacock_idle = {
#include "chao/minimal_models/minimal_peacock_idle.inc"
};
static NJS_MKEY_F minimal_peacock_fly_joint_1_position[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_1_position.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_peacock_fly_joint_2_position[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_2_position.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_peacock_fly_joint_3_position[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_3_position.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_peacock_fly_joint_4_position[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_4_position.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_peacock_fly_joint_5_position[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_5_position.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_peacock_fly_joint_6_position[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_6_position.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_peacock_fly_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_peacock_fly_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_peacock_fly_tracks[10] = {
#include "chao/minimal_models/minimal_peacock_fly_tracks.inc"
};
static NJS_MOTION minimal_peacock_fly = {
#include "chao/minimal_models/minimal_peacock_fly.inc"
};
static Sint16 minimal_bear_polygons_9[59] = {
#include "chao/minimal_models/minimal_bear_polygons_9.inc"
};
static Sint32 minimal_bear_vertices_9[33] = {
#include "chao/minimal_models/minimal_bear_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_9 = {
#include "chao/minimal_models/minimal_bear_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_9 = {
#include "chao/minimal_models/minimal_bear_node_9.inc"
};
static Sint16 minimal_bear_polygons_8[77] = {
#include "chao/minimal_models/minimal_bear_polygons_8.inc"
};
static Sint32 minimal_bear_vertices_8[51] = {
#include "chao/minimal_models/minimal_bear_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_8 = {
#include "chao/minimal_models/minimal_bear_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_8 = {
#include "chao/minimal_models/minimal_bear_node_8.inc"
};
static Sint16 minimal_bear_polygons_7[77] = {
#include "chao/minimal_models/minimal_bear_polygons_7.inc"
};
static Sint32 minimal_bear_vertices_7[51] = {
#include "chao/minimal_models/minimal_bear_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_7 = {
#include "chao/minimal_models/minimal_bear_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_7 = {
#include "chao/minimal_models/minimal_bear_node_7.inc"
};
static Sint16 minimal_bear_polygons_6[25] = {
#include "chao/minimal_models/minimal_bear_polygons_6.inc"
};
static Sint32 minimal_bear_vertices_6[27] = {
#include "chao/minimal_models/minimal_bear_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_6 = {
#include "chao/minimal_models/minimal_bear_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_6 = {
#include "chao/minimal_models/minimal_bear_node_6.inc"
};
static Sint16 minimal_bear_polygons_5[25] = {
#include "chao/minimal_models/minimal_bear_polygons_5.inc"
};
static Sint32 minimal_bear_vertices_5[27] = {
#include "chao/minimal_models/minimal_bear_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_5 = {
#include "chao/minimal_models/minimal_bear_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_5 = {
#include "chao/minimal_models/minimal_bear_node_5.inc"
};
static Sint16 minimal_bear_polygons_4[179] = {
#include "chao/minimal_models/minimal_bear_polygons_4.inc"
};
static Sint32 minimal_bear_vertices_4[105] = {
#include "chao/minimal_models/minimal_bear_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_4 = {
#include "chao/minimal_models/minimal_bear_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_4 = {
#include "chao/minimal_models/minimal_bear_node_4.inc"
};
static Sint16 minimal_bear_polygons_3[103] = {
#include "chao/minimal_models/minimal_bear_polygons_3.inc"
};
static Sint32 minimal_bear_vertices_3[63] = {
#include "chao/minimal_models/minimal_bear_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_3 = {
#include "chao/minimal_models/minimal_bear_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_3 = {
#include "chao/minimal_models/minimal_bear_node_3.inc"
};
static Sint16 minimal_bear_polygons_2[103] = {
#include "chao/minimal_models/minimal_bear_polygons_2.inc"
};
static Sint32 minimal_bear_vertices_2[63] = {
#include "chao/minimal_models/minimal_bear_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_2 = {
#include "chao/minimal_models/minimal_bear_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_2 = {
#include "chao/minimal_models/minimal_bear_node_2.inc"
};
static Sint16 minimal_bear_polygons_1[141] = {
#include "chao/minimal_models/minimal_bear_polygons_1.inc"
};
static Sint32 minimal_bear_vertices_1[111] = {
#include "chao/minimal_models/minimal_bear_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_bear_mesh_1 = {
#include "chao/minimal_models/minimal_bear_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_bear_node_1 = {
#include "chao/minimal_models/minimal_bear_node_1.inc"
};
static NJS_CNK_OBJECT minimal_bear_model = {
#include "chao/minimal_models/minimal_bear_model.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_bear_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_bear_held_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_bear_held_tracks[10] = {
#include "chao/minimal_models/minimal_bear_held_tracks.inc"
};
static NJS_MOTION minimal_bear_held = {
#include "chao/minimal_models/minimal_bear_held.inc"
};
static NJS_MKEY_F minimal_bear_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_bear_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_bear_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_bear_walk_tracks[10] = {
#include "chao/minimal_models/minimal_bear_walk_tracks.inc"
};
static NJS_MOTION minimal_bear_walk = {
#include "chao/minimal_models/minimal_bear_walk.inc"
};
static NJS_MKEY_F minimal_bear_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_bear_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_bear_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_bear_idle_tracks[10] = {
#include "chao/minimal_models/minimal_bear_idle_tracks.inc"
};
static NJS_MOTION minimal_bear_idle = {
#include "chao/minimal_models/minimal_bear_idle.inc"
};
static Sint16 minimal_parrot_polygons_9[29] = {
#include "chao/minimal_models/minimal_parrot_polygons_9.inc"
};
static Sint32 minimal_parrot_vertices_9[33] = {
#include "chao/minimal_models/minimal_parrot_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_9 = {
#include "chao/minimal_models/minimal_parrot_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_9 = {
#include "chao/minimal_models/minimal_parrot_node_9.inc"
};
static Sint16 minimal_parrot_polygons_8[55] = {
#include "chao/minimal_models/minimal_parrot_polygons_8.inc"
};
static Sint32 minimal_parrot_vertices_8[45] = {
#include "chao/minimal_models/minimal_parrot_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_8 = {
#include "chao/minimal_models/minimal_parrot_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_8 = {
#include "chao/minimal_models/minimal_parrot_node_8.inc"
};
static Sint16 minimal_parrot_polygons_7[55] = {
#include "chao/minimal_models/minimal_parrot_polygons_7.inc"
};
static Sint32 minimal_parrot_vertices_7[45] = {
#include "chao/minimal_models/minimal_parrot_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_7 = {
#include "chao/minimal_models/minimal_parrot_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_7 = {
#include "chao/minimal_models/minimal_parrot_node_7.inc"
};
static Sint16 minimal_parrot_polygons_6[25] = {
#include "chao/minimal_models/minimal_parrot_polygons_6.inc"
};
static Sint32 minimal_parrot_vertices_6[27] = {
#include "chao/minimal_models/minimal_parrot_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_6 = {
#include "chao/minimal_models/minimal_parrot_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_6 = {
#include "chao/minimal_models/minimal_parrot_node_6.inc"
};
static Sint16 minimal_parrot_polygons_5[45] = {
#include "chao/minimal_models/minimal_parrot_polygons_5.inc"
};
static Sint32 minimal_parrot_vertices_5[45] = {
#include "chao/minimal_models/minimal_parrot_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_5 = {
#include "chao/minimal_models/minimal_parrot_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_5 = {
#include "chao/minimal_models/minimal_parrot_node_5.inc"
};
static Sint16 minimal_parrot_polygons_4[25] = {
#include "chao/minimal_models/minimal_parrot_polygons_4.inc"
};
static Sint32 minimal_parrot_vertices_4[27] = {
#include "chao/minimal_models/minimal_parrot_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_4 = {
#include "chao/minimal_models/minimal_parrot_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_4 = {
#include "chao/minimal_models/minimal_parrot_node_4.inc"
};
static Sint16 minimal_parrot_polygons_3[45] = {
#include "chao/minimal_models/minimal_parrot_polygons_3.inc"
};
static Sint32 minimal_parrot_vertices_3[45] = {
#include "chao/minimal_models/minimal_parrot_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_3 = {
#include "chao/minimal_models/minimal_parrot_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_3 = {
#include "chao/minimal_models/minimal_parrot_node_3.inc"
};
static Sint16 minimal_parrot_polygons_2[219] = {
#include "chao/minimal_models/minimal_parrot_polygons_2.inc"
};
static Sint32 minimal_parrot_vertices_2[195] = {
#include "chao/minimal_models/minimal_parrot_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_2 = {
#include "chao/minimal_models/minimal_parrot_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_2 = {
#include "chao/minimal_models/minimal_parrot_node_2.inc"
};
static Sint16 minimal_parrot_polygons_1[109] = {
#include "chao/minimal_models/minimal_parrot_polygons_1.inc"
};
static Sint32 minimal_parrot_vertices_1[93] = {
#include "chao/minimal_models/minimal_parrot_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_parrot_mesh_1 = {
#include "chao/minimal_models/minimal_parrot_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_parrot_node_1 = {
#include "chao/minimal_models/minimal_parrot_node_1.inc"
};
static NJS_CNK_OBJECT minimal_parrot_model = {
#include "chao/minimal_models/minimal_parrot_model.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_1_angles[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_8_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_8_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_parrot_held_joint_9_position[2] = {
#include "chao/minimal_models/minimal_parrot_held_joint_9_position.inc"
};
static NJS_MKEY_A minimal_parrot_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_parrot_held_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_parrot_held_tracks[10] = {
#include "chao/minimal_models/minimal_parrot_held_tracks.inc"
};
static NJS_MOTION minimal_parrot_held = {
#include "chao/minimal_models/minimal_parrot_held.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_3_position[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_4_angles[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_5_position[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_6_position[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_6_angles[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_7_position[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_8_position[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_parrot_walk_joint_9_position[2] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_9_position.inc"
};
static NJS_MKEY_A minimal_parrot_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_parrot_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_parrot_walk_tracks[10] = {
#include "chao/minimal_models/minimal_parrot_walk_tracks.inc"
};
static NJS_MOTION minimal_parrot_walk = {
#include "chao/minimal_models/minimal_parrot_walk.inc"
};
static NJS_MKEY_F minimal_parrot_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_parrot_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_parrot_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_parrot_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_parrot_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_parrot_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_parrot_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_parrot_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_parrot_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_parrot_idle_tracks[10] = {
#include "chao/minimal_models/minimal_parrot_idle_tracks.inc"
};
static NJS_MOTION minimal_parrot_idle = {
#include "chao/minimal_models/minimal_parrot_idle.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_0_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_0_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_1_position[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_1_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_1_angles[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_2_position[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_2_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_3_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_3_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_4_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_4_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_4_angles[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_5_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_5_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_6_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_6_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_6_angles[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_7_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_7_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_8_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_8_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_parrot_fly_joint_9_position[2] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_9_position.inc"
};
static NJS_MKEY_A minimal_parrot_fly_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_parrot_fly_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_parrot_fly_tracks[10] = {
#include "chao/minimal_models/minimal_parrot_fly_tracks.inc"
};
static NJS_MOTION minimal_parrot_fly = {
#include "chao/minimal_models/minimal_parrot_fly.inc"
};
static Sint16 minimal_penguin_polygons_7[59] = {
#include "chao/minimal_models/minimal_penguin_polygons_7.inc"
};
static Sint32 minimal_penguin_vertices_7[51] = {
#include "chao/minimal_models/minimal_penguin_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_penguin_mesh_7 = {
#include "chao/minimal_models/minimal_penguin_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_penguin_node_7 = {
#include "chao/minimal_models/minimal_penguin_node_7.inc"
};
static Sint16 minimal_penguin_polygons_6[59] = {
#include "chao/minimal_models/minimal_penguin_polygons_6.inc"
};
static Sint32 minimal_penguin_vertices_6[51] = {
#include "chao/minimal_models/minimal_penguin_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_penguin_mesh_6 = {
#include "chao/minimal_models/minimal_penguin_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_penguin_node_6 = {
#include "chao/minimal_models/minimal_penguin_node_6.inc"
};
static Sint16 minimal_penguin_polygons_5[77] = {
#include "chao/minimal_models/minimal_penguin_polygons_5.inc"
};
static Sint32 minimal_penguin_vertices_5[51] = {
#include "chao/minimal_models/minimal_penguin_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_penguin_mesh_5 = {
#include "chao/minimal_models/minimal_penguin_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_penguin_node_5 = {
#include "chao/minimal_models/minimal_penguin_node_5.inc"
};
static Sint16 minimal_penguin_polygons_4[77] = {
#include "chao/minimal_models/minimal_penguin_polygons_4.inc"
};
static Sint32 minimal_penguin_vertices_4[51] = {
#include "chao/minimal_models/minimal_penguin_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_penguin_mesh_4 = {
#include "chao/minimal_models/minimal_penguin_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_penguin_node_4 = {
#include "chao/minimal_models/minimal_penguin_node_4.inc"
};
static Sint16 minimal_penguin_polygons_3[23] = {
#include "chao/minimal_models/minimal_penguin_polygons_3.inc"
};
static Sint32 minimal_penguin_vertices_3[21] = {
#include "chao/minimal_models/minimal_penguin_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_penguin_mesh_3 = {
#include "chao/minimal_models/minimal_penguin_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_penguin_node_3 = {
#include "chao/minimal_models/minimal_penguin_node_3.inc"
};
static Sint16 minimal_penguin_polygons_2[23] = {
#include "chao/minimal_models/minimal_penguin_polygons_2.inc"
};
static Sint32 minimal_penguin_vertices_2[21] = {
#include "chao/minimal_models/minimal_penguin_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_penguin_mesh_2 = {
#include "chao/minimal_models/minimal_penguin_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_penguin_node_2 = {
#include "chao/minimal_models/minimal_penguin_node_2.inc"
};
static Sint16 minimal_penguin_polygons_1[439] = {
#include "chao/minimal_models/minimal_penguin_polygons_1.inc"
};
static Sint32 minimal_penguin_vertices_1[213] = {
#include "chao/minimal_models/minimal_penguin_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_penguin_mesh_1 = {
#include "chao/minimal_models/minimal_penguin_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_penguin_node_1 = {
#include "chao/minimal_models/minimal_penguin_node_1.inc"
};
static NJS_CNK_OBJECT minimal_penguin_model = {
#include "chao/minimal_models/minimal_penguin_model.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_penguin_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_penguin_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_penguin_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_penguin_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_penguin_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_penguin_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_penguin_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_penguin_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_penguin_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_penguin_held_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_penguin_held_tracks[8] = {
#include "chao/minimal_models/minimal_penguin_held_tracks.inc"
};
static NJS_MOTION minimal_penguin_held = {
#include "chao/minimal_models/minimal_penguin_held.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_3_position[2] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_5_position[2] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_6_position[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_penguin_walk_joint_7_position[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_penguin_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_penguin_walk_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_penguin_walk_tracks[8] = {
#include "chao/minimal_models/minimal_penguin_walk_tracks.inc"
};
static NJS_MOTION minimal_penguin_walk = {
#include "chao/minimal_models/minimal_penguin_walk.inc"
};
static NJS_MKEY_F minimal_penguin_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_penguin_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_penguin_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_penguin_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_penguin_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_penguin_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_penguin_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_penguin_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_penguin_idle_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_penguin_idle_tracks[8] = {
#include "chao/minimal_models/minimal_penguin_idle_tracks.inc"
};
static NJS_MOTION minimal_penguin_idle = {
#include "chao/minimal_models/minimal_penguin_idle.inc"
};
static Sint16 minimal_phoenix_polygons_10[85] = {
#include "chao/minimal_models/minimal_phoenix_polygons_10.inc"
};
static Sint32 minimal_phoenix_vertices_10[87] = {
#include "chao/minimal_models/minimal_phoenix_vertices_10.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_10 = {
#include "chao/minimal_models/minimal_phoenix_mesh_10.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_10 = {
#include "chao/minimal_models/minimal_phoenix_node_10.inc"
};
static Sint16 minimal_phoenix_polygons_9[29] = {
#include "chao/minimal_models/minimal_phoenix_polygons_9.inc"
};
static Sint32 minimal_phoenix_vertices_9[33] = {
#include "chao/minimal_models/minimal_phoenix_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_9 = {
#include "chao/minimal_models/minimal_phoenix_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_9 = {
#include "chao/minimal_models/minimal_phoenix_node_9.inc"
};
static Sint16 minimal_phoenix_polygons_8[75] = {
#include "chao/minimal_models/minimal_phoenix_polygons_8.inc"
};
static Sint32 minimal_phoenix_vertices_8[57] = {
#include "chao/minimal_models/minimal_phoenix_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_8 = {
#include "chao/minimal_models/minimal_phoenix_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_8 = {
#include "chao/minimal_models/minimal_phoenix_node_8.inc"
};
static Sint16 minimal_phoenix_polygons_7[75] = {
#include "chao/minimal_models/minimal_phoenix_polygons_7.inc"
};
static Sint32 minimal_phoenix_vertices_7[57] = {
#include "chao/minimal_models/minimal_phoenix_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_7 = {
#include "chao/minimal_models/minimal_phoenix_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_7 = {
#include "chao/minimal_models/minimal_phoenix_node_7.inc"
};
static Sint16 minimal_phoenix_polygons_6[29] = {
#include "chao/minimal_models/minimal_phoenix_polygons_6.inc"
};
static Sint32 minimal_phoenix_vertices_6[33] = {
#include "chao/minimal_models/minimal_phoenix_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_6 = {
#include "chao/minimal_models/minimal_phoenix_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_6 = {
#include "chao/minimal_models/minimal_phoenix_node_6.inc"
};
static Sint16 minimal_phoenix_polygons_5[51] = {
#include "chao/minimal_models/minimal_phoenix_polygons_5.inc"
};
static Sint32 minimal_phoenix_vertices_5[51] = {
#include "chao/minimal_models/minimal_phoenix_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_5 = {
#include "chao/minimal_models/minimal_phoenix_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_5 = {
#include "chao/minimal_models/minimal_phoenix_node_5.inc"
};
static Sint16 minimal_phoenix_polygons_4[29] = {
#include "chao/minimal_models/minimal_phoenix_polygons_4.inc"
};
static Sint32 minimal_phoenix_vertices_4[33] = {
#include "chao/minimal_models/minimal_phoenix_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_4 = {
#include "chao/minimal_models/minimal_phoenix_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_4 = {
#include "chao/minimal_models/minimal_phoenix_node_4.inc"
};
static Sint16 minimal_phoenix_polygons_3[51] = {
#include "chao/minimal_models/minimal_phoenix_polygons_3.inc"
};
static Sint32 minimal_phoenix_vertices_3[51] = {
#include "chao/minimal_models/minimal_phoenix_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_3 = {
#include "chao/minimal_models/minimal_phoenix_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_3 = {
#include "chao/minimal_models/minimal_phoenix_node_3.inc"
};
static Sint16 minimal_phoenix_polygons_2[281] = {
#include "chao/minimal_models/minimal_phoenix_polygons_2.inc"
};
static Sint32 minimal_phoenix_vertices_2[189] = {
#include "chao/minimal_models/minimal_phoenix_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_2 = {
#include "chao/minimal_models/minimal_phoenix_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_2 = {
#include "chao/minimal_models/minimal_phoenix_node_2.inc"
};
static Sint16 minimal_phoenix_polygons_1[139] = {
#include "chao/minimal_models/minimal_phoenix_polygons_1.inc"
};
static Sint32 minimal_phoenix_vertices_1[93] = {
#include "chao/minimal_models/minimal_phoenix_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_phoenix_mesh_1 = {
#include "chao/minimal_models/minimal_phoenix_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_node_1 = {
#include "chao/minimal_models/minimal_phoenix_node_1.inc"
};
static NJS_CNK_OBJECT minimal_phoenix_model = {
#include "chao/minimal_models/minimal_phoenix_model.inc"
};
static NJS_MKEY_A minimal_phoenix_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_held_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_held_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_phoenix_held_tracks[11] = {
#include "chao/minimal_models/minimal_phoenix_held_tracks.inc"
};
static NJS_MOTION minimal_phoenix_held = {
#include "chao/minimal_models/minimal_phoenix_held.inc"
};
static NJS_MKEY_F minimal_phoenix_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_phoenix_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_phoenix_walk_joint_7_position[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_phoenix_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_phoenix_walk_joint_8_position[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_phoenix_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_phoenix_walk_tracks[11] = {
#include "chao/minimal_models/minimal_phoenix_walk_tracks.inc"
};
static NJS_MOTION minimal_phoenix_walk = {
#include "chao/minimal_models/minimal_phoenix_walk.inc"
};
static NJS_MKEY_F minimal_phoenix_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_phoenix_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_phoenix_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_phoenix_idle_tracks[11] = {
#include "chao/minimal_models/minimal_phoenix_idle_tracks.inc"
};
static NJS_MOTION minimal_phoenix_idle = {
#include "chao/minimal_models/minimal_phoenix_idle.inc"
};
static NJS_MKEY_F minimal_phoenix_fly_joint_1_position[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_1_position.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_1_angles[15] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_phoenix_fly_joint_3_position[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_3_position.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_phoenix_fly_joint_4_position[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_4_position.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_phoenix_fly_joint_5_position[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_5_position.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_phoenix_fly_joint_6_position[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_6_position.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_phoenix_fly_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_phoenix_fly_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_phoenix_fly_tracks[11] = {
#include "chao/minimal_models/minimal_phoenix_fly_tracks.inc"
};
static NJS_MOTION minimal_phoenix_fly = {
#include "chao/minimal_models/minimal_phoenix_fly.inc"
};
static Sint16 minimal_otter_polygons_9[59] = {
#include "chao/minimal_models/minimal_otter_polygons_9.inc"
};
static Sint32 minimal_otter_vertices_9[33] = {
#include "chao/minimal_models/minimal_otter_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_9 = {
#include "chao/minimal_models/minimal_otter_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_9 = {
#include "chao/minimal_models/minimal_otter_node_9.inc"
};
static Sint16 minimal_otter_polygons_8[65] = {
#include "chao/minimal_models/minimal_otter_polygons_8.inc"
};
static Sint32 minimal_otter_vertices_8[39] = {
#include "chao/minimal_models/minimal_otter_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_8 = {
#include "chao/minimal_models/minimal_otter_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_8 = {
#include "chao/minimal_models/minimal_otter_node_8.inc"
};
static Sint16 minimal_otter_polygons_7[65] = {
#include "chao/minimal_models/minimal_otter_polygons_7.inc"
};
static Sint32 minimal_otter_vertices_7[39] = {
#include "chao/minimal_models/minimal_otter_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_7 = {
#include "chao/minimal_models/minimal_otter_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_7 = {
#include "chao/minimal_models/minimal_otter_node_7.inc"
};
static Sint16 minimal_otter_polygons_6[23] = {
#include "chao/minimal_models/minimal_otter_polygons_6.inc"
};
static Sint32 minimal_otter_vertices_6[21] = {
#include "chao/minimal_models/minimal_otter_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_6 = {
#include "chao/minimal_models/minimal_otter_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_6 = {
#include "chao/minimal_models/minimal_otter_node_6.inc"
};
static Sint16 minimal_otter_polygons_5[23] = {
#include "chao/minimal_models/minimal_otter_polygons_5.inc"
};
static Sint32 minimal_otter_vertices_5[21] = {
#include "chao/minimal_models/minimal_otter_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_5 = {
#include "chao/minimal_models/minimal_otter_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_5 = {
#include "chao/minimal_models/minimal_otter_node_5.inc"
};
static Sint16 minimal_otter_polygons_4[205] = {
#include "chao/minimal_models/minimal_otter_polygons_4.inc"
};
static Sint32 minimal_otter_vertices_4[153] = {
#include "chao/minimal_models/minimal_otter_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_4 = {
#include "chao/minimal_models/minimal_otter_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_4 = {
#include "chao/minimal_models/minimal_otter_node_4.inc"
};
static Sint16 minimal_otter_polygons_3[91] = {
#include "chao/minimal_models/minimal_otter_polygons_3.inc"
};
static Sint32 minimal_otter_vertices_3[45] = {
#include "chao/minimal_models/minimal_otter_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_3 = {
#include "chao/minimal_models/minimal_otter_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_3 = {
#include "chao/minimal_models/minimal_otter_node_3.inc"
};
static Sint16 minimal_otter_polygons_2[109] = {
#include "chao/minimal_models/minimal_otter_polygons_2.inc"
};
static Sint32 minimal_otter_vertices_2[69] = {
#include "chao/minimal_models/minimal_otter_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_2 = {
#include "chao/minimal_models/minimal_otter_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_2 = {
#include "chao/minimal_models/minimal_otter_node_2.inc"
};
static Sint16 minimal_otter_polygons_1[147] = {
#include "chao/minimal_models/minimal_otter_polygons_1.inc"
};
static Sint32 minimal_otter_vertices_1[87] = {
#include "chao/minimal_models/minimal_otter_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_otter_mesh_1 = {
#include "chao/minimal_models/minimal_otter_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_otter_node_1 = {
#include "chao/minimal_models/minimal_otter_node_1.inc"
};
static NJS_CNK_OBJECT minimal_otter_model = {
#include "chao/minimal_models/minimal_otter_model.inc"
};
static NJS_MKEY_A minimal_otter_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_otter_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_otter_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_otter_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_otter_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_otter_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_otter_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_otter_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_otter_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_otter_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_otter_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_otter_held_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_otter_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_otter_held_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_otter_held_tracks[10] = {
#include "chao/minimal_models/minimal_otter_held_tracks.inc"
};
static NJS_MOTION minimal_otter_held = {
#include "chao/minimal_models/minimal_otter_held.inc"
};
static NJS_MKEY_F minimal_otter_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_otter_walk_joint_7_position[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_otter_walk_joint_8_position[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_otter_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_otter_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_otter_walk_tracks[10] = {
#include "chao/minimal_models/minimal_otter_walk_tracks.inc"
};
static NJS_MOTION minimal_otter_walk = {
#include "chao/minimal_models/minimal_otter_walk.inc"
};
static NJS_MKEY_F minimal_otter_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_otter_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_otter_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_otter_idle_tracks[10] = {
#include "chao/minimal_models/minimal_otter_idle_tracks.inc"
};
static NJS_MOTION minimal_otter_idle = {
#include "chao/minimal_models/minimal_otter_idle.inc"
};
static Sint16 minimal_sheep_polygons_9[29] = {
#include "chao/minimal_models/minimal_sheep_polygons_9.inc"
};
static Sint32 minimal_sheep_vertices_9[27] = {
#include "chao/minimal_models/minimal_sheep_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_9 = {
#include "chao/minimal_models/minimal_sheep_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_9 = {
#include "chao/minimal_models/minimal_sheep_node_9.inc"
};
static Sint16 minimal_sheep_polygons_8[51] = {
#include "chao/minimal_models/minimal_sheep_polygons_8.inc"
};
static Sint32 minimal_sheep_vertices_8[33] = {
#include "chao/minimal_models/minimal_sheep_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_8 = {
#include "chao/minimal_models/minimal_sheep_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_8 = {
#include "chao/minimal_models/minimal_sheep_node_8.inc"
};
static Sint16 minimal_sheep_polygons_7[51] = {
#include "chao/minimal_models/minimal_sheep_polygons_7.inc"
};
static Sint32 minimal_sheep_vertices_7[33] = {
#include "chao/minimal_models/minimal_sheep_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_7 = {
#include "chao/minimal_models/minimal_sheep_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_7 = {
#include "chao/minimal_models/minimal_sheep_node_7.inc"
};
static Sint16 minimal_sheep_polygons_6[25] = {
#include "chao/minimal_models/minimal_sheep_polygons_6.inc"
};
static Sint32 minimal_sheep_vertices_6[27] = {
#include "chao/minimal_models/minimal_sheep_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_6 = {
#include "chao/minimal_models/minimal_sheep_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_6 = {
#include "chao/minimal_models/minimal_sheep_node_6.inc"
};
static Sint16 minimal_sheep_polygons_5[25] = {
#include "chao/minimal_models/minimal_sheep_polygons_5.inc"
};
static Sint32 minimal_sheep_vertices_5[27] = {
#include "chao/minimal_models/minimal_sheep_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_5 = {
#include "chao/minimal_models/minimal_sheep_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_5 = {
#include "chao/minimal_models/minimal_sheep_node_5.inc"
};
static Sint16 minimal_sheep_polygons_4[407] = {
#include "chao/minimal_models/minimal_sheep_polygons_4.inc"
};
static Sint32 minimal_sheep_vertices_4[231] = {
#include "chao/minimal_models/minimal_sheep_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_4 = {
#include "chao/minimal_models/minimal_sheep_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_4 = {
#include "chao/minimal_models/minimal_sheep_node_4.inc"
};
static Sint16 minimal_sheep_polygons_3[51] = {
#include "chao/minimal_models/minimal_sheep_polygons_3.inc"
};
static Sint32 minimal_sheep_vertices_3[33] = {
#include "chao/minimal_models/minimal_sheep_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_3 = {
#include "chao/minimal_models/minimal_sheep_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_3 = {
#include "chao/minimal_models/minimal_sheep_node_3.inc"
};
static Sint16 minimal_sheep_polygons_2[51] = {
#include "chao/minimal_models/minimal_sheep_polygons_2.inc"
};
static Sint32 minimal_sheep_vertices_2[33] = {
#include "chao/minimal_models/minimal_sheep_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_2 = {
#include "chao/minimal_models/minimal_sheep_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_2 = {
#include "chao/minimal_models/minimal_sheep_node_2.inc"
};
static Sint16 minimal_sheep_polygons_1[135] = {
#include "chao/minimal_models/minimal_sheep_polygons_1.inc"
};
static Sint32 minimal_sheep_vertices_1[93] = {
#include "chao/minimal_models/minimal_sheep_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_sheep_mesh_1 = {
#include "chao/minimal_models/minimal_sheep_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_sheep_node_1 = {
#include "chao/minimal_models/minimal_sheep_node_1.inc"
};
static NJS_CNK_OBJECT minimal_sheep_model = {
#include "chao/minimal_models/minimal_sheep_model.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_sheep_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_sheep_held_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_sheep_held_tracks[10] = {
#include "chao/minimal_models/minimal_sheep_held_tracks.inc"
};
static NJS_MOTION minimal_sheep_held = {
#include "chao/minimal_models/minimal_sheep_held.inc"
};
static NJS_MKEY_F minimal_sheep_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_sheep_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_sheep_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_sheep_walk_tracks[10] = {
#include "chao/minimal_models/minimal_sheep_walk_tracks.inc"
};
static NJS_MOTION minimal_sheep_walk = {
#include "chao/minimal_models/minimal_sheep_walk.inc"
};
static NJS_MKEY_F minimal_sheep_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_sheep_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_sheep_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_sheep_idle_tracks[10] = {
#include "chao/minimal_models/minimal_sheep_idle_tracks.inc"
};
static NJS_MOTION minimal_sheep_idle = {
#include "chao/minimal_models/minimal_sheep_idle.inc"
};
static Sint16 minimal_skunk_polygons_7[107] = {
#include "chao/minimal_models/minimal_skunk_polygons_7.inc"
};
static Sint32 minimal_skunk_vertices_7[81] = {
#include "chao/minimal_models/minimal_skunk_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_skunk_mesh_7 = {
#include "chao/minimal_models/minimal_skunk_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_skunk_node_7 = {
#include "chao/minimal_models/minimal_skunk_node_7.inc"
};
static Sint16 minimal_skunk_polygons_6[51] = {
#include "chao/minimal_models/minimal_skunk_polygons_6.inc"
};
static Sint32 minimal_skunk_vertices_6[33] = {
#include "chao/minimal_models/minimal_skunk_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_skunk_mesh_6 = {
#include "chao/minimal_models/minimal_skunk_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_skunk_node_6 = {
#include "chao/minimal_models/minimal_skunk_node_6.inc"
};
static Sint16 minimal_skunk_polygons_5[51] = {
#include "chao/minimal_models/minimal_skunk_polygons_5.inc"
};
static Sint32 minimal_skunk_vertices_5[33] = {
#include "chao/minimal_models/minimal_skunk_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_skunk_mesh_5 = {
#include "chao/minimal_models/minimal_skunk_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_skunk_node_5 = {
#include "chao/minimal_models/minimal_skunk_node_5.inc"
};
static Sint16 minimal_skunk_polygons_4[51] = {
#include "chao/minimal_models/minimal_skunk_polygons_4.inc"
};
static Sint32 minimal_skunk_vertices_4[33] = {
#include "chao/minimal_models/minimal_skunk_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_skunk_mesh_4 = {
#include "chao/minimal_models/minimal_skunk_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_skunk_node_4 = {
#include "chao/minimal_models/minimal_skunk_node_4.inc"
};
static Sint16 minimal_skunk_polygons_3[51] = {
#include "chao/minimal_models/minimal_skunk_polygons_3.inc"
};
static Sint32 minimal_skunk_vertices_3[33] = {
#include "chao/minimal_models/minimal_skunk_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_skunk_mesh_3 = {
#include "chao/minimal_models/minimal_skunk_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_skunk_node_3 = {
#include "chao/minimal_models/minimal_skunk_node_3.inc"
};
static Sint16 minimal_skunk_polygons_2[217] = {
#include "chao/minimal_models/minimal_skunk_polygons_2.inc"
};
static Sint32 minimal_skunk_vertices_2[159] = {
#include "chao/minimal_models/minimal_skunk_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_skunk_mesh_2 = {
#include "chao/minimal_models/minimal_skunk_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_skunk_node_2 = {
#include "chao/minimal_models/minimal_skunk_node_2.inc"
};
static Sint16 minimal_skunk_polygons_1[133] = {
#include "chao/minimal_models/minimal_skunk_polygons_1.inc"
};
static Sint32 minimal_skunk_vertices_1[93] = {
#include "chao/minimal_models/minimal_skunk_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_skunk_mesh_1 = {
#include "chao/minimal_models/minimal_skunk_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_skunk_node_1 = {
#include "chao/minimal_models/minimal_skunk_node_1.inc"
};
static NJS_CNK_OBJECT minimal_skunk_model = {
#include "chao/minimal_models/minimal_skunk_model.inc"
};
static NJS_MKEY_A minimal_skunk_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_skunk_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_skunk_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_skunk_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_skunk_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_skunk_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_skunk_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_skunk_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_skunk_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_skunk_held_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_skunk_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_skunk_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_skunk_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_skunk_held_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_skunk_held_tracks[8] = {
#include "chao/minimal_models/minimal_skunk_held_tracks.inc"
};
static NJS_MOTION minimal_skunk_held = {
#include "chao/minimal_models/minimal_skunk_held.inc"
};
static NJS_MKEY_F minimal_skunk_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_skunk_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_skunk_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_skunk_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_skunk_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_skunk_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_skunk_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_skunk_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_skunk_walk_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_skunk_walk_tracks[8] = {
#include "chao/minimal_models/minimal_skunk_walk_tracks.inc"
};
static NJS_MOTION minimal_skunk_walk = {
#include "chao/minimal_models/minimal_skunk_walk.inc"
};
static NJS_MKEY_F minimal_skunk_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_skunk_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_skunk_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_skunk_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_skunk_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_skunk_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_skunk_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_skunk_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_skunk_idle_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_skunk_idle_tracks[8] = {
#include "chao/minimal_models/minimal_skunk_idle_tracks.inc"
};
static NJS_MOTION minimal_skunk_idle = {
#include "chao/minimal_models/minimal_skunk_idle.inc"
};
static Sint16 minimal_tiger_polygons_7[61] = {
#include "chao/minimal_models/minimal_tiger_polygons_7.inc"
};
static Sint32 minimal_tiger_vertices_7[45] = {
#include "chao/minimal_models/minimal_tiger_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_tiger_mesh_7 = {
#include "chao/minimal_models/minimal_tiger_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_tiger_node_7 = {
#include "chao/minimal_models/minimal_tiger_node_7.inc"
};
static Sint16 minimal_tiger_polygons_6[65] = {
#include "chao/minimal_models/minimal_tiger_polygons_6.inc"
};
static Sint32 minimal_tiger_vertices_6[45] = {
#include "chao/minimal_models/minimal_tiger_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_tiger_mesh_6 = {
#include "chao/minimal_models/minimal_tiger_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_tiger_node_6 = {
#include "chao/minimal_models/minimal_tiger_node_6.inc"
};
static Sint16 minimal_tiger_polygons_5[65] = {
#include "chao/minimal_models/minimal_tiger_polygons_5.inc"
};
static Sint32 minimal_tiger_vertices_5[45] = {
#include "chao/minimal_models/minimal_tiger_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_tiger_mesh_5 = {
#include "chao/minimal_models/minimal_tiger_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_tiger_node_5 = {
#include "chao/minimal_models/minimal_tiger_node_5.inc"
};
static Sint16 minimal_tiger_polygons_4[57] = {
#include "chao/minimal_models/minimal_tiger_polygons_4.inc"
};
static Sint32 minimal_tiger_vertices_4[45] = {
#include "chao/minimal_models/minimal_tiger_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_tiger_mesh_4 = {
#include "chao/minimal_models/minimal_tiger_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_tiger_node_4 = {
#include "chao/minimal_models/minimal_tiger_node_4.inc"
};
static Sint16 minimal_tiger_polygons_3[57] = {
#include "chao/minimal_models/minimal_tiger_polygons_3.inc"
};
static Sint32 minimal_tiger_vertices_3[45] = {
#include "chao/minimal_models/minimal_tiger_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_tiger_mesh_3 = {
#include "chao/minimal_models/minimal_tiger_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_tiger_node_3 = {
#include "chao/minimal_models/minimal_tiger_node_3.inc"
};
static Sint16 minimal_tiger_polygons_2[219] = {
#include "chao/minimal_models/minimal_tiger_polygons_2.inc"
};
static Sint32 minimal_tiger_vertices_2[147] = {
#include "chao/minimal_models/minimal_tiger_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_tiger_mesh_2 = {
#include "chao/minimal_models/minimal_tiger_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_tiger_node_2 = {
#include "chao/minimal_models/minimal_tiger_node_2.inc"
};
static Sint16 minimal_tiger_polygons_1[157] = {
#include "chao/minimal_models/minimal_tiger_polygons_1.inc"
};
static Sint32 minimal_tiger_vertices_1[93] = {
#include "chao/minimal_models/minimal_tiger_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_tiger_mesh_1 = {
#include "chao/minimal_models/minimal_tiger_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_tiger_node_1 = {
#include "chao/minimal_models/minimal_tiger_node_1.inc"
};
static NJS_CNK_OBJECT minimal_tiger_model = {
#include "chao/minimal_models/minimal_tiger_model.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_2_position[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_tiger_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_tiger_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_tiger_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_tiger_held_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_tiger_held_tracks[8] = {
#include "chao/minimal_models/minimal_tiger_held_tracks.inc"
};
static NJS_MOTION minimal_tiger_held = {
#include "chao/minimal_models/minimal_tiger_held.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_3_position[2] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_5_position[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_6_position[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_tiger_walk_joint_7_position[2] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_tiger_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_tiger_walk_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_tiger_walk_tracks[8] = {
#include "chao/minimal_models/minimal_tiger_walk_tracks.inc"
};
static NJS_MOTION minimal_tiger_walk = {
#include "chao/minimal_models/minimal_tiger_walk.inc"
};
static NJS_MKEY_F minimal_tiger_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_tiger_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_tiger_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_tiger_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_tiger_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_tiger_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_tiger_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_tiger_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_tiger_idle_joint_7_angles.inc"
};
static NJS_MDATA2 minimal_tiger_idle_tracks[8] = {
#include "chao/minimal_models/minimal_tiger_idle_tracks.inc"
};
static NJS_MOTION minimal_tiger_idle = {
#include "chao/minimal_models/minimal_tiger_idle.inc"
};
static Sint16 minimal_unicorn_polygons_11[25] = {
#include "chao/minimal_models/minimal_unicorn_polygons_11.inc"
};
static Sint32 minimal_unicorn_vertices_11[27] = {
#include "chao/minimal_models/minimal_unicorn_vertices_11.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_11 = {
#include "chao/minimal_models/minimal_unicorn_mesh_11.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_11 = {
#include "chao/minimal_models/minimal_unicorn_node_11.inc"
};
static Sint16 minimal_unicorn_polygons_10[51] = {
#include "chao/minimal_models/minimal_unicorn_polygons_10.inc"
};
static Sint32 minimal_unicorn_vertices_10[33] = {
#include "chao/minimal_models/minimal_unicorn_vertices_10.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_10 = {
#include "chao/minimal_models/minimal_unicorn_mesh_10.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_10 = {
#include "chao/minimal_models/minimal_unicorn_node_10.inc"
};
static Sint16 minimal_unicorn_polygons_9[51] = {
#include "chao/minimal_models/minimal_unicorn_polygons_9.inc"
};
static Sint32 minimal_unicorn_vertices_9[33] = {
#include "chao/minimal_models/minimal_unicorn_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_9 = {
#include "chao/minimal_models/minimal_unicorn_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_9 = {
#include "chao/minimal_models/minimal_unicorn_node_9.inc"
};
static Sint16 minimal_unicorn_polygons_8[65] = {
#include "chao/minimal_models/minimal_unicorn_polygons_8.inc"
};
static Sint32 minimal_unicorn_vertices_8[99] = {
#include "chao/minimal_models/minimal_unicorn_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_8 = {
#include "chao/minimal_models/minimal_unicorn_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_8 = {
#include "chao/minimal_models/minimal_unicorn_node_8.inc"
};
static Sint16 minimal_unicorn_polygons_7[25] = {
#include "chao/minimal_models/minimal_unicorn_polygons_7.inc"
};
static Sint32 minimal_unicorn_vertices_7[27] = {
#include "chao/minimal_models/minimal_unicorn_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_7 = {
#include "chao/minimal_models/minimal_unicorn_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_7 = {
#include "chao/minimal_models/minimal_unicorn_node_7.inc"
};
static Sint16 minimal_unicorn_polygons_6[25] = {
#include "chao/minimal_models/minimal_unicorn_polygons_6.inc"
};
static Sint32 minimal_unicorn_vertices_6[27] = {
#include "chao/minimal_models/minimal_unicorn_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_6 = {
#include "chao/minimal_models/minimal_unicorn_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_6 = {
#include "chao/minimal_models/minimal_unicorn_node_6.inc"
};
static Sint16 minimal_unicorn_polygons_5[93] = {
#include "chao/minimal_models/minimal_unicorn_polygons_5.inc"
};
static Sint32 minimal_unicorn_vertices_5[99] = {
#include "chao/minimal_models/minimal_unicorn_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_5 = {
#include "chao/minimal_models/minimal_unicorn_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_5 = {
#include "chao/minimal_models/minimal_unicorn_node_5.inc"
};
static Sint16 minimal_unicorn_polygons_4[221] = {
#include "chao/minimal_models/minimal_unicorn_polygons_4.inc"
};
static Sint32 minimal_unicorn_vertices_4[153] = {
#include "chao/minimal_models/minimal_unicorn_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_4 = {
#include "chao/minimal_models/minimal_unicorn_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_4 = {
#include "chao/minimal_models/minimal_unicorn_node_4.inc"
};
static Sint16 minimal_unicorn_polygons_3[51] = {
#include "chao/minimal_models/minimal_unicorn_polygons_3.inc"
};
static Sint32 minimal_unicorn_vertices_3[33] = {
#include "chao/minimal_models/minimal_unicorn_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_3 = {
#include "chao/minimal_models/minimal_unicorn_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_3 = {
#include "chao/minimal_models/minimal_unicorn_node_3.inc"
};
static Sint16 minimal_unicorn_polygons_2[51] = {
#include "chao/minimal_models/minimal_unicorn_polygons_2.inc"
};
static Sint32 minimal_unicorn_vertices_2[33] = {
#include "chao/minimal_models/minimal_unicorn_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_2 = {
#include "chao/minimal_models/minimal_unicorn_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_2 = {
#include "chao/minimal_models/minimal_unicorn_node_2.inc"
};
static Sint16 minimal_unicorn_polygons_1[115] = {
#include "chao/minimal_models/minimal_unicorn_polygons_1.inc"
};
static Sint32 minimal_unicorn_vertices_1[93] = {
#include "chao/minimal_models/minimal_unicorn_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_unicorn_mesh_1 = {
#include "chao/minimal_models/minimal_unicorn_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_node_1 = {
#include "chao/minimal_models/minimal_unicorn_node_1.inc"
};
static NJS_CNK_OBJECT minimal_unicorn_model = {
#include "chao/minimal_models/minimal_unicorn_model.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_held_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_held_joint_10_angles.inc"
};
static NJS_MDATA2 minimal_unicorn_held_tracks[12] = {
#include "chao/minimal_models/minimal_unicorn_held_tracks.inc"
};
static NJS_MOTION minimal_unicorn_held = {
#include "chao/minimal_models/minimal_unicorn_held.inc"
};
static NJS_MKEY_F minimal_unicorn_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_10_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_10_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_walk_joint_11_angles[16] = {
#include "chao/minimal_models/minimal_unicorn_walk_joint_11_angles.inc"
};
static NJS_MDATA2 minimal_unicorn_walk_tracks[12] = {
#include "chao/minimal_models/minimal_unicorn_walk_tracks.inc"
};
static NJS_MOTION minimal_unicorn_walk = {
#include "chao/minimal_models/minimal_unicorn_walk.inc"
};
static NJS_MKEY_F minimal_unicorn_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_9_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_10_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_10_angles.inc"
};
static NJS_MKEY_A minimal_unicorn_idle_joint_11_angles[4] = {
#include "chao/minimal_models/minimal_unicorn_idle_joint_11_angles.inc"
};
static NJS_MDATA2 minimal_unicorn_idle_tracks[12] = {
#include "chao/minimal_models/minimal_unicorn_idle_tracks.inc"
};
static NJS_MOTION minimal_unicorn_idle = {
#include "chao/minimal_models/minimal_unicorn_idle.inc"
};
static Sint16 minimal_rabbit_polygons_9[51] = {
#include "chao/minimal_models/minimal_rabbit_polygons_9.inc"
};
static Sint32 minimal_rabbit_vertices_9[33] = {
#include "chao/minimal_models/minimal_rabbit_vertices_9.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_9 = {
#include "chao/minimal_models/minimal_rabbit_mesh_9.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_9 = {
#include "chao/minimal_models/minimal_rabbit_node_9.inc"
};
static Sint16 minimal_rabbit_polygons_8[55] = {
#include "chao/minimal_models/minimal_rabbit_polygons_8.inc"
};
static Sint32 minimal_rabbit_vertices_8[45] = {
#include "chao/minimal_models/minimal_rabbit_vertices_8.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_8 = {
#include "chao/minimal_models/minimal_rabbit_mesh_8.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_8 = {
#include "chao/minimal_models/minimal_rabbit_node_8.inc"
};
static Sint16 minimal_rabbit_polygons_7[55] = {
#include "chao/minimal_models/minimal_rabbit_polygons_7.inc"
};
static Sint32 minimal_rabbit_vertices_7[45] = {
#include "chao/minimal_models/minimal_rabbit_vertices_7.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_7 = {
#include "chao/minimal_models/minimal_rabbit_mesh_7.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_7 = {
#include "chao/minimal_models/minimal_rabbit_node_7.inc"
};
static Sint16 minimal_rabbit_polygons_6[39] = {
#include "chao/minimal_models/minimal_rabbit_polygons_6.inc"
};
static Sint32 minimal_rabbit_vertices_6[33] = {
#include "chao/minimal_models/minimal_rabbit_vertices_6.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_6 = {
#include "chao/minimal_models/minimal_rabbit_mesh_6.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_6 = {
#include "chao/minimal_models/minimal_rabbit_node_6.inc"
};
static Sint16 minimal_rabbit_polygons_5[39] = {
#include "chao/minimal_models/minimal_rabbit_polygons_5.inc"
};
static Sint32 minimal_rabbit_vertices_5[33] = {
#include "chao/minimal_models/minimal_rabbit_vertices_5.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_5 = {
#include "chao/minimal_models/minimal_rabbit_mesh_5.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_5 = {
#include "chao/minimal_models/minimal_rabbit_node_5.inc"
};
static Sint16 minimal_rabbit_polygons_4[253] = {
#include "chao/minimal_models/minimal_rabbit_polygons_4.inc"
};
static Sint32 minimal_rabbit_vertices_4[123] = {
#include "chao/minimal_models/minimal_rabbit_vertices_4.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_4 = {
#include "chao/minimal_models/minimal_rabbit_mesh_4.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_4 = {
#include "chao/minimal_models/minimal_rabbit_node_4.inc"
};
static Sint16 minimal_rabbit_polygons_3[39] = {
#include "chao/minimal_models/minimal_rabbit_polygons_3.inc"
};
static Sint32 minimal_rabbit_vertices_3[27] = {
#include "chao/minimal_models/minimal_rabbit_vertices_3.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_3 = {
#include "chao/minimal_models/minimal_rabbit_mesh_3.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_3 = {
#include "chao/minimal_models/minimal_rabbit_node_3.inc"
};
static Sint16 minimal_rabbit_polygons_2[39] = {
#include "chao/minimal_models/minimal_rabbit_polygons_2.inc"
};
static Sint32 minimal_rabbit_vertices_2[27] = {
#include "chao/minimal_models/minimal_rabbit_vertices_2.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_2 = {
#include "chao/minimal_models/minimal_rabbit_mesh_2.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_2 = {
#include "chao/minimal_models/minimal_rabbit_node_2.inc"
};
static Sint16 minimal_rabbit_polygons_1[205] = {
#include "chao/minimal_models/minimal_rabbit_polygons_1.inc"
};
static Sint32 minimal_rabbit_vertices_1[99] = {
#include "chao/minimal_models/minimal_rabbit_vertices_1.inc"
};
static NJS_CNK_MODEL minimal_rabbit_mesh_1 = {
#include "chao/minimal_models/minimal_rabbit_mesh_1.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_node_1 = {
#include "chao/minimal_models/minimal_rabbit_node_1.inc"
};
static NJS_CNK_OBJECT minimal_rabbit_model = {
#include "chao/minimal_models/minimal_rabbit_model.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_0_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_0_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_1_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_1_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_2_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_2_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_3_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_3_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_4_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_4_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_5_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_5_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_6_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_6_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_7_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_7_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_8_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_8_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_held_joint_9_position[2] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_9_position.inc"
};
static NJS_MKEY_A minimal_rabbit_held_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_held_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_rabbit_held_tracks[10] = {
#include "chao/minimal_models/minimal_rabbit_held_tracks.inc"
};
static NJS_MOTION minimal_rabbit_held = {
#include "chao/minimal_models/minimal_rabbit_held.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_0_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_0_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_0_angles[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_0_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_1_position[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_1_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_1_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_1_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_2_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_2_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_2_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_2_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_3_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_3_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_3_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_3_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_4_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_4_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_4_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_4_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_5_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_5_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_5_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_5_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_6_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_6_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_6_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_6_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_7_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_7_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_7_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_7_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_8_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_8_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_8_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_8_angles.inc"
};
static NJS_MKEY_F minimal_rabbit_walk_joint_9_position[2] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_9_position.inc"
};
static NJS_MKEY_A minimal_rabbit_walk_joint_9_angles[16] = {
#include "chao/minimal_models/minimal_rabbit_walk_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_rabbit_walk_tracks[10] = {
#include "chao/minimal_models/minimal_rabbit_walk_tracks.inc"
};
static NJS_MOTION minimal_rabbit_walk = {
#include "chao/minimal_models/minimal_rabbit_walk.inc"
};
static NJS_MKEY_F minimal_rabbit_idle_joint_1_position[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_1_position.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_1_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_1_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_2_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_2_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_3_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_3_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_4_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_4_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_5_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_5_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_6_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_6_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_7_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_7_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_8_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_8_angles.inc"
};
static NJS_MKEY_A minimal_rabbit_idle_joint_9_angles[4] = {
#include "chao/minimal_models/minimal_rabbit_idle_joint_9_angles.inc"
};
static NJS_MDATA2 minimal_rabbit_idle_tracks[10] = {
#include "chao/minimal_models/minimal_rabbit_idle_tracks.inc"
};
static NJS_MOTION minimal_rabbit_idle = {
#include "chao/minimal_models/minimal_rabbit_idle.inc"
};
NJS_CNK_OBJECT *garden_minimal_models[ANIMAL_KINDS] = {
#include "chao/minimal_models/garden_minimal_models.inc"
};
NJS_MOTION *garden_minimal_held_motions[ANIMAL_KINDS] = {
#include "chao/minimal_models/garden_minimal_held_motions.inc"
};
NJS_MOTION *garden_minimal_walk_motions[ANIMAL_KINDS] = {
#include "chao/minimal_models/garden_minimal_walk_motions.inc"
};
NJS_MOTION *garden_minimal_idle_motions[ANIMAL_KINDS] = {
#include "chao/minimal_models/garden_minimal_idle_motions.inc"
};
NJS_MOTION *garden_minimal_fly_motions[ANIMAL_KINDS] = {
#include "chao/minimal_models/garden_minimal_fly_motions.inc"
};
