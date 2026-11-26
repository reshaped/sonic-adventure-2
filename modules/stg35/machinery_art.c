#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_conduit_textures.h"
#include "shared/core_energy_art.h"
#include "stg35/machinery.h"

/* texture names that the machinery's texture lists point at. some are in the
 * texture list of the background's PVM too */

const struct eggman_machinery_texture_names eggman_machinery_texture_names = {
#include "stg35/machinery_art/eggman_machinery_texture_names.inc"
};

const char core_pipe_wave_name[20] = {
#include "stg35/machinery_art/core_pipe_wave_name.inc"
};

const char core_segment_ring_name[16] = {
#include "stg35/machinery_art/core_segment_ring_name.inc"
};

const char core_segment_light_name[16] = {
#include "stg35/machinery_art/core_segment_light_name.inc"
};

const char core_conduit_skin_name[13] = {
#include "stg35/machinery_art/core_conduit_skin_name.inc"
};

/* STG35.PRS's texture lists, chunk models and motions, from the disc. its D
 * lies between panels.c and lamp_art.c */
static NJS_TEXNAME rotor_texlist_names[3] = {
#include "stg35/machinery_art/rotor_texlist_names.inc"
};
NJS_TEXLIST eggman_rotor_texlist = {
#include "stg35/machinery_art/eggman_rotor_texlist.inc"
};
static Sint16 rotor_return_b_polygons[679] = {
#include "stg35/machinery_art/rotor_return_b_polygons.inc"
};
static Sint32 rotor_return_b_vertices[483] = {
#include "stg35/machinery_art/rotor_return_b_vertices.inc"
};
static NJS_CNK_MODEL rotor_return_b_model = {
#include "stg35/machinery_art/rotor_return_b_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_return_b = {
#include "stg35/machinery_art/eggman_rotor_return_b.inc"
};
static Sint16 rotor_medium_b_polygons[679] = {
#include "stg35/machinery_art/rotor_medium_b_polygons.inc"
};
static Sint32 rotor_medium_b_vertices[483] = {
#include "stg35/machinery_art/rotor_medium_b_vertices.inc"
};
static NJS_CNK_MODEL rotor_medium_b_model = {
#include "stg35/machinery_art/rotor_medium_b_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_medium_b = {
#include "stg35/machinery_art/eggman_rotor_medium_b.inc"
};
static Sint16 rotor_reverse_b_polygons[679] = {
#include "stg35/machinery_art/rotor_reverse_b_polygons.inc"
};
static Sint32 rotor_reverse_b_vertices[483] = {
#include "stg35/machinery_art/rotor_reverse_b_vertices.inc"
};
static NJS_CNK_MODEL rotor_reverse_b_model = {
#include "stg35/machinery_art/rotor_reverse_b_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_reverse_b = {
#include "stg35/machinery_art/eggman_rotor_reverse_b.inc"
};
static Sint16 rotor_slow_a_polygons[679] = {
#include "stg35/machinery_art/rotor_slow_a_polygons.inc"
};
static Sint32 rotor_slow_a_vertices[483] = {
#include "stg35/machinery_art/rotor_slow_a_vertices.inc"
};
static NJS_CNK_MODEL rotor_slow_a_model = {
#include "stg35/machinery_art/rotor_slow_a_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_slow_a = {
#include "stg35/machinery_art/eggman_rotor_slow_a.inc"
};
static Sint16 rotor_return_a_polygons[679] = {
#include "stg35/machinery_art/rotor_return_a_polygons.inc"
};
static Sint32 rotor_return_a_vertices[479] = {
#include "stg35/machinery_art/rotor_return_a_vertices.inc"
};
static NJS_CNK_MODEL rotor_return_a_model = {
#include "stg35/machinery_art/rotor_return_a_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_return_a = {
#include "stg35/machinery_art/eggman_rotor_return_a.inc"
};
static Sint16 rotor_medium_a_polygons[679] = {
#include "stg35/machinery_art/rotor_medium_a_polygons.inc"
};
static Sint32 rotor_medium_a_vertices[483] = {
#include "stg35/machinery_art/rotor_medium_a_vertices.inc"
};
static NJS_CNK_MODEL rotor_medium_a_model = {
#include "stg35/machinery_art/rotor_medium_a_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_medium_a = {
#include "stg35/machinery_art/eggman_rotor_medium_a.inc"
};
static Sint16 rotor_reverse_a_polygons[679] = {
#include "stg35/machinery_art/rotor_reverse_a_polygons.inc"
};
static Sint32 rotor_reverse_a_vertices[483] = {
#include "stg35/machinery_art/rotor_reverse_a_vertices.inc"
};
static NJS_CNK_MODEL rotor_reverse_a_model = {
#include "stg35/machinery_art/rotor_reverse_a_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_reverse_a = {
#include "stg35/machinery_art/eggman_rotor_reverse_a.inc"
};
static Sint16 rotor_fast_a_polygons[741] = {
#include "stg35/machinery_art/rotor_fast_a_polygons.inc"
};
static Sint32 rotor_fast_a_vertices[519] = {
#include "stg35/machinery_art/rotor_fast_a_vertices.inc"
};
static NJS_CNK_MODEL rotor_fast_a_model = {
#include "stg35/machinery_art/rotor_fast_a_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_fast_a = {
#include "stg35/machinery_art/eggman_rotor_fast_a.inc"
};
static Sint16 rotor_reverse_c_polygons[679] = {
#include "stg35/machinery_art/rotor_reverse_c_polygons.inc"
};
static Sint32 rotor_reverse_c_vertices[483] = {
#include "stg35/machinery_art/rotor_reverse_c_vertices.inc"
};
static NJS_CNK_MODEL rotor_reverse_c_model = {
#include "stg35/machinery_art/rotor_reverse_c_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_reverse_c = {
#include "stg35/machinery_art/eggman_rotor_reverse_c.inc"
};
static Sint16 rotor_fast_b_polygons[679] = {
#include "stg35/machinery_art/rotor_fast_b_polygons.inc"
};
static Sint32 rotor_fast_b_vertices[483] = {
#include "stg35/machinery_art/rotor_fast_b_vertices.inc"
};
static NJS_CNK_MODEL rotor_fast_b_model = {
#include "stg35/machinery_art/rotor_fast_b_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_fast_b = {
#include "stg35/machinery_art/eggman_rotor_fast_b.inc"
};
static Sint16 rotor_slow_b_polygons[679] = {
#include "stg35/machinery_art/rotor_slow_b_polygons.inc"
};
static Sint32 rotor_slow_b_vertices[403] = {
#include "stg35/machinery_art/rotor_slow_b_vertices.inc"
};
static NJS_CNK_MODEL rotor_slow_b_model = {
#include "stg35/machinery_art/rotor_slow_b_model.inc"
};
NJS_CNK_OBJECT eggman_rotor_slow_b = {
#include "stg35/machinery_art/eggman_rotor_slow_b.inc"
};
NJS_CNK_OBJECT eggman_rotor_assembly[1] = {
#include "stg35/machinery_art/eggman_rotor_assembly.inc"
};
static NJS_TEXNAME glass_1_material_1_names[1] = {
#include "stg35/machinery_art/glass_1_material_1_names.inc"
};
static NJS_TEXLIST glass_1_material_1 = {
#include "stg35/machinery_art/glass_1_material_1.inc"
};
static Sint16 glass_1_polygons[25] = {
#include "stg35/machinery_art/glass_1_polygons.inc"
};
static Sint32 glass_1_vertices[19] = {
#include "stg35/machinery_art/glass_1_vertices.inc"
};
static NJS_CNK_MODEL glass_1_model = {
#include "stg35/machinery_art/glass_1_model.inc"
};
NJS_CNK_OBJECT eggman_glass_1 = {
#include "stg35/machinery_art/eggman_glass_1.inc"
};
static NJS_TEXNAME glass_2_material_1_names[1] = {
#include "stg35/machinery_art/glass_2_material_1_names.inc"
};
static NJS_TEXLIST glass_2_material_1 = {
#include "stg35/machinery_art/glass_2_material_1.inc"
};
static Sint16 glass_2_polygons[25] = {
#include "stg35/machinery_art/glass_2_polygons.inc"
};
static Sint32 glass_2_vertices[19] = {
#include "stg35/machinery_art/glass_2_vertices.inc"
};
static NJS_CNK_MODEL glass_2_model = {
#include "stg35/machinery_art/glass_2_model.inc"
};
NJS_CNK_OBJECT eggman_glass_2 = {
#include "stg35/machinery_art/eggman_glass_2.inc"
};
static NJS_TEXNAME glass_3_material_1_names[1] = {
#include "stg35/machinery_art/glass_3_material_1_names.inc"
};
static NJS_TEXLIST glass_3_material_1 = {
#include "stg35/machinery_art/glass_3_material_1.inc"
};
static Sint16 glass_near_polygons[25] = {
#include "stg35/machinery_art/glass_near_polygons.inc"
};
static Sint32 glass_near_vertices[19] = {
#include "stg35/machinery_art/glass_near_vertices.inc"
};
static NJS_CNK_MODEL glass_near_model = {
#include "stg35/machinery_art/glass_near_model.inc"
};
static NJS_CNK_OBJECT glass_near = {
#include "stg35/machinery_art/glass_near.inc"
};
static NJS_CNK_OBJECT glass_near_group = {
#include "stg35/machinery_art/glass_near_group.inc"
};
static NJS_TEXNAME glass_3_material_2_names[1] = {
#include "stg35/machinery_art/glass_3_material_2_names.inc"
};
static NJS_TEXLIST glass_3_material_2 = {
#include "stg35/machinery_art/glass_3_material_2.inc"
};
static Sint16 glass_far_polygons[25] = {
#include "stg35/machinery_art/glass_far_polygons.inc"
};
static Sint32 glass_far_vertices[19] = {
#include "stg35/machinery_art/glass_far_vertices.inc"
};
static NJS_CNK_MODEL glass_far_model = {
#include "stg35/machinery_art/glass_far_model.inc"
};
static NJS_CNK_OBJECT glass_far = {
#include "stg35/machinery_art/glass_far.inc"
};
static NJS_CNK_OBJECT glass_far_group = {
#include "stg35/machinery_art/glass_far_group.inc"
};
static NJS_TEXNAME glass_3_material_3_names[1] = {
#include "stg35/machinery_art/glass_3_material_3_names.inc"
};
static NJS_TEXLIST glass_3_material_3 = {
#include "stg35/machinery_art/glass_3_material_3.inc"
};
static Sint16 glass_3_polygons[25] = {
#include "stg35/machinery_art/glass_3_polygons.inc"
};
static Sint32 glass_3_vertices[19] = {
#include "stg35/machinery_art/glass_3_vertices.inc"
};
static NJS_CNK_MODEL glass_3_model = {
#include "stg35/machinery_art/glass_3_model.inc"
};
NJS_CNK_OBJECT eggman_glass_3 = {
#include "stg35/machinery_art/eggman_glass_3.inc"
};
static NJS_TEXNAME glass_4_material_1_names[1] = {
#include "stg35/machinery_art/glass_4_material_1_names.inc"
};
static NJS_TEXLIST glass_4_material_1 = {
#include "stg35/machinery_art/glass_4_material_1.inc"
};
static Sint16 glass_4_polygons[25] = {
#include "stg35/machinery_art/glass_4_polygons.inc"
};
static Sint32 glass_4_vertices[19] = {
#include "stg35/machinery_art/glass_4_vertices.inc"
};
static NJS_CNK_MODEL glass_4_model = {
#include "stg35/machinery_art/glass_4_model.inc"
};
NJS_CNK_OBJECT eggman_glass_4 = {
#include "stg35/machinery_art/eggman_glass_4.inc"
};
static NJS_TEXNAME pipe_texlist_names[1] = {
#include "stg35/machinery_art/pipe_texlist_names.inc"
};
NJS_TEXLIST eggman_pipe_texlist = {
#include "stg35/machinery_art/eggman_pipe_texlist.inc"
};
static Sint16 pipe_polygons[25] = {
#include "stg35/machinery_art/pipe_polygons.inc"
};
static Sint32 pipe_vertices[19] = {
#include "stg35/machinery_art/pipe_vertices.inc"
};
static NJS_CNK_MODEL pipe_model = {
#include "stg35/machinery_art/pipe_model.inc"
};
NJS_CNK_OBJECT eggman_pipe[1] = {
#include "stg35/machinery_art/eggman_pipe.inc"
};
static NJS_POINT3 pipe_pose_0[4] = {
#include "stg35/machinery_art/pipe_pose_0.inc"
};
static NJS_POINT3 pipe_pose_19[4] = {
#include "stg35/machinery_art/pipe_pose_19.inc"
};
static NJS_POINT3 pipe_pose_49[4] = {
#include "stg35/machinery_art/pipe_pose_49.inc"
};
static NJS_POINT3 pipe_pose_79[4] = {
#include "stg35/machinery_art/pipe_pose_79.inc"
};
static NJS_POINT3 pipe_pose_99[4] = {
#include "stg35/machinery_art/pipe_pose_99.inc"
};
static NJS_MKEY_P eggman_pipe_shape_joint_0_vertices[5] = {
#include "stg35/machinery_art/eggman_pipe_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 eggman_pipe_shape_tracks[1] = {
#include "stg35/machinery_art/eggman_pipe_shape_tracks.inc"
};
NJS_MOTION eggman_pipe_shape[1] = {
#include "stg35/machinery_art/eggman_pipe_shape.inc"
};
static NJS_MDATA3 eggman_pipe_motion_tracks[1] = {
#include "stg35/machinery_art/eggman_pipe_motion_tracks.inc"
};
NJS_MOTION eggman_pipe_motion[1] = {
#include "stg35/machinery_art/eggman_pipe_motion.inc"
};
static NJS_TEXNAME joint_texlist_names[1] = {
#include "stg35/machinery_art/joint_texlist_names.inc"
};
NJS_TEXLIST eggman_joint_texlist = {
#include "stg35/machinery_art/eggman_joint_texlist.inc"
};
static Sint16 joint_polygons[39] = {
#include "stg35/machinery_art/joint_polygons.inc"
};
static Sint32 joint_vertices[35] = {
#include "stg35/machinery_art/joint_vertices.inc"
};
static NJS_CNK_MODEL joint_model = {
#include "stg35/machinery_art/joint_model.inc"
};
NJS_CNK_OBJECT eggman_joint[1] = {
#include "stg35/machinery_art/eggman_joint.inc"
};
static NJS_MDATA3 eggman_joint_motion_tracks[1] = {
#include "stg35/machinery_art/eggman_joint_motion_tracks.inc"
};
NJS_MOTION eggman_joint_motion[1] = {
#include "stg35/machinery_art/eggman_joint_motion.inc"
};
static NJS_POINT3 joint_pose_0[8] = {
#include "stg35/machinery_art/joint_pose_0.inc"
};
static NJS_POINT3 joint_pose_11[8] = {
#include "stg35/machinery_art/joint_pose_11.inc"
};
static NJS_POINT3 joint_pose_24[8] = {
#include "stg35/machinery_art/joint_pose_24.inc"
};
static NJS_POINT3 joint_pose_36[8] = {
#include "stg35/machinery_art/joint_pose_36.inc"
};
static NJS_POINT3 joint_pose_49[8] = {
#include "stg35/machinery_art/joint_pose_49.inc"
};
static NJS_POINT3 joint_pose_61[8] = {
#include "stg35/machinery_art/joint_pose_61.inc"
};
static NJS_POINT3 joint_pose_74[8] = {
#include "stg35/machinery_art/joint_pose_74.inc"
};
static NJS_POINT3 joint_pose_86[8] = {
#include "stg35/machinery_art/joint_pose_86.inc"
};
static NJS_POINT3 joint_pose_99[8] = {
#include "stg35/machinery_art/joint_pose_99.inc"
};
static NJS_MKEY_P eggman_joint_shape_joint_0_vertices[9] = {
#include "stg35/machinery_art/eggman_joint_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 eggman_joint_shape_tracks[1] = {
#include "stg35/machinery_art/eggman_joint_shape_tracks.inc"
};
NJS_MOTION eggman_joint_shape[1] = {
#include "stg35/machinery_art/eggman_joint_shape.inc"
};
static NJS_TEXNAME core_stream_texlist_names[1] = {
#include "stg35/machinery_art/core_stream_texlist_names.inc"
};
NJS_TEXLIST core_stream_texlist = {
#include "stg35/machinery_art/core_stream_texlist.inc"
};
static Sint16 core_stream_object_polygons[39] = {
#include "stg35/machinery_art/core_stream_object_polygons.inc"
};
static Sint32 core_stream_object_vertices[35] = {
#include "stg35/machinery_art/core_stream_object_vertices.inc"
};
static NJS_CNK_MODEL core_stream_object_model = {
#include "stg35/machinery_art/core_stream_object_model.inc"
};
NJS_CNK_OBJECT core_stream_object[1] = {
#include "stg35/machinery_art/core_stream_object.inc"
};
static NJS_MDATA3 core_stream_motion_tracks[1] = {
#include "stg35/machinery_art/core_stream_motion_tracks.inc"
};
NJS_MOTION core_stream_motion[1] = {
#include "stg35/machinery_art/core_stream_motion.inc"
};
static NJS_POINT3 core_stream_object_pose_0[8] = {
#include "stg35/machinery_art/core_stream_object_pose_0.inc"
};
static NJS_POINT3 core_stream_object_pose_9[8] = {
#include "stg35/machinery_art/core_stream_object_pose_9.inc"
};
static NJS_POINT3 core_stream_object_pose_19[8] = {
#include "stg35/machinery_art/core_stream_object_pose_19.inc"
};
static NJS_POINT3 core_stream_object_pose_29[8] = {
#include "stg35/machinery_art/core_stream_object_pose_29.inc"
};
static NJS_POINT3 core_stream_object_pose_39[8] = {
#include "stg35/machinery_art/core_stream_object_pose_39.inc"
};
static NJS_POINT3 core_stream_object_pose_49[8] = {
#include "stg35/machinery_art/core_stream_object_pose_49.inc"
};
static NJS_POINT3 core_stream_object_pose_59[8] = {
#include "stg35/machinery_art/core_stream_object_pose_59.inc"
};
static NJS_MKEY_P core_stream_shape_joint_0_vertices[7] = {
#include "stg35/machinery_art/core_stream_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 core_stream_shape_tracks[1] = {
#include "stg35/machinery_art/core_stream_shape_tracks.inc"
};
NJS_MOTION core_stream_shape[1] = {
#include "stg35/machinery_art/core_stream_shape.inc"
};
static NJS_TEXNAME core_segment_glow_texlist_names[1] = {
#include "stg35/machinery_art/core_segment_glow_texlist_names.inc"
};
NJS_TEXLIST core_segment_glow_texlist = {
#include "stg35/machinery_art/core_segment_glow_texlist.inc"
};
static Sint16 core_segment_glow_polygons[89] = {
#include "stg35/machinery_art/core_segment_glow_polygons.inc"
};
static Sint32 core_segment_glow_vertices[99] = {
#include "stg35/machinery_art/core_segment_glow_vertices.inc"
};
NJS_CNK_MODEL core_segment_glow = {
#include "stg35/machinery_art/core_segment_glow.inc"
};
static NJS_CNK_OBJECT core_segment_glow_model_object = {
#include "stg35/machinery_art/core_segment_glow_model_object.inc"
};
static NJS_MDATA3 core_segment_glow_motion_tracks[1] = {
#include "stg35/machinery_art/core_segment_glow_motion_tracks.inc"
};
static NJS_MOTION core_segment_glow_motion = {
#include "stg35/machinery_art/core_segment_glow_motion.inc"
};
static NJS_MDATA2 core_segment_glow_shape_tracks[1] = {
#include "stg35/machinery_art/core_segment_glow_shape_tracks.inc"
};
static NJS_MOTION core_segment_glow_shape = {
#include "stg35/machinery_art/core_segment_glow_shape.inc"
};
static NJS_TEXNAME core_segment_texlist_names[1] = {
#include "stg35/machinery_art/core_segment_texlist_names.inc"
};
NJS_TEXLIST core_segment_texlist = {
#include "stg35/machinery_art/core_segment_texlist.inc"
};
static Sint16 core_segment_body_polygons[51] = {
#include "stg35/machinery_art/core_segment_body_polygons.inc"
};
static Sint32 core_segment_body_vertices[51] = {
#include "stg35/machinery_art/core_segment_body_vertices.inc"
};
NJS_CNK_MODEL core_segment_body = {
#include "stg35/machinery_art/core_segment_body.inc"
};
static NJS_CNK_OBJECT core_segment_body_model_object = {
#include "stg35/machinery_art/core_segment_body_model_object.inc"
};
