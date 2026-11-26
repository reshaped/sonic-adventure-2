/* Treasure chest red piece 4. */
#ifndef SHARED_TREASURE_MESHES_CHEST_RED_PIECE_4_H
#define SHARED_TREASURE_MESHES_CHEST_RED_PIECE_4_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 red_piece_4_polygons[221] = {
#include "shared/treasure_meshes/chest_red_piece_4/red_piece_4_polygons.inc"
};

static Sint32 red_piece_4_vertices[139] = {
#include "shared/treasure_meshes/chest_red_piece_4/red_piece_4_vertices.inc"
};

static NJS_CNK_MODEL red_piece_4_model = {
#include "shared/treasure_meshes/chest_red_piece_4/red_piece_4_model.inc"
};

static NJS_CNK_OBJECT red_piece_4_object = {
#include "shared/treasure_meshes/chest_red_piece_4/red_piece_4_object.inc"
};

#endif
