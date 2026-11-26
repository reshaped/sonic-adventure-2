#ifndef STG07_SCENERY_MINE_TRUCK_H
#define STG07_SCENERY_MINE_TRUCK_H

/* Mine truck. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME mine_truck_texlist_names[4] = {
#include "stg07/scenery/mine_truck/mine_truck_texlist_names.inc"
};

static NJS_TEXLIST mine_truck_texlist = {
#include "stg07/scenery/mine_truck/mine_truck_texlist.inc"
};

static Sint16 mine_truck_polygons[545] = {
#include "stg07/scenery/mine_truck/mine_truck_polygons.inc"
};

static Sint32 mine_truck_vertices[591] = {
#include "stg07/scenery/mine_truck/mine_truck_vertices.inc"
};

static NJS_CNK_MODEL mine_truck_model = {
#include "stg07/scenery/mine_truck/mine_truck_model.inc"
};

static NJS_CNK_OBJECT mine_truck_object = {
#include "stg07/scenery/mine_truck/mine_truck_object.inc"
};

static NJS_MATERIAL mine_truck_collision_materials = {
#include "stg07/scenery/mine_truck/mine_truck_collision_materials.inc"
};

static Uint16 mine_truck_collision_strips[19] = {
#include "stg07/scenery/mine_truck/mine_truck_collision_strips.inc"
};

static NJS_MESHSET mine_truck_collision_faces = {
#include "stg07/scenery/mine_truck/mine_truck_collision_faces.inc"
};

static NJS_POINT3 mine_truck_collision_points[8] = {
#include "stg07/scenery/mine_truck/mine_truck_collision_points.inc"
};

static NJS_VECTOR mine_truck_collision_normals[8] = {
#include "stg07/scenery/mine_truck/mine_truck_collision_normals.inc"
};

static NJS_MODEL mine_truck_collision_model = {
#include "stg07/scenery/mine_truck/mine_truck_collision_model.inc"
};

static NJS_OBJECT mine_truck_collision = {
#include "stg07/scenery/mine_truck/mine_truck_collision.inc"
};

static NJS_TEXNAME mine_truck_piece_0_texlist_names[4] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_0_texlist_names.inc"
};

static NJS_TEXLIST mine_truck_piece_0_texlist = {
#include "stg07/scenery/mine_truck/mine_truck_piece_0_texlist.inc"
};

static Sint16 mine_truck_piece_0_polygons[401] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_0_polygons.inc"
};

static Sint32 mine_truck_piece_0_vertices[427] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_0_vertices.inc"
};

static NJS_CNK_MODEL mine_truck_piece_0_model = {
#include "stg07/scenery/mine_truck/mine_truck_piece_0_model.inc"
};

static NJS_CNK_OBJECT mine_truck_piece_0_object = {
#include "stg07/scenery/mine_truck/mine_truck_piece_0_object.inc"
};

static NJS_TEXNAME mine_truck_piece_1_texlist_names[1] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_1_texlist_names.inc"
};

static NJS_TEXLIST mine_truck_piece_1_texlist = {
#include "stg07/scenery/mine_truck/mine_truck_piece_1_texlist.inc"
};

static Sint16 mine_truck_piece_1_polygons[67] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_1_polygons.inc"
};

static Sint32 mine_truck_piece_1_vertices[55] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_1_vertices.inc"
};

static NJS_CNK_MODEL mine_truck_piece_1_model = {
#include "stg07/scenery/mine_truck/mine_truck_piece_1_model.inc"
};

static NJS_CNK_OBJECT mine_truck_piece_1_object = {
#include "stg07/scenery/mine_truck/mine_truck_piece_1_object.inc"
};

static NJS_TEXNAME mine_truck_piece_2_texlist_names[1] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_2_texlist_names.inc"
};

static NJS_TEXLIST mine_truck_piece_2_texlist = {
#include "stg07/scenery/mine_truck/mine_truck_piece_2_texlist.inc"
};

static Sint16 mine_truck_piece_2_polygons[97] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_2_polygons.inc"
};

static Sint32 mine_truck_piece_2_vertices[67] = {
#include "stg07/scenery/mine_truck/mine_truck_piece_2_vertices.inc"
};

static NJS_CNK_MODEL mine_truck_piece_2_model = {
#include "stg07/scenery/mine_truck/mine_truck_piece_2_model.inc"
};

static NJS_CNK_OBJECT mine_truck_piece_2_object = {
#include "stg07/scenery/mine_truck/mine_truck_piece_2_object.inc"
};

#endif
