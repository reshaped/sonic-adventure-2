#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "stg32/scenery/marker/text.inc"

static NJS_TEXNAME herd_small_marker_names[6] = {
#include "stg32/scenery/marker/herd_small_marker_names.inc"
};

NJS_TEXLIST herd_small_marker_texlist = {
#include "stg32/scenery/marker/herd_small_marker_texlist.inc"
};

static Sint16 herd_small_marker_1_polygons[71] = {
#include "stg32/scenery/marker/herd_small_marker_1_polygons.inc"
};

static Sint32 herd_small_marker_1_vertices[51] = {
#include "stg32/scenery/marker/herd_small_marker_1_vertices.inc"
};

static NJS_CNK_MODEL herd_small_marker_1_model = {
#include "stg32/scenery/marker/herd_small_marker_1_model.inc"
};

static NJS_CNK_OBJECT herd_small_marker_1 = {
#include "stg32/scenery/marker/herd_small_marker_1.inc"
};

static Sint16 herd_small_marker_polygons[353] = {
#include "stg32/scenery/marker/herd_small_marker_polygons.inc"
};

static Sint32 herd_small_marker_vertices[275] = {
#include "stg32/scenery/marker/herd_small_marker_vertices.inc"
};

static NJS_CNK_MODEL herd_small_marker_model = {
#include "stg32/scenery/marker/herd_small_marker_model.inc"
};

NJS_CNK_OBJECT herd_small_marker[1] = {
#include "stg32/scenery/marker/herd_small_marker.inc"
};

static NJS_TEXNAME herd_large_marker_names[6] = {
#include "stg32/scenery/marker/herd_large_marker_names.inc"
};

NJS_TEXLIST herd_large_marker_texlist = {
#include "stg32/scenery/marker/herd_large_marker_texlist.inc"
};

static Sint16 herd_large_marker_1_polygons[71] = {
#include "stg32/scenery/marker/herd_large_marker_1_polygons.inc"
};

static Sint32 herd_large_marker_1_vertices[51] = {
#include "stg32/scenery/marker/herd_large_marker_1_vertices.inc"
};

static NJS_CNK_MODEL herd_large_marker_1_model = {
#include "stg32/scenery/marker/herd_large_marker_1_model.inc"
};

static NJS_CNK_OBJECT herd_large_marker_1 = {
#include "stg32/scenery/marker/herd_large_marker_1.inc"
};

static Sint16 herd_large_marker_polygons[353] = {
#include "stg32/scenery/marker/herd_large_marker_polygons.inc"
};

static Sint32 herd_large_marker_vertices[275] = {
#include "stg32/scenery/marker/herd_large_marker_vertices.inc"
};

static NJS_CNK_MODEL herd_large_marker_model = {
#include "stg32/scenery/marker/herd_large_marker_model.inc"
};

NJS_CNK_OBJECT herd_large_marker[1] = {
#include "stg32/scenery/marker/herd_large_marker.inc"
};

static NJS_MATERIAL herd_marker_collision_materials[1] = {
#include "stg32/scenery/marker/herd_marker_collision_materials.inc"
};

static Sint16 herd_marker_collision_faces[36] = {
#include "stg32/scenery/marker/herd_marker_collision_faces.inc"
};

static NJS_MESHSET herd_marker_collision_meshsets[1] = {
#include "stg32/scenery/marker/herd_marker_collision_meshsets.inc"
};

static NJS_POINT3 herd_marker_collision_points[22] = {
#include "stg32/scenery/marker/herd_marker_collision_points.inc"
};

static NJS_VECTOR herd_marker_collision_normals[22] = {
#include "stg32/scenery/marker/herd_marker_collision_normals.inc"
};

static NJS_MODEL herd_marker_collision_model = {
#include "stg32/scenery/marker/herd_marker_collision_model.inc"
};

NJS_OBJECT herd_marker_collision[1] = {
#include "stg32/scenery/marker/herd_marker_collision.inc"
};
