/* Sky Rail's swinging track sections and rotating machinery. */
#ifndef STG06_SCENERY_ART_RAIL_OBJECTS_H
#define STG06_SCENERY_ART_RAIL_OBJECTS_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME railobjcl_texlist_names[4] = {
#include "stg06/scenery_art/rail_objects/railobjcl_texlist_names.inc"
};

static NJS_TEXLIST railobjcl_texlist = {
#include "stg06/scenery_art/rail_objects/railobjcl_texlist.inc"
};

static Sint16 railobjcl_object_child_polygons[69] = {
#include "stg06/scenery_art/rail_objects/railobjcl_object_child_polygons.inc"
};

static Sint32 railobjcl_object_child_vertices[67] = {
#include "stg06/scenery_art/rail_objects/railobjcl_object_child_vertices.inc"
};

static NJS_CNK_MODEL railobjcl_object_child_model = {
#include "stg06/scenery_art/rail_objects/railobjcl_object_child_model.inc"
};

static NJS_CNK_OBJECT railobjcl_object_child = {
#include "stg06/scenery_art/rail_objects/railobjcl_object_child.inc"
};

static Sint16 railobjcl_polygons[453] = {
#include "stg06/scenery_art/rail_objects/railobjcl_polygons.inc"
};

static Sint32 railobjcl_vertices[379] = {
#include "stg06/scenery_art/rail_objects/railobjcl_vertices.inc"
};

static NJS_CNK_MODEL railobjcl_model = {
#include "stg06/scenery_art/rail_objects/railobjcl_model.inc"
};

static NJS_CNK_OBJECT railobjcl_object = {
#include "stg06/scenery_art/rail_objects/railobjcl_object.inc"
};

static NJS_TEXNAME railobjcr_texlist_names[4] = {
#include "stg06/scenery_art/rail_objects/railobjcr_texlist_names.inc"
};

static NJS_TEXLIST railobjcr_texlist = {
#include "stg06/scenery_art/rail_objects/railobjcr_texlist.inc"
};

static Sint16 railobjcr_object_child_polygons[69] = {
#include "stg06/scenery_art/rail_objects/railobjcr_object_child_polygons.inc"
};

static Sint32 railobjcr_object_child_vertices[67] = {
#include "stg06/scenery_art/rail_objects/railobjcr_object_child_vertices.inc"
};

static NJS_CNK_MODEL railobjcr_object_child_model = {
#include "stg06/scenery_art/rail_objects/railobjcr_object_child_model.inc"
};

static NJS_CNK_OBJECT railobjcr_object_child = {
#include "stg06/scenery_art/rail_objects/railobjcr_object_child.inc"
};

static Sint16 railobjcr_polygons[453] = {
#include "stg06/scenery_art/rail_objects/railobjcr_polygons.inc"
};

static Sint32 railobjcr_vertices[391] = {
#include "stg06/scenery_art/rail_objects/railobjcr_vertices.inc"
};

static NJS_CNK_MODEL railobjcr_model = {
#include "stg06/scenery_art/rail_objects/railobjcr_model.inc"
};

static NJS_CNK_OBJECT railobjcr_object = {
#include "stg06/scenery_art/rail_objects/railobjcr_object.inc"
};

static NJS_TEXNAME motool_texlist_names[7] = {
#include "stg06/scenery_art/rail_objects/motool_texlist_names.inc"
};

static NJS_TEXLIST motool_texlist = {
#include "stg06/scenery_art/rail_objects/motool_texlist.inc"
};

static Sint16 motool_object_child_polygons[489] = {
#include "stg06/scenery_art/rail_objects/motool_object_child_polygons.inc"
};

static Sint32 motool_object_child_vertices[471] = {
#include "stg06/scenery_art/rail_objects/motool_object_child_vertices.inc"
};

static NJS_CNK_MODEL motool_object_child_model = {
#include "stg06/scenery_art/rail_objects/motool_object_child_model.inc"
};

static NJS_CNK_OBJECT motool_object_child = {
#include "stg06/scenery_art/rail_objects/motool_object_child.inc"
};

static Sint16 motool_polygons[411] = {
#include "stg06/scenery_art/rail_objects/motool_polygons.inc"
};

static Sint32 motool_vertices[399] = {
#include "stg06/scenery_art/rail_objects/motool_vertices.inc"
};

static NJS_CNK_MODEL motool_model = {
#include "stg06/scenery_art/rail_objects/motool_model.inc"
};

static NJS_CNK_OBJECT motool_object = {
#include "stg06/scenery_art/rail_objects/motool_object.inc"
};

static NJS_MATERIAL motool_collision_materials = {
#include "stg06/scenery_art/rail_objects/motool_collision_materials.inc"
};

static Uint16 motool_collision_strips[14] = {
#include "stg06/scenery_art/rail_objects/motool_collision_strips.inc"
};

static NJS_MESHSET motool_collision_faces = {
#include "stg06/scenery_art/rail_objects/motool_collision_faces.inc"
};

static NJS_POINT3 motool_collision_points[12] = {
#include "stg06/scenery_art/rail_objects/motool_collision_points.inc"
};

static NJS_VECTOR motool_collision_normals[12] = {
#include "stg06/scenery_art/rail_objects/motool_collision_normals.inc"
};

static NJS_MODEL motool_collision_model = {
#include "stg06/scenery_art/rail_objects/motool_collision_model.inc"
};

static NJS_OBJECT motool_collision = {
#include "stg06/scenery_art/rail_objects/motool_collision.inc"
};

static NJS_TEXNAME railobjbl_texlist_names[6] = {
#include "stg06/scenery_art/rail_objects/railobjbl_texlist_names.inc"
};

static NJS_TEXLIST railobjbl_texlist = {
#include "stg06/scenery_art/rail_objects/railobjbl_texlist.inc"
};

static Sint16 railobjbl_object_child_sibling_sibling_polygons[39] = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling_sibling_polygons.inc"
};

static Sint32 railobjbl_object_child_sibling_sibling_vertices[35] = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling_sibling_vertices.inc"
};

static NJS_CNK_MODEL railobjbl_object_child_sibling_sibling_model = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling_sibling_model.inc"
};

static NJS_CNK_OBJECT railobjbl_object_child_sibling_sibling = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling_sibling.inc"
};

static Sint16 railobjbl_object_child_sibling_polygons[39] = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling_polygons.inc"
};

static Sint32 railobjbl_object_child_sibling_vertices[35] = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling_vertices.inc"
};

static NJS_CNK_MODEL railobjbl_object_child_sibling_model = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling_model.inc"
};

static NJS_CNK_OBJECT railobjbl_object_child_sibling = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_sibling.inc"
};

static Sint16 railobjbl_object_child_polygons[39] = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_polygons.inc"
};

static Sint32 railobjbl_object_child_vertices[35] = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_vertices.inc"
};

static NJS_CNK_MODEL railobjbl_object_child_model = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child_model.inc"
};

static NJS_CNK_OBJECT railobjbl_object_child = {
#include "stg06/scenery_art/rail_objects/railobjbl_object_child.inc"
};

static Sint16 railobjbl_polygons[391] = {
#include "stg06/scenery_art/rail_objects/railobjbl_polygons.inc"
};

static Sint32 railobjbl_vertices[407] = {
#include "stg06/scenery_art/rail_objects/railobjbl_vertices.inc"
};

static NJS_CNK_MODEL railobjbl_model = {
#include "stg06/scenery_art/rail_objects/railobjbl_model.inc"
};

static NJS_CNK_OBJECT railobjbl_object = {
#include "stg06/scenery_art/rail_objects/railobjbl_object.inc"
};

static NJS_TEXNAME railobjbr_texlist_names[6] = {
#include "stg06/scenery_art/rail_objects/railobjbr_texlist_names.inc"
};

static NJS_TEXLIST railobjbr_texlist = {
#include "stg06/scenery_art/rail_objects/railobjbr_texlist.inc"
};

static Sint16 railobjbr_object_child_sibling_sibling_polygons[39] = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling_sibling_polygons.inc"
};

static Sint32 railobjbr_object_child_sibling_sibling_vertices[35] = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling_sibling_vertices.inc"
};

static NJS_CNK_MODEL railobjbr_object_child_sibling_sibling_model = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling_sibling_model.inc"
};

static NJS_CNK_OBJECT railobjbr_object_child_sibling_sibling = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling_sibling.inc"
};

static Sint16 railobjbr_object_child_sibling_polygons[39] = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling_polygons.inc"
};

static Sint32 railobjbr_object_child_sibling_vertices[35] = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling_vertices.inc"
};

static NJS_CNK_MODEL railobjbr_object_child_sibling_model = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling_model.inc"
};

static NJS_CNK_OBJECT railobjbr_object_child_sibling = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_sibling.inc"
};

static Sint16 railobjbr_object_child_polygons[25] = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_polygons.inc"
};

static Sint32 railobjbr_object_child_vertices[19] = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_vertices.inc"
};

static NJS_CNK_MODEL railobjbr_object_child_model = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child_model.inc"
};

static NJS_CNK_OBJECT railobjbr_object_child = {
#include "stg06/scenery_art/rail_objects/railobjbr_object_child.inc"
};

static Sint16 railobjbr_polygons[391] = {
#include "stg06/scenery_art/rail_objects/railobjbr_polygons.inc"
};

static Sint32 railobjbr_vertices[407] = {
#include "stg06/scenery_art/rail_objects/railobjbr_vertices.inc"
};

static NJS_CNK_MODEL railobjbr_model = {
#include "stg06/scenery_art/rail_objects/railobjbr_model.inc"
};

static NJS_CNK_OBJECT railobjbr_object = {
#include "stg06/scenery_art/rail_objects/railobjbr_object.inc"
};

#endif
