#include <ninja.h>
#include <sg_xpt.h>

#include "shared/chimney.h"

static const char material_hasira1[16] = {
#include "shared/ocean_chimney_art/material_hasira1.inc"
};

static const char material_bakuha1[14] = {
#include "shared/ocean_chimney_art/material_bakuha1.inc"
};

static NJS_TEXNAME chimney_textures_0[1] = {
#include "shared/ocean_chimney_art/chimney_textures_0.inc"
};

NJS_TEXLIST chimney_texlist_0 = {
#include "shared/ocean_chimney_art/chimney_texlist_0.inc"
};

static Sint16 chimney_model_0_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_0_polygons.inc"
};

static Sint32 chimney_model_0_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_0_vertices.inc"
};

NJS_CNK_MODEL chimney_model_0 = {
#include "shared/ocean_chimney_art/chimney_model_0.inc"
};

static NJS_CNK_OBJECT chimney_model_0_object = {
#include "shared/ocean_chimney_art/chimney_model_0_object.inc"
};

static NJS_TEXNAME chimney_textures_1[1] = {
#include "shared/ocean_chimney_art/chimney_textures_1.inc"
};

NJS_TEXLIST chimney_texlist_1 = {
#include "shared/ocean_chimney_art/chimney_texlist_1.inc"
};

static Sint16 chimney_model_1_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_1_polygons.inc"
};

static Sint32 chimney_model_1_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_1_vertices.inc"
};

NJS_CNK_MODEL chimney_model_1 = {
#include "shared/ocean_chimney_art/chimney_model_1.inc"
};

static NJS_CNK_OBJECT chimney_model_1_object = {
#include "shared/ocean_chimney_art/chimney_model_1_object.inc"
};

static NJS_TEXNAME chimney_textures_2[1] = {
#include "shared/ocean_chimney_art/chimney_textures_2.inc"
};

NJS_TEXLIST chimney_texlist_2 = {
#include "shared/ocean_chimney_art/chimney_texlist_2.inc"
};

static Sint16 chimney_model_2_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_2_polygons.inc"
};

static Sint32 chimney_model_2_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_2_vertices.inc"
};

NJS_CNK_MODEL chimney_model_2 = {
#include "shared/ocean_chimney_art/chimney_model_2.inc"
};

static NJS_CNK_OBJECT chimney_model_2_object = {
#include "shared/ocean_chimney_art/chimney_model_2_object.inc"
};

static NJS_TEXNAME chimney_textures_3[1] = {
#include "shared/ocean_chimney_art/chimney_textures_3.inc"
};

NJS_TEXLIST chimney_texlist_3 = {
#include "shared/ocean_chimney_art/chimney_texlist_3.inc"
};

static Sint16 chimney_model_3_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_3_polygons.inc"
};

static Sint32 chimney_model_3_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_3_vertices.inc"
};

NJS_CNK_MODEL chimney_model_3 = {
#include "shared/ocean_chimney_art/chimney_model_3.inc"
};

static NJS_CNK_OBJECT chimney_model_3_object = {
#include "shared/ocean_chimney_art/chimney_model_3_object.inc"
};

static NJS_TEXNAME chimney_textures_4[1] = {
#include "shared/ocean_chimney_art/chimney_textures_4.inc"
};

NJS_TEXLIST chimney_texlist_4 = {
#include "shared/ocean_chimney_art/chimney_texlist_4.inc"
};

static Sint16 chimney_model_4_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_4_polygons.inc"
};

static Sint32 chimney_model_4_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_4_vertices.inc"
};

NJS_CNK_MODEL chimney_model_4 = {
#include "shared/ocean_chimney_art/chimney_model_4.inc"
};

static NJS_CNK_OBJECT chimney_model_4_object = {
#include "shared/ocean_chimney_art/chimney_model_4_object.inc"
};

static NJS_TEXNAME chimney_textures_5[1] = {
#include "shared/ocean_chimney_art/chimney_textures_5.inc"
};

NJS_TEXLIST chimney_texlist_5 = {
#include "shared/ocean_chimney_art/chimney_texlist_5.inc"
};

static Sint16 chimney_model_5_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_5_polygons.inc"
};

static Sint32 chimney_model_5_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_5_vertices.inc"
};

NJS_CNK_MODEL chimney_model_5 = {
#include "shared/ocean_chimney_art/chimney_model_5.inc"
};

static NJS_CNK_OBJECT chimney_model_5_object = {
#include "shared/ocean_chimney_art/chimney_model_5_object.inc"
};

static NJS_TEXNAME chimney_textures_6[1] = {
#include "shared/ocean_chimney_art/chimney_textures_6.inc"
};

NJS_TEXLIST chimney_texlist_6 = {
#include "shared/ocean_chimney_art/chimney_texlist_6.inc"
};

static Sint16 chimney_model_6_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_6_polygons.inc"
};

static Sint32 chimney_model_6_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_6_vertices.inc"
};

NJS_CNK_MODEL chimney_model_6 = {
#include "shared/ocean_chimney_art/chimney_model_6.inc"
};

static NJS_CNK_OBJECT chimney_model_6_object = {
#include "shared/ocean_chimney_art/chimney_model_6_object.inc"
};

static NJS_TEXNAME chimney_textures_7[1] = {
#include "shared/ocean_chimney_art/chimney_textures_7.inc"
};

NJS_TEXLIST chimney_texlist_7 = {
#include "shared/ocean_chimney_art/chimney_texlist_7.inc"
};

static Sint16 chimney_model_7_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_7_polygons.inc"
};

static Sint32 chimney_model_7_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_7_vertices.inc"
};

NJS_CNK_MODEL chimney_model_7 = {
#include "shared/ocean_chimney_art/chimney_model_7.inc"
};

static NJS_CNK_OBJECT chimney_model_7_object = {
#include "shared/ocean_chimney_art/chimney_model_7_object.inc"
};

static NJS_TEXNAME chimney_textures_8[1] = {
#include "shared/ocean_chimney_art/chimney_textures_8.inc"
};

NJS_TEXLIST chimney_texlist_8 = {
#include "shared/ocean_chimney_art/chimney_texlist_8.inc"
};

static Sint16 chimney_model_8_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_8_polygons.inc"
};

static Sint32 chimney_model_8_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_8_vertices.inc"
};

NJS_CNK_MODEL chimney_model_8 = {
#include "shared/ocean_chimney_art/chimney_model_8.inc"
};

static NJS_CNK_OBJECT chimney_model_8_object = {
#include "shared/ocean_chimney_art/chimney_model_8_object.inc"
};

static NJS_TEXNAME chimney_textures_9[1] = {
#include "shared/ocean_chimney_art/chimney_textures_9.inc"
};

NJS_TEXLIST chimney_texlist_9 = {
#include "shared/ocean_chimney_art/chimney_texlist_9.inc"
};

static Sint16 chimney_model_9_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_9_polygons.inc"
};

static Sint32 chimney_model_9_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_9_vertices.inc"
};

NJS_CNK_MODEL chimney_model_9 = {
#include "shared/ocean_chimney_art/chimney_model_9.inc"
};

static NJS_CNK_OBJECT chimney_model_9_object = {
#include "shared/ocean_chimney_art/chimney_model_9_object.inc"
};

static NJS_TEXNAME chimney_textures_10[1] = {
#include "shared/ocean_chimney_art/chimney_textures_10.inc"
};

NJS_TEXLIST chimney_texlist_10 = {
#include "shared/ocean_chimney_art/chimney_texlist_10.inc"
};

static Sint16 chimney_model_10_polygons[43] = {
#include "shared/ocean_chimney_art/chimney_model_10_polygons.inc"
};

static Sint32 chimney_model_10_vertices[35] = {
#include "shared/ocean_chimney_art/chimney_model_10_vertices.inc"
};

NJS_CNK_MODEL chimney_model_10 = {
#include "shared/ocean_chimney_art/chimney_model_10.inc"
};

static NJS_CNK_OBJECT chimney_model_10_object = {
#include "shared/ocean_chimney_art/chimney_model_10_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_0[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_0.inc"
};

NJS_TEXLIST chimney_blast_texlist_0 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_0.inc"
};

static Sint16 chimney_blast_model_0_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_0_polygons.inc"
};

static Sint32 chimney_blast_model_0_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_0_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_0 = {
#include "shared/ocean_chimney_art/chimney_blast_model_0.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_0_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_0_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_1[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_1.inc"
};

NJS_TEXLIST chimney_blast_texlist_1 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_1.inc"
};

static Sint16 chimney_blast_model_1_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_1_polygons.inc"
};

static Sint32 chimney_blast_model_1_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_1_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_1 = {
#include "shared/ocean_chimney_art/chimney_blast_model_1.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_1_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_1_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_2[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_2.inc"
};

NJS_TEXLIST chimney_blast_texlist_2 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_2.inc"
};

static Sint16 chimney_blast_model_2_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_2_polygons.inc"
};

static Sint32 chimney_blast_model_2_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_2_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_2 = {
#include "shared/ocean_chimney_art/chimney_blast_model_2.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_2_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_2_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_3[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_3.inc"
};

NJS_TEXLIST chimney_blast_texlist_3 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_3.inc"
};

static Sint16 chimney_blast_model_3_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_3_polygons.inc"
};

static Sint32 chimney_blast_model_3_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_3_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_3 = {
#include "shared/ocean_chimney_art/chimney_blast_model_3.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_3_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_3_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_4[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_4.inc"
};

NJS_TEXLIST chimney_blast_texlist_4 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_4.inc"
};

static Sint16 chimney_blast_model_4_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_4_polygons.inc"
};

static Sint32 chimney_blast_model_4_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_4_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_4 = {
#include "shared/ocean_chimney_art/chimney_blast_model_4.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_4_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_4_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_5[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_5.inc"
};

NJS_TEXLIST chimney_blast_texlist_5 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_5.inc"
};

static Sint16 chimney_blast_model_5_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_5_polygons.inc"
};

static Sint32 chimney_blast_model_5_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_5_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_5 = {
#include "shared/ocean_chimney_art/chimney_blast_model_5.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_5_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_5_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_6[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_6.inc"
};

NJS_TEXLIST chimney_blast_texlist_6 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_6.inc"
};

static Sint16 chimney_blast_model_6_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_6_polygons.inc"
};

static Sint32 chimney_blast_model_6_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_6_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_6 = {
#include "shared/ocean_chimney_art/chimney_blast_model_6.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_6_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_6_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_7[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_7.inc"
};

NJS_TEXLIST chimney_blast_texlist_7 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_7.inc"
};

static Sint16 chimney_blast_model_7_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_7_polygons.inc"
};

static Sint32 chimney_blast_model_7_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_7_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_7 = {
#include "shared/ocean_chimney_art/chimney_blast_model_7.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_7_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_7_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_8[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_8.inc"
};

NJS_TEXLIST chimney_blast_texlist_8 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_8.inc"
};

static Sint16 chimney_blast_model_8_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_8_polygons.inc"
};

static Sint32 chimney_blast_model_8_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_8_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_8 = {
#include "shared/ocean_chimney_art/chimney_blast_model_8.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_8_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_8_object.inc"
};

static NJS_TEXNAME chimney_blast_textures_9[1] = {
#include "shared/ocean_chimney_art/chimney_blast_textures_9.inc"
};

NJS_TEXLIST chimney_blast_texlist_9 = {
#include "shared/ocean_chimney_art/chimney_blast_texlist_9.inc"
};

static Sint16 chimney_blast_model_9_polygons[25] = {
#include "shared/ocean_chimney_art/chimney_blast_model_9_polygons.inc"
};

static Sint32 chimney_blast_model_9_vertices[19] = {
#include "shared/ocean_chimney_art/chimney_blast_model_9_vertices.inc"
};

NJS_CNK_MODEL chimney_blast_model_9 = {
#include "shared/ocean_chimney_art/chimney_blast_model_9.inc"
};

static NJS_CNK_OBJECT chimney_blast_model_9_object = {
#include "shared/ocean_chimney_art/chimney_blast_model_9_object.inc"
};
