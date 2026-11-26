/* the elevator's models. elevator_draw draws the shaft, each door and the
 * floor as a model of its own. it slides the doors apart and raises the
 * floor. entrance_elevator_collision is the floor's shape, which rises with
 * it. no code draws entrance_elevator_model */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgentrance/fixture_art.h"

static Sint16 right_door_polygons[39] = {
#include "chaostgentrance/elevator_art/right_door_polygons.inc"
};

static Sint32 right_door_vertices[27] = {
#include "chaostgentrance/elevator_art/right_door_vertices.inc"
};

NJS_CNK_MODEL entrance_elevator_right_door = {
#include "chaostgentrance/elevator_art/entrance_elevator_right_door.inc"
};

static NJS_CNK_OBJECT right_door = {
#include "chaostgentrance/elevator_art/right_door.inc"
};

static Sint16 left_door_polygons[39] = {
#include "chaostgentrance/elevator_art/left_door_polygons.inc"
};

static Sint32 left_door_vertices[27] = {
#include "chaostgentrance/elevator_art/left_door_vertices.inc"
};

NJS_CNK_MODEL entrance_elevator_left_door = {
#include "chaostgentrance/elevator_art/entrance_elevator_left_door.inc"
};

static NJS_CNK_OBJECT left_door = {
#include "chaostgentrance/elevator_art/left_door.inc"
};

static Sint16 untextured_floor_polygons[67] = {
#include "chaostgentrance/elevator_art/untextured_floor_polygons.inc"
};

static Sint32 untextured_floor_vertices[143] = {
#include "chaostgentrance/elevator_art/untextured_floor_vertices.inc"
};

static NJS_CNK_MODEL untextured_floor_geometry = {
#include "chaostgentrance/elevator_art/untextured_floor_geometry.inc"
};

static NJS_CNK_OBJECT untextured_floor = {
#include "chaostgentrance/elevator_art/untextured_floor.inc"
};

static Sint16 elevator_floor_polygons[277] = {
#include "chaostgentrance/elevator_art/elevator_floor_polygons.inc"
};

static Sint32 elevator_floor_vertices[255] = {
#include "chaostgentrance/elevator_art/elevator_floor_vertices.inc"
};

NJS_CNK_MODEL entrance_elevator_floor = {
#include "chaostgentrance/elevator_art/entrance_elevator_floor.inc"
};

static NJS_CNK_OBJECT elevator_floor = {
#include "chaostgentrance/elevator_art/elevator_floor.inc"
};

static Sint16 elevator_shaft_polygons[337] = {
#include "chaostgentrance/elevator_art/elevator_shaft_polygons.inc"
};

static Sint32 elevator_shaft_vertices[303] = {
#include "chaostgentrance/elevator_art/elevator_shaft_vertices.inc"
};

NJS_CNK_MODEL entrance_elevator_shaft = {
#include "chaostgentrance/elevator_art/entrance_elevator_shaft.inc"
};

NJS_CNK_OBJECT entrance_elevator_model = {
#include "chaostgentrance/elevator_art/entrance_elevator_model.inc"
};

/* SHC refuses braces around a material's colors, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL floor_collision_material = {
#include "chaostgentrance/elevator_art/floor_collision_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 floor_collision_strips[36] = {
#include "chaostgentrance/elevator_art/floor_collision_strips.inc"
};

static NJS_MESHSET floor_collision_meshset = {
#include "chaostgentrance/elevator_art/floor_collision_meshset.inc"
};

static NJS_POINT3 floor_collision_points[17] = {
#include "chaostgentrance/elevator_art/floor_collision_points.inc"
};

static NJS_VECTOR floor_collision_normals[17] = {
#include "chaostgentrance/elevator_art/floor_collision_normals.inc"
};

static NJS_MODEL floor_collision_model = {
#include "chaostgentrance/elevator_art/floor_collision_model.inc"
};

NJS_OBJECT entrance_elevator_collision = {
#include "chaostgentrance/elevator_art/entrance_elevator_collision.inc"
};
