/* Rail platform, floor and ceiling. */
#ifndef STG06_SCENERY_ART_PNP_H
#define STG06_SCENERY_ART_PNP_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME pnp_texlist_names[4] = {
#include "stg06/scenery_art/pnp/pnp_texlist_names.inc"
};

static NJS_TEXLIST pnp_texlist = {
#include "stg06/scenery_art/pnp/pnp_texlist.inc"
};

static Sint16 pnp_object_child_sibling_polygons[179] = {
#include "stg06/scenery_art/pnp/pnp_object_child_sibling_polygons.inc"
};

static Sint32 pnp_object_child_sibling_vertices[135] = {
#include "stg06/scenery_art/pnp/pnp_object_child_sibling_vertices.inc"
};

static NJS_CNK_MODEL pnp_object_child_sibling_model = {
#include "stg06/scenery_art/pnp/pnp_object_child_sibling_model.inc"
};

static NJS_CNK_OBJECT pnp_object_child_sibling = {
#include "stg06/scenery_art/pnp/pnp_object_child_sibling.inc"
};

static Sint16 pnp_object_child_polygons[243] = {
#include "stg06/scenery_art/pnp/pnp_object_child_polygons.inc"
};

static Sint32 pnp_object_child_vertices[183] = {
#include "stg06/scenery_art/pnp/pnp_object_child_vertices.inc"
};

static NJS_CNK_MODEL pnp_object_child_model = {
#include "stg06/scenery_art/pnp/pnp_object_child_model.inc"
};

static NJS_CNK_OBJECT pnp_object_child = {
#include "stg06/scenery_art/pnp/pnp_object_child.inc"
};

static Sint16 pnp_polygons[331] = {
#include "stg06/scenery_art/pnp/pnp_polygons.inc"
};

static Sint32 pnp_vertices[267] = {
#include "stg06/scenery_art/pnp/pnp_vertices.inc"
};

static NJS_CNK_MODEL pnp_model = {
#include "stg06/scenery_art/pnp/pnp_model.inc"
};

static NJS_CNK_OBJECT pnp_object = {
#include "stg06/scenery_art/pnp/pnp_object.inc"
};

static NJS_TEXNAME pnp_floor_texlist_names[1] = {
#include "stg06/scenery_art/pnp/pnp_floor_texlist_names.inc"
};

static NJS_TEXLIST pnp_floor_texlist = {
#include "stg06/scenery_art/pnp/pnp_floor_texlist.inc"
};

static Sint16 pnp_floor_polygons[117] = {
#include "stg06/scenery_art/pnp/pnp_floor_polygons.inc"
};

static Sint32 pnp_floor_vertices[195] = {
#include "stg06/scenery_art/pnp/pnp_floor_vertices.inc"
};

static NJS_CNK_MODEL pnp_floor_model = {
#include "stg06/scenery_art/pnp/pnp_floor_model.inc"
};

static NJS_CNK_OBJECT pnp_floor_object[1] = {
#include "stg06/scenery_art/pnp/pnp_floor_object.inc"
};

static NJS_TEXNAME pnp_ceiling_texlist_names[1] = {
#include "stg06/scenery_art/pnp/pnp_ceiling_texlist_names.inc"
};

static NJS_TEXLIST pnp_ceiling_texlist = {
#include "stg06/scenery_art/pnp/pnp_ceiling_texlist.inc"
};

static Sint16 pnp_ceiling_polygons[93] = {
#include "stg06/scenery_art/pnp/pnp_ceiling_polygons.inc"
};

static Sint32 pnp_ceiling_vertices[147] = {
#include "stg06/scenery_art/pnp/pnp_ceiling_vertices.inc"
};

static NJS_CNK_MODEL pnp_ceiling_model = {
#include "stg06/scenery_art/pnp/pnp_ceiling_model.inc"
};

static NJS_CNK_OBJECT pnp_ceiling_object[1] = {
#include "stg06/scenery_art/pnp/pnp_ceiling_object.inc"
};

#endif
