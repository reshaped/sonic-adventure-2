/* Treasure chest red piece 7. */
#ifndef SHARED_TREASURE_MESHES_CHEST_RED_PIECE_7_H
#define SHARED_TREASURE_MESHES_CHEST_RED_PIECE_7_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 red_piece_7_polygons[45] = {
#include "shared/treasure_meshes/chest_red_piece_7/red_piece_7_polygons.inc"
};

static Sint32 red_piece_7_vertices[39] = {
#include "shared/treasure_meshes/chest_red_piece_7/red_piece_7_vertices.inc"
};

static NJS_CNK_MODEL red_piece_7_model = {
#include "shared/treasure_meshes/chest_red_piece_7/red_piece_7_model.inc"
};

static NJS_CNK_OBJECT red_piece_7_object = {
#include "shared/treasure_meshes/chest_red_piece_7/red_piece_7_object.inc"
};

#endif
