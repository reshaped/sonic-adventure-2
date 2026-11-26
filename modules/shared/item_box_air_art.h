/* The floating capsule and its ten-ring picture. */
#ifndef SHARED_ITEM_BOX_AIR_ART_H
#define SHARED_ITEM_BOX_AIR_ART_H

static NJS_TEXNAME item_box_air_textures[3] = {
#include "shared/item_box_air_art/item_box_air_textures.inc"
};

static NJS_TEXLIST item_box_air_texlist = {
#include "shared/item_box_air_art/item_box_air_texlist.inc"
};

static Sint16 item_box_air_middle_polygons[75] = {
#include "shared/item_box_air_art/item_box_air_middle_polygons.inc"
};

static Sint32 item_box_air_middle_vertices[183] = {
#include "shared/item_box_air_art/item_box_air_middle_vertices.inc"
};

static NJS_CNK_MODEL item_box_air_middle_model = {
#include "shared/item_box_air_art/item_box_air_middle_model.inc"
};

static NJS_CNK_OBJECT item_box_air_middle_object = {
#include "shared/item_box_air_art/item_box_air_middle_object.inc"
};

static Sint16 item_box_air_icon_polygons[25] = {
#include "shared/item_box_air_art/item_box_air_icon_polygons.inc"
};

static Sint32 item_box_air_icon_vertices[27] = {
#include "shared/item_box_air_art/item_box_air_icon_vertices.inc"
};

static NJS_CNK_MODEL item_box_air_icon_model = {
#include "shared/item_box_air_art/item_box_air_icon_model.inc"
};

static NJS_CNK_OBJECT item_box_air_icon_object = {
#include "shared/item_box_air_art/item_box_air_icon_object.inc"
};

static Sint16 item_box_air_upper_polygons[178] = {
#include "shared/item_box_air_art/item_box_air_upper_polygons.inc"
};

static Sint32 item_box_air_upper_vertices[87] = {
#include "shared/item_box_air_art/item_box_air_upper_vertices.inc"
};

static NJS_CNK_MODEL item_box_air_upper_model = {
#include "shared/item_box_air_art/item_box_air_upper_model.inc"
};

static NJS_CNK_OBJECT item_box_air_upper_object = {
#include "shared/item_box_air_art/item_box_air_upper_object.inc"
};

static Sint16 item_box_air_lower_polygons[217] = {
#include "shared/item_box_air_art/item_box_air_lower_polygons.inc"
};

static Sint32 item_box_air_lower_vertices[87] = {
#include "shared/item_box_air_art/item_box_air_lower_vertices.inc"
};

static NJS_CNK_MODEL item_box_air_lower_model = {
#include "shared/item_box_air_art/item_box_air_lower_model.inc"
};

static NJS_CNK_OBJECT item_box_air_lower_object = {
#include "shared/item_box_air_art/item_box_air_lower_object.inc"
};

#endif
