#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"
#include "stg32/scenery/container/text.inc"

static NJS_TEXNAME meteor_container_names[6] = {
#include "stg32/scenery/container/meteor_container_names.inc"
};

NJS_TEXLIST meteor_container_texlist = {
#include "stg32/scenery/container/meteor_container_texlist.inc"
};

static Sint16 meteor_container_lamp_polygons[51] = {
#include "stg32/scenery/container/meteor_container_lamp_polygons.inc"
};

static Sint32 meteor_container_lamp_vertices[51] = {
#include "stg32/scenery/container/meteor_container_lamp_vertices.inc"
};

static NJS_CNK_MODEL meteor_container_lamp_model = {
#include "stg32/scenery/container/meteor_container_lamp_model.inc"
};

NJS_CNK_OBJECT meteor_container_lamp[1] = {
#include "stg32/scenery/container/meteor_container_lamp.inc"
};

static Sint16 meteor_container_housing_polygons[689] = {
#include "stg32/scenery/container/meteor_container_housing_polygons.inc"
};

static Sint32 meteor_container_housing_vertices[555] = {
#include "stg32/scenery/container/meteor_container_housing_vertices.inc"
};

static NJS_CNK_MODEL meteor_container_housing_model = {
#include "stg32/scenery/container/meteor_container_housing_model.inc"
};

NJS_CNK_OBJECT meteor_container_housing[1] = {
#include "stg32/scenery/container/meteor_container_housing.inc"
};

static Sint16 meteor_container_shadow_polygons[53] = {
#include "stg32/scenery/container/meteor_container_shadow_polygons.inc"
};

struct meteor_container_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 vertices[10];
    Sint32 end;
};
static struct meteor_container_shadow_vertex_list
    meteor_container_shadow_vertices = {
#include "stg32/scenery/container/meteor_container_shadow_vertices.inc"
};

static NJS_CNK_MODEL meteor_container_shadow_model = {
#include "stg32/scenery/container/meteor_container_shadow_model.inc"
};

NJS_CNK_OBJECT meteor_container_shadow = {
#include "stg32/scenery/container/meteor_container_shadow.inc"
};

static NJS_MATERIAL meteor_container_collision_materials[1] = {
#include "stg32/scenery/container/meteor_container_collision_materials.inc"
};

static Sint16 meteor_container_collision_faces[19] = {
#include "stg32/scenery/container/meteor_container_collision_faces.inc"
};

static NJS_MESHSET meteor_container_collision_meshsets[1] = {
#include "stg32/scenery/container/meteor_container_collision_meshsets.inc"
};

static NJS_POINT3 meteor_container_collision_points[8] = {
#include "stg32/scenery/container/meteor_container_collision_points.inc"
};

static NJS_VECTOR meteor_container_collision_normals[8] = {
#include "stg32/scenery/container/meteor_container_collision_normals.inc"
};

static NJS_MODEL meteor_container_collision_model = {
#include "stg32/scenery/container/meteor_container_collision_model.inc"
};

NJS_OBJECT meteor_container_collision = {
#include "stg32/scenery/container/meteor_container_collision.inc"
};

static NJS_TEXNAME meteor_container_side_names[4] = {
#include "stg32/scenery/container/meteor_container_side_names.inc"
};

NJS_TEXLIST meteor_container_side_texlist = {
#include "stg32/scenery/container/meteor_container_side_texlist.inc"
};

static Sint16 meteor_container_side_polygons[145] = {
#include "stg32/scenery/container/meteor_container_side_polygons.inc"
};

static Sint32 meteor_container_side_vertices[95] = {
#include "stg32/scenery/container/meteor_container_side_vertices.inc"
};

NJS_CNK_MODEL meteor_container_side = {
#include "stg32/scenery/container/meteor_container_side.inc"
};

static NJS_CNK_OBJECT meteor_container_side_object = {
#include "stg32/scenery/container/meteor_container_side_object.inc"
};

static NJS_TEXNAME meteor_container_corner_names[2] = {
#include "stg32/scenery/container/meteor_container_corner_names.inc"
};

NJS_TEXLIST meteor_container_corner_texlist = {
#include "stg32/scenery/container/meteor_container_corner_texlist.inc"
};

static Sint16 meteor_container_corner_polygons[143] = {
#include "stg32/scenery/container/meteor_container_corner_polygons.inc"
};

static Sint32 meteor_container_corner_vertices[75] = {
#include "stg32/scenery/container/meteor_container_corner_vertices.inc"
};

NJS_CNK_MODEL meteor_container_corner = {
#include "stg32/scenery/container/meteor_container_corner.inc"
};

static NJS_CNK_OBJECT meteor_container_corner_object = {
#include "stg32/scenery/container/meteor_container_corner_object.inc"
};

static NJS_TEXNAME meteor_container_lid_names[2] = {
#include "stg32/scenery/container/meteor_container_lid_names.inc"
};

NJS_TEXLIST meteor_container_lid_texlist = {
#include "stg32/scenery/container/meteor_container_lid_texlist.inc"
};

static Sint16 meteor_container_lid_polygons[83] = {
#include "stg32/scenery/container/meteor_container_lid_polygons.inc"
};

static Sint32 meteor_container_lid_vertices[39] = {
#include "stg32/scenery/container/meteor_container_lid_vertices.inc"
};

NJS_CNK_MODEL meteor_container_lid = {
#include "stg32/scenery/container/meteor_container_lid.inc"
};

static NJS_CNK_OBJECT meteor_container_lid_object = {
#include "stg32/scenery/container/meteor_container_lid_object.inc"
};
