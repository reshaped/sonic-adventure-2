#include <ninja.h>
#include <sg_xpt.h>

#include "shared/item_box.h"

#include "shared/item_box_art/text.inc"

/* the item boxes' three texture names, each on a four-byte boundary */
const char item_box_texture_filenames[45] = TEXT_1 TEXT_2 TEXT_3;

/* the texture lists and chunk models that 54 images place, from the disc.
 * its D precedes item_box.c */
static NJS_TEXNAME item_box_textures[3] = {
#include "shared/item_box_art/item_box_textures.inc"
};
NJS_TEXLIST item_box_texlist = {
#include "shared/item_box_art/item_box_texlist.inc"
};
static Sint16 item_box_icon_polygons[25] = {
#include "shared/item_box_art/item_box_icon_polygons.inc"
};
static Sint32 item_box_icon_vertices[27] = {
#include "shared/item_box_art/item_box_icon_vertices.inc"
};
static NJS_CNK_MODEL item_box_icon_model = {
#include "shared/item_box_art/item_box_icon_model.inc"
};
static NJS_CNK_OBJECT item_box_icon_object = {
#include "shared/item_box_art/item_box_icon_object.inc"
};
static Sint16 item_box_middle_polygons[59] = {
#include "shared/item_box_art/item_box_middle_polygons.inc"
};
static Sint32 item_box_middle_vertices[147] = {
#include "shared/item_box_art/item_box_middle_vertices.inc"
};
NJS_CNK_MODEL item_box_middle_model = {
#include "shared/item_box_art/item_box_middle_model.inc"
};
NJS_CNK_OBJECT item_box_middle_object = {
#include "shared/item_box_art/item_box_middle_object.inc"
};
static Sint16 item_box_upper_polygons[181] = {
#include "shared/item_box_art/item_box_upper_polygons.inc"
};
static Sint32 item_box_upper_vertices[87] = {
#include "shared/item_box_art/item_box_upper_vertices.inc"
};
NJS_CNK_MODEL item_box_upper_model = {
#include "shared/item_box_art/item_box_upper_model.inc"
};
NJS_CNK_OBJECT item_box_upper_object = {
#include "shared/item_box_art/item_box_upper_object.inc"
};
static Sint16 item_box_lower_polygons[313] = {
#include "shared/item_box_art/item_box_lower_polygons.inc"
};
static Sint32 item_box_lower_vertices[225] = {
#include "shared/item_box_art/item_box_lower_vertices.inc"
};
NJS_CNK_MODEL item_box_lower_model = {
#include "shared/item_box_art/item_box_lower_model.inc"
};
NJS_CNK_OBJECT item_box_lower_object = {
#include "shared/item_box_art/item_box_lower_object.inc"
};
static NJS_TEXNAME item_box_b_textures[3] = {
#include "shared/item_box_art/item_box_b_textures.inc"
};
NJS_TEXLIST item_box_b_texlist = {
#include "shared/item_box_art/item_box_b_texlist.inc"
};
static Sint16 item_box_b_icon_polygons[25] = {
#include "shared/item_box_art/item_box_b_icon_polygons.inc"
};
static Sint32 item_box_b_icon_vertices[27] = {
#include "shared/item_box_art/item_box_b_icon_vertices.inc"
};
static NJS_CNK_MODEL item_box_b_icon_model = {
#include "shared/item_box_art/item_box_b_icon_model.inc"
};
static NJS_CNK_OBJECT item_box_b_icon_object = {
#include "shared/item_box_art/item_box_b_icon_object.inc"
};
static Sint16 item_box_b_middle_polygons[59] = {
#include "shared/item_box_art/item_box_b_middle_polygons.inc"
};
static Sint32 item_box_b_middle_vertices[147] = {
#include "shared/item_box_art/item_box_b_middle_vertices.inc"
};
NJS_CNK_MODEL item_box_b_middle_model = {
#include "shared/item_box_art/item_box_b_middle_model.inc"
};
NJS_CNK_OBJECT item_box_b_middle_object = {
#include "shared/item_box_art/item_box_b_middle_object.inc"
};
static Sint16 item_box_b_upper_polygons[181] = {
#include "shared/item_box_art/item_box_b_upper_polygons.inc"
};
static Sint32 item_box_b_upper_vertices[87] = {
#include "shared/item_box_art/item_box_b_upper_vertices.inc"
};
NJS_CNK_MODEL item_box_b_upper_model = {
#include "shared/item_box_art/item_box_b_upper_model.inc"
};
NJS_CNK_OBJECT item_box_b_upper_object = {
#include "shared/item_box_art/item_box_b_upper_object.inc"
};
static Sint16 item_box_b_lower_polygons[313] = {
#include "shared/item_box_art/item_box_b_lower_polygons.inc"
};
static Sint32 item_box_b_lower_vertices[225] = {
#include "shared/item_box_art/item_box_b_lower_vertices.inc"
};
NJS_CNK_MODEL item_box_b_lower_model = {
#include "shared/item_box_art/item_box_b_lower_model.inc"
};
NJS_CNK_OBJECT item_box_b_lower_object = {
#include "shared/item_box_art/item_box_b_lower_object.inc"
};
