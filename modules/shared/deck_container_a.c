#include "shared/deck_container_a/text.inc"
#include <ninja.h>
#include "collision/collision.h"
#include "shared/debris.h"
#include <sg_xpt.h>
#include "shared/deck_container.h"

/* the set editor's labels for ang.x, ang.y, scl.x and scl.y, which no code
 * reads. ang.x's two name its crash modes: whether the cargo's pieces land at
 * pos.y or fall past it */
static const char editor_pass_through[24] = {
#include "shared/deck_container_a/editor_pass_through.inc"
};
static const char editor_reflect[24] = {
#include "shared/deck_container_a/editor_reflect.inc"
};
static const char editor_angle_y[12] = {
#include "shared/deck_container_a/editor_angle_y.inc"
};
static const char editor_travel[16] = {
#include "shared/deck_container_a/editor_travel.inc"
};
static const char editor_speed[12] = {
#include "shared/deck_container_a/editor_speed.inc"
};

static NJS_TEXNAME deck_container_a_textures[7] = {
#include "shared/deck_container_a/deck_container_a_textures.inc"
};

static NJS_TEXLIST deck_container_a_texlist = {
#include "shared/deck_container_a/deck_container_a_texlist.inc"
};

static Sint16 deck_container_a_part_1_polygons[319] = {
#include "shared/deck_container_a/deck_container_a_part_1_polygons.inc"
};

static Sint32 deck_container_a_part_1_vertices[299] = {
#include "shared/deck_container_a/deck_container_a_part_1_vertices.inc"
};

static NJS_CNK_MODEL deck_container_a_part_1_model = {
#include "shared/deck_container_a/deck_container_a_part_1_model.inc"
};

static NJS_CNK_OBJECT deck_container_a_part_1 = {
#include "shared/deck_container_a/deck_container_a_part_1.inc"
};

static Sint16 deck_container_a_object_polygons[157] = {
#include "shared/deck_container_a/deck_container_a_object_polygons.inc"
};

static Sint32 deck_container_a_object_vertices[139] = {
#include "shared/deck_container_a/deck_container_a_object_vertices.inc"
};

static NJS_CNK_MODEL deck_container_a_object_model = {
#include "shared/deck_container_a/deck_container_a_object_model.inc"
};

static NJS_CNK_OBJECT deck_container_a_object = {
#include "shared/deck_container_a/deck_container_a_object.inc"
};

static NJS_MATERIAL deck_container_a_collision_material[1] = {
#include "shared/deck_container_a/deck_container_a_collision_material.inc"
};

static Sint16 deck_container_a_collision_faces_0[19] = {
#include "shared/deck_container_a/deck_container_a_collision_faces_0.inc"
};

static NJS_MESHSET deck_container_a_collision_meshes[1] = {
#include "shared/deck_container_a/deck_container_a_collision_meshes.inc"
};

static NJS_POINT3 deck_container_a_collision_points[8] = {
#include "shared/deck_container_a/deck_container_a_collision_points.inc"
};

static NJS_VECTOR deck_container_a_collision_normals[8] = {
#include "shared/deck_container_a/deck_container_a_collision_normals.inc"
};

static NJS_MODEL deck_container_a_collision_model = {
#include "shared/deck_container_a/deck_container_a_collision_model.inc"
};

static NJS_OBJECT deck_container_a_collision = {
#include "shared/deck_container_a/deck_container_a_collision.inc"
};

static NJS_TEXNAME cargo_plate_textures[1] = {
#include "shared/deck_container_a/cargo_plate_textures.inc"
};

NJS_TEXLIST cargo_plate_texlist = {
#include "shared/deck_container_a/cargo_plate_texlist.inc"
};

static Sint16 cargo_plate_node_polygons[105] = {
#include "shared/deck_container_a/cargo_plate_node_polygons.inc"
};

static Sint32 cargo_plate_node_vertices[91] = {
#include "shared/deck_container_a/cargo_plate_node_vertices.inc"
};

NJS_CNK_MODEL cargo_plate = {
#include "shared/deck_container_a/cargo_plate.inc"
};

static NJS_CNK_OBJECT cargo_plate_node = {
#include "shared/deck_container_a/cargo_plate_node.inc"
};

static NJS_TEXNAME cargo_panel_textures[1] = {
#include "shared/deck_container_a/cargo_panel_textures.inc"
};

NJS_TEXLIST cargo_panel_texlist = {
#include "shared/deck_container_a/cargo_panel_texlist.inc"
};

static Sint16 cargo_panel_node_polygons[95] = {
#include "shared/deck_container_a/cargo_panel_node_polygons.inc"
};

static Sint32 cargo_panel_node_vertices[83] = {
#include "shared/deck_container_a/cargo_panel_node_vertices.inc"
};

NJS_CNK_MODEL cargo_panel = {
#include "shared/deck_container_a/cargo_panel.inc"
};

static NJS_CNK_OBJECT cargo_panel_node = {
#include "shared/deck_container_a/cargo_panel_node.inc"
};

static NJS_TEXNAME cargo_side_textures[2] = {
#include "shared/deck_container_a/cargo_side_textures.inc"
};

NJS_TEXLIST cargo_side_texlist = {
#include "shared/deck_container_a/cargo_side_texlist.inc"
};

static Sint16 cargo_side_node_polygons[81] = {
#include "shared/deck_container_a/cargo_side_node_polygons.inc"
};

static Sint32 cargo_side_node_vertices[63] = {
#include "shared/deck_container_a/cargo_side_node_vertices.inc"
};

NJS_CNK_MODEL cargo_side = {
#include "shared/deck_container_a/cargo_side.inc"
};

static NJS_CNK_OBJECT cargo_side_node = {
#include "shared/deck_container_a/cargo_side_node.inc"
};
#define EMPTY_CARGO_COLLISION deck_container_empty

NJS_TEXLIST *deck_container_a_texlists[5] = {
#include "shared/deck_container_a/deck_container_a_texlists.inc"
};

#define RAIL_AXIS z
static struct collision_shape boxes[2] = {
#include "shared/deck_container_a/boxes.inc"
};
static struct debris_part cargo[12] = {
#include "shared/deck_container_a/cargo.inc"
};

#include "shared/deck_container/container_impl.h" /* IWYU pragma: keep -- the code it compiles here */
