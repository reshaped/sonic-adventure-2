#include "shared/deck_container_c/text.inc"
#include <ninja.h>
#include "collision/collision.h"
#include "shared/debris.h"
#include <sg_xpt.h>
#include "shared/deck_container.h"

/* each array rounds its caption up to a multiple of four bytes */
static const char editor_pass_through[24] = "<- CRASHMODE(THROUGH)";
static const char editor_reflect[24] = "<- CRASHMODE(REFRECT)";
static const char editor_angle_y[12] = "<- ANGLE Y";
static const char editor_travel[16] = "<- MOVE LENGTH";
static const char editor_speed[12] = "<- MOVE SPD";

static NJS_TEXNAME deck_container_c_textures[7] = {
#include "shared/deck_container_c/deck_container_c_textures.inc"
};

static NJS_TEXLIST deck_container_c_texlist = {
#include "shared/deck_container_c/deck_container_c_texlist.inc"
};

static Sint16 deck_container_c_part_1_polygons[241] = {
#include "shared/deck_container_c/deck_container_c_part_1_polygons.inc"
};

static Sint32 deck_container_c_part_1_vertices[227] = {
#include "shared/deck_container_c/deck_container_c_part_1_vertices.inc"
};

static NJS_CNK_MODEL deck_container_c_part_1_model = {
#include "shared/deck_container_c/deck_container_c_part_1_model.inc"
};

static NJS_CNK_OBJECT deck_container_c_part_1 = {
#include "shared/deck_container_c/deck_container_c_part_1.inc"
};

static Sint16 deck_container_c_object_polygons[525] = {
#include "shared/deck_container_c/deck_container_c_object_polygons.inc"
};

static Sint32 deck_container_c_object_vertices[487] = {
#include "shared/deck_container_c/deck_container_c_object_vertices.inc"
};

static NJS_CNK_MODEL deck_container_c_object_model = {
#include "shared/deck_container_c/deck_container_c_object_model.inc"
};

static NJS_CNK_OBJECT deck_container_c_object = {
#include "shared/deck_container_c/deck_container_c_object.inc"
};

static NJS_MATERIAL deck_container_c_collision_material[1] = {
#include "shared/deck_container_c/deck_container_c_collision_material.inc"
};

static Sint16 deck_container_c_collision_faces_0[33] = {
#include "shared/deck_container_c/deck_container_c_collision_faces_0.inc"
};

static NJS_MESHSET deck_container_c_collision_meshes[1] = {
#include "shared/deck_container_c/deck_container_c_collision_meshes.inc"
};

static NJS_POINT3 deck_container_c_collision_points[16] = {
#include "shared/deck_container_c/deck_container_c_collision_points.inc"
};

static NJS_VECTOR deck_container_c_collision_normals[16] = {
#include "shared/deck_container_c/deck_container_c_collision_normals.inc"
};

static NJS_MODEL deck_container_c_collision_model = {
#include "shared/deck_container_c/deck_container_c_collision_model.inc"
};

static NJS_OBJECT deck_container_c_collision = {
#include "shared/deck_container_c/deck_container_c_collision.inc"
};

static NJS_MATERIAL deck_container_c_empty_material[1] = {
#include "shared/deck_container_c/deck_container_c_empty_material.inc"
};

static Sint16 deck_container_c_empty_faces_0[14] = {
#include "shared/deck_container_c/deck_container_c_empty_faces_0.inc"
};

static NJS_MESHSET deck_container_c_empty_meshes[1] = {
#include "shared/deck_container_c/deck_container_c_empty_meshes.inc"
};

static NJS_POINT3 deck_container_c_empty_points[8] = {
#include "shared/deck_container_c/deck_container_c_empty_points.inc"
};

static NJS_VECTOR deck_container_c_empty_normals[8] = {
#include "shared/deck_container_c/deck_container_c_empty_normals.inc"
};

static NJS_MODEL deck_container_c_empty_model = {
#include "shared/deck_container_c/deck_container_c_empty_model.inc"
};

static NJS_OBJECT deck_container_c_empty[1] = {
#include "shared/deck_container_c/deck_container_c_empty.inc"
};
#define EMPTY_CARGO_COLLISION deck_container_c_empty

NJS_TEXLIST *deck_container_c_texlists[5] = {
#include "shared/deck_container_c/deck_container_c_texlists.inc"
};

#define RAIL_AXIS x
/* clang-format off */
static struct collision_shape boxes[2] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 1.0F, -10.0F}, 18.0F, 1.0F, 8.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {14.75F, 12.0F, -10.75F}, 9.0F, 9.0F, 9.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static struct debris_part cargo[12] = {
#include "shared/deck_container_c/cargo.inc"
};

#define CROSS_RAIL
/* NOLINTBEGIN(readability-identifier-naming) */
#define deck_container_a_init deck_container_c_init
#define deck_container_a_destroy deck_container_c_destroy
#define deck_container_a_step deck_container_c_step
#define deck_container_a_draw deck_container_c_draw
#define deck_container_a_object deck_container_c_object
#define deck_container_a_collision deck_container_c_collision
#define deck_container_a_texlist deck_container_c_texlist
/* NOLINTEND(readability-identifier-naming) */
#include "shared/deck_container/container_impl.h" /* IWYU pragma: keep -- the code it compiles here */
