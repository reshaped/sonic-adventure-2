#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_conduit_textures.h"
#include "shared/core_energy_art.h"
#include "stg36/machinery_textures.h"
#include "stg36/route.h"

/* texture names that the machinery's texture lists point at. some are in the
 * texture list of the background's PVM too */

const char tails_folded_pipe_texture_name[16] = {
#include "stg36/machinery_art/tails_folded_pipe_texture_name.inc"
};

const char core_pipe_wave_name[20] = {
#include "stg36/machinery_art/core_pipe_wave_name.inc"
};

const char tails_rotor_blade_name[16] = {
#include "stg36/machinery_art/tails_rotor_blade_name.inc"
};

const char tails_rotor_hub_name[16] = {
#include "stg36/machinery_art/tails_rotor_hub_name.inc"
};

const char core_segment_ring_name[16] = {
#include "stg36/machinery_art/core_segment_ring_name.inc"
};

const char core_segment_light_name[16] = {
#include "stg36/machinery_art/core_segment_light_name.inc"
};

const char core_conduit_skin_name[13] = {
#include "stg36/machinery_art/core_conduit_skin_name.inc"
};

/* STG36.PRS's texture lists, chunk models and motions, from the disc. its D
 * lies between panels.c and lamp_art.c */
static NJS_TEXNAME joint_material_1_names[1] = {
#include "stg36/machinery_art/joint_material_1_names.inc"
};
static NJS_TEXLIST joint_material_1 = {
#include "stg36/machinery_art/joint_material_1.inc"
};
static Sint16 joint_fold_polygons[89] = {
#include "stg36/machinery_art/joint_fold_polygons.inc"
};
static Sint32 joint_fold_vertices[83] = {
#include "stg36/machinery_art/joint_fold_vertices.inc"
};
static NJS_CNK_MODEL joint_fold_model = {
#include "stg36/machinery_art/joint_fold_model.inc"
};
static NJS_CNK_OBJECT joint_fold = {
#include "stg36/machinery_art/joint_fold.inc"
};
static NJS_MKEY_F joint_fold_motion_joint_0_position[89] = {
#include "stg36/machinery_art/joint_fold_motion_joint_0_position.inc"
};
static NJS_MDATA3 joint_fold_motion_tracks[1] = {
#include "stg36/machinery_art/joint_fold_motion_tracks.inc"
};
static NJS_MOTION joint_fold_motion = {
#include "stg36/machinery_art/joint_fold_motion.inc"
};
static NJS_MDATA2 joint_fold_shape_tracks[1] = {
#include "stg36/machinery_art/joint_fold_shape_tracks.inc"
};
static NJS_MOTION joint_fold_shape = {
#include "stg36/machinery_art/joint_fold_shape.inc"
};
static NJS_TEXNAME joint_material_2_names[1] = {
#include "stg36/machinery_art/joint_material_2_names.inc"
};
static NJS_TEXLIST joint_material_2 = {
#include "stg36/machinery_art/joint_material_2.inc"
};
static Sint16 outlet_fold_polygons[115] = {
#include "stg36/machinery_art/outlet_fold_polygons.inc"
};
static Sint32 outlet_fold_vertices[83] = {
#include "stg36/machinery_art/outlet_fold_vertices.inc"
};
static NJS_CNK_MODEL outlet_fold_model = {
#include "stg36/machinery_art/outlet_fold_model.inc"
};
static NJS_CNK_OBJECT outlet_fold = {
#include "stg36/machinery_art/outlet_fold.inc"
};
static NJS_MKEY_F outlet_fold_motion_joint_0_position[76] = {
#include "stg36/machinery_art/outlet_fold_motion_joint_0_position.inc"
};
static NJS_MDATA3 outlet_fold_motion_tracks[1] = {
#include "stg36/machinery_art/outlet_fold_motion_tracks.inc"
};
static NJS_MOTION outlet_fold_motion = {
#include "stg36/machinery_art/outlet_fold_motion.inc"
};
static NJS_MDATA2 outlet_fold_shape_tracks[1] = {
#include "stg36/machinery_art/outlet_fold_shape_tracks.inc"
};
static NJS_MOTION outlet_fold_shape = {
#include "stg36/machinery_art/outlet_fold_shape.inc"
};
static NJS_TEXNAME joint_material_3_names[1] = {
#include "stg36/machinery_art/joint_material_3_names.inc"
};
static NJS_TEXLIST joint_material_3 = {
#include "stg36/machinery_art/joint_material_3.inc"
};
static Sint16 elbow_fold_polygons[115] = {
#include "stg36/machinery_art/elbow_fold_polygons.inc"
};
static Sint32 elbow_fold_vertices[83] = {
#include "stg36/machinery_art/elbow_fold_vertices.inc"
};
static NJS_CNK_MODEL elbow_fold_model = {
#include "stg36/machinery_art/elbow_fold_model.inc"
};
static NJS_CNK_OBJECT elbow_fold = {
#include "stg36/machinery_art/elbow_fold.inc"
};
static NJS_MKEY_F elbow_fold_motion_joint_0_position[76] = {
#include "stg36/machinery_art/elbow_fold_motion_joint_0_position.inc"
};
static NJS_MDATA3 elbow_fold_motion_tracks[1] = {
#include "stg36/machinery_art/elbow_fold_motion_tracks.inc"
};
static NJS_MOTION elbow_fold_motion = {
#include "stg36/machinery_art/elbow_fold_motion.inc"
};
static NJS_MDATA2 elbow_fold_shape_tracks[1] = {
#include "stg36/machinery_art/elbow_fold_shape_tracks.inc"
};
static NJS_MOTION elbow_fold_shape = {
#include "stg36/machinery_art/elbow_fold_shape.inc"
};
static NJS_TEXNAME joint_material_4_names[1] = {
#include "stg36/machinery_art/joint_material_4_names.inc"
};
static NJS_TEXLIST joint_material_4 = {
#include "stg36/machinery_art/joint_material_4.inc"
};
static Sint16 end_joint_fold_polygons[407] = {
#include "stg36/machinery_art/end_joint_fold_polygons.inc"
};
static Sint32 end_joint_fold_vertices[251] = {
#include "stg36/machinery_art/end_joint_fold_vertices.inc"
};
static NJS_CNK_MODEL end_joint_fold_model = {
#include "stg36/machinery_art/end_joint_fold_model.inc"
};
static NJS_CNK_OBJECT end_joint_fold = {
#include "stg36/machinery_art/end_joint_fold.inc"
};
static NJS_MDATA3 end_joint_fold_motion_tracks[1] = {
#include "stg36/machinery_art/end_joint_fold_motion_tracks.inc"
};
static NJS_MOTION end_joint_fold_motion = {
#include "stg36/machinery_art/end_joint_fold_motion.inc"
};
static NJS_POINT3 end_joint_fold_pose_0[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_0.inc"
};
static NJS_POINT3 end_joint_fold_pose_11[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_11.inc"
};
static NJS_POINT3 end_joint_fold_pose_24[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_24.inc"
};
static NJS_POINT3 end_joint_fold_pose_36[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_36.inc"
};
static NJS_POINT3 end_joint_fold_pose_49[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_49.inc"
};
static NJS_POINT3 end_joint_fold_pose_61[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_61.inc"
};
static NJS_POINT3 end_joint_fold_pose_74[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_74.inc"
};
static NJS_POINT3 end_joint_fold_pose_86[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_86.inc"
};
static NJS_POINT3 end_joint_fold_pose_99[62] = {
#include "stg36/machinery_art/end_joint_fold_pose_99.inc"
};
static NJS_MKEY_P end_joint_fold_shape_joint_0_vertices[9] = {
#include "stg36/machinery_art/end_joint_fold_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 end_joint_fold_shape_tracks[1] = {
#include "stg36/machinery_art/end_joint_fold_shape_tracks.inc"
};
static NJS_MOTION end_joint_fold_shape = {
#include "stg36/machinery_art/end_joint_fold_shape.inc"
};
static NJS_TEXNAME joint_material_5_names[1] = {
#include "stg36/machinery_art/joint_material_5_names.inc"
};
static NJS_TEXLIST joint_material_5 = {
#include "stg36/machinery_art/joint_material_5.inc"
};
static Sint16 joint_polygons[39] = {
#include "stg36/machinery_art/joint_polygons.inc"
};
static Sint32 joint_vertices[35] = {
#include "stg36/machinery_art/joint_vertices.inc"
};
static NJS_CNK_MODEL joint_model = {
#include "stg36/machinery_art/joint_model.inc"
};
NJS_CNK_OBJECT tails_joint = {
#include "stg36/machinery_art/tails_joint.inc"
};
static NJS_MDATA3 tails_joint_motion_tracks[1] = {
#include "stg36/machinery_art/tails_joint_motion_tracks.inc"
};
NJS_MOTION tails_joint_motion = {
#include "stg36/machinery_art/tails_joint_motion.inc"
};
static NJS_POINT3 joint_pose_0[8] = {
#include "stg36/machinery_art/joint_pose_0.inc"
};
static NJS_POINT3 joint_pose_11[8] = {
#include "stg36/machinery_art/joint_pose_11.inc"
};
static NJS_POINT3 joint_pose_24[8] = {
#include "stg36/machinery_art/joint_pose_24.inc"
};
static NJS_POINT3 joint_pose_36[8] = {
#include "stg36/machinery_art/joint_pose_36.inc"
};
static NJS_POINT3 joint_pose_49[8] = {
#include "stg36/machinery_art/joint_pose_49.inc"
};
static NJS_POINT3 joint_pose_61[8] = {
#include "stg36/machinery_art/joint_pose_61.inc"
};
static NJS_POINT3 joint_pose_74[8] = {
#include "stg36/machinery_art/joint_pose_74.inc"
};
static NJS_POINT3 joint_pose_86[8] = {
#include "stg36/machinery_art/joint_pose_86.inc"
};
static NJS_POINT3 joint_pose_99[8] = {
#include "stg36/machinery_art/joint_pose_99.inc"
};
static NJS_MKEY_P tails_joint_shape_joint_0_vertices[9] = {
#include "stg36/machinery_art/tails_joint_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 tails_joint_shape_tracks[1] = {
#include "stg36/machinery_art/tails_joint_shape_tracks.inc"
};
NJS_MOTION tails_joint_shape = {
#include "stg36/machinery_art/tails_joint_shape.inc"
};
static NJS_TEXNAME outlet_material_1_names[1] = {
#include "stg36/machinery_art/outlet_material_1_names.inc"
};
static NJS_TEXLIST outlet_material_1 = {
#include "stg36/machinery_art/outlet_material_1.inc"
};
static Sint16 outlet_polygons[39] = {
#include "stg36/machinery_art/outlet_polygons.inc"
};
static Sint32 outlet_vertices[35] = {
#include "stg36/machinery_art/outlet_vertices.inc"
};
static NJS_CNK_MODEL outlet_model = {
#include "stg36/machinery_art/outlet_model.inc"
};
NJS_CNK_OBJECT tails_outlet = {
#include "stg36/machinery_art/tails_outlet.inc"
};
static NJS_MDATA3 tails_outlet_motion_tracks[1] = {
#include "stg36/machinery_art/tails_outlet_motion_tracks.inc"
};
NJS_MOTION tails_outlet_motion = {
#include "stg36/machinery_art/tails_outlet_motion.inc"
};
static NJS_POINT3 outlet_pose_0[8] = {
#include "stg36/machinery_art/outlet_pose_0.inc"
};
static NJS_POINT3 outlet_pose_11[8] = {
#include "stg36/machinery_art/outlet_pose_11.inc"
};
static NJS_POINT3 outlet_pose_24[8] = {
#include "stg36/machinery_art/outlet_pose_24.inc"
};
static NJS_POINT3 outlet_pose_36[8] = {
#include "stg36/machinery_art/outlet_pose_36.inc"
};
static NJS_POINT3 outlet_pose_49[8] = {
#include "stg36/machinery_art/outlet_pose_49.inc"
};
static NJS_POINT3 outlet_pose_61[8] = {
#include "stg36/machinery_art/outlet_pose_61.inc"
};
static NJS_POINT3 outlet_pose_74[8] = {
#include "stg36/machinery_art/outlet_pose_74.inc"
};
static NJS_POINT3 outlet_pose_86[8] = {
#include "stg36/machinery_art/outlet_pose_86.inc"
};
static NJS_POINT3 outlet_pose_99[8] = {
#include "stg36/machinery_art/outlet_pose_99.inc"
};
static NJS_MKEY_P tails_outlet_shape_joint_0_vertices[9] = {
#include "stg36/machinery_art/tails_outlet_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 tails_outlet_shape_tracks[1] = {
#include "stg36/machinery_art/tails_outlet_shape_tracks.inc"
};
NJS_MOTION tails_outlet_shape = {
#include "stg36/machinery_art/tails_outlet_shape.inc"
};
static NJS_TEXNAME elbow_material_1_names[1] = {
#include "stg36/machinery_art/elbow_material_1_names.inc"
};
static NJS_TEXLIST elbow_material_1 = {
#include "stg36/machinery_art/elbow_material_1.inc"
};
static Sint16 elbow_polygons[39] = {
#include "stg36/machinery_art/elbow_polygons.inc"
};
static Sint32 elbow_vertices[35] = {
#include "stg36/machinery_art/elbow_vertices.inc"
};
static NJS_CNK_MODEL elbow_model = {
#include "stg36/machinery_art/elbow_model.inc"
};
NJS_CNK_OBJECT tails_elbow = {
#include "stg36/machinery_art/tails_elbow.inc"
};
static NJS_MDATA3 tails_elbow_motion_tracks[1] = {
#include "stg36/machinery_art/tails_elbow_motion_tracks.inc"
};
NJS_MOTION tails_elbow_motion = {
#include "stg36/machinery_art/tails_elbow_motion.inc"
};
static NJS_POINT3 elbow_pose_0[8] = {
#include "stg36/machinery_art/elbow_pose_0.inc"
};
static NJS_POINT3 elbow_pose_11[8] = {
#include "stg36/machinery_art/elbow_pose_11.inc"
};
static NJS_POINT3 elbow_pose_24[8] = {
#include "stg36/machinery_art/elbow_pose_24.inc"
};
static NJS_POINT3 elbow_pose_36[8] = {
#include "stg36/machinery_art/elbow_pose_36.inc"
};
static NJS_POINT3 elbow_pose_49[8] = {
#include "stg36/machinery_art/elbow_pose_49.inc"
};
static NJS_POINT3 elbow_pose_61[8] = {
#include "stg36/machinery_art/elbow_pose_61.inc"
};
static NJS_POINT3 elbow_pose_74[8] = {
#include "stg36/machinery_art/elbow_pose_74.inc"
};
static NJS_POINT3 elbow_pose_86[8] = {
#include "stg36/machinery_art/elbow_pose_86.inc"
};
static NJS_POINT3 elbow_pose_99[8] = {
#include "stg36/machinery_art/elbow_pose_99.inc"
};
static NJS_MKEY_P tails_elbow_shape_joint_0_vertices[9] = {
#include "stg36/machinery_art/tails_elbow_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 tails_elbow_shape_tracks[1] = {
#include "stg36/machinery_art/tails_elbow_shape_tracks.inc"
};
NJS_MOTION tails_elbow_shape = {
#include "stg36/machinery_art/tails_elbow_shape.inc"
};
static NJS_TEXNAME end_joint_material_1_names[1] = {
#include "stg36/machinery_art/end_joint_material_1_names.inc"
};
static NJS_TEXLIST end_joint_material_1 = {
#include "stg36/machinery_art/end_joint_material_1.inc"
};
static Sint16 end_joint_polygons[51] = {
#include "stg36/machinery_art/end_joint_polygons.inc"
};
static Sint32 end_joint_vertices[51] = {
#include "stg36/machinery_art/end_joint_vertices.inc"
};
static NJS_CNK_MODEL end_joint_model = {
#include "stg36/machinery_art/end_joint_model.inc"
};
NJS_CNK_OBJECT tails_end_joint = {
#include "stg36/machinery_art/tails_end_joint.inc"
};
static NJS_MDATA3 tails_end_joint_motion_tracks[1] = {
#include "stg36/machinery_art/tails_end_joint_motion_tracks.inc"
};
NJS_MOTION tails_end_joint_motion = {
#include "stg36/machinery_art/tails_end_joint_motion.inc"
};
static NJS_POINT3 end_joint_pose_0[12] = {
#include "stg36/machinery_art/end_joint_pose_0.inc"
};
static NJS_POINT3 end_joint_pose_11[12] = {
#include "stg36/machinery_art/end_joint_pose_11.inc"
};
static NJS_POINT3 end_joint_pose_24[12] = {
#include "stg36/machinery_art/end_joint_pose_24.inc"
};
static NJS_POINT3 end_joint_pose_36[12] = {
#include "stg36/machinery_art/end_joint_pose_36.inc"
};
static NJS_POINT3 end_joint_pose_49[12] = {
#include "stg36/machinery_art/end_joint_pose_49.inc"
};
static NJS_POINT3 end_joint_pose_61[12] = {
#include "stg36/machinery_art/end_joint_pose_61.inc"
};
static NJS_POINT3 end_joint_pose_74[12] = {
#include "stg36/machinery_art/end_joint_pose_74.inc"
};
static NJS_POINT3 end_joint_pose_86[12] = {
#include "stg36/machinery_art/end_joint_pose_86.inc"
};
static NJS_POINT3 end_joint_pose_99[12] = {
#include "stg36/machinery_art/end_joint_pose_99.inc"
};
static NJS_MKEY_P tails_end_joint_shape_joint_0_vertices[9] = {
#include "stg36/machinery_art/tails_end_joint_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 tails_end_joint_shape_tracks[1] = {
#include "stg36/machinery_art/tails_end_joint_shape_tracks.inc"
};
NJS_MOTION tails_end_joint_shape = {
#include "stg36/machinery_art/tails_end_joint_shape.inc"
};
static NJS_TEXNAME pipe_texlist_names[1] = {
#include "stg36/machinery_art/pipe_texlist_names.inc"
};
NJS_TEXLIST tails_pipe_texlist = {
#include "stg36/machinery_art/tails_pipe_texlist.inc"
};
static Sint16 pipe_polygons[25] = {
#include "stg36/machinery_art/pipe_polygons.inc"
};
static Sint32 pipe_vertices[19] = {
#include "stg36/machinery_art/pipe_vertices.inc"
};
static NJS_CNK_MODEL pipe_model = {
#include "stg36/machinery_art/pipe_model.inc"
};
NJS_CNK_OBJECT tails_pipe[1] = {
#include "stg36/machinery_art/tails_pipe.inc"
};
static NJS_POINT3 pipe_pose_0[4] = {
#include "stg36/machinery_art/pipe_pose_0.inc"
};
static NJS_POINT3 pipe_pose_19[4] = {
#include "stg36/machinery_art/pipe_pose_19.inc"
};
static NJS_POINT3 pipe_pose_49[4] = {
#include "stg36/machinery_art/pipe_pose_49.inc"
};
static NJS_POINT3 pipe_pose_79[4] = {
#include "stg36/machinery_art/pipe_pose_79.inc"
};
static NJS_POINT3 pipe_pose_99[4] = {
#include "stg36/machinery_art/pipe_pose_99.inc"
};
static NJS_MKEY_P tails_pipe_shape_joint_0_vertices[5] = {
#include "stg36/machinery_art/tails_pipe_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 tails_pipe_shape_tracks[1] = {
#include "stg36/machinery_art/tails_pipe_shape_tracks.inc"
};
NJS_MOTION tails_pipe_shape = {
#include "stg36/machinery_art/tails_pipe_shape.inc"
};
static NJS_MDATA3 tails_pipe_motion_tracks[1] = {
#include "stg36/machinery_art/tails_pipe_motion_tracks.inc"
};
NJS_MOTION tails_pipe_motion[1] = {
#include "stg36/machinery_art/tails_pipe_motion.inc"
};
static NJS_TEXNAME rotor_texlist_names[2] = {
#include "stg36/machinery_art/rotor_texlist_names.inc"
};
NJS_TEXLIST tails_rotor_texlist = {
#include "stg36/machinery_art/tails_rotor_texlist.inc"
};
static Sint16 rotor_joint_3_polygons[805] = {
#include "stg36/machinery_art/rotor_joint_3_polygons.inc"
};
static Sint32 rotor_joint_3_vertices[607] = {
#include "stg36/machinery_art/rotor_joint_3_vertices.inc"
};
static NJS_CNK_MODEL rotor_joint_3_model = {
#include "stg36/machinery_art/rotor_joint_3_model.inc"
};
static NJS_CNK_OBJECT rotor_joint_3 = {
#include "stg36/machinery_art/rotor_joint_3.inc"
};
static Sint16 rotor_joint_2_polygons[805] = {
#include "stg36/machinery_art/rotor_joint_2_polygons.inc"
};
static Sint32 rotor_joint_2_vertices[611] = {
#include "stg36/machinery_art/rotor_joint_2_vertices.inc"
};
static NJS_CNK_MODEL rotor_joint_2_model = {
#include "stg36/machinery_art/rotor_joint_2_model.inc"
};
static NJS_CNK_OBJECT rotor_joint_2 = {
#include "stg36/machinery_art/rotor_joint_2.inc"
};
static Sint16 rotor_joint_1_polygons[43] = {
#include "stg36/machinery_art/rotor_joint_1_polygons.inc"
};
static Sint32 rotor_joint_1_vertices[43] = {
#include "stg36/machinery_art/rotor_joint_1_vertices.inc"
};
static NJS_CNK_MODEL rotor_joint_1_model = {
#include "stg36/machinery_art/rotor_joint_1_model.inc"
};
static NJS_CNK_OBJECT rotor_joint_1 = {
#include "stg36/machinery_art/rotor_joint_1.inc"
};
NJS_CNK_OBJECT tails_rotor = {
#include "stg36/machinery_art/tails_rotor.inc"
};
static NJS_MKEY_F tails_rotor_motion_joint_2_position[149] = {
#include "stg36/machinery_art/tails_rotor_motion_joint_2_position.inc"
};
static NJS_MKEY_A tails_rotor_motion_joint_2_angles[47] = {
#include "stg36/machinery_art/tails_rotor_motion_joint_2_angles.inc"
};
static NJS_MKEY_F tails_rotor_motion_joint_3_position[149] = {
#include "stg36/machinery_art/tails_rotor_motion_joint_3_position.inc"
};
static NJS_MKEY_A tails_rotor_motion_joint_3_angles[47] = {
#include "stg36/machinery_art/tails_rotor_motion_joint_3_angles.inc"
};
static NJS_MDATA3 tails_rotor_motion_tracks[4] = {
#include "stg36/machinery_art/tails_rotor_motion_tracks.inc"
};
NJS_MOTION tails_rotor_motion[1] = {
#include "stg36/machinery_art/tails_rotor_motion.inc"
};
static Sint16 rotor_placements_joint_6_polygons[15] = {
#include "stg36/machinery_art/rotor_placements_joint_6_polygons.inc"
};
static Sint32 rotor_placements_joint_6_vertices[15] = {
#include "stg36/machinery_art/rotor_placements_joint_6_vertices.inc"
};
static NJS_CNK_MODEL rotor_placements_joint_6_model = {
#include "stg36/machinery_art/rotor_placements_joint_6_model.inc"
};
static NJS_CNK_OBJECT rotor_placements_joint_6 = {
#include "stg36/machinery_art/rotor_placements_joint_6.inc"
};
static Sint16 rotor_placements_joint_5_polygons[15] = {
#include "stg36/machinery_art/rotor_placements_joint_5_polygons.inc"
};
static Sint32 rotor_placements_joint_5_vertices[15] = {
#include "stg36/machinery_art/rotor_placements_joint_5_vertices.inc"
};
static NJS_CNK_MODEL rotor_placements_joint_5_model = {
#include "stg36/machinery_art/rotor_placements_joint_5_model.inc"
};
static NJS_CNK_OBJECT rotor_placements_joint_5 = {
#include "stg36/machinery_art/rotor_placements_joint_5.inc"
};
static Sint16 rotor_placements_joint_4_polygons[15] = {
#include "stg36/machinery_art/rotor_placements_joint_4_polygons.inc"
};
static Sint32 rotor_placements_joint_4_vertices[15] = {
#include "stg36/machinery_art/rotor_placements_joint_4_vertices.inc"
};
static NJS_CNK_MODEL rotor_placements_joint_4_model = {
#include "stg36/machinery_art/rotor_placements_joint_4_model.inc"
};
static NJS_CNK_OBJECT rotor_placements_joint_4 = {
#include "stg36/machinery_art/rotor_placements_joint_4.inc"
};
static Sint16 rotor_placements_joint_3_polygons[15] = {
#include "stg36/machinery_art/rotor_placements_joint_3_polygons.inc"
};
static Sint32 rotor_placements_joint_3_vertices[15] = {
#include "stg36/machinery_art/rotor_placements_joint_3_vertices.inc"
};
static NJS_CNK_MODEL rotor_placements_joint_3_model = {
#include "stg36/machinery_art/rotor_placements_joint_3_model.inc"
};
static NJS_CNK_OBJECT rotor_placements_joint_3 = {
#include "stg36/machinery_art/rotor_placements_joint_3.inc"
};
static Sint16 rotor_placements_joint_2_polygons[15] = {
#include "stg36/machinery_art/rotor_placements_joint_2_polygons.inc"
};
static Sint32 rotor_placements_joint_2_vertices[15] = {
#include "stg36/machinery_art/rotor_placements_joint_2_vertices.inc"
};
static NJS_CNK_MODEL rotor_placements_joint_2_model = {
#include "stg36/machinery_art/rotor_placements_joint_2_model.inc"
};
static NJS_CNK_OBJECT rotor_placements_joint_2 = {
#include "stg36/machinery_art/rotor_placements_joint_2.inc"
};
static Sint16 rotor_placements_joint_1_polygons[15] = {
#include "stg36/machinery_art/rotor_placements_joint_1_polygons.inc"
};
static Sint32 rotor_placements_joint_1_vertices[15] = {
#include "stg36/machinery_art/rotor_placements_joint_1_vertices.inc"
};
static NJS_CNK_MODEL rotor_placements_joint_1_model = {
#include "stg36/machinery_art/rotor_placements_joint_1_model.inc"
};
static NJS_CNK_OBJECT rotor_placements_joint_1 = {
#include "stg36/machinery_art/rotor_placements_joint_1.inc"
};
NJS_CNK_OBJECT tails_rotor_placements[1] = {
#include "stg36/machinery_art/tails_rotor_placements.inc"
};
static NJS_TEXNAME core_stream_texlist_names[1] = {
#include "stg36/machinery_art/core_stream_texlist_names.inc"
};
NJS_TEXLIST core_stream_texlist = {
#include "stg36/machinery_art/core_stream_texlist.inc"
};
static Sint16 core_stream_object_polygons[39] = {
#include "stg36/machinery_art/core_stream_object_polygons.inc"
};
static Sint32 core_stream_object_vertices[35] = {
#include "stg36/machinery_art/core_stream_object_vertices.inc"
};
static NJS_CNK_MODEL core_stream_object_model = {
#include "stg36/machinery_art/core_stream_object_model.inc"
};
NJS_CNK_OBJECT core_stream_object[1] = {
#include "stg36/machinery_art/core_stream_object.inc"
};
static NJS_MDATA3 core_stream_motion_tracks[1] = {
#include "stg36/machinery_art/core_stream_motion_tracks.inc"
};
NJS_MOTION core_stream_motion[1] = {
#include "stg36/machinery_art/core_stream_motion.inc"
};
static NJS_POINT3 core_stream_object_pose_0[8] = {
#include "stg36/machinery_art/core_stream_object_pose_0.inc"
};
static NJS_POINT3 core_stream_object_pose_9[8] = {
#include "stg36/machinery_art/core_stream_object_pose_9.inc"
};
static NJS_POINT3 core_stream_object_pose_19[8] = {
#include "stg36/machinery_art/core_stream_object_pose_19.inc"
};
static NJS_POINT3 core_stream_object_pose_29[8] = {
#include "stg36/machinery_art/core_stream_object_pose_29.inc"
};
static NJS_POINT3 core_stream_object_pose_39[8] = {
#include "stg36/machinery_art/core_stream_object_pose_39.inc"
};
static NJS_POINT3 core_stream_object_pose_49[8] = {
#include "stg36/machinery_art/core_stream_object_pose_49.inc"
};
static NJS_POINT3 core_stream_object_pose_59[8] = {
#include "stg36/machinery_art/core_stream_object_pose_59.inc"
};
static NJS_MKEY_P core_stream_shape_joint_0_vertices[7] = {
#include "stg36/machinery_art/core_stream_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 core_stream_shape_tracks[1] = {
#include "stg36/machinery_art/core_stream_shape_tracks.inc"
};
NJS_MOTION core_stream_shape[1] = {
#include "stg36/machinery_art/core_stream_shape.inc"
};
static NJS_TEXNAME core_segment_glow_texlist_names[1] = {
#include "stg36/machinery_art/core_segment_glow_texlist_names.inc"
};
NJS_TEXLIST core_segment_glow_texlist = {
#include "stg36/machinery_art/core_segment_glow_texlist.inc"
};
static Sint16 core_segment_glow_polygons[89] = {
#include "stg36/machinery_art/core_segment_glow_polygons.inc"
};
static Sint32 core_segment_glow_vertices[99] = {
#include "stg36/machinery_art/core_segment_glow_vertices.inc"
};
NJS_CNK_MODEL core_segment_glow = {
#include "stg36/machinery_art/core_segment_glow.inc"
};
static NJS_CNK_OBJECT core_segment_glow_model_object = {
#include "stg36/machinery_art/core_segment_glow_model_object.inc"
};
static NJS_MDATA3 core_segment_glow_motion_tracks[1] = {
#include "stg36/machinery_art/core_segment_glow_motion_tracks.inc"
};
static NJS_MOTION core_segment_glow_motion = {
#include "stg36/machinery_art/core_segment_glow_motion.inc"
};
static NJS_MDATA2 core_segment_glow_shape_tracks[1] = {
#include "stg36/machinery_art/core_segment_glow_shape_tracks.inc"
};
static NJS_MOTION core_segment_glow_shape = {
#include "stg36/machinery_art/core_segment_glow_shape.inc"
};
static NJS_TEXNAME core_segment_texlist_names[1] = {
#include "stg36/machinery_art/core_segment_texlist_names.inc"
};
NJS_TEXLIST core_segment_texlist = {
#include "stg36/machinery_art/core_segment_texlist.inc"
};
static Sint16 core_segment_body_polygons[51] = {
#include "stg36/machinery_art/core_segment_body_polygons.inc"
};
static Sint32 core_segment_body_vertices[51] = {
#include "stg36/machinery_art/core_segment_body_vertices.inc"
};
NJS_CNK_MODEL core_segment_body = {
#include "stg36/machinery_art/core_segment_body.inc"
};
static NJS_CNK_OBJECT core_segment_body_model_object = {
#include "stg36/machinery_art/core_segment_body_model_object.inc"
};
