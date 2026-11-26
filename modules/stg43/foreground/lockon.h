#ifndef STG43_FOREGROUND_LOCKON_H
#define STG43_FOREGROUND_LOCKON_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME lockon_double_textures[6] = {
#include "stg43/foreground/lockon/lockon_double_textures.inc"
};

static NJS_TEXLIST lockon_double_texlist = {
#include "stg43/foreground/lockon/lockon_double_texlist.inc"
};

static Sint16 lockon_double_rotor_polygons[209] = {
#include "stg43/foreground/lockon/lockon_double_rotor_polygons.inc"
};

static Sint32 lockon_double_rotor_vertices[219] = {
#include "stg43/foreground/lockon/lockon_double_rotor_vertices.inc"
};

static NJS_CNK_MODEL lockon_double_rotor_model = {
#include "stg43/foreground/lockon/lockon_double_rotor_model.inc"
};

static NJS_CNK_OBJECT lockon_double_rotor = {
#include "stg43/foreground/lockon/lockon_double_rotor.inc"
};

static Sint16 lockon_double_polygons[441] = {
#include "stg43/foreground/lockon/lockon_double_polygons.inc"
};

static Sint32 lockon_double_vertices[299] = {
#include "stg43/foreground/lockon/lockon_double_vertices.inc"
};

static NJS_CNK_MODEL lockon_double_model = {
#include "stg43/foreground/lockon/lockon_double_model.inc"
};

static NJS_CNK_OBJECT lockon_double_object = {
#include "stg43/foreground/lockon/lockon_double_object.inc"
};

static NJS_TEXNAME lockon_single_piece_a_textures[3] = {
#include "stg43/foreground/lockon/lockon_single_piece_a_textures.inc"
};

static NJS_TEXLIST lockon_single_piece_a_texlist = {
#include "stg43/foreground/lockon/lockon_single_piece_a_texlist.inc"
};

static Sint16 lockon_single_piece_a_polygons[179] = {
#include "stg43/foreground/lockon/lockon_single_piece_a_polygons.inc"
};

static Sint32 lockon_single_piece_a_vertices[115] = {
#include "stg43/foreground/lockon/lockon_single_piece_a_vertices.inc"
};

static NJS_CNK_MODEL lockon_single_piece_a = {
#include "stg43/foreground/lockon/lockon_single_piece_a.inc"
};

static NJS_CNK_OBJECT lockon_single_piece_a_object = {
#include "stg43/foreground/lockon/lockon_single_piece_a_object.inc"
};

static NJS_TEXNAME lockon_single_piece_b_textures[2] = {
#include "stg43/foreground/lockon/lockon_single_piece_b_textures.inc"
};

static NJS_TEXLIST lockon_single_piece_b_texlist = {
#include "stg43/foreground/lockon/lockon_single_piece_b_texlist.inc"
};

static Sint16 lockon_single_piece_b_polygons[81] = {
#include "stg43/foreground/lockon/lockon_single_piece_b_polygons.inc"
};

static Sint32 lockon_single_piece_b_vertices[71] = {
#include "stg43/foreground/lockon/lockon_single_piece_b_vertices.inc"
};

static NJS_CNK_MODEL lockon_single_piece_b = {
#include "stg43/foreground/lockon/lockon_single_piece_b.inc"
};

static NJS_CNK_OBJECT lockon_single_piece_b_object = {
#include "stg43/foreground/lockon/lockon_single_piece_b_object.inc"
};

static NJS_TEXNAME lockon_single_textures[6] = {
#include "stg43/foreground/lockon/lockon_single_textures.inc"
};

static NJS_TEXLIST lockon_single_texlist = {
#include "stg43/foreground/lockon/lockon_single_texlist.inc"
};

static Sint16 lockon_single_rotor_polygons[209] = {
#include "stg43/foreground/lockon/lockon_single_rotor_polygons.inc"
};

static Sint32 lockon_single_rotor_vertices[219] = {
#include "stg43/foreground/lockon/lockon_single_rotor_vertices.inc"
};

static NJS_CNK_MODEL lockon_single_rotor_model = {
#include "stg43/foreground/lockon/lockon_single_rotor_model.inc"
};

static NJS_CNK_OBJECT lockon_single_rotor = {
#include "stg43/foreground/lockon/lockon_single_rotor.inc"
};

static Sint16 lockon_single_polygons[441] = {
#include "stg43/foreground/lockon/lockon_single_polygons.inc"
};

static Sint32 lockon_single_vertices[299] = {
#include "stg43/foreground/lockon/lockon_single_vertices.inc"
};

static NJS_CNK_MODEL lockon_single_model = {
#include "stg43/foreground/lockon/lockon_single_model.inc"
};

static NJS_CNK_OBJECT lockon_single_object = {
#include "stg43/foreground/lockon/lockon_single_object.inc"
};

static NJS_TEXNAME lockon_double_piece_a_textures[3] = {
#include "stg43/foreground/lockon/lockon_double_piece_a_textures.inc"
};

static NJS_TEXLIST lockon_double_piece_a_texlist = {
#include "stg43/foreground/lockon/lockon_double_piece_a_texlist.inc"
};

static Sint16 lockon_double_piece_a_polygons[179] = {
#include "stg43/foreground/lockon/lockon_double_piece_a_polygons.inc"
};

static Sint32 lockon_double_piece_a_vertices[123] = {
#include "stg43/foreground/lockon/lockon_double_piece_a_vertices.inc"
};

static NJS_CNK_MODEL lockon_double_piece_a = {
#include "stg43/foreground/lockon/lockon_double_piece_a.inc"
};

static NJS_CNK_OBJECT lockon_double_piece_a_object = {
#include "stg43/foreground/lockon/lockon_double_piece_a_object.inc"
};

static NJS_TEXNAME lockon_double_piece_b_textures[2] = {
#include "stg43/foreground/lockon/lockon_double_piece_b_textures.inc"
};

static NJS_TEXLIST lockon_double_piece_b_texlist = {
#include "stg43/foreground/lockon/lockon_double_piece_b_texlist.inc"
};

static Sint16 lockon_double_piece_b_polygons[81] = {
#include "stg43/foreground/lockon/lockon_double_piece_b_polygons.inc"
};

static Sint32 lockon_double_piece_b_vertices[71] = {
#include "stg43/foreground/lockon/lockon_double_piece_b_vertices.inc"
};

static NJS_CNK_MODEL lockon_double_piece_b = {
#include "stg43/foreground/lockon/lockon_double_piece_b.inc"
};

static NJS_CNK_OBJECT lockon_double_piece_b_object = {
#include "stg43/foreground/lockon/lockon_double_piece_b_object.inc"
};

#endif
