/* Treasure chest blue piece 4. */
#ifndef SHARED_TREASURE_MESHES_CHEST_BLUE_PIECE_4_H
#define SHARED_TREASURE_MESHES_CHEST_BLUE_PIECE_4_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 blue_piece_4_polygons[221] = {
#include "shared/treasure_meshes/chest_blue_piece_4/blue_piece_4_polygons.inc"
};

static Sint32 blue_piece_4_vertices[139] = {
#include "shared/treasure_meshes/chest_blue_piece_4/blue_piece_4_vertices.inc"
};

static NJS_CNK_MODEL blue_piece_4_model = {
#include "shared/treasure_meshes/chest_blue_piece_4/blue_piece_4_model.inc"
};

static NJS_CNK_OBJECT blue_piece_4_object = {
#include "shared/treasure_meshes/chest_blue_piece_4/blue_piece_4_object.inc"
};

#endif
