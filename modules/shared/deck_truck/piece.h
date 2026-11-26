#ifndef SHARED_DECK_TRUCK_PIECE_H
#define SHARED_DECK_TRUCK_PIECE_H

#include <ninja.h>

static NJS_TEXNAME deck_truck_piece_textures[2] = {
#include "shared/deck_truck/piece/deck_truck_piece_textures.inc"
};

static NJS_TEXLIST deck_truck_piece_texlist = {
#include "shared/deck_truck/piece/deck_truck_piece_texlist.inc"
};

static Sint16 deck_truck_piece_node_polygons[147] = {
#include "shared/deck_truck/piece/deck_truck_piece_node_polygons.inc"
};

static Sint32 deck_truck_piece_node_vertices[91] = {
#include "shared/deck_truck/piece/deck_truck_piece_node_vertices.inc"
};

static NJS_CNK_MODEL deck_truck_piece = {
#include "shared/deck_truck/piece/deck_truck_piece.inc"
};

/* The fragment emitter draws the model directly. */
static NJS_CNK_OBJECT deck_truck_piece_node = {
#include "shared/deck_truck/piece/deck_truck_piece_node.inc"
};

#endif
