/* Ghost train engine, rolling stock, collision and four routes. */
#ifndef STG05_SCENERY_ART_GHOST_TRAIN_H
#define STG05_SCENERY_ART_GHOST_TRAIN_H

#include <ninja.h>
#include <sg_xpt.h>

#include "stage/path.h"

static NJS_TEXNAME ghost_train_texlist_names[10] = {
#include "stg05/scenery_art/ghost_train/ghost_train_texlist_names.inc"
};

static NJS_TEXLIST ghost_train_texlist = {
#include "stg05/scenery_art/ghost_train/ghost_train_texlist.inc"
};

static Sint16 ghost_train_bobbing_polygons[261] = {
#include "stg05/scenery_art/ghost_train/ghost_train_bobbing_polygons.inc"
};

static Sint32 ghost_train_bobbing_vertices[195] = {
#include "stg05/scenery_art/ghost_train/ghost_train_bobbing_vertices.inc"
};

static NJS_CNK_MODEL ghost_train_bobbing_model = {
#include "stg05/scenery_art/ghost_train/ghost_train_bobbing_model.inc"
};

static NJS_CNK_OBJECT ghost_train_bobbing[1] = {
#include "stg05/scenery_art/ghost_train/ghost_train_bobbing.inc"
};

static Sint16 ghost_train_spin_polygons[95] = {
#include "stg05/scenery_art/ghost_train/ghost_train_spin_polygons.inc"
};

static Sint32 ghost_train_spin_vertices[99] = {
#include "stg05/scenery_art/ghost_train/ghost_train_spin_vertices.inc"
};

static NJS_CNK_MODEL ghost_train_spin_model = {
#include "stg05/scenery_art/ghost_train/ghost_train_spin_model.inc"
};

static NJS_CNK_OBJECT ghost_train_spin[1] = {
#include "stg05/scenery_art/ghost_train/ghost_train_spin.inc"
};

static Sint16 ghost_train_car_polygons[129] = {
#include "stg05/scenery_art/ghost_train/ghost_train_car_polygons.inc"
};

static Sint32 ghost_train_car_vertices[75] = {
#include "stg05/scenery_art/ghost_train/ghost_train_car_vertices.inc"
};

static NJS_CNK_MODEL ghost_train_car_model = {
#include "stg05/scenery_art/ghost_train/ghost_train_car_model.inc"
};

static NJS_CNK_OBJECT ghost_train_car[1] = {
#include "stg05/scenery_art/ghost_train/ghost_train_car.inc"
};

static Sint16 ghost_train_engine_polygons[1883] = {
#include "stg05/scenery_art/ghost_train/ghost_train_engine_polygons.inc"
};

static Sint32 ghost_train_engine_vertices[1587] = {
#include "stg05/scenery_art/ghost_train/ghost_train_engine_vertices.inc"
};

static NJS_CNK_MODEL ghost_train_engine_model = {
#include "stg05/scenery_art/ghost_train/ghost_train_engine_model.inc"
};

static NJS_CNK_OBJECT ghost_train_engine[1] = {
#include "stg05/scenery_art/ghost_train/ghost_train_engine.inc"
};

static NJS_MATERIAL ghost_train_collision_materials = {
#include "stg05/scenery_art/ghost_train/ghost_train_collision_materials.inc"
};

static Uint16 ghost_train_collision_strips[31] = {
#include "stg05/scenery_art/ghost_train/ghost_train_collision_strips.inc"
};

static NJS_MESHSET ghost_train_collision_faces = {
#include "stg05/scenery_art/ghost_train/ghost_train_collision_faces.inc"
};

static NJS_POINT3 ghost_train_collision_points[12] = {
#include "stg05/scenery_art/ghost_train/ghost_train_collision_points.inc"
};

static NJS_VECTOR ghost_train_collision_normals[12] = {
#include "stg05/scenery_art/ghost_train/ghost_train_collision_normals.inc"
};

static NJS_MODEL ghost_train_collision_model = {
#include "stg05/scenery_art/ghost_train/ghost_train_collision_model.inc"
};

static NJS_OBJECT ghost_train_collision = {
#include "stg05/scenery_art/ghost_train/ghost_train_collision.inc"
};

static NJS_POINT3 ghost_train_path_0_points[40] = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_0_points.inc"
};

static struct path ghost_train_path_0 = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_0.inc"
};

static NJS_POINT3 ghost_train_path_1_points[70] = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_1_points.inc"
};

static struct path ghost_train_path_1 = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_1.inc"
};

static NJS_POINT3 ghost_train_path_2_points[80] = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_2_points.inc"
};

static struct path ghost_train_path_2 = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_2.inc"
};

static NJS_POINT3 ghost_train_path_3_points[27] = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_3_points.inc"
};

static struct path ghost_train_path_3 = {
#include "stg05/scenery_art/ghost_train/ghost_train_path_3.inc"
};

#endif
