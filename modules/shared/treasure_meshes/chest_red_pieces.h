/* Treasure chest red pieces. */
#ifndef SHARED_TREASURE_MESHES_CHEST_RED_PIECES_H
#define SHARED_TREASURE_MESHES_CHEST_RED_PIECES_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_CNK_OBJECT red_piece_0_object;
static NJS_CNK_OBJECT red_piece_1_object;
static NJS_CNK_OBJECT red_piece_2_object;
static NJS_CNK_OBJECT red_piece_3_object;
static NJS_CNK_OBJECT red_piece_4_object;
static NJS_CNK_OBJECT red_piece_5_object;
static NJS_CNK_OBJECT red_piece_6_object;
static NJS_CNK_OBJECT red_piece_7_object;

static NJS_TEXNAME red_pieces_textures[1] = {
#include "shared/treasure_meshes/chest_red_pieces/red_pieces_textures.inc"
};

static NJS_TEXLIST red_pieces_texlist = {
#include "shared/treasure_meshes/chest_red_pieces/red_pieces_texlist.inc"
};

/* The two stages link these pieces in opposite order. */
#ifdef EGG_QUARTERS
#include "shared/treasure_meshes/chest_red_piece_4.h"
#include "shared/treasure_meshes/chest_red_piece_7.h"
#else
#include "shared/treasure_meshes/chest_red_piece_7.h"
#include "shared/treasure_meshes/chest_red_piece_4.h"
#endif

static Sint16 red_piece_6_polygons[57] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_6_polygons.inc"
};

static Sint32 red_piece_6_vertices[59] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_6_vertices.inc"
};

static NJS_CNK_MODEL red_piece_6_model = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_6_model.inc"
};

static NJS_CNK_OBJECT red_piece_6_object = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_6_object.inc"
};

static Sint16 red_piece_5_polygons[77] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_5_polygons.inc"
};

static Sint32 red_piece_5_vertices[83] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_5_vertices.inc"
};

static NJS_CNK_MODEL red_piece_5_model = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_5_model.inc"
};

static NJS_CNK_OBJECT red_piece_5_object = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_5_object.inc"
};

static Sint16 red_piece_3_polygons[115] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_3_polygons.inc"
};

static Sint32 red_piece_3_vertices[131] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_3_vertices.inc"
};

static NJS_CNK_MODEL red_piece_3_model = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_3_model.inc"
};

static NJS_CNK_OBJECT red_piece_3_object = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_3_object.inc"
};

static Sint16 red_piece_2_polygons[77] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_2_polygons.inc"
};

static Sint32 red_piece_2_vertices[71] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_2_vertices.inc"
};

static NJS_CNK_MODEL red_piece_2_model = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_2_model.inc"
};

static NJS_CNK_OBJECT red_piece_2_object = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_2_object.inc"
};

static Sint16 red_piece_1_polygons[69] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_1_polygons.inc"
};

static Sint32 red_piece_1_vertices[71] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_1_vertices.inc"
};

static NJS_CNK_MODEL red_piece_1_model = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_1_model.inc"
};

static NJS_CNK_OBJECT red_piece_1_object = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_1_object.inc"
};

static Sint16 red_piece_0_polygons[65] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_0_polygons.inc"
};

static Sint32 red_piece_0_vertices[51] = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_0_vertices.inc"
};

static NJS_CNK_MODEL red_piece_0_model = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_0_model.inc"
};

static NJS_CNK_OBJECT red_piece_0_object = {
#include "shared/treasure_meshes/chest_red_pieces/red_piece_0_object.inc"
};

static NJS_CNK_OBJECT red_debris = {
#include "shared/treasure_meshes/chest_red_pieces/red_debris.inc"
};

#endif
