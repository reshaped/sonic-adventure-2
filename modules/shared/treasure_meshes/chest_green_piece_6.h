/* Treasure chest green piece 6. */
#ifndef SHARED_TREASURE_MESHES_CHEST_GREEN_PIECE_6_H
#define SHARED_TREASURE_MESHES_CHEST_GREEN_PIECE_6_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 green_piece_6_polygons[57] = {
#include "shared/treasure_meshes/chest_green_piece_6/green_piece_6_polygons.inc"
};

static Sint32 green_piece_6_vertices[59] = {
#include "shared/treasure_meshes/chest_green_piece_6/green_piece_6_vertices.inc"
};

static NJS_CNK_MODEL green_piece_6_model = {
#include "shared/treasure_meshes/chest_green_piece_6/green_piece_6_model.inc"
};

static NJS_CNK_OBJECT green_piece_6_object = {
#include "shared/treasure_meshes/chest_green_piece_6/green_piece_6_object.inc"
};

#endif
