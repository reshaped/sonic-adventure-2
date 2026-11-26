/* the strip lamp's texture list and model, and the four strips' lamps. each of
 * those lamps is an object whose model is a triangle. the draw puts the strip
 * lamp's model at the object's pos and ang. the lamps of the entrance's own
 * strips follow, drawn the same way */

#include "shared/core_lamp_art.h"
#include "stg36/route.h"
#include "stg36/entrance_lamps.h"

#include <ninja.h>
#include <sg_xpt.h>

/* The next stage filename begins on a four-byte boundary. */
const char core_strip_texture_name[16] = {
#include "stg36/lamp_art/core_strip_texture_name.inc"
};

static NJS_TEXNAME strip_names[1] = {
#include "stg36/lamp_art/strip_names.inc"
};

NJS_TEXLIST tails_strip_texlist = {
#include "stg36/lamp_art/tails_strip_texlist.inc"
};

static Sint16 strip_lamp_polygons[25] = {
#include "stg36/lamp_art/strip_lamp_polygons.inc"
};

static Sint32 strip_lamp_vertices[19] = {
#include "stg36/lamp_art/strip_lamp_vertices.inc"
};

NJS_CNK_MODEL core_strip_lamp_model = {
#include "stg36/lamp_art/core_strip_lamp_model.inc"
};

static NJS_CNK_OBJECT core_strip_lamp_object = {
#include "stg36/lamp_art/core_strip_lamp_object.inc"
};

static Sint16 core_lamp_d_4_polygons[15] = {
#include "stg36/lamp_art/core_lamp_d_4_polygons.inc"
};

static Sint32 core_lamp_d_4_vertices[15] = {
#include "stg36/lamp_art/core_lamp_d_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_4_model = {
#include "stg36/lamp_art/core_lamp_d_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_4 = {
#include "stg36/lamp_art/core_lamp_d_4.inc"
};

static Sint16 core_lamp_d_5_polygons[15] = {
#include "stg36/lamp_art/core_lamp_d_5_polygons.inc"
};

static Sint32 core_lamp_d_5_vertices[15] = {
#include "stg36/lamp_art/core_lamp_d_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_5_model = {
#include "stg36/lamp_art/core_lamp_d_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_5 = {
#include "stg36/lamp_art/core_lamp_d_5.inc"
};

static Sint16 core_lamp_d_6_polygons[15] = {
#include "stg36/lamp_art/core_lamp_d_6_polygons.inc"
};

static Sint32 core_lamp_d_6_vertices[15] = {
#include "stg36/lamp_art/core_lamp_d_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_6_model = {
#include "stg36/lamp_art/core_lamp_d_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_6 = {
#include "stg36/lamp_art/core_lamp_d_6.inc"
};

static Sint16 core_lamp_d_3_polygons[15] = {
#include "stg36/lamp_art/core_lamp_d_3_polygons.inc"
};

static Sint32 core_lamp_d_3_vertices[15] = {
#include "stg36/lamp_art/core_lamp_d_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_3_model = {
#include "stg36/lamp_art/core_lamp_d_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_3 = {
#include "stg36/lamp_art/core_lamp_d_3.inc"
};

static Sint16 core_lamp_d_2_polygons[15] = {
#include "stg36/lamp_art/core_lamp_d_2_polygons.inc"
};

static Sint32 core_lamp_d_2_vertices[15] = {
#include "stg36/lamp_art/core_lamp_d_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_2_model = {
#include "stg36/lamp_art/core_lamp_d_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_2 = {
#include "stg36/lamp_art/core_lamp_d_2.inc"
};

static Sint16 core_lamp_d_1_polygons[15] = {
#include "stg36/lamp_art/core_lamp_d_1_polygons.inc"
};

static Sint32 core_lamp_d_1_vertices[15] = {
#include "stg36/lamp_art/core_lamp_d_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_d_1_model = {
#include "stg36/lamp_art/core_lamp_d_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_d_1 = {
#include "stg36/lamp_art/core_lamp_d_1.inc"
};

static Sint16 core_lamp_b_6_polygons[15] = {
#include "stg36/lamp_art/core_lamp_b_6_polygons.inc"
};

static Sint32 core_lamp_b_6_vertices[15] = {
#include "stg36/lamp_art/core_lamp_b_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_6_model = {
#include "stg36/lamp_art/core_lamp_b_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_6 = {
#include "stg36/lamp_art/core_lamp_b_6.inc"
};

static Sint16 core_lamp_b_5_polygons[15] = {
#include "stg36/lamp_art/core_lamp_b_5_polygons.inc"
};

static Sint32 core_lamp_b_5_vertices[15] = {
#include "stg36/lamp_art/core_lamp_b_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_5_model = {
#include "stg36/lamp_art/core_lamp_b_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_5 = {
#include "stg36/lamp_art/core_lamp_b_5.inc"
};

static Sint16 core_lamp_b_4_polygons[15] = {
#include "stg36/lamp_art/core_lamp_b_4_polygons.inc"
};

static Sint32 core_lamp_b_4_vertices[15] = {
#include "stg36/lamp_art/core_lamp_b_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_4_model = {
#include "stg36/lamp_art/core_lamp_b_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_4 = {
#include "stg36/lamp_art/core_lamp_b_4.inc"
};

static Sint16 core_lamp_b_3_polygons[15] = {
#include "stg36/lamp_art/core_lamp_b_3_polygons.inc"
};

static Sint32 core_lamp_b_3_vertices[15] = {
#include "stg36/lamp_art/core_lamp_b_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_3_model = {
#include "stg36/lamp_art/core_lamp_b_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_3 = {
#include "stg36/lamp_art/core_lamp_b_3.inc"
};

static Sint16 core_lamp_b_2_polygons[15] = {
#include "stg36/lamp_art/core_lamp_b_2_polygons.inc"
};

static Sint32 core_lamp_b_2_vertices[15] = {
#include "stg36/lamp_art/core_lamp_b_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_2_model = {
#include "stg36/lamp_art/core_lamp_b_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_2 = {
#include "stg36/lamp_art/core_lamp_b_2.inc"
};

static Sint16 core_lamp_b_1_polygons[15] = {
#include "stg36/lamp_art/core_lamp_b_1_polygons.inc"
};

static Sint32 core_lamp_b_1_vertices[15] = {
#include "stg36/lamp_art/core_lamp_b_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_b_1_model = {
#include "stg36/lamp_art/core_lamp_b_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_b_1 = {
#include "stg36/lamp_art/core_lamp_b_1.inc"
};

static Sint16 core_lamp_c_6_polygons[15] = {
#include "stg36/lamp_art/core_lamp_c_6_polygons.inc"
};

static Sint32 core_lamp_c_6_vertices[15] = {
#include "stg36/lamp_art/core_lamp_c_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_6_model = {
#include "stg36/lamp_art/core_lamp_c_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_6 = {
#include "stg36/lamp_art/core_lamp_c_6.inc"
};

static Sint16 core_lamp_c_5_polygons[15] = {
#include "stg36/lamp_art/core_lamp_c_5_polygons.inc"
};

static Sint32 core_lamp_c_5_vertices[15] = {
#include "stg36/lamp_art/core_lamp_c_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_5_model = {
#include "stg36/lamp_art/core_lamp_c_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_5 = {
#include "stg36/lamp_art/core_lamp_c_5.inc"
};

static Sint16 core_lamp_c_4_polygons[15] = {
#include "stg36/lamp_art/core_lamp_c_4_polygons.inc"
};

static Sint32 core_lamp_c_4_vertices[15] = {
#include "stg36/lamp_art/core_lamp_c_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_4_model = {
#include "stg36/lamp_art/core_lamp_c_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_4 = {
#include "stg36/lamp_art/core_lamp_c_4.inc"
};

static Sint16 core_lamp_c_3_polygons[15] = {
#include "stg36/lamp_art/core_lamp_c_3_polygons.inc"
};

static Sint32 core_lamp_c_3_vertices[15] = {
#include "stg36/lamp_art/core_lamp_c_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_3_model = {
#include "stg36/lamp_art/core_lamp_c_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_3 = {
#include "stg36/lamp_art/core_lamp_c_3.inc"
};

static Sint16 core_lamp_c_2_polygons[15] = {
#include "stg36/lamp_art/core_lamp_c_2_polygons.inc"
};

static Sint32 core_lamp_c_2_vertices[15] = {
#include "stg36/lamp_art/core_lamp_c_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_2_model = {
#include "stg36/lamp_art/core_lamp_c_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_2 = {
#include "stg36/lamp_art/core_lamp_c_2.inc"
};

static Sint16 core_lamp_c_1_polygons[15] = {
#include "stg36/lamp_art/core_lamp_c_1_polygons.inc"
};

static Sint32 core_lamp_c_1_vertices[15] = {
#include "stg36/lamp_art/core_lamp_c_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_c_1_model = {
#include "stg36/lamp_art/core_lamp_c_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_c_1 = {
#include "stg36/lamp_art/core_lamp_c_1.inc"
};

static Sint16 core_lamp_a_1_polygons[15] = {
#include "stg36/lamp_art/core_lamp_a_1_polygons.inc"
};

static Sint32 core_lamp_a_1_vertices[15] = {
#include "stg36/lamp_art/core_lamp_a_1_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_1_model = {
#include "stg36/lamp_art/core_lamp_a_1_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_1 = {
#include "stg36/lamp_art/core_lamp_a_1.inc"
};

static Sint16 core_lamp_a_2_polygons[15] = {
#include "stg36/lamp_art/core_lamp_a_2_polygons.inc"
};

static Sint32 core_lamp_a_2_vertices[15] = {
#include "stg36/lamp_art/core_lamp_a_2_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_2_model = {
#include "stg36/lamp_art/core_lamp_a_2_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_2 = {
#include "stg36/lamp_art/core_lamp_a_2.inc"
};

static Sint16 core_lamp_a_3_polygons[15] = {
#include "stg36/lamp_art/core_lamp_a_3_polygons.inc"
};

static Sint32 core_lamp_a_3_vertices[15] = {
#include "stg36/lamp_art/core_lamp_a_3_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_3_model = {
#include "stg36/lamp_art/core_lamp_a_3_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_3 = {
#include "stg36/lamp_art/core_lamp_a_3.inc"
};

static Sint16 core_lamp_a_4_polygons[15] = {
#include "stg36/lamp_art/core_lamp_a_4_polygons.inc"
};

static Sint32 core_lamp_a_4_vertices[15] = {
#include "stg36/lamp_art/core_lamp_a_4_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_4_model = {
#include "stg36/lamp_art/core_lamp_a_4_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_4 = {
#include "stg36/lamp_art/core_lamp_a_4.inc"
};

static Sint16 core_lamp_a_5_polygons[15] = {
#include "stg36/lamp_art/core_lamp_a_5_polygons.inc"
};

static Sint32 core_lamp_a_5_vertices[15] = {
#include "stg36/lamp_art/core_lamp_a_5_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_5_model = {
#include "stg36/lamp_art/core_lamp_a_5_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_5 = {
#include "stg36/lamp_art/core_lamp_a_5.inc"
};

static Sint16 core_lamp_a_6_polygons[15] = {
#include "stg36/lamp_art/core_lamp_a_6_polygons.inc"
};

static Sint32 core_lamp_a_6_vertices[15] = {
#include "stg36/lamp_art/core_lamp_a_6_vertices.inc"
};

static NJS_CNK_MODEL core_lamp_a_6_model = {
#include "stg36/lamp_art/core_lamp_a_6_model.inc"
};

NJS_CNK_OBJECT core_lamp_a_6 = {
#include "stg36/lamp_art/core_lamp_a_6.inc"
};

static NJS_CNK_OBJECT strip_root = {
#include "stg36/lamp_art/strip_root.inc"
};

static NJS_TEXNAME tails_entrance_strip_texlist_names[1] = {
#include "stg36/lamp_art/tails_entrance_strip_texlist_names.inc"
};

NJS_TEXLIST tails_entrance_strip_texlist = {
#include "stg36/lamp_art/tails_entrance_strip_texlist.inc"
};

static Sint16 entrance_strip_lamp_polygons[25] = {
#include "stg36/lamp_art/entrance_strip_lamp_polygons.inc"
};

static Sint32 entrance_strip_lamp_vertices[19] = {
#include "stg36/lamp_art/entrance_strip_lamp_vertices.inc"
};

static NJS_CNK_MODEL entrance_strip_lamp_model = {
#include "stg36/lamp_art/entrance_strip_lamp_model.inc"
};

static NJS_CNK_OBJECT entrance_lamp_object = {
#include "stg36/lamp_art/entrance_lamp_object.inc"
};

static Sint16 tails_entrance_d_4_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_d_4_polygons.inc"
};

static Sint32 tails_entrance_d_4_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_d_4_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_d_4_model = {
#include "stg36/lamp_art/tails_entrance_d_4_model.inc"
};

NJS_CNK_OBJECT tails_entrance_d_4 = {
#include "stg36/lamp_art/tails_entrance_d_4.inc"
};

static Sint16 tails_entrance_d_5_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_d_5_polygons.inc"
};

static Sint32 tails_entrance_d_5_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_d_5_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_d_5_model = {
#include "stg36/lamp_art/tails_entrance_d_5_model.inc"
};

NJS_CNK_OBJECT tails_entrance_d_5 = {
#include "stg36/lamp_art/tails_entrance_d_5.inc"
};

static Sint16 tails_entrance_d_6_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_d_6_polygons.inc"
};

static Sint32 tails_entrance_d_6_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_d_6_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_d_6_model = {
#include "stg36/lamp_art/tails_entrance_d_6_model.inc"
};

NJS_CNK_OBJECT tails_entrance_d_6 = {
#include "stg36/lamp_art/tails_entrance_d_6.inc"
};

static Sint16 tails_entrance_d_3_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_d_3_polygons.inc"
};

static Sint32 tails_entrance_d_3_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_d_3_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_d_3_model = {
#include "stg36/lamp_art/tails_entrance_d_3_model.inc"
};

NJS_CNK_OBJECT tails_entrance_d_3 = {
#include "stg36/lamp_art/tails_entrance_d_3.inc"
};

static Sint16 tails_entrance_d_2_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_d_2_polygons.inc"
};

static Sint32 tails_entrance_d_2_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_d_2_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_d_2_model = {
#include "stg36/lamp_art/tails_entrance_d_2_model.inc"
};

NJS_CNK_OBJECT tails_entrance_d_2 = {
#include "stg36/lamp_art/tails_entrance_d_2.inc"
};

static Sint16 tails_entrance_d_1_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_d_1_polygons.inc"
};

static Sint32 tails_entrance_d_1_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_d_1_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_d_1_model = {
#include "stg36/lamp_art/tails_entrance_d_1_model.inc"
};

NJS_CNK_OBJECT tails_entrance_d_1 = {
#include "stg36/lamp_art/tails_entrance_d_1.inc"
};

static Sint16 tails_entrance_b_6_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_b_6_polygons.inc"
};

static Sint32 tails_entrance_b_6_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_b_6_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_b_6_model = {
#include "stg36/lamp_art/tails_entrance_b_6_model.inc"
};

NJS_CNK_OBJECT tails_entrance_b_6 = {
#include "stg36/lamp_art/tails_entrance_b_6.inc"
};

static Sint16 tails_entrance_b_5_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_b_5_polygons.inc"
};

static Sint32 tails_entrance_b_5_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_b_5_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_b_5_model = {
#include "stg36/lamp_art/tails_entrance_b_5_model.inc"
};

NJS_CNK_OBJECT tails_entrance_b_5 = {
#include "stg36/lamp_art/tails_entrance_b_5.inc"
};

static Sint16 tails_entrance_b_4_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_b_4_polygons.inc"
};

static Sint32 tails_entrance_b_4_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_b_4_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_b_4_model = {
#include "stg36/lamp_art/tails_entrance_b_4_model.inc"
};

NJS_CNK_OBJECT tails_entrance_b_4 = {
#include "stg36/lamp_art/tails_entrance_b_4.inc"
};

static Sint16 tails_entrance_b_3_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_b_3_polygons.inc"
};

static Sint32 tails_entrance_b_3_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_b_3_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_b_3_model = {
#include "stg36/lamp_art/tails_entrance_b_3_model.inc"
};

NJS_CNK_OBJECT tails_entrance_b_3 = {
#include "stg36/lamp_art/tails_entrance_b_3.inc"
};

static Sint16 tails_entrance_b_2_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_b_2_polygons.inc"
};

static Sint32 tails_entrance_b_2_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_b_2_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_b_2_model = {
#include "stg36/lamp_art/tails_entrance_b_2_model.inc"
};

NJS_CNK_OBJECT tails_entrance_b_2 = {
#include "stg36/lamp_art/tails_entrance_b_2.inc"
};

static Sint16 tails_entrance_b_1_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_b_1_polygons.inc"
};

static Sint32 tails_entrance_b_1_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_b_1_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_b_1_model = {
#include "stg36/lamp_art/tails_entrance_b_1_model.inc"
};

NJS_CNK_OBJECT tails_entrance_b_1 = {
#include "stg36/lamp_art/tails_entrance_b_1.inc"
};

static Sint16 tails_entrance_c_6_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_c_6_polygons.inc"
};

static Sint32 tails_entrance_c_6_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_c_6_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_c_6_model = {
#include "stg36/lamp_art/tails_entrance_c_6_model.inc"
};

NJS_CNK_OBJECT tails_entrance_c_6 = {
#include "stg36/lamp_art/tails_entrance_c_6.inc"
};

static Sint16 tails_entrance_c_5_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_c_5_polygons.inc"
};

static Sint32 tails_entrance_c_5_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_c_5_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_c_5_model = {
#include "stg36/lamp_art/tails_entrance_c_5_model.inc"
};

NJS_CNK_OBJECT tails_entrance_c_5 = {
#include "stg36/lamp_art/tails_entrance_c_5.inc"
};

static Sint16 tails_entrance_c_4_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_c_4_polygons.inc"
};

static Sint32 tails_entrance_c_4_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_c_4_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_c_4_model = {
#include "stg36/lamp_art/tails_entrance_c_4_model.inc"
};

NJS_CNK_OBJECT tails_entrance_c_4 = {
#include "stg36/lamp_art/tails_entrance_c_4.inc"
};

static Sint16 tails_entrance_c_3_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_c_3_polygons.inc"
};

static Sint32 tails_entrance_c_3_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_c_3_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_c_3_model = {
#include "stg36/lamp_art/tails_entrance_c_3_model.inc"
};

NJS_CNK_OBJECT tails_entrance_c_3 = {
#include "stg36/lamp_art/tails_entrance_c_3.inc"
};

static Sint16 tails_entrance_c_2_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_c_2_polygons.inc"
};

static Sint32 tails_entrance_c_2_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_c_2_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_c_2_model = {
#include "stg36/lamp_art/tails_entrance_c_2_model.inc"
};

NJS_CNK_OBJECT tails_entrance_c_2 = {
#include "stg36/lamp_art/tails_entrance_c_2.inc"
};

static Sint16 tails_entrance_c_1_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_c_1_polygons.inc"
};

static Sint32 tails_entrance_c_1_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_c_1_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_c_1_model = {
#include "stg36/lamp_art/tails_entrance_c_1_model.inc"
};

NJS_CNK_OBJECT tails_entrance_c_1 = {
#include "stg36/lamp_art/tails_entrance_c_1.inc"
};

static Sint16 tails_entrance_a_1_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_a_1_polygons.inc"
};

static Sint32 tails_entrance_a_1_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_a_1_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_a_1_model = {
#include "stg36/lamp_art/tails_entrance_a_1_model.inc"
};

NJS_CNK_OBJECT tails_entrance_a_1 = {
#include "stg36/lamp_art/tails_entrance_a_1.inc"
};

static Sint16 tails_entrance_a_2_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_a_2_polygons.inc"
};

static Sint32 tails_entrance_a_2_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_a_2_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_a_2_model = {
#include "stg36/lamp_art/tails_entrance_a_2_model.inc"
};

NJS_CNK_OBJECT tails_entrance_a_2 = {
#include "stg36/lamp_art/tails_entrance_a_2.inc"
};

static Sint16 tails_entrance_a_3_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_a_3_polygons.inc"
};

static Sint32 tails_entrance_a_3_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_a_3_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_a_3_model = {
#include "stg36/lamp_art/tails_entrance_a_3_model.inc"
};

NJS_CNK_OBJECT tails_entrance_a_3 = {
#include "stg36/lamp_art/tails_entrance_a_3.inc"
};

static Sint16 tails_entrance_a_4_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_a_4_polygons.inc"
};

static Sint32 tails_entrance_a_4_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_a_4_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_a_4_model = {
#include "stg36/lamp_art/tails_entrance_a_4_model.inc"
};

NJS_CNK_OBJECT tails_entrance_a_4 = {
#include "stg36/lamp_art/tails_entrance_a_4.inc"
};

static Sint16 tails_entrance_a_5_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_a_5_polygons.inc"
};

static Sint32 tails_entrance_a_5_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_a_5_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_a_5_model = {
#include "stg36/lamp_art/tails_entrance_a_5_model.inc"
};

NJS_CNK_OBJECT tails_entrance_a_5 = {
#include "stg36/lamp_art/tails_entrance_a_5.inc"
};

static Sint16 tails_entrance_a_6_polygons[15] = {
#include "stg36/lamp_art/tails_entrance_a_6_polygons.inc"
};

static Sint32 tails_entrance_a_6_vertices[15] = {
#include "stg36/lamp_art/tails_entrance_a_6_vertices.inc"
};

static NJS_CNK_MODEL tails_entrance_a_6_model = {
#include "stg36/lamp_art/tails_entrance_a_6_model.inc"
};

NJS_CNK_OBJECT tails_entrance_a_6 = {
#include "stg36/lamp_art/tails_entrance_a_6.inc"
};

static NJS_CNK_OBJECT entrance_root = {
#include "stg36/lamp_art/entrance_root.inc"
};
