#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "effect/burst.h"
#include "shared/debris.h"
#include "stage/path.h"
#include "stg22/rail/slots.h"
#include "stg22/rail_art.h"
#include "stg22/rail_layout.h"

#include "stg22/rail_data/text.inc"

static NJS_TEXNAME rail_exit_textures[RAIL_EXIT_TEXTURES] = {
#include "stg22/rail_data/rail_exit_textures.inc"
};

NJS_TEXLIST rail_exit_texlist = {
#include "stg22/rail_data/rail_exit_texlist.inc"
};

static Sint16 rail_exit_panel_polygons[359] = {
#include "stg22/rail_data/rail_exit_panel_polygons.inc"
};

static Sint32 rail_exit_panel_vertices[303] = {
#include "stg22/rail_data/rail_exit_panel_vertices.inc"
};

static NJS_CNK_MODEL rail_exit_panel_mesh = {
#include "stg22/rail_data/rail_exit_panel_mesh.inc"
};

NJS_CNK_OBJECT rail_exit_panel = {
#include "stg22/rail_data/rail_exit_panel.inc"
};

static Sint16 rail_exit_root_node_3_polygons[83] = {
#include "stg22/rail_data/rail_exit_root_node_3_polygons.inc"
};

static Sint32 rail_exit_root_node_3_vertices[59] = {
#include "stg22/rail_data/rail_exit_root_node_3_vertices.inc"
};

static NJS_CNK_MODEL rail_exit_root_node_3_mesh = {
#include "stg22/rail_data/rail_exit_root_node_3_mesh.inc"
};

static NJS_CNK_OBJECT rail_exit_root_node_3 = {
#include "stg22/rail_data/rail_exit_root_node_3.inc"
};

static Sint16 rail_exit_root_node_2_polygons[83] = {
#include "stg22/rail_data/rail_exit_root_node_2_polygons.inc"
};

static Sint32 rail_exit_root_node_2_vertices[59] = {
#include "stg22/rail_data/rail_exit_root_node_2_vertices.inc"
};

static NJS_CNK_MODEL rail_exit_root_node_2_mesh = {
#include "stg22/rail_data/rail_exit_root_node_2_mesh.inc"
};

static NJS_CNK_OBJECT rail_exit_root_node_2 = {
#include "stg22/rail_data/rail_exit_root_node_2.inc"
};

static Sint16 rail_exit_root_node_1_polygons[83] = {
#include "stg22/rail_data/rail_exit_root_node_1_polygons.inc"
};

static Sint32 rail_exit_root_node_1_vertices[59] = {
#include "stg22/rail_data/rail_exit_root_node_1_vertices.inc"
};

static NJS_CNK_MODEL rail_exit_root_node_1_mesh = {
#include "stg22/rail_data/rail_exit_root_node_1_mesh.inc"
};

static NJS_CNK_OBJECT rail_exit_root_node_1 = {
#include "stg22/rail_data/rail_exit_root_node_1.inc"
};

static Sint16 rail_exit_root_polygons[759] = {
#include "stg22/rail_data/rail_exit_root_polygons.inc"
};

static Sint32 rail_exit_root_vertices[591] = {
#include "stg22/rail_data/rail_exit_root_vertices.inc"
};

static NJS_CNK_MODEL rail_exit_root_mesh = {
#include "stg22/rail_data/rail_exit_root_mesh.inc"
};

NJS_CNK_OBJECT rail_exit_root = {
#include "stg22/rail_data/rail_exit_root.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL rail_exit_collision_materials[1] = {
#include "stg22/rail_data/rail_exit_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 rail_exit_collision_indices_0[34] = {
#include "stg22/rail_data/rail_exit_collision_indices_0.inc"
};

static NJS_MESHSET rail_exit_collision_meshsets[1] = {
#include "stg22/rail_data/rail_exit_collision_meshsets.inc"
};

static NJS_POINT3 rail_exit_collision_points[16] = {
#include "stg22/rail_data/rail_exit_collision_points.inc"
};

static NJS_VECTOR rail_exit_collision_normals[16] = {
#include "stg22/rail_data/rail_exit_collision_normals.inc"
};

static NJS_MODEL rail_exit_collision_model = {
#include "stg22/rail_data/rail_exit_collision_model.inc"
};

NJS_OBJECT rail_exit_collision = {
#include "stg22/rail_data/rail_exit_collision.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL rail_intake_collision_materials[1] = {
#include "stg22/rail_data/rail_intake_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 rail_intake_collision_indices_0[50] = {
#include "stg22/rail_data/rail_intake_collision_indices_0.inc"
};

static NJS_MESHSET rail_intake_collision_meshsets[1] = {
#include "stg22/rail_data/rail_intake_collision_meshsets.inc"
};

static NJS_POINT3 rail_intake_collision_points[22] = {
#include "stg22/rail_data/rail_intake_collision_points.inc"
};

static NJS_VECTOR rail_intake_collision_normals[22] = {
#include "stg22/rail_data/rail_intake_collision_normals.inc"
};

static NJS_MODEL rail_intake_collision_model = {
#include "stg22/rail_data/rail_intake_collision_model.inc"
};

NJS_OBJECT rail_intake_collision = {
#include "stg22/rail_data/rail_intake_collision.inc"
};

static NJS_TEXNAME gadget_intake_textures[INTAKE_TEXTURES] = {
#include "stg22/rail_data/gadget_intake_textures.inc"
};

NJS_TEXLIST gadget_intake_texlist = {
#include "stg22/rail_data/gadget_intake_texlist.inc"
};

static Sint16 gadget_intake_rib_a_polygons[67] = {
#include "stg22/rail_data/gadget_intake_rib_a_polygons.inc"
};

static Sint32 gadget_intake_rib_a_vertices[67] = {
#include "stg22/rail_data/gadget_intake_rib_a_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_rib_a_mesh = {
#include "stg22/rail_data/gadget_intake_rib_a_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_rib_a[1] = {
#include "stg22/rail_data/gadget_intake_rib_a.inc"
};

static Sint16 gadget_intake_rib_b_polygons[67] = {
#include "stg22/rail_data/gadget_intake_rib_b_polygons.inc"
};

static Sint32 gadget_intake_rib_b_vertices[67] = {
#include "stg22/rail_data/gadget_intake_rib_b_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_rib_b_mesh = {
#include "stg22/rail_data/gadget_intake_rib_b_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_rib_b[1] = {
#include "stg22/rail_data/gadget_intake_rib_b.inc"
};

static Sint16 gadget_intake_rib_c_polygons[67] = {
#include "stg22/rail_data/gadget_intake_rib_c_polygons.inc"
};

static Sint32 gadget_intake_rib_c_vertices[67] = {
#include "stg22/rail_data/gadget_intake_rib_c_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_rib_c_mesh = {
#include "stg22/rail_data/gadget_intake_rib_c_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_rib_c[1] = {
#include "stg22/rail_data/gadget_intake_rib_c.inc"
};

static Sint16 gadget_intake_cover_polygons[63] = {
#include "stg22/rail_data/gadget_intake_cover_polygons.inc"
};

static Sint32 gadget_intake_cover_vertices[43] = {
#include "stg22/rail_data/gadget_intake_cover_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_cover_mesh = {
#include "stg22/rail_data/gadget_intake_cover_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_cover[1] = {
#include "stg22/rail_data/gadget_intake_cover.inc"
};

static Sint16 gadget_intake_lamp_polygons[83] = {
#include "stg22/rail_data/gadget_intake_lamp_polygons.inc"
};

static Sint32 gadget_intake_lamp_vertices[67] = {
#include "stg22/rail_data/gadget_intake_lamp_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_lamp_mesh = {
#include "stg22/rail_data/gadget_intake_lamp_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_lamp[1] = {
#include "stg22/rail_data/gadget_intake_lamp.inc"
};

static Sint16 gadget_intake_glow_polygons[113] = {
#include "stg22/rail_data/gadget_intake_glow_polygons.inc"
};

static Sint32 gadget_intake_glow_vertices[131] = {
#include "stg22/rail_data/gadget_intake_glow_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_glow_mesh = {
#include "stg22/rail_data/gadget_intake_glow_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_glow[1] = {
#include "stg22/rail_data/gadget_intake_glow.inc"
};

static Sint16 gadget_intake_belt_polygons[25] = {
#include "stg22/rail_data/gadget_intake_belt_polygons.inc"
};

static Sint32 gadget_intake_belt_vertices[19] = {
#include "stg22/rail_data/gadget_intake_belt_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_belt_mesh = {
#include "stg22/rail_data/gadget_intake_belt_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_belt[1] = {
#include "stg22/rail_data/gadget_intake_belt.inc"
};

static Sint16 gadget_intake_base_polygons[675] = {
#include "stg22/rail_data/gadget_intake_base_polygons.inc"
};

static Sint32 gadget_intake_base_vertices[503] = {
#include "stg22/rail_data/gadget_intake_base_vertices.inc"
};

static NJS_CNK_MODEL gadget_intake_base_mesh = {
#include "stg22/rail_data/gadget_intake_base_mesh.inc"
};

NJS_CNK_OBJECT gadget_intake_base[1] = {
#include "stg22/rail_data/gadget_intake_base.inc"
};

/* one uvs entry of one key, as texture_anim.h lays them out. its index is
 * 11, and four vertices' u and v follow */
static Sint16 belt_uvs[12] = {
#include "stg22/rail_data/belt_uvs.inc"
};

struct texture_anim gadget_intake_scroll = {
#include "stg22/rail_data/gadget_intake_scroll.inc"
};

static NJS_POINT3 first_path_points[53] = {
#include "stg22/rail_data/first_path_points.inc"
};

static struct path rail_first_path = {
#include "stg22/rail_data/rail_first_path.inc"
};

static NJS_POINT3 second_path_points[61] = {
#include "stg22/rail_data/second_path_points.inc"
};

static struct path rail_second_path = {
#include "stg22/rail_data/rail_second_path.inc"
};

static NJS_POINT3 third_path_points[4] = {
#include "stg22/rail_data/third_path_points.inc"
};

static struct path rail_third_path = {
#include "stg22/rail_data/rail_third_path.inc"
};

static NJS_POINT3 second_markers_points[61] = {
#include "stg22/rail_data/second_markers_points.inc"
};

static struct path rail_second_markers = {
#include "stg22/rail_data/rail_second_markers.inc"
};

static NJS_TEXNAME gadget_intake_shard_a_textures[INTAKE_SHARD_A_TEXTURES] = {
#include "stg22/rail_data/gadget_intake_shard_a_textures.inc"
};

NJS_TEXLIST gadget_intake_shard_a_texlist = {
#include "stg22/rail_data/gadget_intake_shard_a_texlist.inc"
};

static Sint16 gadget_intake_shard_a_polygons[23] = {
#include "stg22/rail_data/gadget_intake_shard_a_polygons.inc"
};

static Sint32 gadget_intake_shard_a_vertices[15] = {
#include "stg22/rail_data/gadget_intake_shard_a_vertices.inc"
};

NJS_CNK_MODEL gadget_intake_shard_a[1] = {
#include "stg22/rail_data/gadget_intake_shard_a.inc"
};

static NJS_CNK_OBJECT gadget_intake_shard_a_object = {
#include "stg22/rail_data/gadget_intake_shard_a_object.inc"
};

static NJS_TEXNAME gadget_intake_shard_b_textures[INTAKE_SHARD_B_TEXTURES] = {
#include "stg22/rail_data/gadget_intake_shard_b_textures.inc"
};

NJS_TEXLIST gadget_intake_shard_b_texlist = {
#include "stg22/rail_data/gadget_intake_shard_b_texlist.inc"
};

static Sint16 gadget_intake_shard_b_polygons[25] = {
#include "stg22/rail_data/gadget_intake_shard_b_polygons.inc"
};

static Sint32 gadget_intake_shard_b_vertices[19] = {
#include "stg22/rail_data/gadget_intake_shard_b_vertices.inc"
};

NJS_CNK_MODEL gadget_intake_shard_b[1] = {
#include "stg22/rail_data/gadget_intake_shard_b.inc"
};

static NJS_CNK_OBJECT gadget_intake_shard_b_object = {
#include "stg22/rail_data/gadget_intake_shard_b_object.inc"
};

NJS_TEXLIST *gadget_intake_texlists[6] = {
#include "stg22/rail_data/gadget_intake_texlists.inc"
};

/* the intake broken into 11 parts for debris_break, which no code reads.
 * rail_intake.c throws its shards in rows and columns instead */
static struct debris_part intake_shard_layout[11] = {
#include "stg22/rail_data/intake_shard_layout.inc"
};

/* the second rail's markers lie along a path of their own. the third rail's
 * marker_spacing is 0, so it has no markers */
struct gadget_rail gadget_rails[3] = {
#include "stg22/rail_data/gadget_rails.inc"
};

struct collision_shape rail_exit_shape[1] = {
#include "stg22/rail_data/rail_exit_shape.inc"
};

struct collision_shape rail_intake_shapes[3] = {
#include "stg22/rail_data/rail_intake_shapes.inc"
};
