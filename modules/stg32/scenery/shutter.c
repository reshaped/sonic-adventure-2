#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"
#include "stg32/scenery/shutter/text.inc"

static NJS_TEXNAME meteor_shutter_names[6] = {
#include "stg32/scenery/shutter/meteor_shutter_names.inc"
};

NJS_TEXLIST meteor_shutter_texlist = {
#include "stg32/scenery/shutter/meteor_shutter_texlist.inc"
};

static Sint16 meteor_shutter_first_light_polygons[15] = {
#include "stg32/scenery/shutter/meteor_shutter_first_light_polygons.inc"
};

static Sint32 meteor_shutter_first_light_vertices[19] = {
#include "stg32/scenery/shutter/meteor_shutter_first_light_vertices.inc"
};

static NJS_CNK_MODEL meteor_shutter_first_light_model = {
#include "stg32/scenery/shutter/meteor_shutter_first_light_model.inc"
};

NJS_CNK_OBJECT meteor_shutter_first_light[1] = {
#include "stg32/scenery/shutter/meteor_shutter_first_light.inc"
};

static Sint16 meteor_shutter_second_light_polygons[167] = {
#include "stg32/scenery/shutter/meteor_shutter_second_light_polygons.inc"
};

static Sint32 meteor_shutter_second_light_vertices[131] = {
#include "stg32/scenery/shutter/meteor_shutter_second_light_vertices.inc"
};

static NJS_CNK_MODEL meteor_shutter_second_light_model = {
#include "stg32/scenery/shutter/meteor_shutter_second_light_model.inc"
};

NJS_CNK_OBJECT meteor_shutter_second_light[1] = {
#include "stg32/scenery/shutter/meteor_shutter_second_light.inc"
};

static Sint16 meteor_shutter_housing_polygons[279] = {
#include "stg32/scenery/shutter/meteor_shutter_housing_polygons.inc"
};

static Sint32 meteor_shutter_housing_vertices[227] = {
#include "stg32/scenery/shutter/meteor_shutter_housing_vertices.inc"
};

static NJS_CNK_MODEL meteor_shutter_housing_model = {
#include "stg32/scenery/shutter/meteor_shutter_housing_model.inc"
};

NJS_CNK_OBJECT meteor_shutter_housing[1] = {
#include "stg32/scenery/shutter/meteor_shutter_housing.inc"
};

static NJS_MATERIAL meteor_shutter_collision_materials[1] = {
#include "stg32/scenery/shutter/meteor_shutter_collision_materials.inc"
};

static Sint16 meteor_shutter_collision_faces[5] = {
#include "stg32/scenery/shutter/meteor_shutter_collision_faces.inc"
};

static NJS_MESHSET meteor_shutter_collision_meshsets[1] = {
#include "stg32/scenery/shutter/meteor_shutter_collision_meshsets.inc"
};

static NJS_POINT3 meteor_shutter_collision_points[4] = {
#include "stg32/scenery/shutter/meteor_shutter_collision_points.inc"
};

static NJS_VECTOR meteor_shutter_collision_normals[4] = {
#include "stg32/scenery/shutter/meteor_shutter_collision_normals.inc"
};

static NJS_MODEL meteor_shutter_collision_model = {
#include "stg32/scenery/shutter/meteor_shutter_collision_model.inc"
};

NJS_OBJECT meteor_shutter_collision[1] = {
#include "stg32/scenery/shutter/meteor_shutter_collision.inc"
};

static NJS_TEXNAME meteor_shutter_broken_names[4] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_names.inc"
};

NJS_TEXLIST meteor_shutter_broken_texlist = {
#include "stg32/scenery/shutter/meteor_shutter_broken_texlist.inc"
};

static Sint16 meteor_shutter_broken_polygons[363] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_polygons.inc"
};

static Sint32 meteor_shutter_broken_vertices[319] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_vertices.inc"
};

static NJS_CNK_MODEL meteor_shutter_broken_model = {
#include "stg32/scenery/shutter/meteor_shutter_broken_model.inc"
};

NJS_CNK_OBJECT meteor_shutter_broken[1] = {
#include "stg32/scenery/shutter/meteor_shutter_broken.inc"
};

static NJS_MATERIAL meteor_shutter_broken_collision_materials[1] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_collision_materials.inc"
};

static Sint16 meteor_shutter_broken_collision_faces[27] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_collision_faces.inc"
};

static NJS_MESHSET meteor_shutter_broken_collision_meshsets[1] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_collision_meshsets.inc"
};

static NJS_POINT3 meteor_shutter_broken_collision_points[14] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_collision_points.inc"
};

static NJS_VECTOR meteor_shutter_broken_collision_normals[14] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_collision_normals.inc"
};

static NJS_MODEL meteor_shutter_broken_collision_model = {
#include "stg32/scenery/shutter/meteor_shutter_broken_collision_model.inc"
};

NJS_OBJECT meteor_shutter_broken_collision[1] = {
#include "stg32/scenery/shutter/meteor_shutter_broken_collision.inc"
};
