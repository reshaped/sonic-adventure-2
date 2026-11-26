/* STG23.PRS's texture lists and chunk models, from the disc. its D lies
 * between tilted_flag.c and stacked_platform.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stacked_platform.h"
#include "stg23/tier_models.h"
#include "stg23/tier_textures.h"

static NJS_TEXNAME hidden_base_tier_front_base_textures[3] = {
#include "stg23/hidden_base_tier_front_base/hidden_base_tier_front_base_textures.inc"
};
NJS_TEXLIST tier_front_base_texlist = {
#include "stg23/hidden_base_tier_front_base/tier_front_base_texlist.inc"
};
static Sint16 tier_front_base_polygons[443] = {
#include "stg23/hidden_base_tier_front_base/tier_front_base_polygons.inc"
};
static Sint32 tier_front_base_vertices[419] = {
#include "stg23/hidden_base_tier_front_base/tier_front_base_vertices.inc"
};
NJS_CNK_MODEL tier_front_base = {
#include "stg23/hidden_base_tier_front_base/tier_front_base.inc"
};
static NJS_CNK_OBJECT tier_front_base_object = {
#include "stg23/hidden_base_tier_front_base/tier_front_base_object.inc"
};
static NJS_TEXNAME hidden_base_tier_front_shell_textures[2] = {
#include "stg23/hidden_base_tier_front_base/hidden_base_tier_front_shell_textures.inc"
};
NJS_TEXLIST tier_front_shell_texlist = {
#include "stg23/hidden_base_tier_front_base/tier_front_shell_texlist.inc"
};
static Sint16 tier_front_shell_polygons[179] = {
#include "stg23/hidden_base_tier_front_base/tier_front_shell_polygons.inc"
};
static Sint32 tier_front_shell_vertices[115] = {
#include "stg23/hidden_base_tier_front_base/tier_front_shell_vertices.inc"
};
NJS_CNK_MODEL tier_front_shell = {
#include "stg23/hidden_base_tier_front_base/tier_front_shell.inc"
};
static NJS_CNK_OBJECT tier_front_shell_object = {
#include "stg23/hidden_base_tier_front_base/tier_front_shell_object.inc"
};
static NJS_TEXNAME hidden_base_tier_side_base_textures[3] = {
#include "stg23/hidden_base_tier_front_base/hidden_base_tier_side_base_textures.inc"
};
NJS_TEXLIST tier_side_base_texlist = {
#include "stg23/hidden_base_tier_front_base/tier_side_base_texlist.inc"
};
static Sint16 tier_side_base_polygons[443] = {
#include "stg23/hidden_base_tier_front_base/tier_side_base_polygons.inc"
};
static Sint32 tier_side_base_vertices[419] = {
#include "stg23/hidden_base_tier_front_base/tier_side_base_vertices.inc"
};
NJS_CNK_MODEL tier_side_base = {
#include "stg23/hidden_base_tier_front_base/tier_side_base.inc"
};
static NJS_CNK_OBJECT tier_side_base_object = {
#include "stg23/hidden_base_tier_front_base/tier_side_base_object.inc"
};
static NJS_TEXNAME hidden_base_tier_side_shell_textures[2] = {
#include "stg23/hidden_base_tier_front_base/hidden_base_tier_side_shell_textures.inc"
};
NJS_TEXLIST tier_side_shell_texlist = {
#include "stg23/hidden_base_tier_front_base/tier_side_shell_texlist.inc"
};
static Sint16 tier_side_shell_polygons[179] = {
#include "stg23/hidden_base_tier_front_base/tier_side_shell_polygons.inc"
};
static Sint32 tier_side_shell_vertices[115] = {
#include "stg23/hidden_base_tier_front_base/tier_side_shell_vertices.inc"
};
NJS_CNK_MODEL tier_side_shell = {
#include "stg23/hidden_base_tier_front_base/tier_side_shell.inc"
};
static NJS_CNK_OBJECT tier_side_shell_object = {
#include "stg23/hidden_base_tier_front_base/tier_side_shell_object.inc"
};
static NJS_TEXNAME hidden_base_tier_fragments_textures[2] = {
#include "stg23/hidden_base_tier_front_base/hidden_base_tier_fragments_textures.inc"
};
NJS_TEXLIST tier_fragments_texlist = {
#include "stg23/hidden_base_tier_front_base/tier_fragments_texlist.inc"
};
static Sint16 tier_fragment_0_polygons[93] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_0_polygons.inc"
};
static Sint32 tier_fragment_0_vertices[67] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_0_vertices.inc"
};
NJS_CNK_MODEL tier_fragment_0 = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_0.inc"
};
static NJS_CNK_OBJECT tier_fragment_0_object = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_0_object.inc"
};
static Sint16 tier_fragment_6_polygons[45] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_6_polygons.inc"
};
static Sint32 tier_fragment_6_vertices[35] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_6_vertices.inc"
};
NJS_CNK_MODEL tier_fragment_6 = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_6.inc"
};
static NJS_CNK_OBJECT tier_fragment_6_object = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_6_object.inc"
};
static Sint16 tier_fragment_5_polygons[37] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_5_polygons.inc"
};
static Sint32 tier_fragment_5_vertices[35] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_5_vertices.inc"
};
NJS_CNK_MODEL tier_fragment_5 = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_5.inc"
};
static NJS_CNK_OBJECT tier_fragment_5_object = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_5_object.inc"
};
static Sint16 tier_fragment_4_polygons[41] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_4_polygons.inc"
};
static Sint32 tier_fragment_4_vertices[31] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_4_vertices.inc"
};
NJS_CNK_MODEL tier_fragment_4 = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_4.inc"
};
static NJS_CNK_OBJECT tier_fragment_4_object = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_4_object.inc"
};
static Sint16 tier_fragment_3_polygons[45] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_3_polygons.inc"
};
static Sint32 tier_fragment_3_vertices[35] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_3_vertices.inc"
};
NJS_CNK_MODEL tier_fragment_3 = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_3.inc"
};
static NJS_CNK_OBJECT tier_fragment_3_object = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_3_object.inc"
};
static Sint16 tier_fragment_2_polygons[35] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_2_polygons.inc"
};
static Sint32 tier_fragment_2_vertices[31] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_2_vertices.inc"
};
NJS_CNK_MODEL tier_fragment_2 = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_2.inc"
};
static NJS_CNK_OBJECT tier_fragment_2_object = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_2_object.inc"
};
static Sint16 tier_fragment_1_polygons[49] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_1_polygons.inc"
};
static Sint32 tier_fragment_1_vertices[43] = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_1_vertices.inc"
};
NJS_CNK_MODEL tier_fragment_1 = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_1.inc"
};
static NJS_CNK_OBJECT tier_fragment_1_object = {
#include "stg23/hidden_base_tier_front_base/tier_fragment_1_object.inc"
};
static NJS_CNK_OBJECT tier_fragments_root = {
#include "stg23/hidden_base_tier_front_base/tier_fragments_root.inc"
};
