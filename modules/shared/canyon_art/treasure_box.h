/* Wild Canyon chest, collision mesh, detached shards and debris models. */
#ifndef SHARED_CANYON_ART_TREASURE_BOX_H
#define SHARED_CANYON_ART_TREASURE_BOX_H

#include <ninja.h>
#include <sg_xpt.h>

/* the set editor's label for ang.y, which no code reads */
static const char treasure_box_edit_yaw[20] = {
#include "shared/canyon_art/treasure_box/treasure_box_edit_yaw.inc"
};

static NJS_TEXNAME treasure_box_texlist_names[2] = {
#include "shared/canyon_art/treasure_box/treasure_box_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_texlist.inc"
};

static Sint16 treasure_box_polygons[275] = {
#include "shared/canyon_art/treasure_box/treasure_box_polygons.inc"
};

static Sint32 treasure_box_vertices[211] = {
#include "shared/canyon_art/treasure_box/treasure_box_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_model = {
#include "shared/canyon_art/treasure_box/treasure_box_model.inc"
};

static NJS_CNK_OBJECT treasure_box_object = {
#include "shared/canyon_art/treasure_box/treasure_box_object.inc"
};

static NJS_TEXNAME treasure_box_broken_texlist_names[2] = {
#include "shared/canyon_art/treasure_box/treasure_box_broken_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_broken_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_broken_texlist.inc"
};

static Sint16 treasure_box_broken_polygons[291] = {
#include "shared/canyon_art/treasure_box/treasure_box_broken_polygons.inc"
};

static Sint32 treasure_box_broken_vertices[235] = {
#include "shared/canyon_art/treasure_box/treasure_box_broken_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_broken_model = {
#include "shared/canyon_art/treasure_box/treasure_box_broken_model.inc"
};

static NJS_CNK_OBJECT treasure_box_broken_object = {
#include "shared/canyon_art/treasure_box/treasure_box_broken_object.inc"
};

/* These objects form the original five-shard tree. The debris task uses
 * a second set of meshes with the same vertex and polygon data. */
static NJS_TEXNAME treasure_box_shards_texlist_names[1] = {
#include "shared/canyon_art/treasure_box/treasure_box_shards_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_shards_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_shards_texlist.inc"
};

static Sint16 treasure_box_shard1_polygons[221] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard1_polygons.inc"
};

static Sint32 treasure_box_shard1_vertices[139] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard1_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_shard1_model = {
#include "shared/canyon_art/treasure_box/treasure_box_shard1_model.inc"
};

static NJS_CNK_OBJECT treasure_box_shard1 = {
#include "shared/canyon_art/treasure_box/treasure_box_shard1.inc"
};

static Sint16 treasure_box_shard5_polygons[71] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard5_polygons.inc"
};

static Sint32 treasure_box_shard5_vertices[67] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard5_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_shard5_model = {
#include "shared/canyon_art/treasure_box/treasure_box_shard5_model.inc"
};

static NJS_CNK_OBJECT treasure_box_shard5 = {
#include "shared/canyon_art/treasure_box/treasure_box_shard5.inc"
};

static Sint16 treasure_box_shard4_polygons[91] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard4_polygons.inc"
};

static Sint32 treasure_box_shard4_vertices[99] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard4_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_shard4_model = {
#include "shared/canyon_art/treasure_box/treasure_box_shard4_model.inc"
};

static NJS_CNK_OBJECT treasure_box_shard4 = {
#include "shared/canyon_art/treasure_box/treasure_box_shard4.inc"
};

static Sint16 treasure_box_shard2_polygons[129] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard2_polygons.inc"
};

static Sint32 treasure_box_shard2_vertices[147] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard2_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_shard2_model = {
#include "shared/canyon_art/treasure_box/treasure_box_shard2_model.inc"
};

static NJS_CNK_OBJECT treasure_box_shard2 = {
#include "shared/canyon_art/treasure_box/treasure_box_shard2.inc"
};

static Sint16 treasure_box_shard3_polygons[135] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard3_polygons.inc"
};

static Sint32 treasure_box_shard3_vertices[131] = {
#include "shared/canyon_art/treasure_box/treasure_box_shard3_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_shard3_model = {
#include "shared/canyon_art/treasure_box/treasure_box_shard3_model.inc"
};

static NJS_CNK_OBJECT treasure_box_shard3 = {
#include "shared/canyon_art/treasure_box/treasure_box_shard3.inc"
};

static NJS_CNK_OBJECT treasure_box_shards = {
#include "shared/canyon_art/treasure_box/treasure_box_shards.inc"
};

static NJS_MATERIAL treasure_box_collision_materials = {
#include "shared/canyon_art/treasure_box/treasure_box_collision_materials.inc"
};

static Uint16 treasure_box_collision_strips[31] = {
#include "shared/canyon_art/treasure_box/treasure_box_collision_strips.inc"
};

static NJS_MESHSET treasure_box_collision_faces = {
#include "shared/canyon_art/treasure_box/treasure_box_collision_faces.inc"
};

static NJS_POINT3 treasure_box_collision_points[14] = {
#include "shared/canyon_art/treasure_box/treasure_box_collision_points.inc"
};

static NJS_VECTOR treasure_box_collision_normals[14] = {
#include "shared/canyon_art/treasure_box/treasure_box_collision_normals.inc"
};

static NJS_MODEL treasure_box_collision_model = {
#include "shared/canyon_art/treasure_box/treasure_box_collision_model.inc"
};

static NJS_OBJECT treasure_box_collision = {
#include "shared/canyon_art/treasure_box/treasure_box_collision.inc"
};

static NJS_TEXNAME treasure_box_piece1_texlist_names[1] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece1_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_piece1_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_piece1_texlist.inc"
};

static Sint16 treasure_box_piece1_polygons[221] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece1_polygons.inc"
};

static Sint32 treasure_box_piece1_vertices[139] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece1_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_piece1_model = {
#include "shared/canyon_art/treasure_box/treasure_box_piece1_model.inc"
};

static NJS_CNK_OBJECT treasure_box_piece1_object = {
#include "shared/canyon_art/treasure_box/treasure_box_piece1_object.inc"
};

static NJS_TEXNAME treasure_box_piece2_texlist_names[1] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece2_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_piece2_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_piece2_texlist.inc"
};

static Sint16 treasure_box_piece2_polygons[129] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece2_polygons.inc"
};

static Sint32 treasure_box_piece2_vertices[147] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece2_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_piece2_model = {
#include "shared/canyon_art/treasure_box/treasure_box_piece2_model.inc"
};

static NJS_CNK_OBJECT treasure_box_piece2_object = {
#include "shared/canyon_art/treasure_box/treasure_box_piece2_object.inc"
};

static NJS_TEXNAME treasure_box_piece3_texlist_names[1] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece3_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_piece3_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_piece3_texlist.inc"
};

static Sint16 treasure_box_piece3_polygons[135] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece3_polygons.inc"
};

static Sint32 treasure_box_piece3_vertices[131] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece3_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_piece3_model = {
#include "shared/canyon_art/treasure_box/treasure_box_piece3_model.inc"
};

static NJS_CNK_OBJECT treasure_box_piece3_object = {
#include "shared/canyon_art/treasure_box/treasure_box_piece3_object.inc"
};

static NJS_TEXNAME treasure_box_piece4_texlist_names[1] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece4_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_piece4_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_piece4_texlist.inc"
};

static Sint16 treasure_box_piece4_polygons[91] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece4_polygons.inc"
};

static Sint32 treasure_box_piece4_vertices[99] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece4_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_piece4_model = {
#include "shared/canyon_art/treasure_box/treasure_box_piece4_model.inc"
};

static NJS_CNK_OBJECT treasure_box_piece4_object = {
#include "shared/canyon_art/treasure_box/treasure_box_piece4_object.inc"
};

static NJS_TEXNAME treasure_box_piece5_texlist_names[1] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece5_texlist_names.inc"
};

static NJS_TEXLIST treasure_box_piece5_texlist = {
#include "shared/canyon_art/treasure_box/treasure_box_piece5_texlist.inc"
};

static Sint16 treasure_box_piece5_polygons[71] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece5_polygons.inc"
};

static Sint32 treasure_box_piece5_vertices[67] = {
#include "shared/canyon_art/treasure_box/treasure_box_piece5_vertices.inc"
};

static NJS_CNK_MODEL treasure_box_piece5_model = {
#include "shared/canyon_art/treasure_box/treasure_box_piece5_model.inc"
};

static NJS_CNK_OBJECT treasure_box_piece5_object = {
#include "shared/canyon_art/treasure_box/treasure_box_piece5_object.inc"
};

#endif
