/* Shed. */
#ifndef STG05_SCENERY_ART_SHED_H
#define STG05_SCENERY_ART_SHED_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME shed_texlist_names[4] = {
#include "stg05/scenery_art/shed/shed_texlist_names.inc"
};

static NJS_TEXLIST shed_texlist = {
#include "stg05/scenery_art/shed/shed_texlist.inc"
};

static Sint16 shed_polygons[565] = {
#include "stg05/scenery_art/shed/shed_polygons.inc"
};

static Sint32 shed_vertices[415] = {
#include "stg05/scenery_art/shed/shed_vertices.inc"
};

static NJS_CNK_MODEL shed_model = {
#include "stg05/scenery_art/shed/shed_model.inc"
};

static NJS_CNK_OBJECT shed_object = {
#include "stg05/scenery_art/shed/shed_object.inc"
};

static NJS_TEXNAME shed_piece_0_texlist_names[1] = {
#include "stg05/scenery_art/shed/shed_piece_0_texlist_names.inc"
};

static NJS_TEXLIST shed_piece_0_texlist = {
#include "stg05/scenery_art/shed/shed_piece_0_texlist.inc"
};

static Sint16 shed_piece_0_polygons[71] = {
#include "stg05/scenery_art/shed/shed_piece_0_polygons.inc"
};

static Sint32 shed_piece_0_vertices[39] = {
#include "stg05/scenery_art/shed/shed_piece_0_vertices.inc"
};

static NJS_CNK_MODEL shed_piece_0_model = {
#include "stg05/scenery_art/shed/shed_piece_0_model.inc"
};

static NJS_CNK_OBJECT shed_piece_0_object = {
#include "stg05/scenery_art/shed/shed_piece_0_object.inc"
};

static NJS_TEXNAME shed_piece_1_texlist_names[2] = {
#include "stg05/scenery_art/shed/shed_piece_1_texlist_names.inc"
};

static NJS_TEXLIST shed_piece_1_texlist = {
#include "stg05/scenery_art/shed/shed_piece_1_texlist.inc"
};

static Sint16 shed_piece_1_polygons[193] = {
#include "stg05/scenery_art/shed/shed_piece_1_polygons.inc"
};

static Sint32 shed_piece_1_vertices[75] = {
#include "stg05/scenery_art/shed/shed_piece_1_vertices.inc"
};

static NJS_CNK_MODEL shed_piece_1_model = {
#include "stg05/scenery_art/shed/shed_piece_1_model.inc"
};

static NJS_CNK_OBJECT shed_piece_1_object = {
#include "stg05/scenery_art/shed/shed_piece_1_object.inc"
};

static NJS_TEXNAME shed_piece_2_texlist_names[1] = {
#include "stg05/scenery_art/shed/shed_piece_2_texlist_names.inc"
};

static NJS_TEXLIST shed_piece_2_texlist = {
#include "stg05/scenery_art/shed/shed_piece_2_texlist.inc"
};

static Sint16 shed_piece_2_polygons[89] = {
#include "stg05/scenery_art/shed/shed_piece_2_polygons.inc"
};

static Sint32 shed_piece_2_vertices[43] = {
#include "stg05/scenery_art/shed/shed_piece_2_vertices.inc"
};

static NJS_CNK_MODEL shed_piece_2_model = {
#include "stg05/scenery_art/shed/shed_piece_2_model.inc"
};

static NJS_CNK_OBJECT shed_piece_2_object = {
#include "stg05/scenery_art/shed/shed_piece_2_object.inc"
};

#endif
