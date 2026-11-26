/* Treasure chest blue piece 5. */
#ifndef SHARED_TREASURE_MESHES_CHEST_BLUE_PIECE_5_H
#define SHARED_TREASURE_MESHES_CHEST_BLUE_PIECE_5_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 blue_piece_5_polygons[77] = {
#include "shared/treasure_meshes/chest_blue_piece_5/blue_piece_5_polygons.inc"
};

static Sint32 blue_piece_5_vertices[83] = {
#include "shared/treasure_meshes/chest_blue_piece_5/blue_piece_5_vertices.inc"
};

static NJS_CNK_MODEL blue_piece_5_model = {
#include "shared/treasure_meshes/chest_blue_piece_5/blue_piece_5_model.inc"
};

static NJS_CNK_OBJECT blue_piece_5_object = {
#include "shared/treasure_meshes/chest_blue_piece_5/blue_piece_5_object.inc"
};

#endif
