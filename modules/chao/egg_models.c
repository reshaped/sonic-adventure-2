/* CHAO.PRS's chunk models and motions, from the disc. its D lies between
 * editor_state.c and tuning.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/egg.h"

static Sint16 egg_shell_bottom_polygons[227] = {
#include "chao/egg_models/egg_shell_bottom_polygons.inc"
};
static Sint32 egg_shell_bottom_vertices[201] = {
#include "chao/egg_models/egg_shell_bottom_vertices.inc"
};
static NJS_CNK_MODEL egg_shell_bottom_model = {
#include "chao/egg_models/egg_shell_bottom_model.inc"
};
NJS_CNK_OBJECT egg_shell_bottom[1] = {
#include "chao/egg_models/egg_shell_bottom.inc"
};
static Sint16 egg_shell_top_polygons[255] = {
#include "chao/egg_models/egg_shell_top_polygons.inc"
};
static Sint32 egg_shell_top_vertices[201] = {
#include "chao/egg_models/egg_shell_top_vertices.inc"
};
static NJS_CNK_MODEL egg_shell_top_model = {
#include "chao/egg_models/egg_shell_top_model.inc"
};
NJS_CNK_OBJECT egg_shell_top[1] = {
#include "chao/egg_models/egg_shell_top.inc"
};
NJS_CNK_OBJECT egg_open_model[1] = {
#include "chao/egg_models/egg_open_model.inc"
};
static Sint16 egg_whole_model_polygons[315] = {
#include "chao/egg_models/egg_whole_model_polygons.inc"
};
static Sint32 egg_whole_model_vertices[255] = {
#include "chao/egg_models/egg_whole_model_vertices.inc"
};
static NJS_CNK_MODEL egg_whole_mesh = {
#include "chao/egg_models/egg_whole_mesh.inc"
};
NJS_CNK_OBJECT egg_whole_model[1] = {
#include "chao/egg_models/egg_whole_model.inc"
};
static NJS_MKEY_F egg_shake_motion_joint_0_position[12] = {
#include "chao/egg_models/egg_shake_motion_joint_0_position.inc"
};
static NJS_MKEY_A egg_shake_motion_joint_0_angles[13] = {
#include "chao/egg_models/egg_shake_motion_joint_0_angles.inc"
};
static NJS_MDATA2 egg_shake_motion_tracks[1] = {
#include "chao/egg_models/egg_shake_motion_tracks.inc"
};
NJS_MOTION egg_shake_motion[1] = {
#include "chao/egg_models/egg_shake_motion.inc"
};
static NJS_MKEY_A egg_hatch_motion_206_joint_1_angles[27] = {
#include "chao/egg_models/egg_hatch_motion_206_joint_1_angles.inc"
};
static NJS_MKEY_A egg_hatch_motion_206_joint_2_angles[17] = {
#include "chao/egg_models/egg_hatch_motion_206_joint_2_angles.inc"
};
static NJS_MDATA2 egg_hatch_motion_206_tracks[3] = {
#include "chao/egg_models/egg_hatch_motion_206_tracks.inc"
};
NJS_MOTION egg_hatch_motion_206 = {
#include "chao/egg_models/egg_hatch_motion_206.inc"
};
static NJS_MKEY_A egg_hatch_motion_205_joint_1_angles[20] = {
#include "chao/egg_models/egg_hatch_motion_205_joint_1_angles.inc"
};
static NJS_MKEY_A egg_hatch_motion_205_joint_2_angles[13] = {
#include "chao/egg_models/egg_hatch_motion_205_joint_2_angles.inc"
};
static NJS_MDATA2 egg_hatch_motion_205_tracks[3] = {
#include "chao/egg_models/egg_hatch_motion_205_tracks.inc"
};
NJS_MOTION egg_hatch_motion_205 = {
#include "chao/egg_models/egg_hatch_motion_205.inc"
};
