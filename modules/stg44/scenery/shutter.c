#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

#include "stg44/scenery/shutter/text.inc"

static NJS_TEXNAME mad_space_shutter_names[5] = {
#include "stg44/scenery/shutter/mad_space_shutter_names.inc"
};

NJS_TEXLIST mad_space_shutter_texlist = {
#include "stg44/scenery/shutter/mad_space_shutter_texlist.inc"
};

static Sint16 mad_space_shutter_first_light_polygons[15] = {
#include "stg44/scenery/shutter/mad_space_shutter_first_light_polygons.inc"
};

static Sint32 mad_space_shutter_first_light_vertices[19] = {
#include "stg44/scenery/shutter/mad_space_shutter_first_light_vertices.inc"
};

static NJS_CNK_MODEL mad_space_shutter_first_light_model = {
#include "stg44/scenery/shutter/mad_space_shutter_first_light_model.inc"
};

NJS_CNK_OBJECT mad_space_shutter_first_light[1] = {
#include "stg44/scenery/shutter/mad_space_shutter_first_light.inc"
};

static Sint16 mad_space_shutter_second_light_polygons[167] = {
#include "stg44/scenery/shutter/mad_space_shutter_second_light_polygons.inc"
};

static Sint32 mad_space_shutter_second_light_vertices[131] = {
#include "stg44/scenery/shutter/mad_space_shutter_second_light_vertices.inc"
};

static NJS_CNK_MODEL mad_space_shutter_second_light_model = {
#include "stg44/scenery/shutter/mad_space_shutter_second_light_model.inc"
};

NJS_CNK_OBJECT mad_space_shutter_second_light[1] = {
#include "stg44/scenery/shutter/mad_space_shutter_second_light.inc"
};

static Sint16 mad_space_shutter_housing_polygons[195] = {
#include "stg44/scenery/shutter/mad_space_shutter_housing_polygons.inc"
};

static Sint32 mad_space_shutter_housing_vertices[187] = {
#include "stg44/scenery/shutter/mad_space_shutter_housing_vertices.inc"
};

static NJS_CNK_MODEL mad_space_shutter_housing_model = {
#include "stg44/scenery/shutter/mad_space_shutter_housing_model.inc"
};

NJS_CNK_OBJECT mad_space_shutter_housing[1] = {
#include "stg44/scenery/shutter/mad_space_shutter_housing.inc"
};

static NJS_MATERIAL mad_space_shutter_collision_materials[1] = {
#include "stg44/scenery/shutter/mad_space_shutter_collision_materials.inc"
};

static Sint16 mad_space_shutter_collision_faces[5] = {
#include "stg44/scenery/shutter/mad_space_shutter_collision_faces.inc"
};

static NJS_MESHSET mad_space_shutter_collision_meshsets[1] = {
#include "stg44/scenery/shutter/mad_space_shutter_collision_meshsets.inc"
};

static NJS_POINT3 mad_space_shutter_collision_points[4] = {
#include "stg44/scenery/shutter/mad_space_shutter_collision_points.inc"
};

static NJS_VECTOR mad_space_shutter_collision_normals[4] = {
#include "stg44/scenery/shutter/mad_space_shutter_collision_normals.inc"
};

static NJS_MODEL mad_space_shutter_collision_model = {
#include "stg44/scenery/shutter/mad_space_shutter_collision_model.inc"
};

NJS_OBJECT mad_space_shutter_collision[1] = {
#include "stg44/scenery/shutter/mad_space_shutter_collision.inc"
};
