/* STG23.PRS's texture lists and chunk models, from the disc. its D lies
 * between solid_box_e.c and pot.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/pot_models.h"
#include "stg23/pot_textures.h"

static NJS_TEXNAME hidden_base_bright_pot_textures[2] = {
#include "stg23/hidden_base_bright_pot/hidden_base_bright_pot_textures.inc"
};
NJS_TEXLIST bright_pot_texlist = {
#include "stg23/hidden_base_bright_pot/bright_pot_texlist.inc"
};
static Sint16 bright_pot_polygons[313] = {
#include "stg23/hidden_base_bright_pot/bright_pot_polygons.inc"
};
static Sint32 bright_pot_vertices[147] = {
#include "stg23/hidden_base_bright_pot/bright_pot_vertices.inc"
};
NJS_CNK_MODEL bright_pot_model = {
#include "stg23/hidden_base_bright_pot/bright_pot_model.inc"
};
static NJS_CNK_OBJECT bright_pot_object = {
#include "stg23/hidden_base_bright_pot/bright_pot_object.inc"
};
static NJS_TEXNAME hidden_base_bright_fragments_textures[1] = {
#include "stg23/hidden_base_bright_pot/hidden_base_bright_fragments_textures.inc"
};
NJS_TEXLIST bright_fragments_texlist = {
#include "stg23/hidden_base_bright_pot/bright_fragments_texlist.inc"
};
static Sint16 bright_pot_fragment_0_polygons[133] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_0_polygons.inc"
};
static Sint32 bright_pot_fragment_0_vertices[67] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_0_vertices.inc"
};
NJS_CNK_MODEL bright_pot_fragment_0 = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_0.inc"
};
static NJS_CNK_OBJECT bright_pot_fragment_0_object = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_0_object.inc"
};
static Sint16 bright_pot_fragment_2_polygons[75] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_2_polygons.inc"
};
static Sint32 bright_pot_fragment_2_vertices[43] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_2_vertices.inc"
};
NJS_CNK_MODEL bright_pot_fragment_2 = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_2.inc"
};
static NJS_CNK_OBJECT bright_pot_fragment_2_object = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_2_object.inc"
};
static Sint16 bright_pot_fragment_4_polygons[65] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_4_polygons.inc"
};
static Sint32 bright_pot_fragment_4_vertices[35] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_4_vertices.inc"
};
NJS_CNK_MODEL bright_pot_fragment_4 = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_4.inc"
};
static NJS_CNK_OBJECT bright_pot_fragment_4_object = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_4_object.inc"
};
static Sint16 bright_pot_fragment_3_polygons[63] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_3_polygons.inc"
};
static Sint32 bright_pot_fragment_3_vertices[35] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_3_vertices.inc"
};
NJS_CNK_MODEL bright_pot_fragment_3 = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_3.inc"
};
static NJS_CNK_OBJECT bright_pot_fragment_3_object = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_3_object.inc"
};
static Sint16 bright_pot_fragment_1_polygons[87] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_1_polygons.inc"
};
static Sint32 bright_pot_fragment_1_vertices[51] = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_1_vertices.inc"
};
NJS_CNK_MODEL bright_pot_fragment_1 = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_1.inc"
};
static NJS_CNK_OBJECT bright_pot_fragment_1_object = {
#include "stg23/hidden_base_bright_pot/bright_pot_fragment_1_object.inc"
};
static NJS_CNK_OBJECT bright_fragments_root = {
#include "stg23/hidden_base_bright_pot/bright_fragments_root.inc"
};
static NJS_TEXNAME hidden_base_dark_pot_textures[1] = {
#include "stg23/hidden_base_bright_pot/hidden_base_dark_pot_textures.inc"
};
NJS_TEXLIST dark_pot_texlist = {
#include "stg23/hidden_base_bright_pot/dark_pot_texlist.inc"
};
static Sint16 dark_pot_polygons[283] = {
#include "stg23/hidden_base_bright_pot/dark_pot_polygons.inc"
};
static Sint32 dark_pot_vertices[123] = {
#include "stg23/hidden_base_bright_pot/dark_pot_vertices.inc"
};
NJS_CNK_MODEL dark_pot_model = {
#include "stg23/hidden_base_bright_pot/dark_pot_model.inc"
};
static NJS_CNK_OBJECT dark_pot_object = {
#include "stg23/hidden_base_bright_pot/dark_pot_object.inc"
};
static NJS_TEXNAME hidden_base_dark_fragments_textures[1] = {
#include "stg23/hidden_base_bright_pot/hidden_base_dark_fragments_textures.inc"
};
NJS_TEXLIST dark_fragments_texlist = {
#include "stg23/hidden_base_bright_pot/dark_fragments_texlist.inc"
};
static Sint16 dark_pot_fragment_0_polygons[133] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_0_polygons.inc"
};
static Sint32 dark_pot_fragment_0_vertices[67] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_0_vertices.inc"
};
NJS_CNK_MODEL dark_pot_fragment_0 = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_0.inc"
};
static NJS_CNK_OBJECT dark_pot_fragment_0_object = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_0_object.inc"
};
static Sint16 dark_pot_fragment_4_polygons[65] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_4_polygons.inc"
};
static Sint32 dark_pot_fragment_4_vertices[35] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_4_vertices.inc"
};
NJS_CNK_MODEL dark_pot_fragment_4 = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_4.inc"
};
static NJS_CNK_OBJECT dark_pot_fragment_4_object = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_4_object.inc"
};
static Sint16 dark_pot_fragment_3_polygons[63] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_3_polygons.inc"
};
static Sint32 dark_pot_fragment_3_vertices[35] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_3_vertices.inc"
};
NJS_CNK_MODEL dark_pot_fragment_3 = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_3.inc"
};
static NJS_CNK_OBJECT dark_pot_fragment_3_object = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_3_object.inc"
};
static Sint16 dark_pot_fragment_2_polygons[75] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_2_polygons.inc"
};
static Sint32 dark_pot_fragment_2_vertices[43] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_2_vertices.inc"
};
NJS_CNK_MODEL dark_pot_fragment_2 = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_2.inc"
};
static NJS_CNK_OBJECT dark_pot_fragment_2_object = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_2_object.inc"
};
static Sint16 dark_pot_fragment_1_polygons[87] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_1_polygons.inc"
};
static Sint32 dark_pot_fragment_1_vertices[51] = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_1_vertices.inc"
};
NJS_CNK_MODEL dark_pot_fragment_1 = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_1.inc"
};
static NJS_CNK_OBJECT dark_pot_fragment_1_object = {
#include "stg23/hidden_base_bright_pot/dark_pot_fragment_1_object.inc"
};
static NJS_CNK_OBJECT dark_fragments_root = {
#include "stg23/hidden_base_bright_pot/dark_fragments_root.inc"
};
