/* Treasure chest green pieces. */
#ifndef SHARED_TREASURE_MESHES_CHEST_GREEN_PIECES_H
#define SHARED_TREASURE_MESHES_CHEST_GREEN_PIECES_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_CNK_OBJECT green_piece_0_object;
static NJS_CNK_OBJECT green_piece_1_object;
static NJS_CNK_OBJECT green_piece_2_object;
static NJS_CNK_OBJECT green_piece_3_object;
static NJS_CNK_OBJECT green_piece_4_object;
static NJS_CNK_OBJECT green_piece_5_object;
static NJS_CNK_OBJECT green_piece_6_object;
static NJS_CNK_OBJECT green_piece_7_object;

static NJS_TEXNAME green_pieces_textures[1] = {
#include "shared/treasure_meshes/chest_green_pieces/green_pieces_textures.inc"
};

static NJS_TEXLIST green_pieces_texlist = {
#include "shared/treasure_meshes/chest_green_pieces/green_pieces_texlist.inc"
};

static Sint16 green_piece_7_polygons[45] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_7_polygons.inc"
};

static Sint32 green_piece_7_vertices[43] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_7_vertices.inc"
};

static NJS_CNK_MODEL green_piece_7_model = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_7_model.inc"
};

static NJS_CNK_OBJECT green_piece_7_object = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_7_object.inc"
};

/* The two stages link these pieces in opposite order. */
#ifdef EGG_QUARTERS
#include "shared/treasure_meshes/chest_green_piece_4.h"
#include "shared/treasure_meshes/chest_green_piece_6.h"
#else
#include "shared/treasure_meshes/chest_green_piece_6.h"
#include "shared/treasure_meshes/chest_green_piece_4.h"
#endif

static Sint16 green_piece_5_polygons[77] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_5_polygons.inc"
};

static Sint32 green_piece_5_vertices[83] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_5_vertices.inc"
};

static NJS_CNK_MODEL green_piece_5_model = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_5_model.inc"
};

static NJS_CNK_OBJECT green_piece_5_object = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_5_object.inc"
};

static Sint16 green_piece_3_polygons[115] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_3_polygons.inc"
};

static Sint32 green_piece_3_vertices[131] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_3_vertices.inc"
};

static NJS_CNK_MODEL green_piece_3_model = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_3_model.inc"
};

static NJS_CNK_OBJECT green_piece_3_object = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_3_object.inc"
};

static Sint16 green_piece_2_polygons[77] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_2_polygons.inc"
};

static Sint32 green_piece_2_vertices[71] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_2_vertices.inc"
};

static NJS_CNK_MODEL green_piece_2_model = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_2_model.inc"
};

static NJS_CNK_OBJECT green_piece_2_object = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_2_object.inc"
};

static Sint16 green_piece_1_polygons[69] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_1_polygons.inc"
};

static Sint32 green_piece_1_vertices[75] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_1_vertices.inc"
};

static NJS_CNK_MODEL green_piece_1_model = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_1_model.inc"
};

static NJS_CNK_OBJECT green_piece_1_object = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_1_object.inc"
};

static Sint16 green_piece_0_polygons[65] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_0_polygons.inc"
};

static Sint32 green_piece_0_vertices[51] = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_0_vertices.inc"
};

static NJS_CNK_MODEL green_piece_0_model = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_0_model.inc"
};

static NJS_CNK_OBJECT green_piece_0_object = {
#include "shared/treasure_meshes/chest_green_pieces/green_piece_0_object.inc"
};

static NJS_CNK_OBJECT green_debris = {
#include "shared/treasure_meshes/chest_green_pieces/green_debris.inc"
};

#endif
