/* CHAOSTGKINDER.PRS's texture lists, Basic models, chunk models and land,
 * from the disc. its D lies between lesson.c and dat_0ca8c350.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/land_data.h"
#include "chaostgkinder/land_textures.h"
#include "collision/world.h"

static NJS_TEXNAME house_textures[43] = {
#include "chaostgkinder/terrain/house_textures.inc"
};
NJS_TEXLIST kinder_house_texlist = {
#include "chaostgkinder/terrain/kinder_house_texlist.inc"
};
static NJS_MATERIAL house_collision_0_materials[1] = {
#include "chaostgkinder/terrain/house_collision_0_materials.inc"
};
static Sint16 house_collision_0_faces[5] = {
#include "chaostgkinder/terrain/house_collision_0_faces.inc"
};
static NJS_MESHSET house_collision_0_meshsets[1] = {
#include "chaostgkinder/terrain/house_collision_0_meshsets.inc"
};
static NJS_POINT3 house_collision_0_points[4] = {
#include "chaostgkinder/terrain/house_collision_0_points.inc"
};
static NJS_MODEL house_porch_collision_model = {
#include "chaostgkinder/terrain/house_porch_collision_model.inc"
};
static NJS_OBJECT house_porch_collision = {
#include "chaostgkinder/terrain/house_porch_collision.inc"
};
static NJS_MATERIAL house_collision_1_materials[1] = {
#include "chaostgkinder/terrain/house_collision_1_materials.inc"
};
static Sint16 house_collision_1_faces[15] = {
#include "chaostgkinder/terrain/house_collision_1_faces.inc"
};
static NJS_MESHSET house_collision_1_meshsets[1] = {
#include "chaostgkinder/terrain/house_collision_1_meshsets.inc"
};
static NJS_POINT3 house_collision_1_points[14] = {
#include "chaostgkinder/terrain/house_collision_1_points.inc"
};
static NJS_MODEL house_path_collision_model = {
#include "chaostgkinder/terrain/house_path_collision_model.inc"
};
static NJS_OBJECT house_path_collision = {
#include "chaostgkinder/terrain/house_path_collision.inc"
};
static NJS_MATERIAL house_collision_2_materials[1] = {
#include "chaostgkinder/terrain/house_collision_2_materials.inc"
};
static Sint16 house_collision_2_faces[7] = {
#include "chaostgkinder/terrain/house_collision_2_faces.inc"
};
static NJS_MESHSET house_collision_2_meshsets[1] = {
#include "chaostgkinder/terrain/house_collision_2_meshsets.inc"
};
static NJS_POINT3 house_collision_2_points[6] = {
#include "chaostgkinder/terrain/house_collision_2_points.inc"
};
static NJS_MODEL house_steps_collision_model = {
#include "chaostgkinder/terrain/house_steps_collision_model.inc"
};
static NJS_OBJECT house_steps_collision = {
#include "chaostgkinder/terrain/house_steps_collision.inc"
};
static Sint16 house_roof_polygons[409] = {
#include "chaostgkinder/terrain/house_roof_polygons.inc"
};
static Sint32 house_roof_vertices[431] = {
#include "chaostgkinder/terrain/house_roof_vertices.inc"
};
static NJS_CNK_MODEL house_roof_model = {
#include "chaostgkinder/terrain/house_roof_model.inc"
};
static NJS_CNK_OBJECT house_roof = {
#include "chaostgkinder/terrain/house_roof.inc"
};
static Sint16 house_chimney_polygons[415] = {
#include "chaostgkinder/terrain/house_chimney_polygons.inc"
};
static Sint32 house_chimney_vertices[403] = {
#include "chaostgkinder/terrain/house_chimney_vertices.inc"
};
static NJS_CNK_MODEL house_chimney_model = {
#include "chaostgkinder/terrain/house_chimney_model.inc"
};
static NJS_CNK_OBJECT house_chimney = {
#include "chaostgkinder/terrain/house_chimney.inc"
};
static Sint16 house_lawn_polygons[235] = {
#include "chaostgkinder/terrain/house_lawn_polygons.inc"
};
static Sint32 house_lawn_vertices[171] = {
#include "chaostgkinder/terrain/house_lawn_vertices.inc"
};
static NJS_CNK_MODEL house_lawn_model = {
#include "chaostgkinder/terrain/house_lawn_model.inc"
};
static NJS_CNK_OBJECT house_lawn = {
#include "chaostgkinder/terrain/house_lawn.inc"
};
static Sint16 house_mountain_backdrop_polygons[353] = {
#include "chaostgkinder/terrain/house_mountain_backdrop_polygons.inc"
};
static Sint32 house_mountain_backdrop_vertices[243] = {
#include "chaostgkinder/terrain/house_mountain_backdrop_vertices.inc"
};
static NJS_CNK_MODEL house_mountain_backdrop_model = {
#include "chaostgkinder/terrain/house_mountain_backdrop_model.inc"
};
static NJS_CNK_OBJECT house_mountain_backdrop = {
#include "chaostgkinder/terrain/house_mountain_backdrop.inc"
};
static Sint16 house_foundation_polygons[381] = {
#include "chaostgkinder/terrain/house_foundation_polygons.inc"
};
static Sint32 house_foundation_vertices[307] = {
#include "chaostgkinder/terrain/house_foundation_vertices.inc"
};
static NJS_CNK_MODEL house_foundation_model = {
#include "chaostgkinder/terrain/house_foundation_model.inc"
};
static NJS_CNK_OBJECT house_foundation = {
#include "chaostgkinder/terrain/house_foundation.inc"
};
static Sint16 house_left_lamp_polygons[213] = {
#include "chaostgkinder/terrain/house_left_lamp_polygons.inc"
};
static Sint32 house_left_lamp_vertices[215] = {
#include "chaostgkinder/terrain/house_left_lamp_vertices.inc"
};
static NJS_CNK_MODEL house_left_lamp_model = {
#include "chaostgkinder/terrain/house_left_lamp_model.inc"
};
static NJS_CNK_OBJECT house_left_lamp = {
#include "chaostgkinder/terrain/house_left_lamp.inc"
};
static Sint16 house_right_lamp_polygons[213] = {
#include "chaostgkinder/terrain/house_right_lamp_polygons.inc"
};
static Sint32 house_right_lamp_vertices[215] = {
#include "chaostgkinder/terrain/house_right_lamp_vertices.inc"
};
static NJS_CNK_MODEL house_right_lamp_model = {
#include "chaostgkinder/terrain/house_right_lamp_model.inc"
};
static NJS_CNK_OBJECT house_right_lamp = {
#include "chaostgkinder/terrain/house_right_lamp.inc"
};
static Sint16 house_left_upper_glass_polygons[19] = {
#include "chaostgkinder/terrain/house_left_upper_glass_polygons.inc"
};
static Sint32 house_left_upper_glass_vertices[27] = {
#include "chaostgkinder/terrain/house_left_upper_glass_vertices.inc"
};
static NJS_CNK_MODEL house_left_upper_glass_model = {
#include "chaostgkinder/terrain/house_left_upper_glass_model.inc"
};
static NJS_CNK_OBJECT house_left_upper_glass = {
#include "chaostgkinder/terrain/house_left_upper_glass.inc"
};
static Sint16 house_right_upper_glass_polygons[19] = {
#include "chaostgkinder/terrain/house_right_upper_glass_polygons.inc"
};
static Sint32 house_right_upper_glass_vertices[27] = {
#include "chaostgkinder/terrain/house_right_upper_glass_vertices.inc"
};
static NJS_CNK_MODEL house_right_upper_glass_model = {
#include "chaostgkinder/terrain/house_right_upper_glass_model.inc"
};
static NJS_CNK_OBJECT house_right_upper_glass = {
#include "chaostgkinder/terrain/house_right_upper_glass.inc"
};
static Sint16 house_left_upper_opening_polygons[381] = {
#include "chaostgkinder/terrain/house_left_upper_opening_polygons.inc"
};
static Sint32 house_left_upper_opening_vertices[387] = {
#include "chaostgkinder/terrain/house_left_upper_opening_vertices.inc"
};
static NJS_CNK_MODEL house_left_upper_opening_model = {
#include "chaostgkinder/terrain/house_left_upper_opening_model.inc"
};
static NJS_CNK_OBJECT house_left_upper_opening = {
#include "chaostgkinder/terrain/house_left_upper_opening.inc"
};
static Sint16 house_right_upper_opening_polygons[381] = {
#include "chaostgkinder/terrain/house_right_upper_opening_polygons.inc"
};
static Sint32 house_right_upper_opening_vertices[387] = {
#include "chaostgkinder/terrain/house_right_upper_opening_vertices.inc"
};
static NJS_CNK_MODEL house_right_upper_opening_model = {
#include "chaostgkinder/terrain/house_right_upper_opening_model.inc"
};
static NJS_CNK_OBJECT house_right_upper_opening = {
#include "chaostgkinder/terrain/house_right_upper_opening.inc"
};
static Sint16 house_left_porch_pillar_polygons[501] = {
#include "chaostgkinder/terrain/house_left_porch_pillar_polygons.inc"
};
static Sint32 house_left_porch_pillar_vertices[327] = {
#include "chaostgkinder/terrain/house_left_porch_pillar_vertices.inc"
};
static NJS_CNK_MODEL house_left_porch_pillar_model = {
#include "chaostgkinder/terrain/house_left_porch_pillar_model.inc"
};
static NJS_CNK_OBJECT house_left_porch_pillar = {
#include "chaostgkinder/terrain/house_left_porch_pillar.inc"
};
static Sint16 house_right_porch_pillar_polygons[479] = {
#include "chaostgkinder/terrain/house_right_porch_pillar_polygons.inc"
};
static Sint32 house_right_porch_pillar_vertices[327] = {
#include "chaostgkinder/terrain/house_right_porch_pillar_vertices.inc"
};
static NJS_CNK_MODEL house_right_porch_pillar_model = {
#include "chaostgkinder/terrain/house_right_porch_pillar_model.inc"
};
static NJS_CNK_OBJECT house_right_porch_pillar = {
#include "chaostgkinder/terrain/house_right_porch_pillar.inc"
};
static Sint16 house_porch_shadow_polygons[39] = {
#include "chaostgkinder/terrain/house_porch_shadow_polygons.inc"
};
static Sint32 house_porch_shadow_vertices[35] = {
#include "chaostgkinder/terrain/house_porch_shadow_vertices.inc"
};
static NJS_CNK_MODEL house_porch_shadow_model = {
#include "chaostgkinder/terrain/house_porch_shadow_model.inc"
};
static NJS_CNK_OBJECT house_porch_shadow = {
#include "chaostgkinder/terrain/house_porch_shadow.inc"
};
static Sint16 house_left_outer_pillar_polygons[865] = {
#include "chaostgkinder/terrain/house_left_outer_pillar_polygons.inc"
};
static Sint32 house_left_outer_pillar_vertices[615] = {
#include "chaostgkinder/terrain/house_left_outer_pillar_vertices.inc"
};
static NJS_CNK_MODEL house_left_outer_pillar_model = {
#include "chaostgkinder/terrain/house_left_outer_pillar_model.inc"
};
static NJS_CNK_OBJECT house_left_outer_pillar = {
#include "chaostgkinder/terrain/house_left_outer_pillar.inc"
};
static Sint16 house_right_outer_pillar_polygons[865] = {
#include "chaostgkinder/terrain/house_right_outer_pillar_polygons.inc"
};
static Sint32 house_right_outer_pillar_vertices[615] = {
#include "chaostgkinder/terrain/house_right_outer_pillar_vertices.inc"
};
static NJS_CNK_MODEL house_right_outer_pillar_model = {
#include "chaostgkinder/terrain/house_right_outer_pillar_model.inc"
};
static NJS_CNK_OBJECT house_right_outer_pillar = {
#include "chaostgkinder/terrain/house_right_outer_pillar.inc"
};
static Sint16 house_flower_planters_polygons[807] = {
#include "chaostgkinder/terrain/house_flower_planters_polygons.inc"
};
static Sint32 house_flower_planters_vertices[963] = {
#include "chaostgkinder/terrain/house_flower_planters_vertices.inc"
};
static NJS_CNK_MODEL house_flower_planters_model = {
#include "chaostgkinder/terrain/house_flower_planters_model.inc"
};
static NJS_CNK_OBJECT house_flower_planters = {
#include "chaostgkinder/terrain/house_flower_planters.inc"
};
static Sint16 house_left_roof_wing_polygons[449] = {
#include "chaostgkinder/terrain/house_left_roof_wing_polygons.inc"
};
static Sint32 house_left_roof_wing_vertices[415] = {
#include "chaostgkinder/terrain/house_left_roof_wing_vertices.inc"
};
static NJS_CNK_MODEL house_left_roof_wing_model = {
#include "chaostgkinder/terrain/house_left_roof_wing_model.inc"
};
static NJS_CNK_OBJECT house_left_roof_wing = {
#include "chaostgkinder/terrain/house_left_roof_wing.inc"
};
static Sint16 house_right_roof_wing_polygons[449] = {
#include "chaostgkinder/terrain/house_right_roof_wing_polygons.inc"
};
static Sint32 house_right_roof_wing_vertices[415] = {
#include "chaostgkinder/terrain/house_right_roof_wing_vertices.inc"
};
static NJS_CNK_MODEL house_right_roof_wing_model = {
#include "chaostgkinder/terrain/house_right_roof_wing_model.inc"
};
static NJS_CNK_OBJECT house_right_roof_wing = {
#include "chaostgkinder/terrain/house_right_roof_wing.inc"
};
static Sint16 house_entrance_sign_polygons[461] = {
#include "chaostgkinder/terrain/house_entrance_sign_polygons.inc"
};
static Sint32 house_entrance_sign_vertices[411] = {
#include "chaostgkinder/terrain/house_entrance_sign_vertices.inc"
};
static NJS_CNK_MODEL house_entrance_sign_model = {
#include "chaostgkinder/terrain/house_entrance_sign_model.inc"
};
static NJS_CNK_OBJECT house_entrance_sign = {
#include "chaostgkinder/terrain/house_entrance_sign.inc"
};
static Sint16 house_rear_wall_polygons[55] = {
#include "chaostgkinder/terrain/house_rear_wall_polygons.inc"
};
static Sint32 house_rear_wall_vertices[43] = {
#include "chaostgkinder/terrain/house_rear_wall_vertices.inc"
};
static NJS_CNK_MODEL house_rear_wall_model = {
#include "chaostgkinder/terrain/house_rear_wall_model.inc"
};
static NJS_CNK_OBJECT house_rear_wall = {
#include "chaostgkinder/terrain/house_rear_wall.inc"
};
static Sint16 house_footpath_polygons[133] = {
#include "chaostgkinder/terrain/house_footpath_polygons.inc"
};
static Sint32 house_footpath_vertices[147] = {
#include "chaostgkinder/terrain/house_footpath_vertices.inc"
};
static NJS_CNK_MODEL house_footpath_model = {
#include "chaostgkinder/terrain/house_footpath_model.inc"
};
static NJS_CNK_OBJECT house_footpath = {
#include "chaostgkinder/terrain/house_footpath.inc"
};
static Sint16 house_sky_polygons[43] = {
#include "chaostgkinder/terrain/house_sky_polygons.inc"
};
static Sint32 house_sky_vertices[27] = {
#include "chaostgkinder/terrain/house_sky_vertices.inc"
};
static NJS_CNK_MODEL house_sky_model = {
#include "chaostgkinder/terrain/house_sky_model.inc"
};
static NJS_CNK_OBJECT house_sky = {
#include "chaostgkinder/terrain/house_sky.inc"
};
static Sint16 house_left_path_shrub_1_polygons[283] = {
#include "chaostgkinder/terrain/house_left_path_shrub_1_polygons.inc"
};
static Sint32 house_left_path_shrub_1_vertices[159] = {
#include "chaostgkinder/terrain/house_left_path_shrub_1_vertices.inc"
};
static NJS_CNK_MODEL house_left_path_shrub_1_model = {
#include "chaostgkinder/terrain/house_left_path_shrub_1_model.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_1 = {
#include "chaostgkinder/terrain/house_left_path_shrub_1.inc"
};
static Sint16 house_right_path_shrub_1_polygons[283] = {
#include "chaostgkinder/terrain/house_right_path_shrub_1_polygons.inc"
};
static Sint32 house_right_path_shrub_1_vertices[159] = {
#include "chaostgkinder/terrain/house_right_path_shrub_1_vertices.inc"
};
static NJS_CNK_MODEL house_right_path_shrub_1_model = {
#include "chaostgkinder/terrain/house_right_path_shrub_1_model.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_1 = {
#include "chaostgkinder/terrain/house_right_path_shrub_1.inc"
};
static Sint16 house_walls_polygons[919] = {
#include "chaostgkinder/terrain/house_walls_polygons.inc"
};
static Sint32 house_walls_vertices[575] = {
#include "chaostgkinder/terrain/house_walls_vertices.inc"
};
static NJS_CNK_MODEL house_walls_model = {
#include "chaostgkinder/terrain/house_walls_model.inc"
};
static NJS_CNK_OBJECT house_walls = {
#include "chaostgkinder/terrain/house_walls.inc"
};
static Sint16 house_outer_right_window_glass_polygons[19] = {
#include "chaostgkinder/terrain/house_outer_right_window_glass_polygons.inc"
};
static Sint32 house_outer_right_window_glass_vertices[27] = {
#include "chaostgkinder/terrain/house_outer_right_window_glass_vertices.inc"
};
static NJS_CNK_MODEL house_outer_right_window_glass_model = {
#include "chaostgkinder/terrain/house_outer_right_window_glass_model.inc"
};
static NJS_CNK_OBJECT house_outer_right_window_glass = {
#include "chaostgkinder/terrain/house_outer_right_window_glass.inc"
};
static Sint16 house_inner_left_window_frame_polygons[553] = {
#include "chaostgkinder/terrain/house_inner_left_window_frame_polygons.inc"
};
static Sint32 house_inner_left_window_frame_vertices[619] = {
#include "chaostgkinder/terrain/house_inner_left_window_frame_vertices.inc"
};
static NJS_CNK_MODEL house_inner_left_window_frame_model = {
#include "chaostgkinder/terrain/house_inner_left_window_frame_model.inc"
};
static NJS_CNK_OBJECT house_inner_left_window_frame = {
#include "chaostgkinder/terrain/house_inner_left_window_frame.inc"
};
static Sint16 house_inner_right_window_frame_polygons[553] = {
#include "chaostgkinder/terrain/house_inner_right_window_frame_polygons.inc"
};
static Sint32 house_inner_right_window_frame_vertices[619] = {
#include "chaostgkinder/terrain/house_inner_right_window_frame_vertices.inc"
};
static NJS_CNK_MODEL house_inner_right_window_frame_model = {
#include "chaostgkinder/terrain/house_inner_right_window_frame_model.inc"
};
static NJS_CNK_OBJECT house_inner_right_window_frame = {
#include "chaostgkinder/terrain/house_inner_right_window_frame.inc"
};
static Sint16 house_left_trees_polygons[51] = {
#include "chaostgkinder/terrain/house_left_trees_polygons.inc"
};
static Sint32 house_left_trees_vertices[51] = {
#include "chaostgkinder/terrain/house_left_trees_vertices.inc"
};
static NJS_CNK_MODEL house_left_trees_model = {
#include "chaostgkinder/terrain/house_left_trees_model.inc"
};
static NJS_CNK_OBJECT house_left_trees = {
#include "chaostgkinder/terrain/house_left_trees.inc"
};
static Sint16 house_right_trees_polygons[51] = {
#include "chaostgkinder/terrain/house_right_trees_polygons.inc"
};
static Sint32 house_right_trees_vertices[51] = {
#include "chaostgkinder/terrain/house_right_trees_vertices.inc"
};
static NJS_CNK_MODEL house_right_trees_model = {
#include "chaostgkinder/terrain/house_right_trees_model.inc"
};
static NJS_CNK_OBJECT house_right_trees = {
#include "chaostgkinder/terrain/house_right_trees.inc"
};
static NJS_CNK_OBJECT house_left_outer_lamp = {
#include "chaostgkinder/terrain/house_left_outer_lamp.inc"
};
static NJS_CNK_OBJECT house_right_outer_lamp = {
#include "chaostgkinder/terrain/house_right_outer_lamp.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_2 = {
#include "chaostgkinder/terrain/house_left_path_shrub_2.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_3 = {
#include "chaostgkinder/terrain/house_left_path_shrub_3.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_4 = {
#include "chaostgkinder/terrain/house_left_path_shrub_4.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_5 = {
#include "chaostgkinder/terrain/house_left_path_shrub_5.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_6 = {
#include "chaostgkinder/terrain/house_left_path_shrub_6.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_7 = {
#include "chaostgkinder/terrain/house_left_path_shrub_7.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_8 = {
#include "chaostgkinder/terrain/house_left_path_shrub_8.inc"
};
static NJS_CNK_OBJECT house_left_path_shrub_9 = {
#include "chaostgkinder/terrain/house_left_path_shrub_9.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_2 = {
#include "chaostgkinder/terrain/house_right_path_shrub_2.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_3 = {
#include "chaostgkinder/terrain/house_right_path_shrub_3.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_4 = {
#include "chaostgkinder/terrain/house_right_path_shrub_4.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_5 = {
#include "chaostgkinder/terrain/house_right_path_shrub_5.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_6 = {
#include "chaostgkinder/terrain/house_right_path_shrub_6.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_7 = {
#include "chaostgkinder/terrain/house_right_path_shrub_7.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_8 = {
#include "chaostgkinder/terrain/house_right_path_shrub_8.inc"
};
static NJS_CNK_OBJECT house_right_path_shrub_9 = {
#include "chaostgkinder/terrain/house_right_path_shrub_9.inc"
};
static NJS_CNK_OBJECT house_inner_right_window_glass = {
#include "chaostgkinder/terrain/house_inner_right_window_glass.inc"
};
static NJS_CNK_OBJECT house_inner_left_window_glass = {
#include "chaostgkinder/terrain/house_inner_left_window_glass.inc"
};
static NJS_CNK_OBJECT house_outer_left_window_glass = {
#include "chaostgkinder/terrain/house_outer_left_window_glass.inc"
};
static NJS_CNK_OBJECT house_outer_left_window_frame = {
#include "chaostgkinder/terrain/house_outer_left_window_frame.inc"
};
static NJS_CNK_OBJECT house_outer_right_window_frame = {
#include "chaostgkinder/terrain/house_outer_right_window_frame.inc"
};
struct land_entry kinder_house_entries[57] = {
#include "chaostgkinder/terrain/kinder_house_entries.inc"
};
struct land_table kinder_house_land = {
#include "chaostgkinder/terrain/kinder_house_land.inc"
};
