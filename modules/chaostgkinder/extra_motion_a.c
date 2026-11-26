#include <ninja.h>

#include "chaostgkinder/extra_motions.h"

static NJS_MKEY_A extra_motion_a_pose_node_1_angles[16] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_1_angles.inc"
};

static NJS_MKEY_A extra_motion_a_pose_node_2_angles[18] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_2_angles.inc"
};

static NJS_MKEY_F extra_motion_a_pose_node_3_positions[3] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_3_positions.inc"
};

static NJS_MKEY_A extra_motion_a_pose_node_3_angles[3] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_3_angles.inc"
};

static NJS_MKEY_A extra_motion_a_pose_node_4_angles[17] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_4_angles.inc"
};

static NJS_MKEY_F extra_motion_a_pose_node_5_positions[4] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_5_positions.inc"
};

static NJS_MKEY_A extra_motion_a_pose_node_5_angles[9] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_5_angles.inc"
};

static NJS_MKEY_A extra_motion_a_pose_node_6_angles[20] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_6_angles.inc"
};

static NJS_MKEY_F extra_motion_a_pose_node_12_positions[16] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_node_12_positions.inc"
};

static NJS_MDATA2 extra_motion_a_pose_channels[15] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_pose_channels.inc"
};

NJS_MOTION kinder_extra_motion_a_pose = {
#include "chaostgkinder/extra_motion_a/kinder_extra_motion_a_pose.inc"
};

static NJS_POINT3 extra_motion_a_node_7_shapes[8][55] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_node_7_shapes.inc"
};

static NJS_MKEY_P extra_motion_a_shape_node_7_vertices_keys[8] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_shape_node_7_vertices_keys.inc"
};

static NJS_POINT3 extra_motion_a_node_9_shapes[10][55] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_node_9_shapes.inc"
};

static NJS_MKEY_P extra_motion_a_shape_node_9_vertices_keys[10] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_shape_node_9_vertices_keys.inc"
};

static NJS_MDATA2 extra_motion_a_shape_channels[15] = {
#include "chaostgkinder/extra_motion_a/extra_motion_a_shape_channels.inc"
};

NJS_MOTION kinder_extra_motion_a_shape = {
#include "chaostgkinder/extra_motion_a/kinder_extra_motion_a_shape.inc"
};
