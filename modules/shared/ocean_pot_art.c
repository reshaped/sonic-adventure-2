#include <ninja.h>
#include <sg_xpt.h>

#include "shared/ocean_pot.h"

/* clang-format off */

static const char material_kage3[12] = {
#include "shared/ocean_pot_art/material_kage3.inc"
};

static const char material_tubo1[12] = {
#include "shared/ocean_pot_art/material_tubo1.inc"
};

enum {
    OCEAN_POT_KAGE3,
    OCEAN_POT_TUBO1
};

enum {
    OCEAN_POT_PIECE_TEXLIST_0_TUBO1
};

static NJS_TEXNAME ocean_pot_textures[2] = {
#include "shared/ocean_pot_art/ocean_pot_textures.inc"
};

NJS_TEXLIST ocean_pot_texlist = {
#include "shared/ocean_pot_art/ocean_pot_texlist.inc"
};

static Sint16 ocean_pot_polygons[299] = {
#include "shared/ocean_pot_art/ocean_pot_polygons.inc"
};

static Sint32 ocean_pot_vertices[191] = {
#include "shared/ocean_pot_art/ocean_pot_vertices.inc"
};

NJS_CNK_MODEL ocean_pot_model = {
#include "shared/ocean_pot_art/ocean_pot_model.inc"
};

static NJS_CNK_OBJECT ocean_pot_object = {
#include "shared/ocean_pot_art/ocean_pot_object.inc"
};

static NJS_TEXNAME ocean_pot_piece_textures_0[1] = {
#include "shared/ocean_pot_art/ocean_pot_piece_textures_0.inc"
};

NJS_TEXLIST ocean_pot_piece_texlist_0 = {
#include "shared/ocean_pot_art/ocean_pot_piece_texlist_0.inc"
};

static Sint16 ocean_pot_piece_model_0_polygons[105] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_0_polygons.inc"
};

static Sint32 ocean_pot_piece_model_0_vertices[87] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_0_vertices.inc"
};

NJS_CNK_MODEL ocean_pot_piece_model_0 = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_0.inc"
};

static NJS_CNK_OBJECT ocean_pot_piece_model_0_object = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_0_object.inc"
};

static NJS_TEXNAME ocean_pot_piece_textures_1[1] = {
#include "shared/ocean_pot_art/ocean_pot_piece_textures_1.inc"
};

NJS_TEXLIST ocean_pot_piece_texlist_1 = {
#include "shared/ocean_pot_art/ocean_pot_piece_texlist_1.inc"
};

static Sint16 ocean_pot_piece_model_1_polygons[29] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_1_polygons.inc"
};

static Sint32 ocean_pot_piece_model_1_vertices[23] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_1_vertices.inc"
};

NJS_CNK_MODEL ocean_pot_piece_model_1 = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_1.inc"
};

static NJS_CNK_OBJECT ocean_pot_piece_model_1_object = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_1_object.inc"
};

static NJS_TEXNAME ocean_pot_piece_textures_2[1] = {
#include "shared/ocean_pot_art/ocean_pot_piece_textures_2.inc"
};

NJS_TEXLIST ocean_pot_piece_texlist_2 = {
#include "shared/ocean_pot_art/ocean_pot_piece_texlist_2.inc"
};

static Sint16 ocean_pot_piece_model_2_polygons[45] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_2_polygons.inc"
};

static Sint32 ocean_pot_piece_model_2_vertices[35] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_2_vertices.inc"
};

NJS_CNK_MODEL ocean_pot_piece_model_2 = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_2.inc"
};

static NJS_CNK_OBJECT ocean_pot_piece_model_2_object = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_2_object.inc"
};

static NJS_TEXNAME ocean_pot_piece_textures_3[1] = {
#include "shared/ocean_pot_art/ocean_pot_piece_textures_3.inc"
};

NJS_TEXLIST ocean_pot_piece_texlist_3 = {
#include "shared/ocean_pot_art/ocean_pot_piece_texlist_3.inc"
};

static Sint16 ocean_pot_piece_model_3_polygons[45] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_3_polygons.inc"
};

static Sint32 ocean_pot_piece_model_3_vertices[35] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_3_vertices.inc"
};

NJS_CNK_MODEL ocean_pot_piece_model_3 = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_3.inc"
};

static NJS_CNK_OBJECT ocean_pot_piece_model_3_object = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_3_object.inc"
};

static NJS_TEXNAME ocean_pot_piece_textures_4[1] = {
#include "shared/ocean_pot_art/ocean_pot_piece_textures_4.inc"
};

NJS_TEXLIST ocean_pot_piece_texlist_4 = {
#include "shared/ocean_pot_art/ocean_pot_piece_texlist_4.inc"
};

static Sint16 ocean_pot_piece_model_4_polygons[53] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_4_polygons.inc"
};

static Sint32 ocean_pot_piece_model_4_vertices[47] = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_4_vertices.inc"
};

NJS_CNK_MODEL ocean_pot_piece_model_4 = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_4.inc"
};

static NJS_CNK_OBJECT ocean_pot_piece_model_4_object = {
#include "shared/ocean_pot_art/ocean_pot_piece_model_4_object.inc"
};

/* clang-format on */
