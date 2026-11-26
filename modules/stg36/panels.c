#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg36/panels.h"

/* the texture name that the scrolling panels' texture list points at */
const char tails_panel_texture_name[13] = {
#include "stg36/panels/tails_panel_texture_name.inc"
};

/* STG36.PRS's UV scrolls, texture lists and chunk models, from the disc. its
 * D lies between core_mech_route.c and machinery_art.c */
static Sint16 panel_scroll_keys_1[42] = {
#include "stg36/panels/panel_scroll_keys_1.inc"
};
struct texture_anim tails_panel_uv_1 = {
#include "stg36/panels/tails_panel_uv_1.inc"
};
static NJS_TEXNAME panel_names_1[1] = {
#include "stg36/panels/panel_names_1.inc"
};
static NJS_TEXLIST panel_textures_1 = {
#include "stg36/panels/panel_textures_1.inc"
};
static Sint16 panel_polygons_1[65] = {
#include "stg36/panels/panel_polygons_1.inc"
};
static Sint32 panel_vertices_1[35] = {
#include "stg36/panels/panel_vertices_1.inc"
};
static NJS_CNK_MODEL panel_model_1 = {
#include "stg36/panels/panel_model_1.inc"
};
NJS_CNK_OBJECT tails_panel_1 = {
#include "stg36/panels/tails_panel_1.inc"
};
static Sint16 panel_scroll_keys_2[42] = {
#include "stg36/panels/panel_scroll_keys_2.inc"
};
struct texture_anim tails_panel_uv_2 = {
#include "stg36/panels/tails_panel_uv_2.inc"
};
static NJS_TEXNAME panel_names_2[1] = {
#include "stg36/panels/panel_names_2.inc"
};
static NJS_TEXLIST panel_textures_2 = {
#include "stg36/panels/panel_textures_2.inc"
};
static Sint16 panel_polygons_2[65] = {
#include "stg36/panels/panel_polygons_2.inc"
};
static Sint32 panel_vertices_2[35] = {
#include "stg36/panels/panel_vertices_2.inc"
};
static NJS_CNK_MODEL panel_model_2 = {
#include "stg36/panels/panel_model_2.inc"
};
NJS_CNK_OBJECT tails_panel_2 = {
#include "stg36/panels/tails_panel_2.inc"
};
static Sint16 panel_scroll_keys_3[42] = {
#include "stg36/panels/panel_scroll_keys_3.inc"
};
struct texture_anim tails_panel_uv_3 = {
#include "stg36/panels/tails_panel_uv_3.inc"
};
static NJS_TEXNAME panel_names_3[1] = {
#include "stg36/panels/panel_names_3.inc"
};
static NJS_TEXLIST panel_textures_3 = {
#include "stg36/panels/panel_textures_3.inc"
};
static Sint16 panel_polygons_3[65] = {
#include "stg36/panels/panel_polygons_3.inc"
};
static Sint32 panel_vertices_3[35] = {
#include "stg36/panels/panel_vertices_3.inc"
};
static NJS_CNK_MODEL panel_model_3 = {
#include "stg36/panels/panel_model_3.inc"
};
NJS_CNK_OBJECT tails_panel_3 = {
#include "stg36/panels/tails_panel_3.inc"
};
static Sint16 panel_scroll_keys_4[42] = {
#include "stg36/panels/panel_scroll_keys_4.inc"
};
struct texture_anim tails_panel_uv_4 = {
#include "stg36/panels/tails_panel_uv_4.inc"
};
static NJS_TEXNAME panel_names_4[1] = {
#include "stg36/panels/panel_names_4.inc"
};
static NJS_TEXLIST panel_textures_4 = {
#include "stg36/panels/panel_textures_4.inc"
};
static Sint16 panel_polygons_4[65] = {
#include "stg36/panels/panel_polygons_4.inc"
};
static Sint32 panel_vertices_4[35] = {
#include "stg36/panels/panel_vertices_4.inc"
};
static NJS_CNK_MODEL panel_model_4 = {
#include "stg36/panels/panel_model_4.inc"
};
NJS_CNK_OBJECT tails_panel_4 = {
#include "stg36/panels/tails_panel_4.inc"
};
static Sint16 panel_scroll_keys_5[42] = {
#include "stg36/panels/panel_scroll_keys_5.inc"
};
struct texture_anim tails_panel_uv_5 = {
#include "stg36/panels/tails_panel_uv_5.inc"
};
static NJS_TEXNAME panel_names_5[1] = {
#include "stg36/panels/panel_names_5.inc"
};
static NJS_TEXLIST panel_textures_5 = {
#include "stg36/panels/panel_textures_5.inc"
};
static Sint16 panel_polygons_5[65] = {
#include "stg36/panels/panel_polygons_5.inc"
};
static Sint32 panel_vertices_5[35] = {
#include "stg36/panels/panel_vertices_5.inc"
};
static NJS_CNK_MODEL panel_model_5 = {
#include "stg36/panels/panel_model_5.inc"
};
NJS_CNK_OBJECT tails_panel_5 = {
#include "stg36/panels/tails_panel_5.inc"
};
static Sint16 panel_scroll_keys_6[42] = {
#include "stg36/panels/panel_scroll_keys_6.inc"
};
struct texture_anim tails_panel_uv_6 = {
#include "stg36/panels/tails_panel_uv_6.inc"
};
static NJS_TEXNAME panel_names_6[1] = {
#include "stg36/panels/panel_names_6.inc"
};
static NJS_TEXLIST panel_textures_6 = {
#include "stg36/panels/panel_textures_6.inc"
};
static Sint16 panel_polygons_6[65] = {
#include "stg36/panels/panel_polygons_6.inc"
};
static Sint32 panel_vertices_6[35] = {
#include "stg36/panels/panel_vertices_6.inc"
};
static NJS_CNK_MODEL panel_model_6 = {
#include "stg36/panels/panel_model_6.inc"
};
NJS_CNK_OBJECT tails_panel_6 = {
#include "stg36/panels/tails_panel_6.inc"
};
static Sint16 panel_scroll_keys_7[42] = {
#include "stg36/panels/panel_scroll_keys_7.inc"
};
struct texture_anim tails_panel_uv_7 = {
#include "stg36/panels/tails_panel_uv_7.inc"
};
static NJS_TEXNAME panel_names_7[1] = {
#include "stg36/panels/panel_names_7.inc"
};
static NJS_TEXLIST panel_textures_7 = {
#include "stg36/panels/panel_textures_7.inc"
};
static Sint16 panel_polygons_7[65] = {
#include "stg36/panels/panel_polygons_7.inc"
};
static Sint32 panel_vertices_7[35] = {
#include "stg36/panels/panel_vertices_7.inc"
};
static NJS_CNK_MODEL panel_model_7 = {
#include "stg36/panels/panel_model_7.inc"
};
NJS_CNK_OBJECT tails_panel_7 = {
#include "stg36/panels/tails_panel_7.inc"
};
static Sint16 panel_scroll_keys_8[42] = {
#include "stg36/panels/panel_scroll_keys_8.inc"
};
struct texture_anim tails_panel_uv_8 = {
#include "stg36/panels/tails_panel_uv_8.inc"
};
static NJS_TEXNAME panel_names_8[1] = {
#include "stg36/panels/panel_names_8.inc"
};
static NJS_TEXLIST panel_textures_8 = {
#include "stg36/panels/panel_textures_8.inc"
};
static Sint16 panel_polygons_8[65] = {
#include "stg36/panels/panel_polygons_8.inc"
};
static Sint32 panel_vertices_8[35] = {
#include "stg36/panels/panel_vertices_8.inc"
};
static NJS_CNK_MODEL panel_model_8 = {
#include "stg36/panels/panel_model_8.inc"
};
NJS_CNK_OBJECT tails_panel_8 = {
#include "stg36/panels/tails_panel_8.inc"
};
static Sint16 panel_scroll_keys_9[42] = {
#include "stg36/panels/panel_scroll_keys_9.inc"
};
struct texture_anim tails_panel_uv_9 = {
#include "stg36/panels/tails_panel_uv_9.inc"
};
static NJS_TEXNAME panel_names_9[1] = {
#include "stg36/panels/panel_names_9.inc"
};
static NJS_TEXLIST panel_textures_9 = {
#include "stg36/panels/panel_textures_9.inc"
};
static Sint16 panel_polygons_9[65] = {
#include "stg36/panels/panel_polygons_9.inc"
};
static Sint32 panel_vertices_9[35] = {
#include "stg36/panels/panel_vertices_9.inc"
};
static NJS_CNK_MODEL panel_model_9 = {
#include "stg36/panels/panel_model_9.inc"
};
NJS_CNK_OBJECT tails_panel_9 = {
#include "stg36/panels/tails_panel_9.inc"
};
static Sint16 panel_scroll_keys_10[42] = {
#include "stg36/panels/panel_scroll_keys_10.inc"
};
struct texture_anim tails_panel_uv_10 = {
#include "stg36/panels/tails_panel_uv_10.inc"
};
static NJS_TEXNAME panel_names_10[1] = {
#include "stg36/panels/panel_names_10.inc"
};
static NJS_TEXLIST panel_textures_10 = {
#include "stg36/panels/panel_textures_10.inc"
};
static Sint16 panel_polygons_10[65] = {
#include "stg36/panels/panel_polygons_10.inc"
};
static Sint32 panel_vertices_10[35] = {
#include "stg36/panels/panel_vertices_10.inc"
};
static NJS_CNK_MODEL panel_model_10 = {
#include "stg36/panels/panel_model_10.inc"
};
NJS_CNK_OBJECT tails_panel_10 = {
#include "stg36/panels/tails_panel_10.inc"
};
static Sint16 panel_scroll_keys_11[42] = {
#include "stg36/panels/panel_scroll_keys_11.inc"
};
struct texture_anim tails_panel_uv_11 = {
#include "stg36/panels/tails_panel_uv_11.inc"
};
static NJS_TEXNAME panel_names_11[1] = {
#include "stg36/panels/panel_names_11.inc"
};
static NJS_TEXLIST panel_textures_11 = {
#include "stg36/panels/panel_textures_11.inc"
};
static Sint16 panel_polygons_11[65] = {
#include "stg36/panels/panel_polygons_11.inc"
};
static Sint32 panel_vertices_11[35] = {
#include "stg36/panels/panel_vertices_11.inc"
};
static NJS_CNK_MODEL panel_model_11 = {
#include "stg36/panels/panel_model_11.inc"
};
NJS_CNK_OBJECT tails_panel_11 = {
#include "stg36/panels/tails_panel_11.inc"
};
static Sint16 panel_scroll_keys_12[52] = {
#include "stg36/panels/panel_scroll_keys_12.inc"
};
struct texture_anim tails_panel_uv_12 = {
#include "stg36/panels/tails_panel_uv_12.inc"
};
static NJS_TEXNAME panel_names_12[1] = {
#include "stg36/panels/panel_names_12.inc"
};
static NJS_TEXLIST panel_textures_12 = {
#include "stg36/panels/panel_textures_12.inc"
};
static Sint16 panel_polygons_12[77] = {
#include "stg36/panels/panel_polygons_12.inc"
};
static Sint32 panel_vertices_12[51] = {
#include "stg36/panels/panel_vertices_12.inc"
};
static NJS_CNK_MODEL panel_model_12 = {
#include "stg36/panels/panel_model_12.inc"
};
NJS_CNK_OBJECT tails_panel_12 = {
#include "stg36/panels/tails_panel_12.inc"
};
static Sint16 panel_scroll_keys_13[52] = {
#include "stg36/panels/panel_scroll_keys_13.inc"
};
struct texture_anim tails_panel_uv_13 = {
#include "stg36/panels/tails_panel_uv_13.inc"
};
static NJS_TEXNAME panel_names_13[1] = {
#include "stg36/panels/panel_names_13.inc"
};
static NJS_TEXLIST panel_textures_13 = {
#include "stg36/panels/panel_textures_13.inc"
};
static Sint16 panel_polygons_13[77] = {
#include "stg36/panels/panel_polygons_13.inc"
};
static Sint32 panel_vertices_13[51] = {
#include "stg36/panels/panel_vertices_13.inc"
};
static NJS_CNK_MODEL panel_model_13 = {
#include "stg36/panels/panel_model_13.inc"
};
NJS_CNK_OBJECT tails_panel_13 = {
#include "stg36/panels/tails_panel_13.inc"
};
static Sint16 panel_scroll_keys_14[52] = {
#include "stg36/panels/panel_scroll_keys_14.inc"
};
struct texture_anim tails_panel_uv_14 = {
#include "stg36/panels/tails_panel_uv_14.inc"
};
static NJS_TEXNAME panel_names_14[1] = {
#include "stg36/panels/panel_names_14.inc"
};
static NJS_TEXLIST panel_textures_14 = {
#include "stg36/panels/panel_textures_14.inc"
};
static Sint16 panel_polygons_14[77] = {
#include "stg36/panels/panel_polygons_14.inc"
};
static Sint32 panel_vertices_14[51] = {
#include "stg36/panels/panel_vertices_14.inc"
};
static NJS_CNK_MODEL panel_model_14 = {
#include "stg36/panels/panel_model_14.inc"
};
NJS_CNK_OBJECT tails_panel_14 = {
#include "stg36/panels/tails_panel_14.inc"
};
static Sint16 panel_scroll_keys_15[52] = {
#include "stg36/panels/panel_scroll_keys_15.inc"
};
struct texture_anim tails_panel_uv_15 = {
#include "stg36/panels/tails_panel_uv_15.inc"
};
static NJS_TEXNAME panel_names_15[1] = {
#include "stg36/panels/panel_names_15.inc"
};
static NJS_TEXLIST panel_textures_15 = {
#include "stg36/panels/panel_textures_15.inc"
};
static Sint16 panel_polygons_15[77] = {
#include "stg36/panels/panel_polygons_15.inc"
};
static Sint32 panel_vertices_15[51] = {
#include "stg36/panels/panel_vertices_15.inc"
};
static NJS_CNK_MODEL panel_model_15 = {
#include "stg36/panels/panel_model_15.inc"
};
NJS_CNK_OBJECT tails_panel_15 = {
#include "stg36/panels/tails_panel_15.inc"
};
static Sint16 panel_scroll_keys_16[42] = {
#include "stg36/panels/panel_scroll_keys_16.inc"
};
struct texture_anim tails_panel_uv_16 = {
#include "stg36/panels/tails_panel_uv_16.inc"
};
static NJS_TEXNAME panel_names_16[1] = {
#include "stg36/panels/panel_names_16.inc"
};
static NJS_TEXLIST panel_textures_16 = {
#include "stg36/panels/panel_textures_16.inc"
};
static Sint16 panel_polygons_16[65] = {
#include "stg36/panels/panel_polygons_16.inc"
};
static Sint32 panel_vertices_16[35] = {
#include "stg36/panels/panel_vertices_16.inc"
};
static NJS_CNK_MODEL panel_model_16 = {
#include "stg36/panels/panel_model_16.inc"
};
NJS_CNK_OBJECT tails_panel_16 = {
#include "stg36/panels/tails_panel_16.inc"
};
static Sint16 panel_scroll_keys_17[16] = {
#include "stg36/panels/panel_scroll_keys_17.inc"
};
struct texture_anim tails_panel_uv_17 = {
#include "stg36/panels/tails_panel_uv_17.inc"
};
static NJS_TEXNAME panel_names_17[1] = {
#include "stg36/panels/panel_names_17.inc"
};
static NJS_TEXLIST panel_textures_17 = {
#include "stg36/panels/panel_textures_17.inc"
};
static Sint16 panel_polygons_17[31] = {
#include "stg36/panels/panel_polygons_17.inc"
};
static Sint32 panel_vertices_17[27] = {
#include "stg36/panels/panel_vertices_17.inc"
};
static NJS_CNK_MODEL panel_model_17 = {
#include "stg36/panels/panel_model_17.inc"
};
NJS_CNK_OBJECT tails_panel_17 = {
#include "stg36/panels/tails_panel_17.inc"
};
static Sint16 panel_scroll_keys_18[16] = {
#include "stg36/panels/panel_scroll_keys_18.inc"
};
struct texture_anim tails_panel_uv_18 = {
#include "stg36/panels/tails_panel_uv_18.inc"
};
static NJS_TEXNAME panel_names_18[1] = {
#include "stg36/panels/panel_names_18.inc"
};
static NJS_TEXLIST panel_textures_18 = {
#include "stg36/panels/panel_textures_18.inc"
};
static Sint16 panel_polygons_18[31] = {
#include "stg36/panels/panel_polygons_18.inc"
};
static Sint32 panel_vertices_18[27] = {
#include "stg36/panels/panel_vertices_18.inc"
};
static NJS_CNK_MODEL panel_model_18 = {
#include "stg36/panels/panel_model_18.inc"
};
NJS_CNK_OBJECT tails_panel_18 = {
#include "stg36/panels/tails_panel_18.inc"
};
static Sint16 panel_scroll_keys_19[16] = {
#include "stg36/panels/panel_scroll_keys_19.inc"
};
struct texture_anim tails_panel_uv_19 = {
#include "stg36/panels/tails_panel_uv_19.inc"
};
static NJS_TEXNAME panel_names_19[1] = {
#include "stg36/panels/panel_names_19.inc"
};
static NJS_TEXLIST panel_textures_19 = {
#include "stg36/panels/panel_textures_19.inc"
};
static Sint16 panel_polygons_19[31] = {
#include "stg36/panels/panel_polygons_19.inc"
};
static Sint32 panel_vertices_19[27] = {
#include "stg36/panels/panel_vertices_19.inc"
};
static NJS_CNK_MODEL panel_model_19 = {
#include "stg36/panels/panel_model_19.inc"
};
NJS_CNK_OBJECT tails_panel_19 = {
#include "stg36/panels/tails_panel_19.inc"
};
static Sint16 panel_scroll_keys_20[16] = {
#include "stg36/panels/panel_scroll_keys_20.inc"
};
struct texture_anim tails_panel_uv_20 = {
#include "stg36/panels/tails_panel_uv_20.inc"
};
static NJS_TEXNAME panel_names_20[1] = {
#include "stg36/panels/panel_names_20.inc"
};
static NJS_TEXLIST panel_textures_20 = {
#include "stg36/panels/panel_textures_20.inc"
};
static Sint16 panel_polygons_20[31] = {
#include "stg36/panels/panel_polygons_20.inc"
};
static Sint32 panel_vertices_20[27] = {
#include "stg36/panels/panel_vertices_20.inc"
};
static NJS_CNK_MODEL panel_model_20 = {
#include "stg36/panels/panel_model_20.inc"
};
NJS_CNK_OBJECT tails_panel_20 = {
#include "stg36/panels/tails_panel_20.inc"
};
static Sint16 panel_scroll_keys_21[16] = {
#include "stg36/panels/panel_scroll_keys_21.inc"
};
struct texture_anim tails_panel_uv_21 = {
#include "stg36/panels/tails_panel_uv_21.inc"
};
static NJS_TEXNAME panel_names_21[1] = {
#include "stg36/panels/panel_names_21.inc"
};
static NJS_TEXLIST panel_textures_21 = {
#include "stg36/panels/panel_textures_21.inc"
};
static Sint16 panel_polygons_21[31] = {
#include "stg36/panels/panel_polygons_21.inc"
};
static Sint32 panel_vertices_21[27] = {
#include "stg36/panels/panel_vertices_21.inc"
};
static NJS_CNK_MODEL panel_model_21 = {
#include "stg36/panels/panel_model_21.inc"
};
NJS_CNK_OBJECT tails_panel_21 = {
#include "stg36/panels/tails_panel_21.inc"
};
static Sint16 panel_scroll_keys_22[16] = {
#include "stg36/panels/panel_scroll_keys_22.inc"
};
struct texture_anim tails_panel_uv_22 = {
#include "stg36/panels/tails_panel_uv_22.inc"
};
static NJS_TEXNAME panel_names_22[1] = {
#include "stg36/panels/panel_names_22.inc"
};
static NJS_TEXLIST panel_textures_22 = {
#include "stg36/panels/panel_textures_22.inc"
};
static Sint16 panel_polygons_22[31] = {
#include "stg36/panels/panel_polygons_22.inc"
};
static Sint32 panel_vertices_22[27] = {
#include "stg36/panels/panel_vertices_22.inc"
};
static NJS_CNK_MODEL panel_model_22 = {
#include "stg36/panels/panel_model_22.inc"
};
NJS_CNK_OBJECT tails_panel_22 = {
#include "stg36/panels/tails_panel_22.inc"
};
static Sint16 panel_scroll_keys_23[34] = {
#include "stg36/panels/panel_scroll_keys_23.inc"
};
struct texture_anim tails_panel_uv_23 = {
#include "stg36/panels/tails_panel_uv_23.inc"
};
static NJS_TEXNAME panel_names_23[1] = {
#include "stg36/panels/panel_names_23.inc"
};
static NJS_TEXLIST panel_textures_23 = {
#include "stg36/panels/panel_textures_23.inc"
};
static Sint16 panel_polygons_23[53] = {
#include "stg36/panels/panel_polygons_23.inc"
};
static Sint32 panel_vertices_23[23] = {
#include "stg36/panels/panel_vertices_23.inc"
};
static NJS_CNK_MODEL panel_model_23 = {
#include "stg36/panels/panel_model_23.inc"
};
NJS_CNK_OBJECT tails_panel_23 = {
#include "stg36/panels/tails_panel_23.inc"
};
static Sint16 panel_scroll_keys_24[34] = {
#include "stg36/panels/panel_scroll_keys_24.inc"
};
struct texture_anim tails_panel_uv_24 = {
#include "stg36/panels/tails_panel_uv_24.inc"
};
static NJS_TEXNAME panel_names_24[1] = {
#include "stg36/panels/panel_names_24.inc"
};
static NJS_TEXLIST panel_textures_24 = {
#include "stg36/panels/panel_textures_24.inc"
};
static Sint16 panel_polygons_24[53] = {
#include "stg36/panels/panel_polygons_24.inc"
};
static Sint32 panel_vertices_24[23] = {
#include "stg36/panels/panel_vertices_24.inc"
};
static NJS_CNK_MODEL panel_model_24 = {
#include "stg36/panels/panel_model_24.inc"
};
NJS_CNK_OBJECT tails_panel_24 = {
#include "stg36/panels/tails_panel_24.inc"
};
static Sint16 panel_scroll_keys_25[34] = {
#include "stg36/panels/panel_scroll_keys_25.inc"
};
struct texture_anim tails_panel_uv_25 = {
#include "stg36/panels/tails_panel_uv_25.inc"
};
static NJS_TEXNAME panel_names_25[1] = {
#include "stg36/panels/panel_names_25.inc"
};
static NJS_TEXLIST panel_textures_25 = {
#include "stg36/panels/panel_textures_25.inc"
};
static Sint16 panel_polygons_25[53] = {
#include "stg36/panels/panel_polygons_25.inc"
};
static Sint32 panel_vertices_25[23] = {
#include "stg36/panels/panel_vertices_25.inc"
};
static NJS_CNK_MODEL panel_model_25 = {
#include "stg36/panels/panel_model_25.inc"
};
NJS_CNK_OBJECT tails_panel_25 = {
#include "stg36/panels/tails_panel_25.inc"
};
static Sint16 panel_scroll_keys_26[34] = {
#include "stg36/panels/panel_scroll_keys_26.inc"
};
struct texture_anim tails_panel_uv_26 = {
#include "stg36/panels/tails_panel_uv_26.inc"
};
static NJS_TEXNAME panel_names_26[1] = {
#include "stg36/panels/panel_names_26.inc"
};
static NJS_TEXLIST panel_textures_26 = {
#include "stg36/panels/panel_textures_26.inc"
};
static Sint16 panel_polygons_26[53] = {
#include "stg36/panels/panel_polygons_26.inc"
};
static Sint32 panel_vertices_26[23] = {
#include "stg36/panels/panel_vertices_26.inc"
};
static NJS_CNK_MODEL panel_model_26 = {
#include "stg36/panels/panel_model_26.inc"
};
NJS_CNK_OBJECT tails_panel_26 = {
#include "stg36/panels/tails_panel_26.inc"
};
static Sint16 panel_scroll_keys_27[34] = {
#include "stg36/panels/panel_scroll_keys_27.inc"
};
struct texture_anim tails_panel_uv_27 = {
#include "stg36/panels/tails_panel_uv_27.inc"
};
static NJS_TEXNAME panel_names_27[1] = {
#include "stg36/panels/panel_names_27.inc"
};
static NJS_TEXLIST panel_textures_27 = {
#include "stg36/panels/panel_textures_27.inc"
};
static Sint16 panel_polygons_27[53] = {
#include "stg36/panels/panel_polygons_27.inc"
};
static Sint32 panel_vertices_27[23] = {
#include "stg36/panels/panel_vertices_27.inc"
};
static NJS_CNK_MODEL panel_model_27 = {
#include "stg36/panels/panel_model_27.inc"
};
NJS_CNK_OBJECT tails_panel_27 = {
#include "stg36/panels/tails_panel_27.inc"
};
static Sint16 panel_scroll_keys_28[34] = {
#include "stg36/panels/panel_scroll_keys_28.inc"
};
struct texture_anim tails_panel_uv_28 = {
#include "stg36/panels/tails_panel_uv_28.inc"
};
static NJS_TEXNAME panel_names_28[1] = {
#include "stg36/panels/panel_names_28.inc"
};
static NJS_TEXLIST panel_textures_28 = {
#include "stg36/panels/panel_textures_28.inc"
};
static Sint16 panel_polygons_28[53] = {
#include "stg36/panels/panel_polygons_28.inc"
};
static Sint32 panel_vertices_28[23] = {
#include "stg36/panels/panel_vertices_28.inc"
};
static NJS_CNK_MODEL panel_model_28 = {
#include "stg36/panels/panel_model_28.inc"
};
NJS_CNK_OBJECT tails_panel_28 = {
#include "stg36/panels/tails_panel_28.inc"
};
static Sint16 panel_scroll_keys_31[82] = {
#include "stg36/panels/panel_scroll_keys_31.inc"
};
struct texture_anim tails_panel_uv_31 = {
#include "stg36/panels/tails_panel_uv_31.inc"
};
static NJS_TEXNAME panel_names_31[1] = {
#include "stg36/panels/panel_names_31.inc"
};
static NJS_TEXLIST panel_textures_31 = {
#include "stg36/panels/panel_textures_31.inc"
};
static Sint16 panel_polygons_31[117] = {
#include "stg36/panels/panel_polygons_31.inc"
};
static Sint32 panel_vertices_31[131] = {
#include "stg36/panels/panel_vertices_31.inc"
};
static NJS_CNK_MODEL panel_model_31 = {
#include "stg36/panels/panel_model_31.inc"
};
NJS_CNK_OBJECT tails_panel_31 = {
#include "stg36/panels/tails_panel_31.inc"
};
static Sint16 panel_scroll_keys_30[82] = {
#include "stg36/panels/panel_scroll_keys_30.inc"
};
struct texture_anim tails_panel_uv_30 = {
#include "stg36/panels/tails_panel_uv_30.inc"
};
static NJS_TEXNAME panel_names_30[1] = {
#include "stg36/panels/panel_names_30.inc"
};
static NJS_TEXLIST panel_textures_30 = {
#include "stg36/panels/panel_textures_30.inc"
};
static Sint16 panel_polygons_30[117] = {
#include "stg36/panels/panel_polygons_30.inc"
};
static Sint32 panel_vertices_30[131] = {
#include "stg36/panels/panel_vertices_30.inc"
};
static NJS_CNK_MODEL panel_model_30 = {
#include "stg36/panels/panel_model_30.inc"
};
NJS_CNK_OBJECT tails_panel_30 = {
#include "stg36/panels/tails_panel_30.inc"
};
static Sint16 panel_scroll_keys_29[82] = {
#include "stg36/panels/panel_scroll_keys_29.inc"
};
struct texture_anim tails_panel_uv_29 = {
#include "stg36/panels/tails_panel_uv_29.inc"
};
static NJS_TEXNAME panel_names_29[1] = {
#include "stg36/panels/panel_names_29.inc"
};
static NJS_TEXLIST panel_textures_29 = {
#include "stg36/panels/panel_textures_29.inc"
};
static Sint16 panel_polygons_29[117] = {
#include "stg36/panels/panel_polygons_29.inc"
};
static Sint32 panel_vertices_29[131] = {
#include "stg36/panels/panel_vertices_29.inc"
};
static NJS_CNK_MODEL panel_model_29 = {
#include "stg36/panels/panel_model_29.inc"
};
NJS_CNK_OBJECT tails_panel_29 = {
#include "stg36/panels/tails_panel_29.inc"
};
static Sint16 panel_scroll_keys_32[58] = {
#include "stg36/panels/panel_scroll_keys_32.inc"
};
struct texture_anim tails_panel_uv_32 = {
#include "stg36/panels/tails_panel_uv_32.inc"
};
static NJS_TEXNAME panel_names_32[1] = {
#include "stg36/panels/panel_names_32.inc"
};
static NJS_TEXLIST panel_textures_32 = {
#include "stg36/panels/panel_textures_32.inc"
};
static Sint16 panel_polygons_32[89] = {
#include "stg36/panels/panel_polygons_32.inc"
};
static Sint32 panel_vertices_32[51] = {
#include "stg36/panels/panel_vertices_32.inc"
};
static NJS_CNK_MODEL panel_model_32 = {
#include "stg36/panels/panel_model_32.inc"
};
NJS_CNK_OBJECT tails_panel_32 = {
#include "stg36/panels/tails_panel_32.inc"
};
static Sint16 panel_scroll_keys_33[42] = {
#include "stg36/panels/panel_scroll_keys_33.inc"
};
struct texture_anim tails_panel_uv_33 = {
#include "stg36/panels/tails_panel_uv_33.inc"
};
static NJS_TEXNAME panel_names_33[1] = {
#include "stg36/panels/panel_names_33.inc"
};
static NJS_TEXLIST panel_textures_33 = {
#include "stg36/panels/panel_textures_33.inc"
};
static Sint16 panel_polygons_33[65] = {
#include "stg36/panels/panel_polygons_33.inc"
};
static Sint32 panel_vertices_33[35] = {
#include "stg36/panels/panel_vertices_33.inc"
};
static NJS_CNK_MODEL panel_model_33 = {
#include "stg36/panels/panel_model_33.inc"
};
NJS_CNK_OBJECT tails_panel_33 = {
#include "stg36/panels/tails_panel_33.inc"
};
static Sint16 panel_scroll_keys_34[42] = {
#include "stg36/panels/panel_scroll_keys_34.inc"
};
struct texture_anim tails_panel_uv_34 = {
#include "stg36/panels/tails_panel_uv_34.inc"
};
static NJS_TEXNAME panel_names_34[1] = {
#include "stg36/panels/panel_names_34.inc"
};
static NJS_TEXLIST panel_textures_34 = {
#include "stg36/panels/panel_textures_34.inc"
};
static Sint16 panel_polygons_34[65] = {
#include "stg36/panels/panel_polygons_34.inc"
};
static Sint32 panel_vertices_34[35] = {
#include "stg36/panels/panel_vertices_34.inc"
};
static NJS_CNK_MODEL panel_model_34 = {
#include "stg36/panels/panel_model_34.inc"
};
NJS_CNK_OBJECT tails_panel_34 = {
#include "stg36/panels/tails_panel_34.inc"
};
static Sint16 panel_scroll_keys_35[42] = {
#include "stg36/panels/panel_scroll_keys_35.inc"
};
struct texture_anim tails_panel_uv_35 = {
#include "stg36/panels/tails_panel_uv_35.inc"
};
static NJS_TEXNAME panel_names_35[1] = {
#include "stg36/panels/panel_names_35.inc"
};
static NJS_TEXLIST panel_textures_35 = {
#include "stg36/panels/panel_textures_35.inc"
};
static Sint16 panel_polygons_35[65] = {
#include "stg36/panels/panel_polygons_35.inc"
};
static Sint32 panel_vertices_35[35] = {
#include "stg36/panels/panel_vertices_35.inc"
};
static NJS_CNK_MODEL panel_model_35 = {
#include "stg36/panels/panel_model_35.inc"
};
NJS_CNK_OBJECT tails_panel_35 = {
#include "stg36/panels/tails_panel_35.inc"
};
static Sint16 panel_scroll_keys_36[42] = {
#include "stg36/panels/panel_scroll_keys_36.inc"
};
struct texture_anim tails_panel_uv_36 = {
#include "stg36/panels/tails_panel_uv_36.inc"
};
static NJS_TEXNAME panel_names_36[1] = {
#include "stg36/panels/panel_names_36.inc"
};
static NJS_TEXLIST panel_textures_36 = {
#include "stg36/panels/panel_textures_36.inc"
};
static Sint16 panel_polygons_36[65] = {
#include "stg36/panels/panel_polygons_36.inc"
};
static Sint32 panel_vertices_36[35] = {
#include "stg36/panels/panel_vertices_36.inc"
};
static NJS_CNK_MODEL panel_model_36 = {
#include "stg36/panels/panel_model_36.inc"
};
NJS_CNK_OBJECT tails_panel_36 = {
#include "stg36/panels/tails_panel_36.inc"
};
static Sint16 panel_scroll_keys_37[42] = {
#include "stg36/panels/panel_scroll_keys_37.inc"
};
struct texture_anim tails_panel_uv_37 = {
#include "stg36/panels/tails_panel_uv_37.inc"
};
static NJS_TEXNAME panel_names_37[1] = {
#include "stg36/panels/panel_names_37.inc"
};
static NJS_TEXLIST panel_textures_37 = {
#include "stg36/panels/panel_textures_37.inc"
};
static Sint16 panel_polygons_37[65] = {
#include "stg36/panels/panel_polygons_37.inc"
};
static Sint32 panel_vertices_37[35] = {
#include "stg36/panels/panel_vertices_37.inc"
};
static NJS_CNK_MODEL panel_model_37 = {
#include "stg36/panels/panel_model_37.inc"
};
NJS_CNK_OBJECT tails_panel_37 = {
#include "stg36/panels/tails_panel_37.inc"
};
static Sint16 panel_scroll_keys_38[42] = {
#include "stg36/panels/panel_scroll_keys_38.inc"
};
struct texture_anim tails_panel_uv_38 = {
#include "stg36/panels/tails_panel_uv_38.inc"
};
static NJS_TEXNAME panel_names_38[1] = {
#include "stg36/panels/panel_names_38.inc"
};
static NJS_TEXLIST panel_textures_38 = {
#include "stg36/panels/panel_textures_38.inc"
};
static Sint16 panel_polygons_38[65] = {
#include "stg36/panels/panel_polygons_38.inc"
};
static Sint32 panel_vertices_38[35] = {
#include "stg36/panels/panel_vertices_38.inc"
};
static NJS_CNK_MODEL panel_model_38 = {
#include "stg36/panels/panel_model_38.inc"
};
NJS_CNK_OBJECT tails_panel_38 = {
#include "stg36/panels/tails_panel_38.inc"
};
static Sint16 panel_scroll_keys_39[42] = {
#include "stg36/panels/panel_scroll_keys_39.inc"
};
struct texture_anim tails_panel_uv_39 = {
#include "stg36/panels/tails_panel_uv_39.inc"
};
static NJS_TEXNAME panel_names_39[1] = {
#include "stg36/panels/panel_names_39.inc"
};
static NJS_TEXLIST panel_textures_39 = {
#include "stg36/panels/panel_textures_39.inc"
};
static Sint16 panel_polygons_39[65] = {
#include "stg36/panels/panel_polygons_39.inc"
};
static Sint32 panel_vertices_39[35] = {
#include "stg36/panels/panel_vertices_39.inc"
};
static NJS_CNK_MODEL panel_model_39 = {
#include "stg36/panels/panel_model_39.inc"
};
NJS_CNK_OBJECT tails_panel_39 = {
#include "stg36/panels/tails_panel_39.inc"
};
static Sint16 panel_scroll_keys_40[42] = {
#include "stg36/panels/panel_scroll_keys_40.inc"
};
struct texture_anim tails_panel_uv_40 = {
#include "stg36/panels/tails_panel_uv_40.inc"
};
static NJS_TEXNAME panel_names_40[1] = {
#include "stg36/panels/panel_names_40.inc"
};
static NJS_TEXLIST panel_textures_40 = {
#include "stg36/panels/panel_textures_40.inc"
};
static Sint16 panel_polygons_40[65] = {
#include "stg36/panels/panel_polygons_40.inc"
};
static Sint32 panel_vertices_40[35] = {
#include "stg36/panels/panel_vertices_40.inc"
};
static NJS_CNK_MODEL panel_model_40 = {
#include "stg36/panels/panel_model_40.inc"
};
NJS_CNK_OBJECT tails_panel_40 = {
#include "stg36/panels/tails_panel_40.inc"
};
static Sint16 panel_scroll_keys_41[42] = {
#include "stg36/panels/panel_scroll_keys_41.inc"
};
struct texture_anim tails_panel_uv_41 = {
#include "stg36/panels/tails_panel_uv_41.inc"
};
static NJS_TEXNAME panel_names_41[1] = {
#include "stg36/panels/panel_names_41.inc"
};
static NJS_TEXLIST panel_textures_41 = {
#include "stg36/panels/panel_textures_41.inc"
};
static Sint16 panel_polygons_41[65] = {
#include "stg36/panels/panel_polygons_41.inc"
};
static Sint32 panel_vertices_41[35] = {
#include "stg36/panels/panel_vertices_41.inc"
};
static NJS_CNK_MODEL panel_model_41 = {
#include "stg36/panels/panel_model_41.inc"
};
NJS_CNK_OBJECT tails_panel_41 = {
#include "stg36/panels/tails_panel_41.inc"
};
static Sint16 panel_scroll_keys_42[42] = {
#include "stg36/panels/panel_scroll_keys_42.inc"
};
struct texture_anim tails_panel_uv_42 = {
#include "stg36/panels/tails_panel_uv_42.inc"
};
static NJS_TEXNAME panel_names_42[1] = {
#include "stg36/panels/panel_names_42.inc"
};
static NJS_TEXLIST panel_textures_42 = {
#include "stg36/panels/panel_textures_42.inc"
};
static Sint16 panel_polygons_42[65] = {
#include "stg36/panels/panel_polygons_42.inc"
};
static Sint32 panel_vertices_42[35] = {
#include "stg36/panels/panel_vertices_42.inc"
};
static NJS_CNK_MODEL panel_model_42 = {
#include "stg36/panels/panel_model_42.inc"
};
NJS_CNK_OBJECT tails_panel_42 = {
#include "stg36/panels/tails_panel_42.inc"
};
static Sint16 panel_scroll_keys_43[42] = {
#include "stg36/panels/panel_scroll_keys_43.inc"
};
struct texture_anim tails_panel_uv_43 = {
#include "stg36/panels/tails_panel_uv_43.inc"
};
static NJS_TEXNAME panel_names_43[1] = {
#include "stg36/panels/panel_names_43.inc"
};
static NJS_TEXLIST panel_textures_43 = {
#include "stg36/panels/panel_textures_43.inc"
};
static Sint16 panel_polygons_43[65] = {
#include "stg36/panels/panel_polygons_43.inc"
};
static Sint32 panel_vertices_43[35] = {
#include "stg36/panels/panel_vertices_43.inc"
};
static NJS_CNK_MODEL panel_model_43 = {
#include "stg36/panels/panel_model_43.inc"
};
NJS_CNK_OBJECT tails_panel_43 = {
#include "stg36/panels/tails_panel_43.inc"
};
static Sint16 panel_scroll_keys_44[42] = {
#include "stg36/panels/panel_scroll_keys_44.inc"
};
struct texture_anim tails_panel_uv_44 = {
#include "stg36/panels/tails_panel_uv_44.inc"
};
static NJS_TEXNAME panel_names_44[1] = {
#include "stg36/panels/panel_names_44.inc"
};
static NJS_TEXLIST panel_textures_44 = {
#include "stg36/panels/panel_textures_44.inc"
};
static Sint16 panel_polygons_44[65] = {
#include "stg36/panels/panel_polygons_44.inc"
};
static Sint32 panel_vertices_44[35] = {
#include "stg36/panels/panel_vertices_44.inc"
};
static NJS_CNK_MODEL panel_model_44 = {
#include "stg36/panels/panel_model_44.inc"
};
NJS_CNK_OBJECT tails_panel_44 = {
#include "stg36/panels/tails_panel_44.inc"
};
static Sint16 panel_scroll_keys_45[52] = {
#include "stg36/panels/panel_scroll_keys_45.inc"
};
struct texture_anim tails_panel_uv_45 = {
#include "stg36/panels/tails_panel_uv_45.inc"
};
static NJS_TEXNAME panel_names_45[1] = {
#include "stg36/panels/panel_names_45.inc"
};
static NJS_TEXLIST panel_textures_45 = {
#include "stg36/panels/panel_textures_45.inc"
};
static Sint16 panel_polygons_45[77] = {
#include "stg36/panels/panel_polygons_45.inc"
};
static Sint32 panel_vertices_45[51] = {
#include "stg36/panels/panel_vertices_45.inc"
};
static NJS_CNK_MODEL panel_model_45 = {
#include "stg36/panels/panel_model_45.inc"
};
NJS_CNK_OBJECT tails_panel_45 = {
#include "stg36/panels/tails_panel_45.inc"
};
