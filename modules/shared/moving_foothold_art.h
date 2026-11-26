/* Six ASHIBA platforms, their common shadow and the three collision hulls. */
#ifndef SHARED_MOVING_FOOTHOLD_ART_H
#define SHARED_MOVING_FOOTHOLD_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME short_f_textures[3] = {
#include "shared/moving_foothold_art/short_f_textures.inc"
};

static NJS_TEXLIST short_f_texlist = {
#include "shared/moving_foothold_art/short_f_texlist.inc"
};

static Sint16 short_f_polygons[145] = {
#include "shared/moving_foothold_art/short_f_polygons.inc"
};

static Sint32 short_f_vertices[99] = {
#include "shared/moving_foothold_art/short_f_vertices.inc"
};

static NJS_CNK_MODEL short_f_mesh = {
#include "shared/moving_foothold_art/short_f_mesh.inc"
};

static NJS_CNK_OBJECT short_f_object = {
#include "shared/moving_foothold_art/short_f_object.inc"
};

static NJS_TEXNAME medium_f_textures[3] = {
#include "shared/moving_foothold_art/medium_f_textures.inc"
};

static NJS_TEXLIST medium_f_texlist = {
#include "shared/moving_foothold_art/medium_f_texlist.inc"
};

static Sint16 medium_f_polygons[145] = {
#include "shared/moving_foothold_art/medium_f_polygons.inc"
};

static Sint32 medium_f_vertices[99] = {
#include "shared/moving_foothold_art/medium_f_vertices.inc"
};

static NJS_CNK_MODEL medium_f_mesh = {
#include "shared/moving_foothold_art/medium_f_mesh.inc"
};

static NJS_CNK_OBJECT medium_f_object = {
#include "shared/moving_foothold_art/medium_f_object.inc"
};

static NJS_TEXNAME long_f_textures[3] = {
#include "shared/moving_foothold_art/long_f_textures.inc"
};

static NJS_TEXLIST long_f_texlist = {
#include "shared/moving_foothold_art/long_f_texlist.inc"
};

static Sint16 long_f_polygons[145] = {
#include "shared/moving_foothold_art/long_f_polygons.inc"
};

static Sint32 long_f_vertices[99] = {
#include "shared/moving_foothold_art/long_f_vertices.inc"
};

static NJS_CNK_MODEL long_f_mesh = {
#include "shared/moving_foothold_art/long_f_mesh.inc"
};

static NJS_CNK_OBJECT long_f_object = {
#include "shared/moving_foothold_art/long_f_object.inc"
};

static NJS_TEXNAME short_textures[3] = {
#include "shared/moving_foothold_art/short_textures.inc"
};

static NJS_TEXLIST short_texlist = {
#include "shared/moving_foothold_art/short_texlist.inc"
};

static Sint16 short_polygons[145] = {
#include "shared/moving_foothold_art/short_polygons.inc"
};

static Sint32 short_vertices[99] = {
#include "shared/moving_foothold_art/short_vertices.inc"
};

static NJS_CNK_MODEL short_mesh = {
#include "shared/moving_foothold_art/short_mesh.inc"
};

static NJS_CNK_OBJECT short_object = {
#include "shared/moving_foothold_art/short_object.inc"
};

static NJS_TEXNAME medium_textures[3] = {
#include "shared/moving_foothold_art/medium_textures.inc"
};

static NJS_TEXLIST medium_texlist = {
#include "shared/moving_foothold_art/medium_texlist.inc"
};

static Sint16 medium_polygons[145] = {
#include "shared/moving_foothold_art/medium_polygons.inc"
};

static Sint32 medium_vertices[99] = {
#include "shared/moving_foothold_art/medium_vertices.inc"
};

static NJS_CNK_MODEL medium_mesh = {
#include "shared/moving_foothold_art/medium_mesh.inc"
};

static NJS_CNK_OBJECT medium_object = {
#include "shared/moving_foothold_art/medium_object.inc"
};

static NJS_TEXNAME long_textures[3] = {
#include "shared/moving_foothold_art/long_textures.inc"
};

static NJS_TEXLIST long_texlist = {
#include "shared/moving_foothold_art/long_texlist.inc"
};

static Sint16 long_polygons[145] = {
#include "shared/moving_foothold_art/long_polygons.inc"
};

static Sint32 long_vertices[99] = {
#include "shared/moving_foothold_art/long_vertices.inc"
};

static NJS_CNK_MODEL long_mesh = {
#include "shared/moving_foothold_art/long_mesh.inc"
};

static NJS_CNK_OBJECT long_object = {
#include "shared/moving_foothold_art/long_object.inc"
};

static NJS_TEXNAME shadow_textures[4] = {
#include "shared/moving_foothold_art/shadow_textures.inc"
};

static NJS_TEXLIST shadow_texlist = {
#include "shared/moving_foothold_art/shadow_texlist.inc"
};

static Sint16 shadow_polygons[51] = {
#include "shared/moving_foothold_art/shadow_polygons.inc"
};

static Sint32 shadow_vertices[35] = {
#include "shared/moving_foothold_art/shadow_vertices.inc"
};

static NJS_CNK_MODEL shadow_mesh = {
#include "shared/moving_foothold_art/shadow_mesh.inc"
};

static NJS_CNK_OBJECT foothold_shadow_node = {
#include "shared/moving_foothold_art/foothold_shadow_node.inc"
};

static NJS_MATERIAL short_material[1] = {
#include "shared/moving_foothold_art/short_material.inc"
};

static Sint16 short_strips[16] = {
#include "shared/moving_foothold_art/short_strips.inc"
};

static NJS_MESHSET short_faces[1] = {
#include "shared/moving_foothold_art/short_faces.inc"
};

static NJS_POINT3 short_points[8] = {
#include "shared/moving_foothold_art/short_points.inc"
};

static NJS_VECTOR short_normals[8] = {
#include "shared/moving_foothold_art/short_normals.inc"
};

static NJS_MODEL short_hull = {
#include "shared/moving_foothold_art/short_hull.inc"
};

static NJS_OBJECT short_collision = {
#include "shared/moving_foothold_art/short_collision.inc"
};

static NJS_MATERIAL medium_material[1] = {
#include "shared/moving_foothold_art/medium_material.inc"
};

static Sint16 medium_strips[16] = {
#include "shared/moving_foothold_art/medium_strips.inc"
};

static NJS_MESHSET medium_faces[1] = {
#include "shared/moving_foothold_art/medium_faces.inc"
};

static NJS_POINT3 medium_points[8] = {
#include "shared/moving_foothold_art/medium_points.inc"
};

static NJS_VECTOR medium_normals[8] = {
#include "shared/moving_foothold_art/medium_normals.inc"
};

static NJS_MODEL medium_hull = {
#include "shared/moving_foothold_art/medium_hull.inc"
};

static NJS_OBJECT medium_collision = {
#include "shared/moving_foothold_art/medium_collision.inc"
};

static NJS_MATERIAL long_material[1] = {
#include "shared/moving_foothold_art/long_material.inc"
};

static Sint16 long_strips[16] = {
#include "shared/moving_foothold_art/long_strips.inc"
};

static NJS_MESHSET long_faces[1] = {
#include "shared/moving_foothold_art/long_faces.inc"
};

static NJS_POINT3 long_points[8] = {
#include "shared/moving_foothold_art/long_points.inc"
};

static NJS_VECTOR long_normals[8] = {
#include "shared/moving_foothold_art/long_normals.inc"
};

static NJS_MODEL long_hull = {
#include "shared/moving_foothold_art/long_hull.inc"
};

static NJS_OBJECT long_collision = {
#include "shared/moving_foothold_art/long_collision.inc"
};

#endif
