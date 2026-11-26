/* the strip lamp's texture list and model, and the four strips' lamps. each of
 * those lamps is an object whose model is a triangle. the draw puts the strip
 * lamp's model at the object's pos and ang. the rings' lamps follow */

#include "shared/core_lamp_art.h"
#include "stg35/machinery.h"
#include "stg35/ring_lamps.h"

#include <ninja.h>
#include <sg_xpt.h>

/* The next stage filename begins on a four-byte boundary. */
const char core_strip_texture_name[16] = {
#include "stg35/lamp_art/core_strip_texture_name.inc"
};

static NJS_TEXNAME strip_names[1] = {
#include "stg35/lamp_art/strip_names.inc"
};

NJS_TEXLIST eggman_strip_texlist = {
#include "stg35/lamp_art/eggman_strip_texlist.inc"
};

static Sint16 strip_lamp_polygons[25] = {
#include "stg35/lamp_art/strip_lamp_polygons.inc"
};

static Sint32 strip_lamp_vertices[19] = {
#include "stg35/lamp_art/strip_lamp_vertices.inc"
};

NJS_CNK_MODEL core_strip_lamp_model = {
#include "stg35/lamp_art/core_strip_lamp_model.inc"
};

static NJS_CNK_OBJECT core_strip_lamp_object = {
#include "stg35/lamp_art/core_strip_lamp_object.inc"
};

static Sint16 core_lamp_d_4_polygons[15] = {
#include "stg35/lamp_art/core_lamp_d_4_polygons.inc"
};

static Sint32 core_lamp_d_4_vertices[15] = {
#include "stg35/lamp_art/core_lamp_d_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_4_model = {
#include "stg35/lamp_art/core_lamp_d_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_4 = {
#include "stg35/lamp_art/core_lamp_d_4.inc"
};

static Sint16 core_lamp_d_5_polygons[15] = {
#include "stg35/lamp_art/core_lamp_d_5_polygons.inc"
};

static Sint32 core_lamp_d_5_vertices[15] = {
#include "stg35/lamp_art/core_lamp_d_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_5_model = {
#include "stg35/lamp_art/core_lamp_d_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_5 = {
#include "stg35/lamp_art/core_lamp_d_5.inc"
};

static Sint16 core_lamp_d_6_polygons[15] = {
#include "stg35/lamp_art/core_lamp_d_6_polygons.inc"
};

static Sint32 core_lamp_d_6_vertices[15] = {
#include "stg35/lamp_art/core_lamp_d_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_6_model = {
#include "stg35/lamp_art/core_lamp_d_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_6 = {
#include "stg35/lamp_art/core_lamp_d_6.inc"
};

static Sint16 core_lamp_d_3_polygons[15] = {
#include "stg35/lamp_art/core_lamp_d_3_polygons.inc"
};

static Sint32 core_lamp_d_3_vertices[15] = {
#include "stg35/lamp_art/core_lamp_d_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_3_model = {
#include "stg35/lamp_art/core_lamp_d_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_3 = {
#include "stg35/lamp_art/core_lamp_d_3.inc"
};

static Sint16 core_lamp_d_2_polygons[15] = {
#include "stg35/lamp_art/core_lamp_d_2_polygons.inc"
};

static Sint32 core_lamp_d_2_vertices[15] = {
#include "stg35/lamp_art/core_lamp_d_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_2_model = {
#include "stg35/lamp_art/core_lamp_d_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_2 = {
#include "stg35/lamp_art/core_lamp_d_2.inc"
};

static Sint16 core_lamp_d_1_polygons[15] = {
#include "stg35/lamp_art/core_lamp_d_1_polygons.inc"
};

static Sint32 core_lamp_d_1_vertices[15] = {
#include "stg35/lamp_art/core_lamp_d_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_1_model = {
#include "stg35/lamp_art/core_lamp_d_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_1 = {
#include "stg35/lamp_art/core_lamp_d_1.inc"
};

static Sint16 core_lamp_b_6_polygons[15] = {
#include "stg35/lamp_art/core_lamp_b_6_polygons.inc"
};

static Sint32 core_lamp_b_6_vertices[15] = {
#include "stg35/lamp_art/core_lamp_b_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_6_model = {
#include "stg35/lamp_art/core_lamp_b_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_6 = {
#include "stg35/lamp_art/core_lamp_b_6.inc"
};

static Sint16 core_lamp_b_5_polygons[15] = {
#include "stg35/lamp_art/core_lamp_b_5_polygons.inc"
};

static Sint32 core_lamp_b_5_vertices[15] = {
#include "stg35/lamp_art/core_lamp_b_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_5_model = {
#include "stg35/lamp_art/core_lamp_b_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_5 = {
#include "stg35/lamp_art/core_lamp_b_5.inc"
};

static Sint16 core_lamp_b_4_polygons[15] = {
#include "stg35/lamp_art/core_lamp_b_4_polygons.inc"
};

static Sint32 core_lamp_b_4_vertices[15] = {
#include "stg35/lamp_art/core_lamp_b_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_4_model = {
#include "stg35/lamp_art/core_lamp_b_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_4 = {
#include "stg35/lamp_art/core_lamp_b_4.inc"
};

static Sint16 core_lamp_b_3_polygons[15] = {
#include "stg35/lamp_art/core_lamp_b_3_polygons.inc"
};

static Sint32 core_lamp_b_3_vertices[15] = {
#include "stg35/lamp_art/core_lamp_b_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_3_model = {
#include "stg35/lamp_art/core_lamp_b_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_3 = {
#include "stg35/lamp_art/core_lamp_b_3.inc"
};

static Sint16 core_lamp_b_2_polygons[15] = {
#include "stg35/lamp_art/core_lamp_b_2_polygons.inc"
};

static Sint32 core_lamp_b_2_vertices[15] = {
#include "stg35/lamp_art/core_lamp_b_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_2_model = {
#include "stg35/lamp_art/core_lamp_b_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_2 = {
#include "stg35/lamp_art/core_lamp_b_2.inc"
};

static Sint16 core_lamp_b_1_polygons[15] = {
#include "stg35/lamp_art/core_lamp_b_1_polygons.inc"
};

static Sint32 core_lamp_b_1_vertices[15] = {
#include "stg35/lamp_art/core_lamp_b_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_1_model = {
#include "stg35/lamp_art/core_lamp_b_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_1 = {
#include "stg35/lamp_art/core_lamp_b_1.inc"
};

static Sint16 core_lamp_c_6_polygons[15] = {
#include "stg35/lamp_art/core_lamp_c_6_polygons.inc"
};

static Sint32 core_lamp_c_6_vertices[15] = {
#include "stg35/lamp_art/core_lamp_c_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_6_model = {
#include "stg35/lamp_art/core_lamp_c_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_6 = {
#include "stg35/lamp_art/core_lamp_c_6.inc"
};

static Sint16 core_lamp_c_5_polygons[15] = {
#include "stg35/lamp_art/core_lamp_c_5_polygons.inc"
};

static Sint32 core_lamp_c_5_vertices[15] = {
#include "stg35/lamp_art/core_lamp_c_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_5_model = {
#include "stg35/lamp_art/core_lamp_c_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_5 = {
#include "stg35/lamp_art/core_lamp_c_5.inc"
};

static Sint16 core_lamp_c_4_polygons[15] = {
#include "stg35/lamp_art/core_lamp_c_4_polygons.inc"
};

static Sint32 core_lamp_c_4_vertices[15] = {
#include "stg35/lamp_art/core_lamp_c_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_4_model = {
#include "stg35/lamp_art/core_lamp_c_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_4 = {
#include "stg35/lamp_art/core_lamp_c_4.inc"
};

static Sint16 core_lamp_c_3_polygons[15] = {
#include "stg35/lamp_art/core_lamp_c_3_polygons.inc"
};

static Sint32 core_lamp_c_3_vertices[15] = {
#include "stg35/lamp_art/core_lamp_c_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_3_model = {
#include "stg35/lamp_art/core_lamp_c_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_3 = {
#include "stg35/lamp_art/core_lamp_c_3.inc"
};

static Sint16 core_lamp_c_2_polygons[15] = {
#include "stg35/lamp_art/core_lamp_c_2_polygons.inc"
};

static Sint32 core_lamp_c_2_vertices[15] = {
#include "stg35/lamp_art/core_lamp_c_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_2_model = {
#include "stg35/lamp_art/core_lamp_c_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_2 = {
#include "stg35/lamp_art/core_lamp_c_2.inc"
};

static Sint16 core_lamp_c_1_polygons[15] = {
#include "stg35/lamp_art/core_lamp_c_1_polygons.inc"
};

static Sint32 core_lamp_c_1_vertices[15] = {
#include "stg35/lamp_art/core_lamp_c_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_1_model = {
#include "stg35/lamp_art/core_lamp_c_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_1 = {
#include "stg35/lamp_art/core_lamp_c_1.inc"
};

static Sint16 core_lamp_a_1_polygons[15] = {
#include "stg35/lamp_art/core_lamp_a_1_polygons.inc"
};

static Sint32 core_lamp_a_1_vertices[15] = {
#include "stg35/lamp_art/core_lamp_a_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_1_model = {
#include "stg35/lamp_art/core_lamp_a_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_1 = {
#include "stg35/lamp_art/core_lamp_a_1.inc"
};

static Sint16 core_lamp_a_2_polygons[15] = {
#include "stg35/lamp_art/core_lamp_a_2_polygons.inc"
};

static Sint32 core_lamp_a_2_vertices[15] = {
#include "stg35/lamp_art/core_lamp_a_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_2_model = {
#include "stg35/lamp_art/core_lamp_a_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_2 = {
#include "stg35/lamp_art/core_lamp_a_2.inc"
};

static Sint16 core_lamp_a_3_polygons[15] = {
#include "stg35/lamp_art/core_lamp_a_3_polygons.inc"
};

static Sint32 core_lamp_a_3_vertices[15] = {
#include "stg35/lamp_art/core_lamp_a_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_3_model = {
#include "stg35/lamp_art/core_lamp_a_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_3 = {
#include "stg35/lamp_art/core_lamp_a_3.inc"
};

static Sint16 core_lamp_a_4_polygons[15] = {
#include "stg35/lamp_art/core_lamp_a_4_polygons.inc"
};

static Sint32 core_lamp_a_4_vertices[15] = {
#include "stg35/lamp_art/core_lamp_a_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_4_model = {
#include "stg35/lamp_art/core_lamp_a_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_4 = {
#include "stg35/lamp_art/core_lamp_a_4.inc"
};

static Sint16 core_lamp_a_5_polygons[15] = {
#include "stg35/lamp_art/core_lamp_a_5_polygons.inc"
};

static Sint32 core_lamp_a_5_vertices[15] = {
#include "stg35/lamp_art/core_lamp_a_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_5_model = {
#include "stg35/lamp_art/core_lamp_a_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_5 = {
#include "stg35/lamp_art/core_lamp_a_5.inc"
};

static Sint16 core_lamp_a_6_polygons[15] = {
#include "stg35/lamp_art/core_lamp_a_6_polygons.inc"
};

static Sint32 core_lamp_a_6_vertices[15] = {
#include "stg35/lamp_art/core_lamp_a_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_6_model = {
#include "stg35/lamp_art/core_lamp_a_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_6 = {
#include "stg35/lamp_art/core_lamp_a_6.inc"
};

static NJS_CNK_OBJECT strip_root = {
#include "stg35/lamp_art/strip_root.inc"
};

/* The next stage filename begins on a four-byte boundary. */
const char eggman_inner_lamp_texture_name[12] = {
#include "stg35/lamp_art/eggman_inner_lamp_texture_name.inc"
};

static NJS_TEXNAME eggman_inner_lamp_texlist_names[1] = {
#include "stg35/lamp_art/eggman_inner_lamp_texlist_names.inc"
};

NJS_TEXLIST eggman_inner_lamp_texlist = {
#include "stg35/lamp_art/eggman_inner_lamp_texlist.inc"
};

static Sint16 inner_lamp_b_1_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_b_1_polygons.inc"
};

static Sint32 inner_lamp_b_1_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_b_1_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_b_1_model = {
#include "stg35/lamp_art/inner_lamp_b_1_model.inc"
};

NJS_CNK_OBJECT inner_lamp_b_1 = {
#include "stg35/lamp_art/inner_lamp_b_1.inc"
};

static Sint16 inner_lamp_b_3_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_b_3_polygons.inc"
};

static Sint32 inner_lamp_b_3_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_b_3_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_b_3_model = {
#include "stg35/lamp_art/inner_lamp_b_3_model.inc"
};

NJS_CNK_OBJECT inner_lamp_b_3 = {
#include "stg35/lamp_art/inner_lamp_b_3.inc"
};

static Sint16 inner_lamp_b_4_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_b_4_polygons.inc"
};

static Sint32 inner_lamp_b_4_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_b_4_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_b_4_model = {
#include "stg35/lamp_art/inner_lamp_b_4_model.inc"
};

NJS_CNK_OBJECT inner_lamp_b_4 = {
#include "stg35/lamp_art/inner_lamp_b_4.inc"
};

static Sint16 inner_lamp_b_5_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_b_5_polygons.inc"
};

static Sint32 inner_lamp_b_5_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_b_5_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_b_5_model = {
#include "stg35/lamp_art/inner_lamp_b_5_model.inc"
};

NJS_CNK_OBJECT inner_lamp_b_5 = {
#include "stg35/lamp_art/inner_lamp_b_5.inc"
};

static Sint16 inner_lamp_b_6_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_b_6_polygons.inc"
};

static Sint32 inner_lamp_b_6_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_b_6_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_b_6_model = {
#include "stg35/lamp_art/inner_lamp_b_6_model.inc"
};

NJS_CNK_OBJECT inner_lamp_b_6 = {
#include "stg35/lamp_art/inner_lamp_b_6.inc"
};

static Sint16 inner_lamp_b_2_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_b_2_polygons.inc"
};

static Sint32 inner_lamp_b_2_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_b_2_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_b_2_model = {
#include "stg35/lamp_art/inner_lamp_b_2_model.inc"
};

NJS_CNK_OBJECT inner_lamp_b_2 = {
#include "stg35/lamp_art/inner_lamp_b_2.inc"
};

static Sint16 inner_lamp_a_6_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_a_6_polygons.inc"
};

static Sint32 inner_lamp_a_6_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_a_6_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_a_6_model = {
#include "stg35/lamp_art/inner_lamp_a_6_model.inc"
};

NJS_CNK_OBJECT inner_lamp_a_6 = {
#include "stg35/lamp_art/inner_lamp_a_6.inc"
};

static Sint16 inner_lamp_a_5_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_a_5_polygons.inc"
};

static Sint32 inner_lamp_a_5_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_a_5_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_a_5_model = {
#include "stg35/lamp_art/inner_lamp_a_5_model.inc"
};

NJS_CNK_OBJECT inner_lamp_a_5 = {
#include "stg35/lamp_art/inner_lamp_a_5.inc"
};

static Sint16 inner_lamp_a_4_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_a_4_polygons.inc"
};

static Sint32 inner_lamp_a_4_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_a_4_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_a_4_model = {
#include "stg35/lamp_art/inner_lamp_a_4_model.inc"
};

NJS_CNK_OBJECT inner_lamp_a_4 = {
#include "stg35/lamp_art/inner_lamp_a_4.inc"
};

static Sint16 inner_lamp_a_3_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_a_3_polygons.inc"
};

static Sint32 inner_lamp_a_3_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_a_3_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_a_3_model = {
#include "stg35/lamp_art/inner_lamp_a_3_model.inc"
};

NJS_CNK_OBJECT inner_lamp_a_3 = {
#include "stg35/lamp_art/inner_lamp_a_3.inc"
};

static Sint16 inner_lamp_a_2_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_a_2_polygons.inc"
};

static Sint32 inner_lamp_a_2_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_a_2_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_a_2_model = {
#include "stg35/lamp_art/inner_lamp_a_2_model.inc"
};

NJS_CNK_OBJECT inner_lamp_a_2 = {
#include "stg35/lamp_art/inner_lamp_a_2.inc"
};

static Sint16 inner_lamp_a_1_polygons[25] = {
#include "stg35/lamp_art/inner_lamp_a_1_polygons.inc"
};

static Sint32 inner_lamp_a_1_vertices[19] = {
#include "stg35/lamp_art/inner_lamp_a_1_vertices.inc"
};

static NJS_CNK_MODEL inner_lamp_a_1_model = {
#include "stg35/lamp_art/inner_lamp_a_1_model.inc"
};

NJS_CNK_OBJECT inner_lamp_a_1 = {
#include "stg35/lamp_art/inner_lamp_a_1.inc"
};

static NJS_CNK_OBJECT inner_root = {
#include "stg35/lamp_art/inner_root.inc"
};

const char eggman_outer_lamp_texture_name[9] = {
#include "stg35/lamp_art/eggman_outer_lamp_texture_name.inc"
};

static NJS_TEXNAME eggman_outer_lamp_texlist_names[1] = {
#include "stg35/lamp_art/eggman_outer_lamp_texlist_names.inc"
};

NJS_TEXLIST eggman_outer_lamp_texlist = {
#include "stg35/lamp_art/eggman_outer_lamp_texlist.inc"
};

static Sint16 outer_lamp_c_1_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_1_polygons.inc"
};

static Sint32 outer_lamp_c_1_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_1_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_1_model = {
#include "stg35/lamp_art/outer_lamp_c_1_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_1 = {
#include "stg35/lamp_art/outer_lamp_c_1.inc"
};

static Sint16 outer_lamp_c_3_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_3_polygons.inc"
};

static Sint32 outer_lamp_c_3_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_3_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_3_model = {
#include "stg35/lamp_art/outer_lamp_c_3_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_3 = {
#include "stg35/lamp_art/outer_lamp_c_3.inc"
};

static Sint16 outer_lamp_c_4_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_4_polygons.inc"
};

static Sint32 outer_lamp_c_4_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_4_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_4_model = {
#include "stg35/lamp_art/outer_lamp_c_4_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_4 = {
#include "stg35/lamp_art/outer_lamp_c_4.inc"
};

static Sint16 outer_lamp_c_5_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_5_polygons.inc"
};

static Sint32 outer_lamp_c_5_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_5_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_5_model = {
#include "stg35/lamp_art/outer_lamp_c_5_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_5 = {
#include "stg35/lamp_art/outer_lamp_c_5.inc"
};

static Sint16 outer_lamp_c_6_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_6_polygons.inc"
};

static Sint32 outer_lamp_c_6_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_6_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_6_model = {
#include "stg35/lamp_art/outer_lamp_c_6_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_6 = {
#include "stg35/lamp_art/outer_lamp_c_6.inc"
};

static Sint16 outer_lamp_c_7_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_7_polygons.inc"
};

static Sint32 outer_lamp_c_7_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_7_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_7_model = {
#include "stg35/lamp_art/outer_lamp_c_7_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_7 = {
#include "stg35/lamp_art/outer_lamp_c_7.inc"
};

static Sint16 outer_lamp_c_8_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_8_polygons.inc"
};

static Sint32 outer_lamp_c_8_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_8_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_8_model = {
#include "stg35/lamp_art/outer_lamp_c_8_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_8 = {
#include "stg35/lamp_art/outer_lamp_c_8.inc"
};

static Sint16 outer_lamp_c_9_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_9_polygons.inc"
};

static Sint32 outer_lamp_c_9_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_9_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_9_model = {
#include "stg35/lamp_art/outer_lamp_c_9_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_9 = {
#include "stg35/lamp_art/outer_lamp_c_9.inc"
};

static Sint16 outer_lamp_c_10_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_10_polygons.inc"
};

static Sint32 outer_lamp_c_10_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_10_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_10_model = {
#include "stg35/lamp_art/outer_lamp_c_10_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_10 = {
#include "stg35/lamp_art/outer_lamp_c_10.inc"
};

static Sint16 outer_lamp_c_11_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_11_polygons.inc"
};

static Sint32 outer_lamp_c_11_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_11_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_11_model = {
#include "stg35/lamp_art/outer_lamp_c_11_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_11 = {
#include "stg35/lamp_art/outer_lamp_c_11.inc"
};

static Sint16 outer_lamp_b_2_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_2_polygons.inc"
};

static Sint32 outer_lamp_b_2_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_2_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_2_model = {
#include "stg35/lamp_art/outer_lamp_b_2_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_2 = {
#include "stg35/lamp_art/outer_lamp_b_2.inc"
};

static Sint16 outer_lamp_a_11_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_11_polygons.inc"
};

static Sint32 outer_lamp_a_11_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_11_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_11_model = {
#include "stg35/lamp_art/outer_lamp_a_11_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_11 = {
#include "stg35/lamp_art/outer_lamp_a_11.inc"
};

static Sint16 outer_lamp_a_1_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_1_polygons.inc"
};

static Sint32 outer_lamp_a_1_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_1_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_1_model = {
#include "stg35/lamp_art/outer_lamp_a_1_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_1 = {
#include "stg35/lamp_art/outer_lamp_a_1.inc"
};

static Sint16 outer_lamp_d_4_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_4_polygons.inc"
};

static Sint32 outer_lamp_d_4_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_4_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_4_model = {
#include "stg35/lamp_art/outer_lamp_d_4_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_4 = {
#include "stg35/lamp_art/outer_lamp_d_4.inc"
};

static Sint16 outer_lamp_d_6_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_6_polygons.inc"
};

static Sint32 outer_lamp_d_6_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_6_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_6_model = {
#include "stg35/lamp_art/outer_lamp_d_6_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_6 = {
#include "stg35/lamp_art/outer_lamp_d_6.inc"
};

static Sint16 outer_lamp_b_9_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_9_polygons.inc"
};

static Sint32 outer_lamp_b_9_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_9_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_9_model = {
#include "stg35/lamp_art/outer_lamp_b_9_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_9 = {
#include "stg35/lamp_art/outer_lamp_b_9.inc"
};

static Sint16 outer_lamp_d_10_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_10_polygons.inc"
};

static Sint32 outer_lamp_d_10_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_10_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_10_model = {
#include "stg35/lamp_art/outer_lamp_d_10_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_10 = {
#include "stg35/lamp_art/outer_lamp_d_10.inc"
};

static Sint16 outer_lamp_a_2_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_2_polygons.inc"
};

static Sint32 outer_lamp_a_2_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_2_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_2_model = {
#include "stg35/lamp_art/outer_lamp_a_2_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_2 = {
#include "stg35/lamp_art/outer_lamp_a_2.inc"
};

static Sint16 outer_lamp_b_8_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_8_polygons.inc"
};

static Sint32 outer_lamp_b_8_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_8_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_8_model = {
#include "stg35/lamp_art/outer_lamp_b_8_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_8 = {
#include "stg35/lamp_art/outer_lamp_b_8.inc"
};

static Sint16 outer_lamp_d_7_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_7_polygons.inc"
};

static Sint32 outer_lamp_d_7_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_7_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_7_model = {
#include "stg35/lamp_art/outer_lamp_d_7_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_7 = {
#include "stg35/lamp_art/outer_lamp_d_7.inc"
};

static Sint16 outer_lamp_a_12_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_12_polygons.inc"
};

static Sint32 outer_lamp_a_12_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_12_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_12_model = {
#include "stg35/lamp_art/outer_lamp_a_12_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_12 = {
#include "stg35/lamp_art/outer_lamp_a_12.inc"
};

static Sint16 outer_lamp_b_7_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_7_polygons.inc"
};

static Sint32 outer_lamp_b_7_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_7_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_7_model = {
#include "stg35/lamp_art/outer_lamp_b_7_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_7 = {
#include "stg35/lamp_art/outer_lamp_b_7.inc"
};

static Sint16 outer_lamp_a_13_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_13_polygons.inc"
};

static Sint32 outer_lamp_a_13_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_13_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_13_model = {
#include "stg35/lamp_art/outer_lamp_a_13_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_13 = {
#include "stg35/lamp_art/outer_lamp_a_13.inc"
};

static Sint16 outer_lamp_a_3_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_3_polygons.inc"
};

static Sint32 outer_lamp_a_3_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_3_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_3_model = {
#include "stg35/lamp_art/outer_lamp_a_3_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_3 = {
#include "stg35/lamp_art/outer_lamp_a_3.inc"
};

static Sint16 outer_lamp_d_11_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_11_polygons.inc"
};

static Sint32 outer_lamp_d_11_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_11_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_11_model = {
#include "stg35/lamp_art/outer_lamp_d_11_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_11 = {
#include "stg35/lamp_art/outer_lamp_d_11.inc"
};

static Sint16 outer_lamp_d_15_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_15_polygons.inc"
};

static Sint32 outer_lamp_d_15_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_15_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_15_model = {
#include "stg35/lamp_art/outer_lamp_d_15_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_15 = {
#include "stg35/lamp_art/outer_lamp_d_15.inc"
};

static Sint16 outer_lamp_b_12_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_12_polygons.inc"
};

static Sint32 outer_lamp_b_12_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_12_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_12_model = {
#include "stg35/lamp_art/outer_lamp_b_12_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_12 = {
#include "stg35/lamp_art/outer_lamp_b_12.inc"
};

static Sint16 outer_lamp_a_4_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_4_polygons.inc"
};

static Sint32 outer_lamp_a_4_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_4_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_4_model = {
#include "stg35/lamp_art/outer_lamp_a_4_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_4 = {
#include "stg35/lamp_art/outer_lamp_a_4.inc"
};

static Sint16 outer_lamp_b_6_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_6_polygons.inc"
};

static Sint32 outer_lamp_b_6_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_6_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_6_model = {
#include "stg35/lamp_art/outer_lamp_b_6_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_6 = {
#include "stg35/lamp_art/outer_lamp_b_6.inc"
};

static Sint16 outer_lamp_a_14_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_14_polygons.inc"
};

static Sint32 outer_lamp_a_14_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_14_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_14_model = {
#include "stg35/lamp_art/outer_lamp_a_14_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_14 = {
#include "stg35/lamp_art/outer_lamp_a_14.inc"
};

static Sint16 outer_lamp_d_14_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_14_polygons.inc"
};

static Sint32 outer_lamp_d_14_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_14_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_14_model = {
#include "stg35/lamp_art/outer_lamp_d_14_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_14 = {
#include "stg35/lamp_art/outer_lamp_d_14.inc"
};

static Sint16 outer_lamp_b_1_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_1_polygons.inc"
};

static Sint32 outer_lamp_b_1_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_1_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_1_model = {
#include "stg35/lamp_art/outer_lamp_b_1_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_1 = {
#include "stg35/lamp_art/outer_lamp_b_1.inc"
};

static Sint16 outer_lamp_d_13_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_13_polygons.inc"
};

static Sint32 outer_lamp_d_13_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_13_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_13_model = {
#include "stg35/lamp_art/outer_lamp_d_13_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_13 = {
#include "stg35/lamp_art/outer_lamp_d_13.inc"
};

static Sint16 outer_lamp_a_15_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_15_polygons.inc"
};

static Sint32 outer_lamp_a_15_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_15_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_15_model = {
#include "stg35/lamp_art/outer_lamp_a_15_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_15 = {
#include "stg35/lamp_art/outer_lamp_a_15.inc"
};

static Sint16 outer_lamp_a_5_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_5_polygons.inc"
};

static Sint32 outer_lamp_a_5_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_5_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_5_model = {
#include "stg35/lamp_art/outer_lamp_a_5_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_5 = {
#include "stg35/lamp_art/outer_lamp_a_5.inc"
};

static Sint16 outer_lamp_b_11_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_11_polygons.inc"
};

static Sint32 outer_lamp_b_11_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_11_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_11_model = {
#include "stg35/lamp_art/outer_lamp_b_11_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_11 = {
#include "stg35/lamp_art/outer_lamp_b_11.inc"
};

static Sint16 outer_lamp_b_10_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_10_polygons.inc"
};

static Sint32 outer_lamp_b_10_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_10_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_10_model = {
#include "stg35/lamp_art/outer_lamp_b_10_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_10 = {
#include "stg35/lamp_art/outer_lamp_b_10.inc"
};

static Sint16 outer_lamp_d_9_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_9_polygons.inc"
};

static Sint32 outer_lamp_d_9_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_9_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_9_model = {
#include "stg35/lamp_art/outer_lamp_d_9_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_9 = {
#include "stg35/lamp_art/outer_lamp_d_9.inc"
};

static Sint16 outer_lamp_d_5_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_5_polygons.inc"
};

static Sint32 outer_lamp_d_5_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_5_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_5_model = {
#include "stg35/lamp_art/outer_lamp_d_5_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_5 = {
#include "stg35/lamp_art/outer_lamp_d_5.inc"
};

static Sint16 outer_lamp_d_12_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_12_polygons.inc"
};

static Sint32 outer_lamp_d_12_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_12_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_12_model = {
#include "stg35/lamp_art/outer_lamp_d_12_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_12 = {
#include "stg35/lamp_art/outer_lamp_d_12.inc"
};

static Sint16 outer_lamp_c_2_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_c_2_polygons.inc"
};

static Sint32 outer_lamp_c_2_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_c_2_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_c_2_model = {
#include "stg35/lamp_art/outer_lamp_c_2_model.inc"
};

NJS_CNK_OBJECT outer_lamp_c_2 = {
#include "stg35/lamp_art/outer_lamp_c_2.inc"
};

static Sint16 outer_lamp_d_1_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_1_polygons.inc"
};

static Sint32 outer_lamp_d_1_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_1_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_1_model = {
#include "stg35/lamp_art/outer_lamp_d_1_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_1 = {
#include "stg35/lamp_art/outer_lamp_d_1.inc"
};

static Sint16 outer_lamp_d_8_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_8_polygons.inc"
};

static Sint32 outer_lamp_d_8_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_8_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_8_model = {
#include "stg35/lamp_art/outer_lamp_d_8_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_8 = {
#include "stg35/lamp_art/outer_lamp_d_8.inc"
};

static Sint16 outer_lamp_a_7_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_7_polygons.inc"
};

static Sint32 outer_lamp_a_7_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_7_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_7_model = {
#include "stg35/lamp_art/outer_lamp_a_7_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_7 = {
#include "stg35/lamp_art/outer_lamp_a_7.inc"
};

static Sint16 outer_lamp_d_2_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_2_polygons.inc"
};

static Sint32 outer_lamp_d_2_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_2_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_2_model = {
#include "stg35/lamp_art/outer_lamp_d_2_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_2 = {
#include "stg35/lamp_art/outer_lamp_d_2.inc"
};

static Sint16 outer_lamp_d_3_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_d_3_polygons.inc"
};

static Sint32 outer_lamp_d_3_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_d_3_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_d_3_model = {
#include "stg35/lamp_art/outer_lamp_d_3_model.inc"
};

NJS_CNK_OBJECT outer_lamp_d_3 = {
#include "stg35/lamp_art/outer_lamp_d_3.inc"
};

static Sint16 outer_lamp_a_6_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_6_polygons.inc"
};

static Sint32 outer_lamp_a_6_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_6_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_6_model = {
#include "stg35/lamp_art/outer_lamp_a_6_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_6 = {
#include "stg35/lamp_art/outer_lamp_a_6.inc"
};

static Sint16 outer_lamp_a_10_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_10_polygons.inc"
};

static Sint32 outer_lamp_a_10_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_10_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_10_model = {
#include "stg35/lamp_art/outer_lamp_a_10_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_10 = {
#include "stg35/lamp_art/outer_lamp_a_10.inc"
};

static Sint16 outer_lamp_a_8_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_8_polygons.inc"
};

static Sint32 outer_lamp_a_8_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_8_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_8_model = {
#include "stg35/lamp_art/outer_lamp_a_8_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_8 = {
#include "stg35/lamp_art/outer_lamp_a_8.inc"
};

static Sint16 outer_lamp_a_9_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_a_9_polygons.inc"
};

static Sint32 outer_lamp_a_9_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_a_9_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_a_9_model = {
#include "stg35/lamp_art/outer_lamp_a_9_model.inc"
};

NJS_CNK_OBJECT outer_lamp_a_9 = {
#include "stg35/lamp_art/outer_lamp_a_9.inc"
};

static Sint16 outer_lamp_b_3_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_3_polygons.inc"
};

static Sint32 outer_lamp_b_3_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_3_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_3_model = {
#include "stg35/lamp_art/outer_lamp_b_3_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_3 = {
#include "stg35/lamp_art/outer_lamp_b_3.inc"
};

static Sint16 outer_lamp_b_5_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_5_polygons.inc"
};

static Sint32 outer_lamp_b_5_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_5_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_5_model = {
#include "stg35/lamp_art/outer_lamp_b_5_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_5 = {
#include "stg35/lamp_art/outer_lamp_b_5.inc"
};

static Sint16 outer_lamp_b_4_polygons[25] = {
#include "stg35/lamp_art/outer_lamp_b_4_polygons.inc"
};

static Sint32 outer_lamp_b_4_vertices[19] = {
#include "stg35/lamp_art/outer_lamp_b_4_vertices.inc"
};

static NJS_CNK_MODEL outer_lamp_b_4_model = {
#include "stg35/lamp_art/outer_lamp_b_4_model.inc"
};

NJS_CNK_OBJECT outer_lamp_b_4 = {
#include "stg35/lamp_art/outer_lamp_b_4.inc"
};

static NJS_CNK_OBJECT outer_root = {
#include "stg35/lamp_art/outer_root.inc"
};
