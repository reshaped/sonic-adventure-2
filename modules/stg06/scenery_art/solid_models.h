/* Solid models. */
#ifndef STG06_SCENERY_ART_SOLID_MODELS_H
#define STG06_SCENERY_ART_SOLID_MODELS_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME moasiba_texlist_names[5] = {
#include "stg06/scenery_art/solid_models/moasiba_texlist_names.inc"
};

static NJS_TEXLIST moasiba_texlist[1] = {
#include "stg06/scenery_art/solid_models/moasiba_texlist.inc"
};

static Sint16 moasiba_polygons[415] = {
#include "stg06/scenery_art/solid_models/moasiba_polygons.inc"
};

static Sint32 moasiba_vertices[359] = {
#include "stg06/scenery_art/solid_models/moasiba_vertices.inc"
};

static NJS_CNK_MODEL moasiba_model[1] = {
#include "stg06/scenery_art/solid_models/moasiba_model.inc"
};

static NJS_CNK_OBJECT moasiba_object = {
#include "stg06/scenery_art/solid_models/moasiba_object.inc"
};

static NJS_MATERIAL moasiba_collision_materials = {
#include "stg06/scenery_art/solid_models/moasiba_collision_materials.inc"
};

static Uint16 moasiba_collision_strips[73] = {
#include "stg06/scenery_art/solid_models/moasiba_collision_strips.inc"
};

static NJS_MESHSET moasiba_collision_faces = {
#include "stg06/scenery_art/solid_models/moasiba_collision_faces.inc"
};

static NJS_POINT3 moasiba_collision_points[30] = {
#include "stg06/scenery_art/solid_models/moasiba_collision_points.inc"
};

static NJS_VECTOR moasiba_collision_normals[30] = {
#include "stg06/scenery_art/solid_models/moasiba_collision_normals.inc"
};

static NJS_MODEL moasiba_collision_model = {
#include "stg06/scenery_art/solid_models/moasiba_collision_model.inc"
};

static NJS_OBJECT moasiba_collision = {
#include "stg06/scenery_art/solid_models/moasiba_collision.inc"
};

static NJS_TEXNAME railobja_texlist_names[4] = {
#include "stg06/scenery_art/solid_models/railobja_texlist_names.inc"
};

static NJS_TEXLIST railobja_texlist[1] = {
#include "stg06/scenery_art/solid_models/railobja_texlist.inc"
};

static Sint16 railobja_polygons[663] = {
#include "stg06/scenery_art/solid_models/railobja_polygons.inc"
};

static Sint32 railobja_vertices[695] = {
#include "stg06/scenery_art/solid_models/railobja_vertices.inc"
};

static NJS_CNK_MODEL railobja_model[1] = {
#include "stg06/scenery_art/solid_models/railobja_model.inc"
};

static NJS_CNK_OBJECT railobja_object = {
#include "stg06/scenery_art/solid_models/railobja_object.inc"
};

static NJS_MATERIAL railobja_collision_materials = {
#include "stg06/scenery_art/solid_models/railobja_collision_materials.inc"
};

static Uint16 railobja_collision_strips[40] = {
#include "stg06/scenery_art/solid_models/railobja_collision_strips.inc"
};

static NJS_MESHSET railobja_collision_faces = {
#include "stg06/scenery_art/solid_models/railobja_collision_faces.inc"
};

static NJS_POINT3 railobja_collision_points[28] = {
#include "stg06/scenery_art/solid_models/railobja_collision_points.inc"
};

static NJS_VECTOR railobja_collision_normals[28] = {
#include "stg06/scenery_art/solid_models/railobja_collision_normals.inc"
};

static NJS_MODEL railobja_collision_model = {
#include "stg06/scenery_art/solid_models/railobja_collision_model.inc"
};

static NJS_OBJECT railobja_collision = {
#include "stg06/scenery_art/solid_models/railobja_collision.inc"
};

#endif
