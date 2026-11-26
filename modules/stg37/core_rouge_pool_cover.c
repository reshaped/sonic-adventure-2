/* STG37.PRS's motions, Basic models, texture lists, chunk models and chunk
 * pose tree, from the disc. its D lies between core_shaft_rotor.c and
 * shaft_inner_wall_models.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_scenery_art.h"
#include "shared/water_shaft/textures.h"
#include "stg37/core_rouge_cover_filenames.h"
#include "stg37/water_cover.h"
#include "stg37/water_materials.h"
#include "stg37/water_ripple_meshes.h"
#include "stg37/water_scenery.h"

struct cover_ripple_a_first_pose {
    Sint16 polygons[25];
    Sint32 vertices[19];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct cover_ripple_a_second_pose {
    Sint16 polygons[31];
    Sint32 vertices[27];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct cover_ripple_a_third_pose {
    Sint16 polygons[25];
    Sint32 vertices[19];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct cover_ripple_a_tree {
    struct cover_ripple_a_first_pose first[16];
    struct cover_ripple_a_second_pose second[29];
    struct cover_ripple_a_third_pose third[15];
    NJS_CNK_OBJECT root;
};

struct cover_ripple_b_first_pose {
    Sint16 polygons[25];
    Sint32 vertices[19];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct cover_ripple_b_second_pose {
    Sint16 polygons[31];
    Sint32 vertices[27];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct cover_ripple_b_third_pose {
    Sint16 polygons[25];
    Sint32 vertices[19];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct cover_ripple_b_tree {
    struct cover_ripple_b_first_pose first[16];
    struct cover_ripple_b_second_pose second[29];
    struct cover_ripple_b_third_pose third[15];
    NJS_CNK_OBJECT root;
};

static NJS_MKEY_F shaft_rotor_motion_joint_0_position[2] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_0_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_0_angles[2] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_0_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_1_position[90] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_1_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_1_angles[48] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_1_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_2_position[134] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_2_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_2_angles[2] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_2_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_3_position[135] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_3_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_3_angles[32] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_3_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_4_position[134] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_4_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_4_angles[2] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_4_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_5_position[90] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_5_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_5_angles[48] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_5_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_6_position[93] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_6_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_6_angles[2] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_6_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_7_position[159] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_7_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_7_angles[2] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_joint_7_angles.inc"
};
static NJS_MDATA3 shaft_rotor_motion_tracks[8] = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion_tracks.inc"
};
NJS_MOTION shaft_rotor_motion = {
#include "stg37/core_rouge_pool_cover/shaft_rotor_motion.inc"
};
static NJS_MATERIAL cover_closed_rim_solid_materials[1] = {
#include "stg37/core_rouge_pool_cover/cover_closed_rim_solid_materials.inc"
};
static Sint16 cover_closed_rim_solid_strips[211] = {
#include "stg37/core_rouge_pool_cover/cover_closed_rim_solid_strips.inc"
};
static NJS_MESHSET cover_closed_rim_solid_faces[1] = {
#include "stg37/core_rouge_pool_cover/cover_closed_rim_solid_faces.inc"
};
static NJS_POINT3 cover_closed_rim_solid_points[97] = {
#include "stg37/core_rouge_pool_cover/cover_closed_rim_solid_points.inc"
};
static NJS_POINT3 cover_closed_rim_solid_normals[97] = {
#include "stg37/core_rouge_pool_cover/cover_closed_rim_solid_normals.inc"
};
static NJS_MODEL cover_closed_rim_solid_model = {
#include "stg37/core_rouge_pool_cover/cover_closed_rim_solid_model.inc"
};
NJS_OBJECT cover_closed_rim_collision = {
#include "stg37/core_rouge_pool_cover/cover_closed_rim_collision.inc"
};
static NJS_MATERIAL cover_open_rim_solid_materials[1] = {
#include "stg37/core_rouge_pool_cover/cover_open_rim_solid_materials.inc"
};
static Sint16 cover_open_rim_solid_strips[379] = {
#include "stg37/core_rouge_pool_cover/cover_open_rim_solid_strips.inc"
};
static NJS_MESHSET cover_open_rim_solid_faces[1] = {
#include "stg37/core_rouge_pool_cover/cover_open_rim_solid_faces.inc"
};
static NJS_POINT3 cover_open_rim_solid_points[144] = {
#include "stg37/core_rouge_pool_cover/cover_open_rim_solid_points.inc"
};
static NJS_POINT3 cover_open_rim_solid_normals[144] = {
#include "stg37/core_rouge_pool_cover/cover_open_rim_solid_normals.inc"
};
static NJS_MODEL cover_open_rim_solid_model = {
#include "stg37/core_rouge_pool_cover/cover_open_rim_solid_model.inc"
};
NJS_OBJECT cover_open_rim_collision = {
#include "stg37/core_rouge_pool_cover/cover_open_rim_collision.inc"
};
static NJS_MATERIAL cover_lid_solid_materials[1] = {
#include "stg37/core_rouge_pool_cover/cover_lid_solid_materials.inc"
};
static Sint16 cover_lid_solid_strips[19] = {
#include "stg37/core_rouge_pool_cover/cover_lid_solid_strips.inc"
};
static NJS_MESHSET cover_lid_solid_faces[1] = {
#include "stg37/core_rouge_pool_cover/cover_lid_solid_faces.inc"
};
static NJS_POINT3 cover_lid_solid_points[16] = {
#include "stg37/core_rouge_pool_cover/cover_lid_solid_points.inc"
};
static NJS_POINT3 cover_lid_solid_normals[16] = {
#include "stg37/core_rouge_pool_cover/cover_lid_solid_normals.inc"
};
static NJS_MODEL cover_lid_solid_model = {
#include "stg37/core_rouge_pool_cover/cover_lid_solid_model.inc"
};
NJS_OBJECT cover_lid_collision = {
#include "stg37/core_rouge_pool_cover/cover_lid_collision.inc"
};
static NJS_TEXNAME cover_closed_names[3] = {
#include "stg37/core_rouge_pool_cover/cover_closed_names.inc"
};
NJS_TEXLIST cover_closed_textures = {
#include "stg37/core_rouge_pool_cover/cover_closed_textures.inc"
};
static Sint16 cover_closed_object_polygons[2007] = {
#include "stg37/core_rouge_pool_cover/cover_closed_object_polygons.inc"
};
static Sint32 cover_closed_object_vertices[1775] = {
#include "stg37/core_rouge_pool_cover/cover_closed_object_vertices.inc"
};
static NJS_CNK_MODEL cover_closed_object_model = {
#include "stg37/core_rouge_pool_cover/cover_closed_object_model.inc"
};
NJS_CNK_OBJECT cover_closed_object = {
#include "stg37/core_rouge_pool_cover/cover_closed_object.inc"
};
static NJS_TEXNAME cover_open_names[2] = {
#include "stg37/core_rouge_pool_cover/cover_open_names.inc"
};
NJS_TEXLIST cover_open_textures = {
#include "stg37/core_rouge_pool_cover/cover_open_textures.inc"
};
static Sint16 cover_open_object_polygons[2567] = {
#include "stg37/core_rouge_pool_cover/cover_open_object_polygons.inc"
};
static Sint32 cover_open_object_vertices[2707] = {
#include "stg37/core_rouge_pool_cover/cover_open_object_vertices.inc"
};
static NJS_CNK_MODEL cover_open_object_model = {
#include "stg37/core_rouge_pool_cover/cover_open_object_model.inc"
};
NJS_CNK_OBJECT cover_open_object = {
#include "stg37/core_rouge_pool_cover/cover_open_object.inc"
};
static NJS_TEXNAME cover_ripple_names[1] = {
#include "stg37/core_rouge_pool_cover/cover_ripple_names.inc"
};
NJS_TEXLIST cover_ripple_textures = {
#include "stg37/core_rouge_pool_cover/cover_ripple_textures.inc"
};
struct cover_ripple_a_bank cover_ripple_a = {
#include "stg37/core_rouge_pool_cover/cover_ripple_a.inc"
};
static NJS_TEXNAME cover_ripple_b_names[1] = {
#include "stg37/core_rouge_pool_cover/cover_ripple_b_names.inc"
};
NJS_TEXLIST cover_ripple_textures_b = {
#include "stg37/core_rouge_pool_cover/cover_ripple_textures_b.inc"
};
struct cover_ripple_b_bank cover_ripple_b = {
#include "stg37/core_rouge_pool_cover/cover_ripple_b.inc"
};
static NJS_TEXNAME poison_surface_part_2_textures_slots[1] = {
#include "stg37/core_rouge_pool_cover/poison_surface_part_2_textures_slots.inc"
};
static NJS_TEXLIST poison_surface_part_2_textures = {
#include "stg37/core_rouge_pool_cover/poison_surface_part_2_textures.inc"
};
static Sint16 poison_surface_panel_2_polygons[37] = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_2_polygons.inc"
};
static Sint32 poison_surface_panel_2_vertices[35] = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_2_vertices.inc"
};
static NJS_CNK_MODEL poison_surface_panel_2_mesh = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT poison_surface_panel_2 = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_2.inc"
};
static Sint16 poison_surface_panel_1_polygons[37] = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_1_polygons.inc"
};
static Sint32 poison_surface_panel_1_vertices[35] = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_1_vertices.inc"
};
static NJS_CNK_MODEL poison_surface_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT poison_surface_panel_1 = {
#include "stg37/core_rouge_pool_cover/poison_surface_panel_1.inc"
};
NJS_CNK_OBJECT poison_surface = {
#include "stg37/core_rouge_pool_cover/poison_surface.inc"
};
static NJS_TEXNAME poison_wall_marker_textures_slots[1] = {
#include "stg37/core_rouge_pool_cover/poison_wall_marker_textures_slots.inc"
};
static NJS_TEXLIST poison_wall_marker_textures = {
#include "stg37/core_rouge_pool_cover/poison_wall_marker_textures.inc"
};
static Sint16 poison_wall_panel_4_polygons[187] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_4_polygons.inc"
};
static Sint32 poison_wall_panel_4_vertices[155] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_4_vertices.inc"
};
static NJS_CNK_MODEL poison_wall_panel_4_mesh = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT poison_wall_panel_4 = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_4.inc"
};
static Sint16 poison_wall_panel_3_polygons[187] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_3_polygons.inc"
};
static Sint32 poison_wall_panel_3_vertices[155] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_3_vertices.inc"
};
static NJS_CNK_MODEL poison_wall_panel_3_mesh = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT poison_wall_panel_3 = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_3.inc"
};
static Sint16 poison_wall_panel_2_polygons[187] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_2_polygons.inc"
};
static Sint32 poison_wall_panel_2_vertices[155] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL poison_wall_panel_2_mesh = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT poison_wall_panel_2 = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_2.inc"
};
static Sint16 poison_wall_panel_1_polygons[187] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_1_polygons.inc"
};
static Sint32 poison_wall_panel_1_vertices[155] = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL poison_wall_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT poison_wall_panel_1 = {
#include "stg37/core_rouge_pool_cover/poison_wall_panel_1.inc"
};
NJS_CNK_OBJECT poison_walls = {
#include "stg37/core_rouge_pool_cover/poison_walls.inc"
};
static NJS_TEXNAME poison_wall_textures_slots[1] = {
#include "stg37/core_rouge_pool_cover/poison_wall_textures_slots.inc"
};
NJS_TEXLIST poison_wall_textures = {
#include "stg37/core_rouge_pool_cover/poison_wall_textures.inc"
};
static Sint16 poison_wall_model_object_polygons[169] = {
#include "stg37/core_rouge_pool_cover/poison_wall_model_object_polygons.inc"
};
static Sint32 poison_wall_model_object_vertices[123] = {
#include "stg37/core_rouge_pool_cover/poison_wall_model_object_vertices.inc"
};
NJS_CNK_MODEL poison_wall_model = {
#include "stg37/core_rouge_pool_cover/poison_wall_model.inc"
};
static NJS_CNK_OBJECT poison_wall_model_object = {
#include "stg37/core_rouge_pool_cover/poison_wall_model_object.inc"
};
static Sint16 inner_wall_entry_panel_3_polygons[41] = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_3_polygons.inc"
};
static Sint32 inner_wall_entry_panel_3_vertices[67] = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_3_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_entry_panel_3_mesh = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_entry_panel_3 = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_3.inc"
};
static Sint16 inner_wall_entry_panel_2_polygons[41] = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_2_polygons.inc"
};
static Sint32 inner_wall_entry_panel_2_vertices[67] = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_2_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_entry_panel_2_mesh = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_entry_panel_2 = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_2.inc"
};
static Sint16 inner_wall_entry_panel_1_polygons[41] = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_1_polygons.inc"
};
static Sint32 inner_wall_entry_panel_1_vertices[67] = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_entry_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_entry_panel_1 = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry_panel_1.inc"
};
NJS_CNK_OBJECT inner_wall_entry = {
#include "stg37/core_rouge_pool_cover/inner_wall_entry.inc"
};
static Sint16 inner_wall_shaft_panel_1_polygons[41] = {
#include "stg37/core_rouge_pool_cover/inner_wall_shaft_panel_1_polygons.inc"
};
static Sint32 inner_wall_shaft_panel_1_vertices[67] = {
#include "stg37/core_rouge_pool_cover/inner_wall_shaft_panel_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_shaft_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/inner_wall_shaft_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_shaft_panel_1 = {
#include "stg37/core_rouge_pool_cover/inner_wall_shaft_panel_1.inc"
};
NJS_CNK_OBJECT inner_wall_shaft = {
#include "stg37/core_rouge_pool_cover/inner_wall_shaft.inc"
};
static Sint16 middle_wall_shaft_panel_2_polygons[41] = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_2_polygons.inc"
};
static Sint32 middle_wall_shaft_panel_2_vertices[67] = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_2_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_shaft_panel_2_mesh = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_shaft_panel_2 = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_2.inc"
};
static Sint16 middle_wall_shaft_panel_1_polygons[41] = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_1_polygons.inc"
};
static Sint32 middle_wall_shaft_panel_1_vertices[67] = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_shaft_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_shaft_panel_1 = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft_panel_1.inc"
};
NJS_CNK_OBJECT middle_wall_shaft = {
#include "stg37/core_rouge_pool_cover/middle_wall_shaft.inc"
};
static Sint16 middle_wall_lower_panel_1_polygons[41] = {
#include "stg37/core_rouge_pool_cover/middle_wall_lower_panel_1_polygons.inc"
};
static Sint32 middle_wall_lower_panel_1_vertices[67] = {
#include "stg37/core_rouge_pool_cover/middle_wall_lower_panel_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_lower_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/middle_wall_lower_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_lower_panel_1 = {
#include "stg37/core_rouge_pool_cover/middle_wall_lower_panel_1.inc"
};
NJS_CNK_OBJECT middle_wall_lower = {
#include "stg37/core_rouge_pool_cover/middle_wall_lower.inc"
};
static Sint16 outer_wall_shaft_panel_2_polygons[41] = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_2_polygons.inc"
};
static Sint32 outer_wall_shaft_panel_2_vertices[67] = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_2_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_shaft_panel_2_mesh = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_shaft_panel_2 = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_2.inc"
};
static Sint16 outer_wall_shaft_panel_1_polygons[41] = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_1_polygons.inc"
};
static Sint32 outer_wall_shaft_panel_1_vertices[67] = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_shaft_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_shaft_panel_1 = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft_panel_1.inc"
};
NJS_CNK_OBJECT outer_wall_shaft = {
#include "stg37/core_rouge_pool_cover/outer_wall_shaft.inc"
};
static Sint16 outer_wall_lower_panel_1_polygons[41] = {
#include "stg37/core_rouge_pool_cover/outer_wall_lower_panel_1_polygons.inc"
};
static Sint32 outer_wall_lower_panel_1_vertices[67] = {
#include "stg37/core_rouge_pool_cover/outer_wall_lower_panel_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_lower_panel_1_mesh = {
#include "stg37/core_rouge_pool_cover/outer_wall_lower_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_lower_panel_1 = {
#include "stg37/core_rouge_pool_cover/outer_wall_lower_panel_1.inc"
};
NJS_CNK_OBJECT outer_wall_lower = {
#include "stg37/core_rouge_pool_cover/outer_wall_lower.inc"
};
