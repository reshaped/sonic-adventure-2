#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/stacked_platform.h"

static const char material_asiba1[16] = {
#include "stg21/ocean_tier_art/material_asiba1.inc"
};

static const char material_asiba3[16] = {
#include "stg21/ocean_tier_art/material_asiba3.inc"
};

static const char material_asiba2[16] = {
#include "stg21/ocean_tier_art/material_asiba2.inc"
};

static const char material_asiba4[16] = {
#include "stg21/ocean_tier_art/material_asiba4.inc"
};

static const char material_kage3[12] = {
#include "stg21/ocean_tier_art/material_kage3.inc"
};

static NJS_TEXNAME tier_bright_shell_textures[2] = {
#include "stg21/ocean_tier_art/tier_bright_shell_textures.inc"
};

NJS_TEXLIST tier_bright_shell_texlist = {
#include "stg21/ocean_tier_art/tier_bright_shell_texlist.inc"
};

static Sint16 tier_bright_shell_polygons[165] = {
#include "stg21/ocean_tier_art/tier_bright_shell_polygons.inc"
};

static Sint32 tier_bright_shell_vertices[115] = {
#include "stg21/ocean_tier_art/tier_bright_shell_vertices.inc"
};

NJS_CNK_MODEL tier_bright_shell = {
#include "stg21/ocean_tier_art/tier_bright_shell.inc"
};

static NJS_CNK_OBJECT tier_bright_shell_object = {
#include "stg21/ocean_tier_art/tier_bright_shell_object.inc"
};

static NJS_TEXNAME tier_bright_base_textures[3] = {
#include "stg21/ocean_tier_art/tier_bright_base_textures.inc"
};

NJS_TEXLIST tier_bright_base_texlist = {
#include "stg21/ocean_tier_art/tier_bright_base_texlist.inc"
};

static Sint16 tier_bright_base_polygons[491] = {
#include "stg21/ocean_tier_art/tier_bright_base_polygons.inc"
};

static Sint32 tier_bright_base_vertices[511] = {
#include "stg21/ocean_tier_art/tier_bright_base_vertices.inc"
};

NJS_CNK_MODEL tier_bright_base = {
#include "stg21/ocean_tier_art/tier_bright_base.inc"
};

static NJS_CNK_OBJECT tier_bright_base_object = {
#include "stg21/ocean_tier_art/tier_bright_base_object.inc"
};

static NJS_TEXNAME tier_dark_shell_textures[2] = {
#include "stg21/ocean_tier_art/tier_dark_shell_textures.inc"
};

NJS_TEXLIST tier_dark_shell_texlist = {
#include "stg21/ocean_tier_art/tier_dark_shell_texlist.inc"
};

static Sint16 tier_dark_shell_polygons[165] = {
#include "stg21/ocean_tier_art/tier_dark_shell_polygons.inc"
};

static Sint32 tier_dark_shell_vertices[115] = {
#include "stg21/ocean_tier_art/tier_dark_shell_vertices.inc"
};

NJS_CNK_MODEL tier_dark_shell = {
#include "stg21/ocean_tier_art/tier_dark_shell.inc"
};

static NJS_CNK_OBJECT tier_dark_shell_object = {
#include "stg21/ocean_tier_art/tier_dark_shell_object.inc"
};

static NJS_TEXNAME tier_dark_base_textures[3] = {
#include "stg21/ocean_tier_art/tier_dark_base_textures.inc"
};

NJS_TEXLIST tier_dark_base_texlist = {
#include "stg21/ocean_tier_art/tier_dark_base_texlist.inc"
};

static Sint16 tier_dark_base_polygons[499] = {
#include "stg21/ocean_tier_art/tier_dark_base_polygons.inc"
};

static Sint32 tier_dark_base_vertices[515] = {
#include "stg21/ocean_tier_art/tier_dark_base_vertices.inc"
};

NJS_CNK_MODEL tier_dark_base = {
#include "stg21/ocean_tier_art/tier_dark_base.inc"
};

static NJS_CNK_OBJECT tier_dark_base_object = {
#include "stg21/ocean_tier_art/tier_dark_base_object.inc"
};

static NJS_TEXNAME stack_shadow_textures[1] = {
#include "stg21/ocean_tier_art/stack_shadow_textures.inc"
};

NJS_TEXLIST stack_shadow_texlist = {
#include "stg21/ocean_tier_art/stack_shadow_texlist.inc"
};

static Sint16 stack_shadow_polygons[25] = {
#include "stg21/ocean_tier_art/stack_shadow_polygons.inc"
};

static Sint32 stack_shadow_vertices[19] = {
#include "stg21/ocean_tier_art/stack_shadow_vertices.inc"
};

NJS_CNK_MODEL stack_shadow = {
#include "stg21/ocean_tier_art/stack_shadow.inc"
};

static NJS_CNK_OBJECT stack_shadow_object = {
#include "stg21/ocean_tier_art/stack_shadow_object.inc"
};

static NJS_TEXNAME tier_fragment_0_textures[1] = {
#include "stg21/ocean_tier_art/tier_fragment_0_textures.inc"
};

NJS_TEXLIST tier_fragment_0_texlist = {
#include "stg21/ocean_tier_art/tier_fragment_0_texlist.inc"
};

static Sint16 tier_fragment_0_polygons[93] = {
#include "stg21/ocean_tier_art/tier_fragment_0_polygons.inc"
};

static Sint32 tier_fragment_0_vertices[67] = {
#include "stg21/ocean_tier_art/tier_fragment_0_vertices.inc"
};

NJS_CNK_MODEL tier_fragment_0 = {
#include "stg21/ocean_tier_art/tier_fragment_0.inc"
};

static NJS_CNK_OBJECT tier_fragment_0_object = {
#include "stg21/ocean_tier_art/tier_fragment_0_object.inc"
};

static NJS_TEXNAME tier_fragment_1_textures[2] = {
#include "stg21/ocean_tier_art/tier_fragment_1_textures.inc"
};

NJS_TEXLIST tier_fragment_1_texlist = {
#include "stg21/ocean_tier_art/tier_fragment_1_texlist.inc"
};

static Sint16 tier_fragment_1_polygons[49] = {
#include "stg21/ocean_tier_art/tier_fragment_1_polygons.inc"
};

static Sint32 tier_fragment_1_vertices[43] = {
#include "stg21/ocean_tier_art/tier_fragment_1_vertices.inc"
};

NJS_CNK_MODEL tier_fragment_1 = {
#include "stg21/ocean_tier_art/tier_fragment_1.inc"
};

static NJS_CNK_OBJECT tier_fragment_1_object = {
#include "stg21/ocean_tier_art/tier_fragment_1_object.inc"
};

static NJS_TEXNAME tier_fragment_2_textures[1] = {
#include "stg21/ocean_tier_art/tier_fragment_2_textures.inc"
};

NJS_TEXLIST tier_fragment_2_texlist = {
#include "stg21/ocean_tier_art/tier_fragment_2_texlist.inc"
};

static Sint16 tier_fragment_2_polygons[35] = {
#include "stg21/ocean_tier_art/tier_fragment_2_polygons.inc"
};

static Sint32 tier_fragment_2_vertices[31] = {
#include "stg21/ocean_tier_art/tier_fragment_2_vertices.inc"
};

NJS_CNK_MODEL tier_fragment_2 = {
#include "stg21/ocean_tier_art/tier_fragment_2.inc"
};

static NJS_CNK_OBJECT tier_fragment_2_object = {
#include "stg21/ocean_tier_art/tier_fragment_2_object.inc"
};

static NJS_TEXNAME tier_fragment_3_textures[1] = {
#include "stg21/ocean_tier_art/tier_fragment_3_textures.inc"
};

NJS_TEXLIST tier_fragment_3_texlist = {
#include "stg21/ocean_tier_art/tier_fragment_3_texlist.inc"
};

static Sint16 tier_fragment_3_polygons[45] = {
#include "stg21/ocean_tier_art/tier_fragment_3_polygons.inc"
};

static Sint32 tier_fragment_3_vertices[35] = {
#include "stg21/ocean_tier_art/tier_fragment_3_vertices.inc"
};

NJS_CNK_MODEL tier_fragment_3 = {
#include "stg21/ocean_tier_art/tier_fragment_3.inc"
};

static NJS_CNK_OBJECT tier_fragment_3_object = {
#include "stg21/ocean_tier_art/tier_fragment_3_object.inc"
};

static NJS_TEXNAME tier_fragment_4_textures[1] = {
#include "stg21/ocean_tier_art/tier_fragment_4_textures.inc"
};

NJS_TEXLIST tier_fragment_4_texlist = {
#include "stg21/ocean_tier_art/tier_fragment_4_texlist.inc"
};

static Sint16 tier_fragment_4_polygons[41] = {
#include "stg21/ocean_tier_art/tier_fragment_4_polygons.inc"
};

static Sint32 tier_fragment_4_vertices[31] = {
#include "stg21/ocean_tier_art/tier_fragment_4_vertices.inc"
};

NJS_CNK_MODEL tier_fragment_4 = {
#include "stg21/ocean_tier_art/tier_fragment_4.inc"
};

static NJS_CNK_OBJECT tier_fragment_4_object = {
#include "stg21/ocean_tier_art/tier_fragment_4_object.inc"
};

static NJS_TEXNAME tier_fragment_5_textures[1] = {
#include "stg21/ocean_tier_art/tier_fragment_5_textures.inc"
};

NJS_TEXLIST tier_fragment_5_texlist = {
#include "stg21/ocean_tier_art/tier_fragment_5_texlist.inc"
};

static Sint16 tier_fragment_5_polygons[37] = {
#include "stg21/ocean_tier_art/tier_fragment_5_polygons.inc"
};

static Sint32 tier_fragment_5_vertices[35] = {
#include "stg21/ocean_tier_art/tier_fragment_5_vertices.inc"
};

NJS_CNK_MODEL tier_fragment_5 = {
#include "stg21/ocean_tier_art/tier_fragment_5.inc"
};

static NJS_CNK_OBJECT tier_fragment_5_object = {
#include "stg21/ocean_tier_art/tier_fragment_5_object.inc"
};

static NJS_TEXNAME tier_fragment_6_textures[1] = {
#include "stg21/ocean_tier_art/tier_fragment_6_textures.inc"
};

NJS_TEXLIST tier_fragment_6_texlist = {
#include "stg21/ocean_tier_art/tier_fragment_6_texlist.inc"
};

static Sint16 tier_fragment_6_polygons[45] = {
#include "stg21/ocean_tier_art/tier_fragment_6_polygons.inc"
};

static Sint32 tier_fragment_6_vertices[35] = {
#include "stg21/ocean_tier_art/tier_fragment_6_vertices.inc"
};

NJS_CNK_MODEL tier_fragment_6 = {
#include "stg21/ocean_tier_art/tier_fragment_6.inc"
};

static NJS_CNK_OBJECT tier_fragment_6_object = {
#include "stg21/ocean_tier_art/tier_fragment_6_object.inc"
};
