#include <ninja.h>

#include "chaostgkinder/extra_motions.h"

static NJS_MKEY_A extra_motion_c_pose_node_1_angles[12] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_1_angles.inc"
};

static NJS_MKEY_A extra_motion_c_pose_node_2_angles[12] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_2_angles.inc"
};

static NJS_MKEY_F extra_motion_c_pose_node_3_positions[2] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_3_positions.inc"
};

static NJS_MKEY_A extra_motion_c_pose_node_3_angles[2] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_3_angles.inc"
};

static NJS_MKEY_A extra_motion_c_pose_node_4_angles[12] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_4_angles.inc"
};

static NJS_MKEY_F extra_motion_c_pose_node_5_positions[2] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_5_positions.inc"
};

static NJS_MKEY_A extra_motion_c_pose_node_5_angles[2] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_5_angles.inc"
};

static NJS_MKEY_F extra_motion_c_pose_node_12_positions[9] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_12_positions.inc"
};

static NJS_MKEY_A extra_motion_c_pose_node_13_angles[12] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_13_angles.inc"
};

static NJS_MKEY_A extra_motion_c_pose_node_14_angles[12] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_node_14_angles.inc"
};

static NJS_MDATA2 extra_motion_c_pose_channels[15] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_pose_channels.inc"
};

NJS_MOTION kinder_extra_motion_c_pose = {
#include "chaostgkinder/extra_motion_c/kinder_extra_motion_c_pose.inc"
};

static NJS_POINT3 extra_motion_c_node_7_shapes[10][55] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_node_7_shapes.inc"
};

static NJS_MKEY_P extra_motion_c_shape_node_7_vertices_keys[10] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_shape_node_7_vertices_keys.inc"
};

static NJS_POINT3 extra_motion_c_node_9_shapes[10][55] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_node_9_shapes.inc"
};

static NJS_MKEY_P extra_motion_c_shape_node_9_vertices_keys[10] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_shape_node_9_vertices_keys.inc"
};

static NJS_MDATA2 extra_motion_c_shape_channels[15] = {
#include "chaostgkinder/extra_motion_c/extra_motion_c_shape_channels.inc"
};

NJS_MOTION kinder_extra_motion_c_shape = {
#include "chaostgkinder/extra_motion_c/kinder_extra_motion_c_shape.inc"
};
