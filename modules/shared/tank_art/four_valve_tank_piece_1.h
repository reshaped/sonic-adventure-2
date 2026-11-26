#ifndef SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_1_H
#define SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_1_H

/* Tank debris, piece 1. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME four_valve_tank_piece_1_names[4] = {
#include "shared/tank_art/four_valve_tank_piece_1/four_valve_tank_piece_1_names.inc"
};

static NJS_TEXLIST four_valve_tank_piece_1_texlist = {
#include "shared/tank_art/four_valve_tank_piece_1/four_valve_tank_piece_1_texlist.inc"
};

static Sint16 four_valve_tank_piece_1_polygons[171] = {
#include "shared/tank_art/four_valve_tank_piece_1/four_valve_tank_piece_1_polygons.inc"
};

static Sint32 four_valve_tank_piece_1_vertices[123] = {
#include "shared/tank_art/four_valve_tank_piece_1/four_valve_tank_piece_1_vertices.inc"
};

static NJS_CNK_MODEL four_valve_tank_piece_1_model = {
#include "shared/tank_art/four_valve_tank_piece_1/four_valve_tank_piece_1_model.inc"
};

static NJS_CNK_OBJECT four_valve_tank_piece_1_object = {
#include "shared/tank_art/four_valve_tank_piece_1/four_valve_tank_piece_1_object.inc"
};

#endif
