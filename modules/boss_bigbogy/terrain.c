/* BOSS_BIGBOGY.PRS's land, texture lists, Basic models, chunk models,
 * motions and texture names, from the disc. its C lies between stage.c and
 * model_textures.c, and its D lies between stage.c and title.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigbogy/model_art.h"
#include "boss_bigbogy/motion_art.h"
#include "boss_bigbogy/stage.h"
#include "boss_bigbogy/terrain.h"
#include "boss_bigbogy/texture_names.h"
#include "collision/world.h"

#include "boss_bigbogy/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[26];
struct land_table bigbogy_land;
static NJS_TEXNAME textures[14];
static NJS_TEXLIST terrain_textures;
static NJS_MATERIAL inside_wall_materials[1];
static Sint16 inside_wall_faces[105];
static NJS_MESHSET inside_wall_meshsets[1];
static NJS_POINT3 inside_wall_points[64];
static NJS_MODEL inside_wall_model;
static NJS_OBJECT inside_wall;
static NJS_MATERIAL outside_wall_materials[1];
static Sint16 outside_wall_faces[70];
static NJS_MESHSET outside_wall_meshsets[1];
static NJS_POINT3 outside_wall_points[48];
static NJS_MODEL outside_wall_model;
static NJS_OBJECT outside_wall;
static NJS_MATERIAL floor_collision_materials[1];
static Sint16 floor_collision_faces[241];
static NJS_MESHSET floor_collision_meshsets[1];
static NJS_POINT3 floor_collision_points[96];
static NJS_MODEL floor_collision_model;
static NJS_OBJECT floor_collision;
static Sint16 floor_polygons[377];
static Sint32 floor_vertices[419];
static NJS_CNK_MODEL floor_model;
static NJS_CNK_OBJECT floor;
static Sint16 lit_upper_ring_polygons[327];
static Sint32 lit_upper_ring_vertices[323];
static NJS_CNK_MODEL lit_upper_ring_model;
static NJS_CNK_OBJECT lit_upper_ring;
static Sint16 lit_lower_ring_polygons[881];
static Sint32 lit_lower_ring_vertices[643];
static NJS_CNK_MODEL lit_lower_ring_model;
static NJS_CNK_OBJECT lit_lower_ring;
static Sint16 lit_ceiling_polygons[913];
static Sint32 lit_ceiling_vertices[659];
static NJS_CNK_MODEL lit_ceiling_model;
static NJS_CNK_OBJECT lit_ceiling;
static Sint16 lit_floor_base_polygons[257];
static Sint32 lit_floor_base_vertices[195];
static NJS_CNK_MODEL lit_floor_base_model;
static NJS_CNK_OBJECT lit_floor_base;
static Sint16 lit_east_alcove_polygons[997];
static Sint32 lit_east_alcove_vertices[571];
static NJS_CNK_MODEL lit_east_alcove_model;
static NJS_CNK_OBJECT lit_south_alcove;
static Sint16 lit_walls_polygons[1211];
static Sint32 lit_walls_vertices[1075];
static NJS_CNK_MODEL lit_walls_model;
static NJS_CNK_OBJECT lit_walls;
static Sint16 lit_pillar_base_polygons[671];
static Sint32 lit_pillar_base_vertices[483];
static NJS_CNK_MODEL lit_pillar_base_model;
static NJS_CNK_OBJECT lit_pillar_base;
static Sint16 lit_pillar_cap_polygons[229];
static Sint32 lit_pillar_cap_vertices[259];
static NJS_CNK_MODEL lit_pillar_cap_model;
static NJS_CNK_OBJECT lit_pillar_cap;
static Sint16 dark_pillar_cap_polygons[229];
static Sint32 dark_pillar_cap_vertices[259];
static NJS_CNK_MODEL dark_pillar_cap_model;
static NJS_CNK_OBJECT dark_pillar_cap;
static Sint16 dark_pillar_base_polygons[671];
static Sint32 dark_pillar_base_vertices[355];
static NJS_CNK_MODEL dark_pillar_base_model;
static NJS_CNK_OBJECT dark_pillar_base;
static Sint16 dark_walls_polygons[1211];
static Sint32 dark_walls_vertices[1071];
static NJS_CNK_MODEL dark_walls_model;
static NJS_CNK_OBJECT dark_walls;
static Sint16 dark_south_alcove_polygons[997];
static Sint32 dark_south_alcove_vertices[575];
static NJS_CNK_MODEL dark_south_alcove_model;
static NJS_CNK_OBJECT dark_south_alcove;
static Sint16 dark_ceiling_polygons[913];
static Sint32 dark_ceiling_vertices[591];
static NJS_CNK_MODEL dark_ceiling_model;
static NJS_CNK_OBJECT dark_ceiling;
static Sint16 dark_upper_ring_polygons[327];
static Sint32 dark_upper_ring_vertices[323];
static NJS_CNK_MODEL dark_upper_ring_model;
static NJS_CNK_OBJECT dark_upper_ring;
static Sint16 dark_lower_ring_polygons[881];
static Sint32 dark_lower_ring_vertices[643];
static NJS_CNK_MODEL dark_lower_ring_model;
static NJS_CNK_OBJECT dark_lower_ring;
static Sint16 dark_floor_base_polygons[257];
static Sint32 dark_floor_base_vertices[195];
static NJS_CNK_MODEL dark_floor_base_model;
static NJS_CNK_OBJECT dark_floor_base;
static NJS_CNK_OBJECT lit_east_alcove;
static NJS_CNK_OBJECT lit_north_alcove;
static NJS_CNK_OBJECT lit_west_alcove;
static NJS_CNK_OBJECT dark_west_alcove;
static NJS_CNK_OBJECT dark_north_alcove;
static NJS_CNK_OBJECT dark_east_alcove;

static struct land_entry entries[26] = {
#include "boss_bigbogy/terrain/entries.inc"
};
struct land_table bigbogy_land = {
#include "boss_bigbogy/terrain/bigbogy_land.inc"
};
static NJS_TEXNAME textures[14] = {
#include "boss_bigbogy/terrain/textures.inc"
};
static NJS_TEXLIST terrain_textures = {
#include "boss_bigbogy/terrain/terrain_textures.inc"
};
static NJS_MATERIAL inside_wall_materials[1] = {
#include "boss_bigbogy/terrain/inside_wall_materials.inc"
};
static Sint16 inside_wall_faces[105] = {
#include "boss_bigbogy/terrain/inside_wall_faces.inc"
};
static NJS_MESHSET inside_wall_meshsets[1] = {
#include "boss_bigbogy/terrain/inside_wall_meshsets.inc"
};
static NJS_POINT3 inside_wall_points[64] = {
#include "boss_bigbogy/terrain/inside_wall_points.inc"
};
static NJS_MODEL inside_wall_model = {
#include "boss_bigbogy/terrain/inside_wall_model.inc"
};
static NJS_OBJECT inside_wall = {
#include "boss_bigbogy/terrain/inside_wall.inc"
};
static NJS_MATERIAL outside_wall_materials[1] = {
#include "boss_bigbogy/terrain/outside_wall_materials.inc"
};
static Sint16 outside_wall_faces[70] = {
#include "boss_bigbogy/terrain/outside_wall_faces.inc"
};
static NJS_MESHSET outside_wall_meshsets[1] = {
#include "boss_bigbogy/terrain/outside_wall_meshsets.inc"
};
static NJS_POINT3 outside_wall_points[48] = {
#include "boss_bigbogy/terrain/outside_wall_points.inc"
};
static NJS_MODEL outside_wall_model = {
#include "boss_bigbogy/terrain/outside_wall_model.inc"
};
static NJS_OBJECT outside_wall = {
#include "boss_bigbogy/terrain/outside_wall.inc"
};
static NJS_MATERIAL floor_collision_materials[1] = {
#include "boss_bigbogy/terrain/floor_collision_materials.inc"
};
static Sint16 floor_collision_faces[241] = {
#include "boss_bigbogy/terrain/floor_collision_faces.inc"
};
static NJS_MESHSET floor_collision_meshsets[1] = {
#include "boss_bigbogy/terrain/floor_collision_meshsets.inc"
};
static NJS_POINT3 floor_collision_points[96] = {
#include "boss_bigbogy/terrain/floor_collision_points.inc"
};
static NJS_MODEL floor_collision_model = {
#include "boss_bigbogy/terrain/floor_collision_model.inc"
};
static NJS_OBJECT floor_collision = {
#include "boss_bigbogy/terrain/floor_collision.inc"
};
static Sint16 floor_polygons[377] = {
#include "boss_bigbogy/terrain/floor_polygons.inc"
};
static Sint32 floor_vertices[419] = {
#include "boss_bigbogy/terrain/floor_vertices.inc"
};
static NJS_CNK_MODEL floor_model = {
#include "boss_bigbogy/terrain/floor_model.inc"
};
static NJS_CNK_OBJECT floor = {
#include "boss_bigbogy/terrain/floor.inc"
};
static Sint16 lit_upper_ring_polygons[327] = {
#include "boss_bigbogy/terrain/lit_upper_ring_polygons.inc"
};
static Sint32 lit_upper_ring_vertices[323] = {
#include "boss_bigbogy/terrain/lit_upper_ring_vertices.inc"
};
static NJS_CNK_MODEL lit_upper_ring_model = {
#include "boss_bigbogy/terrain/lit_upper_ring_model.inc"
};
static NJS_CNK_OBJECT lit_upper_ring = {
#include "boss_bigbogy/terrain/lit_upper_ring.inc"
};
static Sint16 lit_lower_ring_polygons[881] = {
#include "boss_bigbogy/terrain/lit_lower_ring_polygons.inc"
};
static Sint32 lit_lower_ring_vertices[643] = {
#include "boss_bigbogy/terrain/lit_lower_ring_vertices.inc"
};
static NJS_CNK_MODEL lit_lower_ring_model = {
#include "boss_bigbogy/terrain/lit_lower_ring_model.inc"
};
static NJS_CNK_OBJECT lit_lower_ring = {
#include "boss_bigbogy/terrain/lit_lower_ring.inc"
};
static Sint16 lit_ceiling_polygons[913] = {
#include "boss_bigbogy/terrain/lit_ceiling_polygons.inc"
};
static Sint32 lit_ceiling_vertices[659] = {
#include "boss_bigbogy/terrain/lit_ceiling_vertices.inc"
};
static NJS_CNK_MODEL lit_ceiling_model = {
#include "boss_bigbogy/terrain/lit_ceiling_model.inc"
};
static NJS_CNK_OBJECT lit_ceiling = {
#include "boss_bigbogy/terrain/lit_ceiling.inc"
};
static Sint16 lit_floor_base_polygons[257] = {
#include "boss_bigbogy/terrain/lit_floor_base_polygons.inc"
};
static Sint32 lit_floor_base_vertices[195] = {
#include "boss_bigbogy/terrain/lit_floor_base_vertices.inc"
};
static NJS_CNK_MODEL lit_floor_base_model = {
#include "boss_bigbogy/terrain/lit_floor_base_model.inc"
};
static NJS_CNK_OBJECT lit_floor_base = {
#include "boss_bigbogy/terrain/lit_floor_base.inc"
};
static Sint16 lit_east_alcove_polygons[997] = {
#include "boss_bigbogy/terrain/lit_east_alcove_polygons.inc"
};
static Sint32 lit_east_alcove_vertices[571] = {
#include "boss_bigbogy/terrain/lit_east_alcove_vertices.inc"
};
static NJS_CNK_MODEL lit_east_alcove_model = {
#include "boss_bigbogy/terrain/lit_east_alcove_model.inc"
};
static NJS_CNK_OBJECT lit_south_alcove = {
#include "boss_bigbogy/terrain/lit_south_alcove.inc"
};
static Sint16 lit_walls_polygons[1211] = {
#include "boss_bigbogy/terrain/lit_walls_polygons.inc"
};
static Sint32 lit_walls_vertices[1075] = {
#include "boss_bigbogy/terrain/lit_walls_vertices.inc"
};
static NJS_CNK_MODEL lit_walls_model = {
#include "boss_bigbogy/terrain/lit_walls_model.inc"
};
static NJS_CNK_OBJECT lit_walls = {
#include "boss_bigbogy/terrain/lit_walls.inc"
};
static Sint16 lit_pillar_base_polygons[671] = {
#include "boss_bigbogy/terrain/lit_pillar_base_polygons.inc"
};
static Sint32 lit_pillar_base_vertices[483] = {
#include "boss_bigbogy/terrain/lit_pillar_base_vertices.inc"
};
static NJS_CNK_MODEL lit_pillar_base_model = {
#include "boss_bigbogy/terrain/lit_pillar_base_model.inc"
};
static NJS_CNK_OBJECT lit_pillar_base = {
#include "boss_bigbogy/terrain/lit_pillar_base.inc"
};
static Sint16 lit_pillar_cap_polygons[229] = {
#include "boss_bigbogy/terrain/lit_pillar_cap_polygons.inc"
};
static Sint32 lit_pillar_cap_vertices[259] = {
#include "boss_bigbogy/terrain/lit_pillar_cap_vertices.inc"
};
static NJS_CNK_MODEL lit_pillar_cap_model = {
#include "boss_bigbogy/terrain/lit_pillar_cap_model.inc"
};
static NJS_CNK_OBJECT lit_pillar_cap = {
#include "boss_bigbogy/terrain/lit_pillar_cap.inc"
};
static Sint16 dark_pillar_cap_polygons[229] = {
#include "boss_bigbogy/terrain/dark_pillar_cap_polygons.inc"
};
static Sint32 dark_pillar_cap_vertices[259] = {
#include "boss_bigbogy/terrain/dark_pillar_cap_vertices.inc"
};
static NJS_CNK_MODEL dark_pillar_cap_model = {
#include "boss_bigbogy/terrain/dark_pillar_cap_model.inc"
};
static NJS_CNK_OBJECT dark_pillar_cap = {
#include "boss_bigbogy/terrain/dark_pillar_cap.inc"
};
static Sint16 dark_pillar_base_polygons[671] = {
#include "boss_bigbogy/terrain/dark_pillar_base_polygons.inc"
};
static Sint32 dark_pillar_base_vertices[355] = {
#include "boss_bigbogy/terrain/dark_pillar_base_vertices.inc"
};
static NJS_CNK_MODEL dark_pillar_base_model = {
#include "boss_bigbogy/terrain/dark_pillar_base_model.inc"
};
static NJS_CNK_OBJECT dark_pillar_base = {
#include "boss_bigbogy/terrain/dark_pillar_base.inc"
};
static Sint16 dark_walls_polygons[1211] = {
#include "boss_bigbogy/terrain/dark_walls_polygons.inc"
};
static Sint32 dark_walls_vertices[1071] = {
#include "boss_bigbogy/terrain/dark_walls_vertices.inc"
};
static NJS_CNK_MODEL dark_walls_model = {
#include "boss_bigbogy/terrain/dark_walls_model.inc"
};
static NJS_CNK_OBJECT dark_walls = {
#include "boss_bigbogy/terrain/dark_walls.inc"
};
static Sint16 dark_south_alcove_polygons[997] = {
#include "boss_bigbogy/terrain/dark_south_alcove_polygons.inc"
};
static Sint32 dark_south_alcove_vertices[575] = {
#include "boss_bigbogy/terrain/dark_south_alcove_vertices.inc"
};
static NJS_CNK_MODEL dark_south_alcove_model = {
#include "boss_bigbogy/terrain/dark_south_alcove_model.inc"
};
static NJS_CNK_OBJECT dark_south_alcove = {
#include "boss_bigbogy/terrain/dark_south_alcove.inc"
};
static Sint16 dark_ceiling_polygons[913] = {
#include "boss_bigbogy/terrain/dark_ceiling_polygons.inc"
};
static Sint32 dark_ceiling_vertices[591] = {
#include "boss_bigbogy/terrain/dark_ceiling_vertices.inc"
};
static NJS_CNK_MODEL dark_ceiling_model = {
#include "boss_bigbogy/terrain/dark_ceiling_model.inc"
};
static NJS_CNK_OBJECT dark_ceiling = {
#include "boss_bigbogy/terrain/dark_ceiling.inc"
};
static Sint16 dark_upper_ring_polygons[327] = {
#include "boss_bigbogy/terrain/dark_upper_ring_polygons.inc"
};
static Sint32 dark_upper_ring_vertices[323] = {
#include "boss_bigbogy/terrain/dark_upper_ring_vertices.inc"
};
static NJS_CNK_MODEL dark_upper_ring_model = {
#include "boss_bigbogy/terrain/dark_upper_ring_model.inc"
};
static NJS_CNK_OBJECT dark_upper_ring = {
#include "boss_bigbogy/terrain/dark_upper_ring.inc"
};
static Sint16 dark_lower_ring_polygons[881] = {
#include "boss_bigbogy/terrain/dark_lower_ring_polygons.inc"
};
static Sint32 dark_lower_ring_vertices[643] = {
#include "boss_bigbogy/terrain/dark_lower_ring_vertices.inc"
};
static NJS_CNK_MODEL dark_lower_ring_model = {
#include "boss_bigbogy/terrain/dark_lower_ring_model.inc"
};
static NJS_CNK_OBJECT dark_lower_ring = {
#include "boss_bigbogy/terrain/dark_lower_ring.inc"
};
static Sint16 dark_floor_base_polygons[257] = {
#include "boss_bigbogy/terrain/dark_floor_base_polygons.inc"
};
static Sint32 dark_floor_base_vertices[195] = {
#include "boss_bigbogy/terrain/dark_floor_base_vertices.inc"
};
static NJS_CNK_MODEL dark_floor_base_model = {
#include "boss_bigbogy/terrain/dark_floor_base_model.inc"
};
static NJS_CNK_OBJECT dark_floor_base = {
#include "boss_bigbogy/terrain/dark_floor_base.inc"
};
static NJS_CNK_OBJECT lit_east_alcove = {
#include "boss_bigbogy/terrain/lit_east_alcove.inc"
};
static NJS_CNK_OBJECT lit_north_alcove = {
#include "boss_bigbogy/terrain/lit_north_alcove.inc"
};
static NJS_CNK_OBJECT lit_west_alcove = {
#include "boss_bigbogy/terrain/lit_west_alcove.inc"
};
static NJS_CNK_OBJECT dark_west_alcove = {
#include "boss_bigbogy/terrain/dark_west_alcove.inc"
};
static NJS_CNK_OBJECT dark_north_alcove = {
#include "boss_bigbogy/terrain/dark_north_alcove.inc"
};
static NJS_CNK_OBJECT dark_east_alcove = {
#include "boss_bigbogy/terrain/dark_east_alcove.inc"
};
static NJS_TEXNAME bigbogy_model_textures_textures[9] = {
#include "boss_bigbogy/terrain/bigbogy_model_textures_textures.inc"
};
NJS_TEXLIST bigbogy_model_texlist = {
#include "boss_bigbogy/terrain/bigbogy_model_texlist.inc"
};
static Sint16 glow_polygons[1111] = {
#include "boss_bigbogy/terrain/glow_polygons.inc"
};
static NJS_CNK_MODEL glow_mesh = {
#include "boss_bigbogy/terrain/glow_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_glow_object = {
#include "boss_bigbogy/terrain/bigbogy_glow_object.inc"
};
static NJS_CNK_OBJECT crest_tip = {
#include "boss_bigbogy/terrain/crest_tip.inc"
};
static Sint16 crest_polygons[2] = {
#include "boss_bigbogy/terrain/crest_polygons.inc"
};
static Sint32 crest_vertices[593] = {
#include "boss_bigbogy/terrain/crest_vertices.inc"
};
static NJS_CNK_MODEL crest_mesh = {
#include "boss_bigbogy/terrain/crest_mesh.inc"
};
static NJS_CNK_OBJECT crest = {
#include "boss_bigbogy/terrain/crest.inc"
};
static Sint32 crest_base_vertices[861] = {
#include "boss_bigbogy/terrain/crest_base_vertices.inc"
};
static NJS_CNK_MODEL crest_base_mesh = {
#include "boss_bigbogy/terrain/crest_base_mesh.inc"
};
static NJS_CNK_OBJECT crest_base = {
#include "boss_bigbogy/terrain/crest_base.inc"
};
static Sint16 part_1_polygons[479] = {
#include "boss_bigbogy/terrain/part_1_polygons.inc"
};
static Sint32 part_1_vertices[453] = {
#include "boss_bigbogy/terrain/part_1_vertices.inc"
};
static NJS_CNK_MODEL part_1_mesh = {
#include "boss_bigbogy/terrain/part_1_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_part_object_1 = {
#include "boss_bigbogy/terrain/bigbogy_part_object_1.inc"
};
static Sint16 part_pivot_1_polygons[67] = {
#include "boss_bigbogy/terrain/part_pivot_1_polygons.inc"
};
static Sint32 part_pivot_1_vertices[99] = {
#include "boss_bigbogy/terrain/part_pivot_1_vertices.inc"
};
static NJS_CNK_MODEL part_pivot_1_mesh = {
#include "boss_bigbogy/terrain/part_pivot_1_mesh.inc"
};
static NJS_CNK_OBJECT part_pivot_1 = {
#include "boss_bigbogy/terrain/part_pivot_1.inc"
};
static Sint16 part_0_polygons[479] = {
#include "boss_bigbogy/terrain/part_0_polygons.inc"
};
static Sint32 part_0_vertices[453] = {
#include "boss_bigbogy/terrain/part_0_vertices.inc"
};
static NJS_CNK_MODEL part_0_mesh = {
#include "boss_bigbogy/terrain/part_0_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_part_object_0 = {
#include "boss_bigbogy/terrain/bigbogy_part_object_0.inc"
};
static Sint16 part_pivot_0_polygons[67] = {
#include "boss_bigbogy/terrain/part_pivot_0_polygons.inc"
};
static Sint32 part_pivot_0_vertices[99] = {
#include "boss_bigbogy/terrain/part_pivot_0_vertices.inc"
};
static NJS_CNK_MODEL part_pivot_0_mesh = {
#include "boss_bigbogy/terrain/part_pivot_0_mesh.inc"
};
static NJS_CNK_OBJECT part_pivot_0 = {
#include "boss_bigbogy/terrain/part_pivot_0.inc"
};
static NJS_CNK_OBJECT face_tip = {
#include "boss_bigbogy/terrain/face_tip.inc"
};
static Sint32 face_vertices[1133] = {
#include "boss_bigbogy/terrain/face_vertices.inc"
};
static NJS_CNK_MODEL face_mesh = {
#include "boss_bigbogy/terrain/face_mesh.inc"
};
static NJS_CNK_OBJECT face = {
#include "boss_bigbogy/terrain/face.inc"
};
static NJS_CNK_OBJECT face_pivot = {
#include "boss_bigbogy/terrain/face_pivot.inc"
};
static NJS_CNK_OBJECT tongue_tip = {
#include "boss_bigbogy/terrain/tongue_tip.inc"
};
static Sint16 mouth_polygons[2] = {
#include "boss_bigbogy/terrain/mouth_polygons.inc"
};
static Sint32 mouth_vertices[759] = {
#include "boss_bigbogy/terrain/mouth_vertices.inc"
};
static NJS_CNK_MODEL mouth_mesh = {
#include "boss_bigbogy/terrain/mouth_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_mouth_node = {
#include "boss_bigbogy/terrain/bigbogy_mouth_node.inc"
};
static Sint32 tongue_vertices[759] = {
#include "boss_bigbogy/terrain/tongue_vertices.inc"
};
static NJS_CNK_MODEL tongue_mesh = {
#include "boss_bigbogy/terrain/tongue_mesh.inc"
};
static NJS_CNK_OBJECT tongue = {
#include "boss_bigbogy/terrain/tongue.inc"
};
static NJS_CNK_OBJECT tongue_pivot = {
#include "boss_bigbogy/terrain/tongue_pivot.inc"
};
static Sint16 tongue_base_polygons[803] = {
#include "boss_bigbogy/terrain/tongue_base_polygons.inc"
};
static NJS_CNK_MODEL tongue_base_mesh = {
#include "boss_bigbogy/terrain/tongue_base_mesh.inc"
};
static NJS_CNK_OBJECT tongue_base = {
#include "boss_bigbogy/terrain/tongue_base.inc"
};
static NJS_CNK_OBJECT jaw_tip = {
#include "boss_bigbogy/terrain/jaw_tip.inc"
};
static Sint32 jaw_vertices[1616] = {
#include "boss_bigbogy/terrain/jaw_vertices.inc"
};
static NJS_CNK_MODEL jaw_mesh = {
#include "boss_bigbogy/terrain/jaw_mesh.inc"
};
static NJS_CNK_OBJECT jaw = {
#include "boss_bigbogy/terrain/jaw.inc"
};
static NJS_CNK_OBJECT head_pivot = {
#include "boss_bigbogy/terrain/head_pivot.inc"
};
static Sint32 head_vertices[952] = {
#include "boss_bigbogy/terrain/head_vertices.inc"
};
static NJS_CNK_MODEL head_mesh = {
#include "boss_bigbogy/terrain/head_mesh.inc"
};
static NJS_CNK_OBJECT head = {
#include "boss_bigbogy/terrain/head.inc"
};
static NJS_CNK_OBJECT neck_mount = {
#include "boss_bigbogy/terrain/neck_mount.inc"
};
static NJS_CNK_OBJECT sway_tip = {
#include "boss_bigbogy/terrain/sway_tip.inc"
};
static NJS_CNK_OBJECT fingertip_1 = {
#include "boss_bigbogy/terrain/fingertip_1.inc"
};
static Sint32 hand_1_vertices[536] = {
#include "boss_bigbogy/terrain/hand_1_vertices.inc"
};
static NJS_CNK_MODEL hand_1_mesh = {
#include "boss_bigbogy/terrain/hand_1_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_hand_node_1 = {
#include "boss_bigbogy/terrain/bigbogy_hand_node_1.inc"
};
static Sint32 wrist_1_vertices[698] = {
#include "boss_bigbogy/terrain/wrist_1_vertices.inc"
};
static NJS_CNK_MODEL wrist_1_mesh = {
#include "boss_bigbogy/terrain/wrist_1_mesh.inc"
};
static NJS_CNK_OBJECT wrist_1 = {
#include "boss_bigbogy/terrain/wrist_1.inc"
};
static Sint32 forearm_1_vertices[502] = {
#include "boss_bigbogy/terrain/forearm_1_vertices.inc"
};
static NJS_CNK_MODEL forearm_1_mesh = {
#include "boss_bigbogy/terrain/forearm_1_mesh.inc"
};
static NJS_CNK_OBJECT forearm_1 = {
#include "boss_bigbogy/terrain/forearm_1.inc"
};
static Sint32 upper_arm_1_vertices[426] = {
#include "boss_bigbogy/terrain/upper_arm_1_vertices.inc"
};
static NJS_CNK_MODEL upper_arm_1_mesh = {
#include "boss_bigbogy/terrain/upper_arm_1_mesh.inc"
};
static NJS_CNK_OBJECT upper_arm_1 = {
#include "boss_bigbogy/terrain/upper_arm_1.inc"
};
static NJS_CNK_OBJECT shoulder_1 = {
#include "boss_bigbogy/terrain/shoulder_1.inc"
};
static NJS_CNK_OBJECT fingertip_0 = {
#include "boss_bigbogy/terrain/fingertip_0.inc"
};
static Sint32 hand_0_vertices[536] = {
#include "boss_bigbogy/terrain/hand_0_vertices.inc"
};
static NJS_CNK_MODEL hand_0_mesh = {
#include "boss_bigbogy/terrain/hand_0_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_hand_node_0 = {
#include "boss_bigbogy/terrain/bigbogy_hand_node_0.inc"
};
static Sint32 wrist_0_vertices[698] = {
#include "boss_bigbogy/terrain/wrist_0_vertices.inc"
};
static NJS_CNK_MODEL wrist_0_mesh = {
#include "boss_bigbogy/terrain/wrist_0_mesh.inc"
};
static NJS_CNK_OBJECT wrist_0 = {
#include "boss_bigbogy/terrain/wrist_0.inc"
};
static Sint32 forearm_0_vertices[502] = {
#include "boss_bigbogy/terrain/forearm_0_vertices.inc"
};
static NJS_CNK_MODEL forearm_0_mesh = {
#include "boss_bigbogy/terrain/forearm_0_mesh.inc"
};
static NJS_CNK_OBJECT forearm_0 = {
#include "boss_bigbogy/terrain/forearm_0.inc"
};
static Sint32 upper_arm_0_vertices[426] = {
#include "boss_bigbogy/terrain/upper_arm_0_vertices.inc"
};
static NJS_CNK_MODEL upper_arm_0_mesh = {
#include "boss_bigbogy/terrain/upper_arm_0_mesh.inc"
};
static NJS_CNK_OBJECT upper_arm_0 = {
#include "boss_bigbogy/terrain/upper_arm_0.inc"
};
static NJS_CNK_OBJECT shoulder_0 = {
#include "boss_bigbogy/terrain/shoulder_0.inc"
};
static Sint32 sway_vertices[1229] = {
#include "boss_bigbogy/terrain/sway_vertices.inc"
};
static NJS_CNK_MODEL sway_mesh = {
#include "boss_bigbogy/terrain/sway_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_sway_model = {
#include "boss_bigbogy/terrain/bigbogy_sway_model.inc"
};
static NJS_CNK_OBJECT sway_pivot = {
#include "boss_bigbogy/terrain/sway_pivot.inc"
};
static Sint16 torso_polygons[3805] = {
#include "boss_bigbogy/terrain/torso_polygons.inc"
};
static NJS_CNK_MODEL torso_mesh = {
#include "boss_bigbogy/terrain/torso_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_root_node = {
#include "boss_bigbogy/terrain/bigbogy_root_node.inc"
};
NJS_CNK_OBJECT bigbogy_body = {
#include "boss_bigbogy/terrain/bigbogy_body.inc"
};
static Sint16 center_tip_polygons[53] = {
#include "boss_bigbogy/terrain/center_tip_polygons.inc"
};
static Sint32 center_tip_vertices[33] = {
#include "boss_bigbogy/terrain/center_tip_vertices.inc"
};
static NJS_CNK_MODEL center_tip_mesh = {
#include "boss_bigbogy/terrain/center_tip_mesh.inc"
};
static NJS_CNK_OBJECT center_tip = {
#include "boss_bigbogy/terrain/center_tip.inc"
};
static Sint16 center_middle_polygons[65] = {
#include "boss_bigbogy/terrain/center_middle_polygons.inc"
};
static Sint32 center_middle_vertices[39] = {
#include "boss_bigbogy/terrain/center_middle_vertices.inc"
};
static NJS_CNK_MODEL center_middle_mesh = {
#include "boss_bigbogy/terrain/center_middle_mesh.inc"
};
static NJS_CNK_OBJECT center_middle = {
#include "boss_bigbogy/terrain/center_middle.inc"
};
static Sint16 center_base_polygons[65] = {
#include "boss_bigbogy/terrain/center_base_polygons.inc"
};
static Sint32 center_base_vertices[39] = {
#include "boss_bigbogy/terrain/center_base_vertices.inc"
};
static NJS_CNK_MODEL center_base_mesh = {
#include "boss_bigbogy/terrain/center_base_mesh.inc"
};
static NJS_CNK_OBJECT center_base = {
#include "boss_bigbogy/terrain/center_base.inc"
};
static Sint16 right_tip_polygons[101] = {
#include "boss_bigbogy/terrain/right_tip_polygons.inc"
};
static Sint32 right_tip_vertices[57] = {
#include "boss_bigbogy/terrain/right_tip_vertices.inc"
};
static NJS_CNK_MODEL right_tip_mesh = {
#include "boss_bigbogy/terrain/right_tip_mesh.inc"
};
static NJS_CNK_OBJECT right_tip = {
#include "boss_bigbogy/terrain/right_tip.inc"
};
static Sint16 right_middle_polygons[65] = {
#include "boss_bigbogy/terrain/right_middle_polygons.inc"
};
static Sint32 right_middle_vertices[39] = {
#include "boss_bigbogy/terrain/right_middle_vertices.inc"
};
static NJS_CNK_MODEL right_middle_mesh = {
#include "boss_bigbogy/terrain/right_middle_mesh.inc"
};
static NJS_CNK_OBJECT right_middle = {
#include "boss_bigbogy/terrain/right_middle.inc"
};
static Sint16 right_base_polygons[41] = {
#include "boss_bigbogy/terrain/right_base_polygons.inc"
};
static Sint32 right_base_vertices[27] = {
#include "boss_bigbogy/terrain/right_base_vertices.inc"
};
static NJS_CNK_MODEL right_base_mesh = {
#include "boss_bigbogy/terrain/right_base_mesh.inc"
};
static NJS_CNK_OBJECT right_base = {
#include "boss_bigbogy/terrain/right_base.inc"
};
static Sint16 left_tip_polygons[101] = {
#include "boss_bigbogy/terrain/left_tip_polygons.inc"
};
static Sint32 left_tip_vertices[57] = {
#include "boss_bigbogy/terrain/left_tip_vertices.inc"
};
static NJS_CNK_MODEL left_tip_mesh = {
#include "boss_bigbogy/terrain/left_tip_mesh.inc"
};
static NJS_CNK_OBJECT left_tip = {
#include "boss_bigbogy/terrain/left_tip.inc"
};
static Sint16 left_middle_polygons[65] = {
#include "boss_bigbogy/terrain/left_middle_polygons.inc"
};
static Sint32 left_middle_vertices[39] = {
#include "boss_bigbogy/terrain/left_middle_vertices.inc"
};
static NJS_CNK_MODEL left_middle_mesh = {
#include "boss_bigbogy/terrain/left_middle_mesh.inc"
};
static NJS_CNK_OBJECT left_middle = {
#include "boss_bigbogy/terrain/left_middle.inc"
};
static Sint16 left_base_polygons[41] = {
#include "boss_bigbogy/terrain/left_base_polygons.inc"
};
static Sint32 left_base_vertices[27] = {
#include "boss_bigbogy/terrain/left_base_vertices.inc"
};
static NJS_CNK_MODEL left_base_mesh = {
#include "boss_bigbogy/terrain/left_base_mesh.inc"
};
static NJS_CNK_OBJECT left_base = {
#include "boss_bigbogy/terrain/left_base.inc"
};
static Sint16 shadow_polygons[533] = {
#include "boss_bigbogy/terrain/shadow_polygons.inc"
};
static Sint32 shadow_vertices[279] = {
#include "boss_bigbogy/terrain/shadow_vertices.inc"
};
static NJS_CNK_MODEL shadow_mesh = {
#include "boss_bigbogy/terrain/shadow_mesh.inc"
};
NJS_CNK_OBJECT bigbogy_shadow = {
#include "boss_bigbogy/terrain/bigbogy_shadow.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_3_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_5_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_6_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_7_angles[17] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_8_angles[17] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_11_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_12_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_13_angles[21] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_14_angles[21] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_18_angles[17] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_20_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_24_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_25_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_27_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_28_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_34_angles[17] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_throw_motion_joint_35_angles[17] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_throw_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion_tracks.inc"
};
NJS_MOTION bigbogy_throw_motion = {
#include "boss_bigbogy/terrain/bigbogy_throw_motion.inc"
};
static NJS_MKEY_F bigbogy_idle_motion_joint_0_position[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_0_position.inc"
};
static NJS_MKEY_F bigbogy_idle_motion_joint_1_position[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_3_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_5_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_6_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_7_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_8_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_11_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_12_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_13_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_14_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_18_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_20_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_24_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_25_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_28_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_34_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_idle_motion_joint_35_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_idle_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion_tracks.inc"
};
NJS_MOTION bigbogy_idle_motion = {
#include "boss_bigbogy/terrain/bigbogy_idle_motion.inc"
};
static NJS_MKEY_F bigbogy_breath_motion_joint_0_position[33] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_0_position.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_3_angles[126] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_5_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_6_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_7_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_8_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_11_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_12_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_13_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_14_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_18_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_20_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_24_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_25_angles[128] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_28_angles[121] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_34_angles[29] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_motion_joint_35_angles[29] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_breath_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion_tracks.inc"
};
NJS_MOTION bigbogy_breath_motion = {
#include "boss_bigbogy/terrain/bigbogy_breath_motion.inc"
};
static NJS_MKEY_F bigbogy_recoil_motion_joint_0_position[21] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_0_position.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_3_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_5_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_6_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_7_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_8_angles[21] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_11_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_12_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_13_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_14_angles[21] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_18_angles[22] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_20_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_24_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_25_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_28_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_34_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_recoil_motion_joint_35_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_recoil_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion_tracks.inc"
};
NJS_MOTION bigbogy_recoil_motion = {
#include "boss_bigbogy/terrain/bigbogy_recoil_motion.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_3_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_5_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_6_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_7_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_8_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_11_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_12_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_13_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_14_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_18_angles[17] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_20_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_24_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_25_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_28_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_34_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_bite_motion_joint_35_angles[24] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_bite_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion_tracks.inc"
};
NJS_MOTION bigbogy_bite_motion = {
#include "boss_bigbogy/terrain/bigbogy_bite_motion.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_0_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_0_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_1_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_1_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_2_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_3_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_4_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_5_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_6_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_7_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_8_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_shadow_motion_joint_9_angles[16] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_joint_9_angles.inc"
};
static NJS_MDATA2 bigbogy_shadow_motion_tracks[10] = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion_tracks.inc"
};
NJS_MOTION bigbogy_shadow_motion = {
#include "boss_bigbogy/terrain/bigbogy_shadow_motion.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_3_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_5_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_6_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_7_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_8_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_11_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_12_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_13_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_14_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_18_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_20_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_24_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_25_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_28_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_34_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_start_motion_joint_35_angles[13] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_start_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_start_motion_tracks.inc"
};
NJS_MOTION bigbogy_start_motion = {
#include "boss_bigbogy/terrain/bigbogy_start_motion.inc"
};
static NJS_MKEY_F bigbogy_ground_motion_joint_0_position[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_0_position.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_3_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_5_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_6_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_7_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_8_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_11_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_12_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_13_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_14_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_18_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_20_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_24_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_25_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_28_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_34_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_ground_motion_joint_35_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_ground_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion_tracks.inc"
};
NJS_MOTION bigbogy_ground_motion = {
#include "boss_bigbogy/terrain/bigbogy_ground_motion.inc"
};
static NJS_MKEY_F bigbogy_restore_motion_joint_0_position[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_0_position.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_0_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_0_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_3_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_5_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_6_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_7_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_8_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_11_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_12_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_13_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_14_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_18_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_20_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_24_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_25_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_28_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_34_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_restore_motion_joint_35_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_restore_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion_tracks.inc"
};
NJS_MOTION bigbogy_restore_motion = {
#include "boss_bigbogy/terrain/bigbogy_restore_motion.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_3_angles[31] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_5_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_6_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_7_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_8_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_11_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_12_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_13_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_14_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_18_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_20_angles[10] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_24_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_25_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_28_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_34_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_motion_10_joint_35_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_motion_10_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_motion_10_tracks.inc"
};
NJS_MOTION bigbogy_motion_10 = {
#include "boss_bigbogy/terrain/bigbogy_motion_10.inc"
};
static NJS_TEXNAME bigbogy_effect_textures_textures[42] = {
#include "boss_bigbogy/terrain/bigbogy_effect_textures_textures.inc"
};
NJS_TEXLIST bigbogy_effect_texlist = {
#include "boss_bigbogy/terrain/bigbogy_effect_texlist.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_3_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_5_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_6_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_7_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_8_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_11_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_12_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_13_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_14_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_18_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_20_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_24_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_25_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_28_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_34_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_circle_motion_joint_35_angles[32] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_circle_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion_tracks.inc"
};
NJS_MOTION bigbogy_circle_motion = {
#include "boss_bigbogy/terrain/bigbogy_circle_motion.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_3_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_5_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_6_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_7_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_8_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_11_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_12_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_13_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_14_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_18_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_20_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_24_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_25_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_28_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_34_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_breath_hold_motion_joint_35_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_breath_hold_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion_tracks.inc"
};
NJS_MOTION bigbogy_breath_hold_motion = {
#include "boss_bigbogy/terrain/bigbogy_breath_hold_motion.inc"
};
static NJS_MKEY_F bigbogy_taunt_motion_joint_0_position[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_0_position.inc"
};
static NJS_MKEY_F bigbogy_taunt_motion_joint_1_position[2] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_3_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_5_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_6_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_7_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_8_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_11_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_12_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_13_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_14_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_18_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_20_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_24_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_25_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_28_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_34_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_taunt_motion_joint_35_angles[8] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_taunt_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion_tracks.inc"
};
NJS_MOTION bigbogy_taunt_motion = {
#include "boss_bigbogy/terrain/bigbogy_taunt_motion.inc"
};
static NJS_MKEY_F bigbogy_fold_motion_joint_0_position[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_0_position.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_3_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_3_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_5_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_6_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_7_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_8_angles[2] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_11_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_12_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_13_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_14_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_18_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_20_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_20_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_24_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_25_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_25_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_28_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_34_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_34_angles.inc"
};
static NJS_MKEY_A bigbogy_fold_motion_joint_35_angles[9] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_joint_35_angles.inc"
};
static NJS_MDATA2 bigbogy_fold_motion_tracks[38] = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion_tracks.inc"
};
NJS_MOTION bigbogy_fold_motion = {
#include "boss_bigbogy/terrain/bigbogy_fold_motion.inc"
};
static NJS_TEXNAME bigbogy_title_textures_textures[17] = {
#include "boss_bigbogy/terrain/bigbogy_title_textures_textures.inc"
};
NJS_TEXLIST bigbogy_title_texlist = {
#include "boss_bigbogy/terrain/bigbogy_title_texlist.inc"
};
