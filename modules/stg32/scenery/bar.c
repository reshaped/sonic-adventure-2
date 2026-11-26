#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"

static Sint16 meteo_bar_model_polygons[407] = {
#include "stg32/scenery/bar/meteo_bar_model_polygons.inc"
};

static Sint32 meteo_bar_model_vertices[427] = {
#include "stg32/scenery/bar/meteo_bar_model_vertices.inc"
};

static NJS_CNK_MODEL meteo_bar_model_model = {
#include "stg32/scenery/bar/meteo_bar_model_model.inc"
};

NJS_CNK_OBJECT meteo_bar_model[1] = {
#include "stg32/scenery/bar/meteo_bar_model.inc"
};

static NJS_MATERIAL meteo_bar_collision_materials[1] = {
#include "stg32/scenery/bar/meteo_bar_collision_materials.inc"
};

static Sint16 meteo_bar_collision_faces[21] = {
#include "stg32/scenery/bar/meteo_bar_collision_faces.inc"
};

static NJS_MESHSET meteo_bar_collision_meshsets[1] = {
#include "stg32/scenery/bar/meteo_bar_collision_meshsets.inc"
};

static NJS_POINT3 meteo_bar_collision_points[8] = {
#include "stg32/scenery/bar/meteo_bar_collision_points.inc"
};

static NJS_VECTOR meteo_bar_collision_normals[8] = {
#include "stg32/scenery/bar/meteo_bar_collision_normals.inc"
};

static NJS_MODEL meteo_bar_collision_model = {
#include "stg32/scenery/bar/meteo_bar_collision_model.inc"
};

NJS_OBJECT meteo_bar_collision = {
#include "stg32/scenery/bar/meteo_bar_collision.inc"
};
