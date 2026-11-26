/* Road barricade piece 0 in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_ROAD_BARRICADE_PIECE_0_H
#define SHARED_STREET_FIXTURES_ROAD_BARRICADE_PIECE_0_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME road_barricade_piece_0_textures[1] = {
#include "shared/street_fixtures/road_barricade_piece_0/road_barricade_piece_0_textures.inc"
};

static NJS_TEXLIST road_barricade_piece_0_texlist = {
#include "shared/street_fixtures/road_barricade_piece_0/road_barricade_piece_0_texlist.inc"
};

static Sint16 road_barricade_piece_0_polygons[51] = {
#include "shared/street_fixtures/road_barricade_piece_0/road_barricade_piece_0_polygons.inc"
};

static Sint32 road_barricade_piece_0_vertices[47] = {
#include "shared/street_fixtures/road_barricade_piece_0/road_barricade_piece_0_vertices.inc"
};

static NJS_CNK_MODEL road_barricade_piece_0_model = {
#include "shared/street_fixtures/road_barricade_piece_0/road_barricade_piece_0_model.inc"
};

static NJS_CNK_OBJECT road_barricade_piece_0_object = {
#include "shared/street_fixtures/road_barricade_piece_0/road_barricade_piece_0_object.inc"
};

#endif
