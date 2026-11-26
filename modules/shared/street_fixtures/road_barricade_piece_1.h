/* Road barricade piece 1 in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_ROAD_BARRICADE_PIECE_1_H
#define SHARED_STREET_FIXTURES_ROAD_BARRICADE_PIECE_1_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME road_barricade_piece_1_textures[2] = {
#include "shared/street_fixtures/road_barricade_piece_1/road_barricade_piece_1_textures.inc"
};

static NJS_TEXLIST road_barricade_piece_1_texlist = {
#include "shared/street_fixtures/road_barricade_piece_1/road_barricade_piece_1_texlist.inc"
};

static Sint16 road_barricade_piece_1_polygons[95] = {
#include "shared/street_fixtures/road_barricade_piece_1/road_barricade_piece_1_polygons.inc"
};

static Sint32 road_barricade_piece_1_vertices[91] = {
#include "shared/street_fixtures/road_barricade_piece_1/road_barricade_piece_1_vertices.inc"
};

static NJS_CNK_MODEL road_barricade_piece_1_model = {
#include "shared/street_fixtures/road_barricade_piece_1/road_barricade_piece_1_model.inc"
};

static NJS_CNK_OBJECT road_barricade_piece_1_object = {
#include "shared/street_fixtures/road_barricade_piece_1/road_barricade_piece_1_object.inc"
};

#endif
