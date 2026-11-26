/* Road barricade piece 2 in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_ROAD_BARRICADE_PIECE_2_H
#define SHARED_STREET_FIXTURES_ROAD_BARRICADE_PIECE_2_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME road_barricade_piece_2_textures[1] = {
#include "shared/street_fixtures/road_barricade_piece_2/road_barricade_piece_2_textures.inc"
};

static NJS_TEXLIST road_barricade_piece_2_texlist = {
#include "shared/street_fixtures/road_barricade_piece_2/road_barricade_piece_2_texlist.inc"
};

static Sint16 road_barricade_piece_2_polygons[97] = {
#include "shared/street_fixtures/road_barricade_piece_2/road_barricade_piece_2_polygons.inc"
};

static Sint32 road_barricade_piece_2_vertices[39] = {
#include "shared/street_fixtures/road_barricade_piece_2/road_barricade_piece_2_vertices.inc"
};

static NJS_CNK_MODEL road_barricade_piece_2_model = {
#include "shared/street_fixtures/road_barricade_piece_2/road_barricade_piece_2_model.inc"
};

static NJS_CNK_OBJECT road_barricade_piece_2_object = {
#include "shared/street_fixtures/road_barricade_piece_2/road_barricade_piece_2_object.inc"
};

#endif
