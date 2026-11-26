#ifndef STG43_FOREGROUND_VULCAN_H
#define STG43_FOREGROUND_VULCAN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME vulcan_textures[1] = {
#include "stg43/foreground/vulcan/vulcan_textures.inc"
};

static NJS_TEXLIST vulcan_texlist = {
#include "stg43/foreground/vulcan/vulcan_texlist.inc"
};

static Sint16 vulcan_light_polygons[115] = {
#include "stg43/foreground/vulcan/vulcan_light_polygons.inc"
};

static Sint32 vulcan_light_vertices[119] = {
#include "stg43/foreground/vulcan/vulcan_light_vertices.inc"
};

static NJS_CNK_MODEL vulcan_light_model = {
#include "stg43/foreground/vulcan/vulcan_light_model.inc"
};

static NJS_CNK_OBJECT vulcan_light = {
#include "stg43/foreground/vulcan/vulcan_light.inc"
};

static Sint16 vulcan_polygons[287] = {
#include "stg43/foreground/vulcan/vulcan_polygons.inc"
};

static Sint32 vulcan_vertices[323] = {
#include "stg43/foreground/vulcan/vulcan_vertices.inc"
};

static NJS_CNK_MODEL vulcan_model = {
#include "stg43/foreground/vulcan/vulcan_model.inc"
};

static NJS_CNK_OBJECT vulcan_object = {
#include "stg43/foreground/vulcan/vulcan_object.inc"
};

static NJS_MATERIAL vulcan_material = {
#include "stg43/foreground/vulcan/vulcan_material.inc"
};

static Sint16 vulcan_indices[21] = {
#include "stg43/foreground/vulcan/vulcan_indices.inc"
};

static NJS_MESHSET vulcan_faces = {
#include "stg43/foreground/vulcan/vulcan_faces.inc"
};

static NJS_POINT3 vulcan_points[9] = {
#include "stg43/foreground/vulcan/vulcan_points.inc"
};

static NJS_VECTOR vulcan_normals[9] = {
#include "stg43/foreground/vulcan/vulcan_normals.inc"
};

static NJS_MODEL vulcan_solid_model = {
#include "stg43/foreground/vulcan/vulcan_solid_model.inc"
};

static NJS_OBJECT vulcan_collision = {
#include "stg43/foreground/vulcan/vulcan_collision.inc"
};

static NJS_TEXNAME vulcan_cap_textures[1] = {
#include "stg43/foreground/vulcan/vulcan_cap_textures.inc"
};

static NJS_TEXLIST vulcan_cap_texlist = {
#include "stg43/foreground/vulcan/vulcan_cap_texlist.inc"
};

static Sint16 vulcan_cap_polygons[89] = {
#include "stg43/foreground/vulcan/vulcan_cap_polygons.inc"
};

static Sint32 vulcan_cap_vertices[79] = {
#include "stg43/foreground/vulcan/vulcan_cap_vertices.inc"
};

static NJS_CNK_MODEL vulcan_cap = {
#include "stg43/foreground/vulcan/vulcan_cap.inc"
};

static NJS_CNK_OBJECT vulcan_cap_object = {
#include "stg43/foreground/vulcan/vulcan_cap_object.inc"
};

static NJS_TEXNAME vulcan_corner_textures[1] = {
#include "stg43/foreground/vulcan/vulcan_corner_textures.inc"
};

static NJS_TEXLIST vulcan_corner_texlist = {
#include "stg43/foreground/vulcan/vulcan_corner_texlist.inc"
};

static Sint16 vulcan_corner_polygons[77] = {
#include "stg43/foreground/vulcan/vulcan_corner_polygons.inc"
};

static Sint32 vulcan_corner_vertices[67] = {
#include "stg43/foreground/vulcan/vulcan_corner_vertices.inc"
};

static NJS_CNK_MODEL vulcan_corner = {
#include "stg43/foreground/vulcan/vulcan_corner.inc"
};

static NJS_CNK_OBJECT vulcan_corner_object = {
#include "stg43/foreground/vulcan/vulcan_corner_object.inc"
};

static NJS_TEXNAME vulcan_side_textures[2] = {
#include "stg43/foreground/vulcan/vulcan_side_textures.inc"
};

static NJS_TEXLIST vulcan_side_texlist = {
#include "stg43/foreground/vulcan/vulcan_side_texlist.inc"
};

static Sint16 vulcan_side_polygons[135] = {
#include "stg43/foreground/vulcan/vulcan_side_polygons.inc"
};

static Sint32 vulcan_side_vertices[47] = {
#include "stg43/foreground/vulcan/vulcan_side_vertices.inc"
};

static NJS_CNK_MODEL vulcan_side = {
#include "stg43/foreground/vulcan/vulcan_side.inc"
};

static NJS_CNK_OBJECT vulcan_side_object = {
#include "stg43/foreground/vulcan/vulcan_side_object.inc"
};

#endif
