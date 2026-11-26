#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg35/panels.h"

/* the texture name that the scrolling panels' texture list points at */
const char eggman_panel_texture_name[13] = {
#include "stg35/panels/eggman_panel_texture_name.inc"
};

/* STG35.PRS's UV scrolls, texture lists and chunk models, from the disc. its
 * D lies between core_mech_route.c and machinery_art.c */
static Sint16 panel_scroll_keys_1[42] = {
#include "stg35/panels/panel_scroll_keys_1.inc"
};
struct texture_anim eggman_panel_uv_1 = {
#include "stg35/panels/eggman_panel_uv_1.inc"
};
static NJS_TEXNAME panel_names_1[1] = {
#include "stg35/panels/panel_names_1.inc"
};
static NJS_TEXLIST panel_textures_1 = {
#include "stg35/panels/panel_textures_1.inc"
};
static Sint16 panel_polygons_1[65] = {
#include "stg35/panels/panel_polygons_1.inc"
};
static Sint32 panel_vertices_1[35] = {
#include "stg35/panels/panel_vertices_1.inc"
};
static NJS_CNK_MODEL panel_model_1 = {
#include "stg35/panels/panel_model_1.inc"
};
NJS_CNK_OBJECT eggman_panel_1 = {
#include "stg35/panels/eggman_panel_1.inc"
};
static Sint16 panel_scroll_keys_2[42] = {
#include "stg35/panels/panel_scroll_keys_2.inc"
};
struct texture_anim eggman_panel_uv_2 = {
#include "stg35/panels/eggman_panel_uv_2.inc"
};
static NJS_TEXNAME panel_names_2[1] = {
#include "stg35/panels/panel_names_2.inc"
};
static NJS_TEXLIST panel_textures_2 = {
#include "stg35/panels/panel_textures_2.inc"
};
static Sint16 panel_polygons_2[65] = {
#include "stg35/panels/panel_polygons_2.inc"
};
static Sint32 panel_vertices_2[35] = {
#include "stg35/panels/panel_vertices_2.inc"
};
static NJS_CNK_MODEL panel_model_2 = {
#include "stg35/panels/panel_model_2.inc"
};
NJS_CNK_OBJECT eggman_panel_2 = {
#include "stg35/panels/eggman_panel_2.inc"
};
static Sint16 panel_scroll_keys_3[42] = {
#include "stg35/panels/panel_scroll_keys_3.inc"
};
struct texture_anim eggman_panel_uv_3 = {
#include "stg35/panels/eggman_panel_uv_3.inc"
};
static NJS_TEXNAME panel_names_3[1] = {
#include "stg35/panels/panel_names_3.inc"
};
static NJS_TEXLIST panel_textures_3 = {
#include "stg35/panels/panel_textures_3.inc"
};
static Sint16 panel_polygons_3[65] = {
#include "stg35/panels/panel_polygons_3.inc"
};
static Sint32 panel_vertices_3[35] = {
#include "stg35/panels/panel_vertices_3.inc"
};
static NJS_CNK_MODEL panel_model_3 = {
#include "stg35/panels/panel_model_3.inc"
};
NJS_CNK_OBJECT eggman_panel_3 = {
#include "stg35/panels/eggman_panel_3.inc"
};
static Sint16 panel_scroll_keys_4[42] = {
#include "stg35/panels/panel_scroll_keys_4.inc"
};
struct texture_anim eggman_panel_uv_4 = {
#include "stg35/panels/eggman_panel_uv_4.inc"
};
static NJS_TEXNAME panel_names_4[1] = {
#include "stg35/panels/panel_names_4.inc"
};
static NJS_TEXLIST panel_textures_4 = {
#include "stg35/panels/panel_textures_4.inc"
};
static Sint16 panel_polygons_4[65] = {
#include "stg35/panels/panel_polygons_4.inc"
};
static Sint32 panel_vertices_4[35] = {
#include "stg35/panels/panel_vertices_4.inc"
};
static NJS_CNK_MODEL panel_model_4 = {
#include "stg35/panels/panel_model_4.inc"
};
NJS_CNK_OBJECT eggman_panel_4 = {
#include "stg35/panels/eggman_panel_4.inc"
};
static Sint16 panel_scroll_keys_5[42] = {
#include "stg35/panels/panel_scroll_keys_5.inc"
};
struct texture_anim eggman_panel_uv_5 = {
#include "stg35/panels/eggman_panel_uv_5.inc"
};
static NJS_TEXNAME panel_names_5[1] = {
#include "stg35/panels/panel_names_5.inc"
};
static NJS_TEXLIST panel_textures_5 = {
#include "stg35/panels/panel_textures_5.inc"
};
static Sint16 panel_polygons_5[65] = {
#include "stg35/panels/panel_polygons_5.inc"
};
static Sint32 panel_vertices_5[35] = {
#include "stg35/panels/panel_vertices_5.inc"
};
static NJS_CNK_MODEL panel_model_5 = {
#include "stg35/panels/panel_model_5.inc"
};
NJS_CNK_OBJECT eggman_panel_5 = {
#include "stg35/panels/eggman_panel_5.inc"
};
static Sint16 panel_scroll_keys_6[42] = {
#include "stg35/panels/panel_scroll_keys_6.inc"
};
struct texture_anim eggman_panel_uv_6 = {
#include "stg35/panels/eggman_panel_uv_6.inc"
};
static NJS_TEXNAME panel_names_6[1] = {
#include "stg35/panels/panel_names_6.inc"
};
static NJS_TEXLIST panel_textures_6 = {
#include "stg35/panels/panel_textures_6.inc"
};
static Sint16 panel_polygons_6[65] = {
#include "stg35/panels/panel_polygons_6.inc"
};
static Sint32 panel_vertices_6[35] = {
#include "stg35/panels/panel_vertices_6.inc"
};
static NJS_CNK_MODEL panel_model_6 = {
#include "stg35/panels/panel_model_6.inc"
};
NJS_CNK_OBJECT eggman_panel_6 = {
#include "stg35/panels/eggman_panel_6.inc"
};
static Sint16 panel_scroll_keys_7[42] = {
#include "stg35/panels/panel_scroll_keys_7.inc"
};
struct texture_anim eggman_panel_uv_7 = {
#include "stg35/panels/eggman_panel_uv_7.inc"
};
static NJS_TEXNAME panel_names_7[1] = {
#include "stg35/panels/panel_names_7.inc"
};
static NJS_TEXLIST panel_textures_7 = {
#include "stg35/panels/panel_textures_7.inc"
};
static Sint16 panel_polygons_7[65] = {
#include "stg35/panels/panel_polygons_7.inc"
};
static Sint32 panel_vertices_7[35] = {
#include "stg35/panels/panel_vertices_7.inc"
};
static NJS_CNK_MODEL panel_model_7 = {
#include "stg35/panels/panel_model_7.inc"
};
NJS_CNK_OBJECT eggman_panel_7 = {
#include "stg35/panels/eggman_panel_7.inc"
};
static Sint16 panel_scroll_keys_8[42] = {
#include "stg35/panels/panel_scroll_keys_8.inc"
};
struct texture_anim eggman_panel_uv_8 = {
#include "stg35/panels/eggman_panel_uv_8.inc"
};
static NJS_TEXNAME panel_names_8[1] = {
#include "stg35/panels/panel_names_8.inc"
};
static NJS_TEXLIST panel_textures_8 = {
#include "stg35/panels/panel_textures_8.inc"
};
static Sint16 panel_polygons_8[65] = {
#include "stg35/panels/panel_polygons_8.inc"
};
static Sint32 panel_vertices_8[35] = {
#include "stg35/panels/panel_vertices_8.inc"
};
static NJS_CNK_MODEL panel_model_8 = {
#include "stg35/panels/panel_model_8.inc"
};
NJS_CNK_OBJECT eggman_panel_8 = {
#include "stg35/panels/eggman_panel_8.inc"
};
static Sint16 panel_scroll_keys_9[42] = {
#include "stg35/panels/panel_scroll_keys_9.inc"
};
struct texture_anim eggman_panel_uv_9 = {
#include "stg35/panels/eggman_panel_uv_9.inc"
};
static NJS_TEXNAME panel_names_9[1] = {
#include "stg35/panels/panel_names_9.inc"
};
static NJS_TEXLIST panel_textures_9 = {
#include "stg35/panels/panel_textures_9.inc"
};
static Sint16 panel_polygons_9[65] = {
#include "stg35/panels/panel_polygons_9.inc"
};
static Sint32 panel_vertices_9[35] = {
#include "stg35/panels/panel_vertices_9.inc"
};
static NJS_CNK_MODEL panel_model_9 = {
#include "stg35/panels/panel_model_9.inc"
};
NJS_CNK_OBJECT eggman_panel_9 = {
#include "stg35/panels/eggman_panel_9.inc"
};
static Sint16 panel_scroll_keys_10[42] = {
#include "stg35/panels/panel_scroll_keys_10.inc"
};
struct texture_anim eggman_panel_uv_10 = {
#include "stg35/panels/eggman_panel_uv_10.inc"
};
static NJS_TEXNAME panel_names_10[1] = {
#include "stg35/panels/panel_names_10.inc"
};
static NJS_TEXLIST panel_textures_10 = {
#include "stg35/panels/panel_textures_10.inc"
};
static Sint16 panel_polygons_10[65] = {
#include "stg35/panels/panel_polygons_10.inc"
};
static Sint32 panel_vertices_10[35] = {
#include "stg35/panels/panel_vertices_10.inc"
};
static NJS_CNK_MODEL panel_model_10 = {
#include "stg35/panels/panel_model_10.inc"
};
NJS_CNK_OBJECT eggman_panel_10 = {
#include "stg35/panels/eggman_panel_10.inc"
};
static Sint16 panel_scroll_keys_11[42] = {
#include "stg35/panels/panel_scroll_keys_11.inc"
};
struct texture_anim eggman_panel_uv_11 = {
#include "stg35/panels/eggman_panel_uv_11.inc"
};
static NJS_TEXNAME panel_names_11[1] = {
#include "stg35/panels/panel_names_11.inc"
};
static NJS_TEXLIST panel_textures_11 = {
#include "stg35/panels/panel_textures_11.inc"
};
static Sint16 panel_polygons_11[65] = {
#include "stg35/panels/panel_polygons_11.inc"
};
static Sint32 panel_vertices_11[35] = {
#include "stg35/panels/panel_vertices_11.inc"
};
static NJS_CNK_MODEL panel_model_11 = {
#include "stg35/panels/panel_model_11.inc"
};
NJS_CNK_OBJECT eggman_panel_11 = {
#include "stg35/panels/eggman_panel_11.inc"
};
static Sint16 panel_scroll_keys_12[42] = {
#include "stg35/panels/panel_scroll_keys_12.inc"
};
struct texture_anim eggman_panel_uv_12 = {
#include "stg35/panels/eggman_panel_uv_12.inc"
};
static NJS_TEXNAME panel_names_12[1] = {
#include "stg35/panels/panel_names_12.inc"
};
static NJS_TEXLIST panel_textures_12 = {
#include "stg35/panels/panel_textures_12.inc"
};
static Sint16 panel_polygons_12[65] = {
#include "stg35/panels/panel_polygons_12.inc"
};
static Sint32 panel_vertices_12[35] = {
#include "stg35/panels/panel_vertices_12.inc"
};
static NJS_CNK_MODEL panel_model_12 = {
#include "stg35/panels/panel_model_12.inc"
};
NJS_CNK_OBJECT eggman_panel_12 = {
#include "stg35/panels/eggman_panel_12.inc"
};
static Sint16 panel_scroll_keys_13[52] = {
#include "stg35/panels/panel_scroll_keys_13.inc"
};
struct texture_anim eggman_panel_uv_13 = {
#include "stg35/panels/eggman_panel_uv_13.inc"
};
static NJS_TEXNAME panel_names_13[1] = {
#include "stg35/panels/panel_names_13.inc"
};
static NJS_TEXLIST panel_textures_13 = {
#include "stg35/panels/panel_textures_13.inc"
};
static Sint16 panel_polygons_13[77] = {
#include "stg35/panels/panel_polygons_13.inc"
};
static Sint32 panel_vertices_13[51] = {
#include "stg35/panels/panel_vertices_13.inc"
};
static NJS_CNK_MODEL panel_model_13 = {
#include "stg35/panels/panel_model_13.inc"
};
NJS_CNK_OBJECT eggman_panel_13 = {
#include "stg35/panels/eggman_panel_13.inc"
};
static Sint16 panel_scroll_keys_14[52] = {
#include "stg35/panels/panel_scroll_keys_14.inc"
};
struct texture_anim eggman_panel_uv_14 = {
#include "stg35/panels/eggman_panel_uv_14.inc"
};
static NJS_TEXNAME panel_names_14[1] = {
#include "stg35/panels/panel_names_14.inc"
};
static NJS_TEXLIST panel_textures_14 = {
#include "stg35/panels/panel_textures_14.inc"
};
static Sint16 panel_polygons_14[77] = {
#include "stg35/panels/panel_polygons_14.inc"
};
static Sint32 panel_vertices_14[51] = {
#include "stg35/panels/panel_vertices_14.inc"
};
static NJS_CNK_MODEL panel_model_14 = {
#include "stg35/panels/panel_model_14.inc"
};
NJS_CNK_OBJECT eggman_panel_14 = {
#include "stg35/panels/eggman_panel_14.inc"
};
static Sint16 panel_scroll_keys_15[52] = {
#include "stg35/panels/panel_scroll_keys_15.inc"
};
struct texture_anim eggman_panel_uv_15 = {
#include "stg35/panels/eggman_panel_uv_15.inc"
};
static NJS_TEXNAME panel_names_15[1] = {
#include "stg35/panels/panel_names_15.inc"
};
static NJS_TEXLIST panel_textures_15 = {
#include "stg35/panels/panel_textures_15.inc"
};
static Sint16 panel_polygons_15[77] = {
#include "stg35/panels/panel_polygons_15.inc"
};
static Sint32 panel_vertices_15[51] = {
#include "stg35/panels/panel_vertices_15.inc"
};
static NJS_CNK_MODEL panel_model_15 = {
#include "stg35/panels/panel_model_15.inc"
};
NJS_CNK_OBJECT eggman_panel_15 = {
#include "stg35/panels/eggman_panel_15.inc"
};
static Sint16 panel_scroll_keys_16[52] = {
#include "stg35/panels/panel_scroll_keys_16.inc"
};
struct texture_anim eggman_panel_uv_16 = {
#include "stg35/panels/eggman_panel_uv_16.inc"
};
static NJS_TEXNAME panel_names_16[1] = {
#include "stg35/panels/panel_names_16.inc"
};
static NJS_TEXLIST panel_textures_16 = {
#include "stg35/panels/panel_textures_16.inc"
};
static Sint16 panel_polygons_16[77] = {
#include "stg35/panels/panel_polygons_16.inc"
};
static Sint32 panel_vertices_16[51] = {
#include "stg35/panels/panel_vertices_16.inc"
};
static NJS_CNK_MODEL panel_model_16 = {
#include "stg35/panels/panel_model_16.inc"
};
NJS_CNK_OBJECT eggman_panel_16 = {
#include "stg35/panels/eggman_panel_16.inc"
};
static Sint16 panel_scroll_keys_17[16] = {
#include "stg35/panels/panel_scroll_keys_17.inc"
};
struct texture_anim eggman_panel_uv_17 = {
#include "stg35/panels/eggman_panel_uv_17.inc"
};
static NJS_TEXNAME panel_names_17[1] = {
#include "stg35/panels/panel_names_17.inc"
};
static NJS_TEXLIST panel_textures_17 = {
#include "stg35/panels/panel_textures_17.inc"
};
static Sint16 panel_polygons_17[31] = {
#include "stg35/panels/panel_polygons_17.inc"
};
static Sint32 panel_vertices_17[27] = {
#include "stg35/panels/panel_vertices_17.inc"
};
static NJS_CNK_MODEL panel_model_17 = {
#include "stg35/panels/panel_model_17.inc"
};
NJS_CNK_OBJECT eggman_panel_17 = {
#include "stg35/panels/eggman_panel_17.inc"
};
static Sint16 panel_scroll_keys_18[16] = {
#include "stg35/panels/panel_scroll_keys_18.inc"
};
struct texture_anim eggman_panel_uv_18 = {
#include "stg35/panels/eggman_panel_uv_18.inc"
};
static NJS_TEXNAME panel_names_18[1] = {
#include "stg35/panels/panel_names_18.inc"
};
static NJS_TEXLIST panel_textures_18 = {
#include "stg35/panels/panel_textures_18.inc"
};
static Sint16 panel_polygons_18[31] = {
#include "stg35/panels/panel_polygons_18.inc"
};
static Sint32 panel_vertices_18[27] = {
#include "stg35/panels/panel_vertices_18.inc"
};
static NJS_CNK_MODEL panel_model_18 = {
#include "stg35/panels/panel_model_18.inc"
};
NJS_CNK_OBJECT eggman_panel_18 = {
#include "stg35/panels/eggman_panel_18.inc"
};
static Sint16 panel_scroll_keys_19[16] = {
#include "stg35/panels/panel_scroll_keys_19.inc"
};
struct texture_anim eggman_panel_uv_19 = {
#include "stg35/panels/eggman_panel_uv_19.inc"
};
static NJS_TEXNAME panel_names_19[1] = {
#include "stg35/panels/panel_names_19.inc"
};
static NJS_TEXLIST panel_textures_19 = {
#include "stg35/panels/panel_textures_19.inc"
};
static Sint16 panel_polygons_19[31] = {
#include "stg35/panels/panel_polygons_19.inc"
};
static Sint32 panel_vertices_19[27] = {
#include "stg35/panels/panel_vertices_19.inc"
};
static NJS_CNK_MODEL panel_model_19 = {
#include "stg35/panels/panel_model_19.inc"
};
NJS_CNK_OBJECT eggman_panel_19 = {
#include "stg35/panels/eggman_panel_19.inc"
};
static Sint16 panel_scroll_keys_20[16] = {
#include "stg35/panels/panel_scroll_keys_20.inc"
};
struct texture_anim eggman_panel_uv_20 = {
#include "stg35/panels/eggman_panel_uv_20.inc"
};
static NJS_TEXNAME panel_names_20[1] = {
#include "stg35/panels/panel_names_20.inc"
};
static NJS_TEXLIST panel_textures_20 = {
#include "stg35/panels/panel_textures_20.inc"
};
static Sint16 panel_polygons_20[31] = {
#include "stg35/panels/panel_polygons_20.inc"
};
static Sint32 panel_vertices_20[27] = {
#include "stg35/panels/panel_vertices_20.inc"
};
static NJS_CNK_MODEL panel_model_20 = {
#include "stg35/panels/panel_model_20.inc"
};
NJS_CNK_OBJECT eggman_panel_20 = {
#include "stg35/panels/eggman_panel_20.inc"
};
static Sint16 panel_scroll_keys_21[16] = {
#include "stg35/panels/panel_scroll_keys_21.inc"
};
struct texture_anim eggman_panel_uv_21 = {
#include "stg35/panels/eggman_panel_uv_21.inc"
};
static NJS_TEXNAME panel_names_21[1] = {
#include "stg35/panels/panel_names_21.inc"
};
static NJS_TEXLIST panel_textures_21 = {
#include "stg35/panels/panel_textures_21.inc"
};
static Sint16 panel_polygons_21[31] = {
#include "stg35/panels/panel_polygons_21.inc"
};
static Sint32 panel_vertices_21[27] = {
#include "stg35/panels/panel_vertices_21.inc"
};
static NJS_CNK_MODEL panel_model_21 = {
#include "stg35/panels/panel_model_21.inc"
};
NJS_CNK_OBJECT eggman_panel_21 = {
#include "stg35/panels/eggman_panel_21.inc"
};
static Sint16 panel_scroll_keys_22[16] = {
#include "stg35/panels/panel_scroll_keys_22.inc"
};
struct texture_anim eggman_panel_uv_22 = {
#include "stg35/panels/eggman_panel_uv_22.inc"
};
static NJS_TEXNAME panel_names_22[1] = {
#include "stg35/panels/panel_names_22.inc"
};
static NJS_TEXLIST panel_textures_22 = {
#include "stg35/panels/panel_textures_22.inc"
};
static Sint16 panel_polygons_22[31] = {
#include "stg35/panels/panel_polygons_22.inc"
};
static Sint32 panel_vertices_22[27] = {
#include "stg35/panels/panel_vertices_22.inc"
};
static NJS_CNK_MODEL panel_model_22 = {
#include "stg35/panels/panel_model_22.inc"
};
NJS_CNK_OBJECT eggman_panel_22 = {
#include "stg35/panels/eggman_panel_22.inc"
};
static Sint16 panel_scroll_keys_23[16] = {
#include "stg35/panels/panel_scroll_keys_23.inc"
};
struct texture_anim eggman_panel_uv_23 = {
#include "stg35/panels/eggman_panel_uv_23.inc"
};
static NJS_TEXNAME panel_names_23[1] = {
#include "stg35/panels/panel_names_23.inc"
};
static NJS_TEXLIST panel_textures_23 = {
#include "stg35/panels/panel_textures_23.inc"
};
static Sint16 panel_polygons_23[31] = {
#include "stg35/panels/panel_polygons_23.inc"
};
static Sint32 panel_vertices_23[27] = {
#include "stg35/panels/panel_vertices_23.inc"
};
static NJS_CNK_MODEL panel_model_23 = {
#include "stg35/panels/panel_model_23.inc"
};
NJS_CNK_OBJECT eggman_panel_23 = {
#include "stg35/panels/eggman_panel_23.inc"
};
static Sint16 panel_scroll_keys_24[16] = {
#include "stg35/panels/panel_scroll_keys_24.inc"
};
struct texture_anim eggman_panel_uv_24 = {
#include "stg35/panels/eggman_panel_uv_24.inc"
};
static NJS_TEXNAME panel_names_24[1] = {
#include "stg35/panels/panel_names_24.inc"
};
static NJS_TEXLIST panel_textures_24 = {
#include "stg35/panels/panel_textures_24.inc"
};
static Sint16 panel_polygons_24[31] = {
#include "stg35/panels/panel_polygons_24.inc"
};
static Sint32 panel_vertices_24[27] = {
#include "stg35/panels/panel_vertices_24.inc"
};
static NJS_CNK_MODEL panel_model_24 = {
#include "stg35/panels/panel_model_24.inc"
};
NJS_CNK_OBJECT eggman_panel_24 = {
#include "stg35/panels/eggman_panel_24.inc"
};
static Sint16 panel_scroll_keys_25[16] = {
#include "stg35/panels/panel_scroll_keys_25.inc"
};
struct texture_anim eggman_panel_uv_25 = {
#include "stg35/panels/eggman_panel_uv_25.inc"
};
static NJS_TEXNAME panel_names_25[1] = {
#include "stg35/panels/panel_names_25.inc"
};
static NJS_TEXLIST panel_textures_25 = {
#include "stg35/panels/panel_textures_25.inc"
};
static Sint16 panel_polygons_25[31] = {
#include "stg35/panels/panel_polygons_25.inc"
};
static Sint32 panel_vertices_25[27] = {
#include "stg35/panels/panel_vertices_25.inc"
};
static NJS_CNK_MODEL panel_model_25 = {
#include "stg35/panels/panel_model_25.inc"
};
NJS_CNK_OBJECT eggman_panel_25 = {
#include "stg35/panels/eggman_panel_25.inc"
};
static Sint16 panel_scroll_keys_26[16] = {
#include "stg35/panels/panel_scroll_keys_26.inc"
};
struct texture_anim eggman_panel_uv_26 = {
#include "stg35/panels/eggman_panel_uv_26.inc"
};
static NJS_TEXNAME panel_names_26[1] = {
#include "stg35/panels/panel_names_26.inc"
};
static NJS_TEXLIST panel_textures_26 = {
#include "stg35/panels/panel_textures_26.inc"
};
static Sint16 panel_polygons_26[31] = {
#include "stg35/panels/panel_polygons_26.inc"
};
static Sint32 panel_vertices_26[27] = {
#include "stg35/panels/panel_vertices_26.inc"
};
static NJS_CNK_MODEL panel_model_26 = {
#include "stg35/panels/panel_model_26.inc"
};
NJS_CNK_OBJECT eggman_panel_26 = {
#include "stg35/panels/eggman_panel_26.inc"
};
static Sint16 panel_scroll_keys_27[16] = {
#include "stg35/panels/panel_scroll_keys_27.inc"
};
struct texture_anim eggman_panel_uv_27 = {
#include "stg35/panels/eggman_panel_uv_27.inc"
};
static NJS_TEXNAME panel_names_27[1] = {
#include "stg35/panels/panel_names_27.inc"
};
static NJS_TEXLIST panel_textures_27 = {
#include "stg35/panels/panel_textures_27.inc"
};
static Sint16 panel_polygons_27[31] = {
#include "stg35/panels/panel_polygons_27.inc"
};
static Sint32 panel_vertices_27[27] = {
#include "stg35/panels/panel_vertices_27.inc"
};
static NJS_CNK_MODEL panel_model_27 = {
#include "stg35/panels/panel_model_27.inc"
};
NJS_CNK_OBJECT eggman_panel_27 = {
#include "stg35/panels/eggman_panel_27.inc"
};
static Sint16 panel_scroll_keys_28[16] = {
#include "stg35/panels/panel_scroll_keys_28.inc"
};
struct texture_anim eggman_panel_uv_28 = {
#include "stg35/panels/eggman_panel_uv_28.inc"
};
static NJS_TEXNAME panel_names_28[1] = {
#include "stg35/panels/panel_names_28.inc"
};
static NJS_TEXLIST panel_textures_28 = {
#include "stg35/panels/panel_textures_28.inc"
};
static Sint16 panel_polygons_28[31] = {
#include "stg35/panels/panel_polygons_28.inc"
};
static Sint32 panel_vertices_28[27] = {
#include "stg35/panels/panel_vertices_28.inc"
};
static NJS_CNK_MODEL panel_model_28 = {
#include "stg35/panels/panel_model_28.inc"
};
NJS_CNK_OBJECT eggman_panel_28 = {
#include "stg35/panels/eggman_panel_28.inc"
};
static Sint16 panel_scroll_keys_29[16] = {
#include "stg35/panels/panel_scroll_keys_29.inc"
};
struct texture_anim eggman_panel_uv_29 = {
#include "stg35/panels/eggman_panel_uv_29.inc"
};
static NJS_TEXNAME panel_names_29[1] = {
#include "stg35/panels/panel_names_29.inc"
};
static NJS_TEXLIST panel_textures_29 = {
#include "stg35/panels/panel_textures_29.inc"
};
static Sint16 panel_polygons_29[31] = {
#include "stg35/panels/panel_polygons_29.inc"
};
static Sint32 panel_vertices_29[27] = {
#include "stg35/panels/panel_vertices_29.inc"
};
static NJS_CNK_MODEL panel_model_29 = {
#include "stg35/panels/panel_model_29.inc"
};
NJS_CNK_OBJECT eggman_panel_29 = {
#include "stg35/panels/eggman_panel_29.inc"
};
static Sint16 panel_scroll_keys_30[16] = {
#include "stg35/panels/panel_scroll_keys_30.inc"
};
struct texture_anim eggman_panel_uv_30 = {
#include "stg35/panels/eggman_panel_uv_30.inc"
};
static NJS_TEXNAME panel_names_30[1] = {
#include "stg35/panels/panel_names_30.inc"
};
static NJS_TEXLIST panel_textures_30 = {
#include "stg35/panels/panel_textures_30.inc"
};
static Sint16 panel_polygons_30[31] = {
#include "stg35/panels/panel_polygons_30.inc"
};
static Sint32 panel_vertices_30[27] = {
#include "stg35/panels/panel_vertices_30.inc"
};
static NJS_CNK_MODEL panel_model_30 = {
#include "stg35/panels/panel_model_30.inc"
};
NJS_CNK_OBJECT eggman_panel_30 = {
#include "stg35/panels/eggman_panel_30.inc"
};
static Sint16 panel_scroll_keys_31[70] = {
#include "stg35/panels/panel_scroll_keys_31.inc"
};
struct texture_anim eggman_panel_uv_31 = {
#include "stg35/panels/eggman_panel_uv_31.inc"
};
static NJS_TEXNAME panel_names_31[1] = {
#include "stg35/panels/panel_names_31.inc"
};
static NJS_TEXLIST panel_textures_31 = {
#include "stg35/panels/panel_textures_31.inc"
};
static Sint16 panel_polygons_31[107] = {
#include "stg35/panels/panel_polygons_31.inc"
};
static Sint32 panel_vertices_31[115] = {
#include "stg35/panels/panel_vertices_31.inc"
};
static NJS_CNK_MODEL panel_model_31 = {
#include "stg35/panels/panel_model_31.inc"
};
NJS_CNK_OBJECT eggman_panel_31 = {
#include "stg35/panels/eggman_panel_31.inc"
};
static Sint16 panel_scroll_keys_32[34] = {
#include "stg35/panels/panel_scroll_keys_32.inc"
};
struct texture_anim eggman_panel_uv_32 = {
#include "stg35/panels/eggman_panel_uv_32.inc"
};
static NJS_TEXNAME panel_names_32[1] = {
#include "stg35/panels/panel_names_32.inc"
};
static NJS_TEXLIST panel_textures_32 = {
#include "stg35/panels/panel_textures_32.inc"
};
static Sint16 panel_polygons_32[53] = {
#include "stg35/panels/panel_polygons_32.inc"
};
static Sint32 panel_vertices_32[23] = {
#include "stg35/panels/panel_vertices_32.inc"
};
static NJS_CNK_MODEL panel_model_32 = {
#include "stg35/panels/panel_model_32.inc"
};
NJS_CNK_OBJECT eggman_panel_32 = {
#include "stg35/panels/eggman_panel_32.inc"
};
static Sint16 panel_scroll_keys_33[34] = {
#include "stg35/panels/panel_scroll_keys_33.inc"
};
struct texture_anim eggman_panel_uv_33 = {
#include "stg35/panels/eggman_panel_uv_33.inc"
};
static NJS_TEXNAME panel_names_33[1] = {
#include "stg35/panels/panel_names_33.inc"
};
static NJS_TEXLIST panel_textures_33 = {
#include "stg35/panels/panel_textures_33.inc"
};
static Sint16 panel_polygons_33[53] = {
#include "stg35/panels/panel_polygons_33.inc"
};
static Sint32 panel_vertices_33[23] = {
#include "stg35/panels/panel_vertices_33.inc"
};
static NJS_CNK_MODEL panel_model_33 = {
#include "stg35/panels/panel_model_33.inc"
};
NJS_CNK_OBJECT eggman_panel_33 = {
#include "stg35/panels/eggman_panel_33.inc"
};
static Sint16 panel_scroll_keys_34[34] = {
#include "stg35/panels/panel_scroll_keys_34.inc"
};
struct texture_anim eggman_panel_uv_34 = {
#include "stg35/panels/eggman_panel_uv_34.inc"
};
static NJS_TEXNAME panel_names_34[1] = {
#include "stg35/panels/panel_names_34.inc"
};
static NJS_TEXLIST panel_textures_34 = {
#include "stg35/panels/panel_textures_34.inc"
};
static Sint16 panel_polygons_34[53] = {
#include "stg35/panels/panel_polygons_34.inc"
};
static Sint32 panel_vertices_34[23] = {
#include "stg35/panels/panel_vertices_34.inc"
};
static NJS_CNK_MODEL panel_model_34 = {
#include "stg35/panels/panel_model_34.inc"
};
NJS_CNK_OBJECT eggman_panel_34 = {
#include "stg35/panels/eggman_panel_34.inc"
};
static Sint16 panel_scroll_keys_35[34] = {
#include "stg35/panels/panel_scroll_keys_35.inc"
};
struct texture_anim eggman_panel_uv_35 = {
#include "stg35/panels/eggman_panel_uv_35.inc"
};
static NJS_TEXNAME panel_names_35[1] = {
#include "stg35/panels/panel_names_35.inc"
};
static NJS_TEXLIST panel_textures_35 = {
#include "stg35/panels/panel_textures_35.inc"
};
static Sint16 panel_polygons_35[53] = {
#include "stg35/panels/panel_polygons_35.inc"
};
static Sint32 panel_vertices_35[23] = {
#include "stg35/panels/panel_vertices_35.inc"
};
static NJS_CNK_MODEL panel_model_35 = {
#include "stg35/panels/panel_model_35.inc"
};
NJS_CNK_OBJECT eggman_panel_35 = {
#include "stg35/panels/eggman_panel_35.inc"
};
static Sint16 panel_scroll_keys_36[34] = {
#include "stg35/panels/panel_scroll_keys_36.inc"
};
struct texture_anim eggman_panel_uv_36 = {
#include "stg35/panels/eggman_panel_uv_36.inc"
};
static NJS_TEXNAME panel_names_36[1] = {
#include "stg35/panels/panel_names_36.inc"
};
static NJS_TEXLIST panel_textures_36 = {
#include "stg35/panels/panel_textures_36.inc"
};
static Sint16 panel_polygons_36[53] = {
#include "stg35/panels/panel_polygons_36.inc"
};
static Sint32 panel_vertices_36[23] = {
#include "stg35/panels/panel_vertices_36.inc"
};
static NJS_CNK_MODEL panel_model_36 = {
#include "stg35/panels/panel_model_36.inc"
};
NJS_CNK_OBJECT eggman_panel_36 = {
#include "stg35/panels/eggman_panel_36.inc"
};
static Sint16 panel_scroll_keys_37[34] = {
#include "stg35/panels/panel_scroll_keys_37.inc"
};
struct texture_anim eggman_panel_uv_37 = {
#include "stg35/panels/eggman_panel_uv_37.inc"
};
static NJS_TEXNAME panel_names_37[1] = {
#include "stg35/panels/panel_names_37.inc"
};
static NJS_TEXLIST panel_textures_37 = {
#include "stg35/panels/panel_textures_37.inc"
};
static Sint16 panel_polygons_37[53] = {
#include "stg35/panels/panel_polygons_37.inc"
};
static Sint32 panel_vertices_37[23] = {
#include "stg35/panels/panel_vertices_37.inc"
};
static NJS_CNK_MODEL panel_model_37 = {
#include "stg35/panels/panel_model_37.inc"
};
NJS_CNK_OBJECT eggman_panel_37 = {
#include "stg35/panels/eggman_panel_37.inc"
};
static Sint16 panel_scroll_keys_38[82] = {
#include "stg35/panels/panel_scroll_keys_38.inc"
};
struct texture_anim eggman_panel_uv_38 = {
#include "stg35/panels/eggman_panel_uv_38.inc"
};
static NJS_TEXNAME panel_names_38[1] = {
#include "stg35/panels/panel_names_38.inc"
};
static NJS_TEXLIST panel_textures_38 = {
#include "stg35/panels/panel_textures_38.inc"
};
static Sint16 panel_polygons_38[117] = {
#include "stg35/panels/panel_polygons_38.inc"
};
static Sint32 panel_vertices_38[131] = {
#include "stg35/panels/panel_vertices_38.inc"
};
static NJS_CNK_MODEL panel_model_38 = {
#include "stg35/panels/panel_model_38.inc"
};
NJS_CNK_OBJECT eggman_panel_38 = {
#include "stg35/panels/eggman_panel_38.inc"
};
static Sint16 panel_scroll_keys_39[82] = {
#include "stg35/panels/panel_scroll_keys_39.inc"
};
struct texture_anim eggman_panel_uv_39 = {
#include "stg35/panels/eggman_panel_uv_39.inc"
};
static NJS_TEXNAME panel_names_39[1] = {
#include "stg35/panels/panel_names_39.inc"
};
static NJS_TEXLIST panel_textures_39 = {
#include "stg35/panels/panel_textures_39.inc"
};
static Sint16 panel_polygons_39[117] = {
#include "stg35/panels/panel_polygons_39.inc"
};
static Sint32 panel_vertices_39[131] = {
#include "stg35/panels/panel_vertices_39.inc"
};
static NJS_CNK_MODEL panel_model_39 = {
#include "stg35/panels/panel_model_39.inc"
};
NJS_CNK_OBJECT eggman_panel_39 = {
#include "stg35/panels/eggman_panel_39.inc"
};
static Sint16 panel_scroll_keys_40[82] = {
#include "stg35/panels/panel_scroll_keys_40.inc"
};
struct texture_anim eggman_panel_uv_40 = {
#include "stg35/panels/eggman_panel_uv_40.inc"
};
static NJS_TEXNAME panel_names_40[1] = {
#include "stg35/panels/panel_names_40.inc"
};
static NJS_TEXLIST panel_textures_40 = {
#include "stg35/panels/panel_textures_40.inc"
};
static Sint16 panel_polygons_40[117] = {
#include "stg35/panels/panel_polygons_40.inc"
};
static Sint32 panel_vertices_40[131] = {
#include "stg35/panels/panel_vertices_40.inc"
};
static NJS_CNK_MODEL panel_model_40 = {
#include "stg35/panels/panel_model_40.inc"
};
NJS_CNK_OBJECT eggman_panel_40 = {
#include "stg35/panels/eggman_panel_40.inc"
};
static Sint16 panel_scroll_keys_41[42] = {
#include "stg35/panels/panel_scroll_keys_41.inc"
};
struct texture_anim eggman_panel_uv_41 = {
#include "stg35/panels/eggman_panel_uv_41.inc"
};
static NJS_TEXNAME panel_names_41[1] = {
#include "stg35/panels/panel_names_41.inc"
};
static NJS_TEXLIST panel_textures_41 = {
#include "stg35/panels/panel_textures_41.inc"
};
static Sint16 panel_polygons_41[65] = {
#include "stg35/panels/panel_polygons_41.inc"
};
static Sint32 panel_vertices_41[35] = {
#include "stg35/panels/panel_vertices_41.inc"
};
static NJS_CNK_MODEL panel_model_41 = {
#include "stg35/panels/panel_model_41.inc"
};
NJS_CNK_OBJECT eggman_panel_41 = {
#include "stg35/panels/eggman_panel_41.inc"
};
static Sint16 panel_scroll_keys_42[42] = {
#include "stg35/panels/panel_scroll_keys_42.inc"
};
struct texture_anim eggman_panel_uv_42 = {
#include "stg35/panels/eggman_panel_uv_42.inc"
};
static NJS_TEXNAME panel_names_42[1] = {
#include "stg35/panels/panel_names_42.inc"
};
static NJS_TEXLIST panel_textures_42 = {
#include "stg35/panels/panel_textures_42.inc"
};
static Sint16 panel_polygons_42[65] = {
#include "stg35/panels/panel_polygons_42.inc"
};
static Sint32 panel_vertices_42[35] = {
#include "stg35/panels/panel_vertices_42.inc"
};
static NJS_CNK_MODEL panel_model_42 = {
#include "stg35/panels/panel_model_42.inc"
};
NJS_CNK_OBJECT eggman_panel_42 = {
#include "stg35/panels/eggman_panel_42.inc"
};
static Sint16 panel_scroll_keys_43[42] = {
#include "stg35/panels/panel_scroll_keys_43.inc"
};
struct texture_anim eggman_panel_uv_43 = {
#include "stg35/panels/eggman_panel_uv_43.inc"
};
static NJS_TEXNAME panel_names_43[1] = {
#include "stg35/panels/panel_names_43.inc"
};
static NJS_TEXLIST panel_textures_43 = {
#include "stg35/panels/panel_textures_43.inc"
};
static Sint16 panel_polygons_43[65] = {
#include "stg35/panels/panel_polygons_43.inc"
};
static Sint32 panel_vertices_43[35] = {
#include "stg35/panels/panel_vertices_43.inc"
};
static NJS_CNK_MODEL panel_model_43 = {
#include "stg35/panels/panel_model_43.inc"
};
NJS_CNK_OBJECT eggman_panel_43 = {
#include "stg35/panels/eggman_panel_43.inc"
};
static Sint16 panel_scroll_keys_44[42] = {
#include "stg35/panels/panel_scroll_keys_44.inc"
};
struct texture_anim eggman_panel_uv_44 = {
#include "stg35/panels/eggman_panel_uv_44.inc"
};
static NJS_TEXNAME panel_names_44[1] = {
#include "stg35/panels/panel_names_44.inc"
};
static NJS_TEXLIST panel_textures_44 = {
#include "stg35/panels/panel_textures_44.inc"
};
static Sint16 panel_polygons_44[65] = {
#include "stg35/panels/panel_polygons_44.inc"
};
static Sint32 panel_vertices_44[35] = {
#include "stg35/panels/panel_vertices_44.inc"
};
static NJS_CNK_MODEL panel_model_44 = {
#include "stg35/panels/panel_model_44.inc"
};
NJS_CNK_OBJECT eggman_panel_44 = {
#include "stg35/panels/eggman_panel_44.inc"
};
static Sint16 panel_scroll_keys_45[42] = {
#include "stg35/panels/panel_scroll_keys_45.inc"
};
struct texture_anim eggman_panel_uv_45 = {
#include "stg35/panels/eggman_panel_uv_45.inc"
};
static NJS_TEXNAME panel_names_45[1] = {
#include "stg35/panels/panel_names_45.inc"
};
static NJS_TEXLIST panel_textures_45 = {
#include "stg35/panels/panel_textures_45.inc"
};
static Sint16 panel_polygons_45[65] = {
#include "stg35/panels/panel_polygons_45.inc"
};
static Sint32 panel_vertices_45[35] = {
#include "stg35/panels/panel_vertices_45.inc"
};
static NJS_CNK_MODEL panel_model_45 = {
#include "stg35/panels/panel_model_45.inc"
};
NJS_CNK_OBJECT eggman_panel_45 = {
#include "stg35/panels/eggman_panel_45.inc"
};
static Sint16 panel_scroll_keys_46[42] = {
#include "stg35/panels/panel_scroll_keys_46.inc"
};
struct texture_anim eggman_panel_uv_46 = {
#include "stg35/panels/eggman_panel_uv_46.inc"
};
static NJS_TEXNAME panel_names_46[1] = {
#include "stg35/panels/panel_names_46.inc"
};
static NJS_TEXLIST panel_textures_46 = {
#include "stg35/panels/panel_textures_46.inc"
};
static Sint16 panel_polygons_46[65] = {
#include "stg35/panels/panel_polygons_46.inc"
};
static Sint32 panel_vertices_46[35] = {
#include "stg35/panels/panel_vertices_46.inc"
};
static NJS_CNK_MODEL panel_model_46 = {
#include "stg35/panels/panel_model_46.inc"
};
NJS_CNK_OBJECT eggman_panel_46 = {
#include "stg35/panels/eggman_panel_46.inc"
};
static Sint16 panel_scroll_keys_47[42] = {
#include "stg35/panels/panel_scroll_keys_47.inc"
};
struct texture_anim eggman_panel_uv_47 = {
#include "stg35/panels/eggman_panel_uv_47.inc"
};
static NJS_TEXNAME panel_names_47[1] = {
#include "stg35/panels/panel_names_47.inc"
};
static NJS_TEXLIST panel_textures_47 = {
#include "stg35/panels/panel_textures_47.inc"
};
static Sint16 panel_polygons_47[65] = {
#include "stg35/panels/panel_polygons_47.inc"
};
static Sint32 panel_vertices_47[35] = {
#include "stg35/panels/panel_vertices_47.inc"
};
static NJS_CNK_MODEL panel_model_47 = {
#include "stg35/panels/panel_model_47.inc"
};
NJS_CNK_OBJECT eggman_panel_47 = {
#include "stg35/panels/eggman_panel_47.inc"
};
static Sint16 panel_scroll_keys_48[42] = {
#include "stg35/panels/panel_scroll_keys_48.inc"
};
struct texture_anim eggman_panel_uv_48 = {
#include "stg35/panels/eggman_panel_uv_48.inc"
};
static NJS_TEXNAME panel_names_48[1] = {
#include "stg35/panels/panel_names_48.inc"
};
static NJS_TEXLIST panel_textures_48 = {
#include "stg35/panels/panel_textures_48.inc"
};
static Sint16 panel_polygons_48[65] = {
#include "stg35/panels/panel_polygons_48.inc"
};
static Sint32 panel_vertices_48[35] = {
#include "stg35/panels/panel_vertices_48.inc"
};
static NJS_CNK_MODEL panel_model_48 = {
#include "stg35/panels/panel_model_48.inc"
};
NJS_CNK_OBJECT eggman_panel_48 = {
#include "stg35/panels/eggman_panel_48.inc"
};
static Sint16 panel_scroll_keys_49[42] = {
#include "stg35/panels/panel_scroll_keys_49.inc"
};
struct texture_anim eggman_panel_uv_49 = {
#include "stg35/panels/eggman_panel_uv_49.inc"
};
static NJS_TEXNAME panel_names_49[1] = {
#include "stg35/panels/panel_names_49.inc"
};
static NJS_TEXLIST panel_textures_49 = {
#include "stg35/panels/panel_textures_49.inc"
};
static Sint16 panel_polygons_49[65] = {
#include "stg35/panels/panel_polygons_49.inc"
};
static Sint32 panel_vertices_49[35] = {
#include "stg35/panels/panel_vertices_49.inc"
};
static NJS_CNK_MODEL panel_model_49 = {
#include "stg35/panels/panel_model_49.inc"
};
NJS_CNK_OBJECT eggman_panel_49 = {
#include "stg35/panels/eggman_panel_49.inc"
};
static Sint16 panel_scroll_keys_50[42] = {
#include "stg35/panels/panel_scroll_keys_50.inc"
};
struct texture_anim eggman_panel_uv_50 = {
#include "stg35/panels/eggman_panel_uv_50.inc"
};
static NJS_TEXNAME panel_names_50[1] = {
#include "stg35/panels/panel_names_50.inc"
};
static NJS_TEXLIST panel_textures_50 = {
#include "stg35/panels/panel_textures_50.inc"
};
static Sint16 panel_polygons_50[65] = {
#include "stg35/panels/panel_polygons_50.inc"
};
static Sint32 panel_vertices_50[35] = {
#include "stg35/panels/panel_vertices_50.inc"
};
static NJS_CNK_MODEL panel_model_50 = {
#include "stg35/panels/panel_model_50.inc"
};
NJS_CNK_OBJECT eggman_panel_50 = {
#include "stg35/panels/eggman_panel_50.inc"
};
static Sint16 panel_scroll_keys_51[42] = {
#include "stg35/panels/panel_scroll_keys_51.inc"
};
struct texture_anim eggman_panel_uv_51 = {
#include "stg35/panels/eggman_panel_uv_51.inc"
};
static NJS_TEXNAME panel_names_51[1] = {
#include "stg35/panels/panel_names_51.inc"
};
static NJS_TEXLIST panel_textures_51 = {
#include "stg35/panels/panel_textures_51.inc"
};
static Sint16 panel_polygons_51[65] = {
#include "stg35/panels/panel_polygons_51.inc"
};
static Sint32 panel_vertices_51[35] = {
#include "stg35/panels/panel_vertices_51.inc"
};
static NJS_CNK_MODEL panel_model_51 = {
#include "stg35/panels/panel_model_51.inc"
};
NJS_CNK_OBJECT eggman_panel_51 = {
#include "stg35/panels/eggman_panel_51.inc"
};
static Sint16 panel_scroll_keys_52[42] = {
#include "stg35/panels/panel_scroll_keys_52.inc"
};
struct texture_anim eggman_panel_uv_52 = {
#include "stg35/panels/eggman_panel_uv_52.inc"
};
static NJS_TEXNAME panel_names_52[1] = {
#include "stg35/panels/panel_names_52.inc"
};
static NJS_TEXLIST panel_textures_52 = {
#include "stg35/panels/panel_textures_52.inc"
};
static Sint16 panel_polygons_52[65] = {
#include "stg35/panels/panel_polygons_52.inc"
};
static Sint32 panel_vertices_52[35] = {
#include "stg35/panels/panel_vertices_52.inc"
};
static NJS_CNK_MODEL panel_model_52 = {
#include "stg35/panels/panel_model_52.inc"
};
NJS_CNK_OBJECT eggman_panel_52 = {
#include "stg35/panels/eggman_panel_52.inc"
};
static Sint16 panel_scroll_keys_53[42] = {
#include "stg35/panels/panel_scroll_keys_53.inc"
};
struct texture_anim eggman_panel_uv_53 = {
#include "stg35/panels/eggman_panel_uv_53.inc"
};
static NJS_TEXNAME panel_names_53[1] = {
#include "stg35/panels/panel_names_53.inc"
};
static NJS_TEXLIST panel_textures_53 = {
#include "stg35/panels/panel_textures_53.inc"
};
static Sint16 panel_polygons_53[65] = {
#include "stg35/panels/panel_polygons_53.inc"
};
static Sint32 panel_vertices_53[35] = {
#include "stg35/panels/panel_vertices_53.inc"
};
static NJS_CNK_MODEL panel_model_53 = {
#include "stg35/panels/panel_model_53.inc"
};
NJS_CNK_OBJECT eggman_panel_53 = {
#include "stg35/panels/eggman_panel_53.inc"
};
static Sint16 panel_scroll_keys_54[90] = {
#include "stg35/panels/panel_scroll_keys_54.inc"
};
struct texture_anim eggman_panel_uv_54 = {
#include "stg35/panels/eggman_panel_uv_54.inc"
};
static NJS_TEXNAME panel_names_54[1] = {
#include "stg35/panels/panel_names_54.inc"
};
static NJS_TEXLIST panel_textures_54 = {
#include "stg35/panels/panel_textures_54.inc"
};
static Sint16 panel_polygons_54[137] = {
#include "stg35/panels/panel_polygons_54.inc"
};
static Sint32 panel_vertices_54[163] = {
#include "stg35/panels/panel_vertices_54.inc"
};
static NJS_CNK_MODEL panel_model_54 = {
#include "stg35/panels/panel_model_54.inc"
};
NJS_CNK_OBJECT eggman_panel_54 = {
#include "stg35/panels/eggman_panel_54.inc"
};
static Sint16 panel_scroll_keys_55[102] = {
#include "stg35/panels/panel_scroll_keys_55.inc"
};
struct texture_anim eggman_panel_uv_55 = {
#include "stg35/panels/eggman_panel_uv_55.inc"
};
static NJS_TEXNAME panel_names_55[1] = {
#include "stg35/panels/panel_names_55.inc"
};
static NJS_TEXLIST panel_textures_55 = {
#include "stg35/panels/panel_textures_55.inc"
};
static Sint16 panel_polygons_55[151] = {
#include "stg35/panels/panel_polygons_55.inc"
};
static Sint32 panel_vertices_55[99] = {
#include "stg35/panels/panel_vertices_55.inc"
};
static NJS_CNK_MODEL panel_model_55 = {
#include "stg35/panels/panel_model_55.inc"
};
NJS_CNK_OBJECT eggman_panel_55 = {
#include "stg35/panels/eggman_panel_55.inc"
};
static Sint16 panel_scroll_keys_56[144] = {
#include "stg35/panels/panel_scroll_keys_56.inc"
};
struct texture_anim eggman_panel_uv_56 = {
#include "stg35/panels/eggman_panel_uv_56.inc"
};
static NJS_TEXNAME panel_names_56[1] = {
#include "stg35/panels/panel_names_56.inc"
};
static NJS_TEXLIST panel_textures_56 = {
#include "stg35/panels/panel_textures_56.inc"
};
static Sint16 panel_polygons_56[219] = {
#include "stg35/panels/panel_polygons_56.inc"
};
static Sint32 panel_vertices_56[163] = {
#include "stg35/panels/panel_vertices_56.inc"
};
static NJS_CNK_MODEL panel_model_56 = {
#include "stg35/panels/panel_model_56.inc"
};
NJS_CNK_OBJECT eggman_panel_56 = {
#include "stg35/panels/eggman_panel_56.inc"
};
static Sint16 panel_scroll_keys_57[46] = {
#include "stg35/panels/panel_scroll_keys_57.inc"
};
struct texture_anim eggman_panel_uv_57 = {
#include "stg35/panels/eggman_panel_uv_57.inc"
};
static NJS_TEXNAME panel_names_57[1] = {
#include "stg35/panels/panel_names_57.inc"
};
static NJS_TEXLIST panel_textures_57 = {
#include "stg35/panels/panel_textures_57.inc"
};
static Sint16 panel_polygons_57[75] = {
#include "stg35/panels/panel_polygons_57.inc"
};
static Sint32 panel_vertices_57[51] = {
#include "stg35/panels/panel_vertices_57.inc"
};
static NJS_CNK_MODEL panel_model_57 = {
#include "stg35/panels/panel_model_57.inc"
};
NJS_CNK_OBJECT eggman_panel_57 = {
#include "stg35/panels/eggman_panel_57.inc"
};
static Sint16 panel_scroll_keys_58[38] = {
#include "stg35/panels/panel_scroll_keys_58.inc"
};
struct texture_anim eggman_panel_uv_58 = {
#include "stg35/panels/eggman_panel_uv_58.inc"
};
static NJS_TEXNAME panel_names_58[1] = {
#include "stg35/panels/panel_names_58.inc"
};
static NJS_TEXLIST panel_textures_58 = {
#include "stg35/panels/panel_textures_58.inc"
};
static Sint16 panel_polygons_58[63] = {
#include "stg35/panels/panel_polygons_58.inc"
};
static Sint32 panel_vertices_58[67] = {
#include "stg35/panels/panel_vertices_58.inc"
};
static NJS_CNK_MODEL panel_model_58 = {
#include "stg35/panels/panel_model_58.inc"
};
NJS_CNK_OBJECT eggman_panel_58 = {
#include "stg35/panels/eggman_panel_58.inc"
};
static Sint16 panel_scroll_keys_59[38] = {
#include "stg35/panels/panel_scroll_keys_59.inc"
};
struct texture_anim eggman_panel_uv_59 = {
#include "stg35/panels/eggman_panel_uv_59.inc"
};
static NJS_TEXNAME panel_names_59[1] = {
#include "stg35/panels/panel_names_59.inc"
};
static NJS_TEXLIST panel_textures_59 = {
#include "stg35/panels/panel_textures_59.inc"
};
static Sint16 panel_polygons_59[63] = {
#include "stg35/panels/panel_polygons_59.inc"
};
static Sint32 panel_vertices_59[67] = {
#include "stg35/panels/panel_vertices_59.inc"
};
static NJS_CNK_MODEL panel_model_59 = {
#include "stg35/panels/panel_model_59.inc"
};
NJS_CNK_OBJECT eggman_panel_59 = {
#include "stg35/panels/eggman_panel_59.inc"
};
static Sint16 panel_scroll_keys_60[38] = {
#include "stg35/panels/panel_scroll_keys_60.inc"
};
struct texture_anim eggman_panel_uv_60 = {
#include "stg35/panels/eggman_panel_uv_60.inc"
};
static NJS_TEXNAME panel_names_60[1] = {
#include "stg35/panels/panel_names_60.inc"
};
static NJS_TEXLIST panel_textures_60 = {
#include "stg35/panels/panel_textures_60.inc"
};
static Sint16 panel_polygons_60[63] = {
#include "stg35/panels/panel_polygons_60.inc"
};
static Sint32 panel_vertices_60[67] = {
#include "stg35/panels/panel_vertices_60.inc"
};
static NJS_CNK_MODEL panel_model_60 = {
#include "stg35/panels/panel_model_60.inc"
};
NJS_CNK_OBJECT eggman_panel_60 = {
#include "stg35/panels/eggman_panel_60.inc"
};
static Sint16 panel_scroll_keys_61[24] = {
#include "stg35/panels/panel_scroll_keys_61.inc"
};
struct texture_anim eggman_panel_uv_61 = {
#include "stg35/panels/eggman_panel_uv_61.inc"
};
static NJS_TEXNAME panel_names_61[1] = {
#include "stg35/panels/panel_names_61.inc"
};
static NJS_TEXLIST panel_textures_61 = {
#include "stg35/panels/panel_textures_61.inc"
};
static Sint16 panel_polygons_61[43] = {
#include "stg35/panels/panel_polygons_61.inc"
};
static Sint32 panel_vertices_61[43] = {
#include "stg35/panels/panel_vertices_61.inc"
};
static NJS_CNK_MODEL panel_model_61 = {
#include "stg35/panels/panel_model_61.inc"
};
NJS_CNK_OBJECT eggman_panel_61 = {
#include "stg35/panels/eggman_panel_61.inc"
};
