#ifndef SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_2_H
#define SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_2_H

/* Tank debris, piece 2. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME four_valve_tank_piece_2_names[2] = {
#include "shared/tank_art/four_valve_tank_piece_2/four_valve_tank_piece_2_names.inc"
};

static NJS_TEXLIST four_valve_tank_piece_2_texlist = {
#include "shared/tank_art/four_valve_tank_piece_2/four_valve_tank_piece_2_texlist.inc"
};

static Sint16 four_valve_tank_piece_2_polygons[57] = {
#include "shared/tank_art/four_valve_tank_piece_2/four_valve_tank_piece_2_polygons.inc"
};

static Sint32 four_valve_tank_piece_2_vertices[51] = {
#include "shared/tank_art/four_valve_tank_piece_2/four_valve_tank_piece_2_vertices.inc"
};

static NJS_CNK_MODEL four_valve_tank_piece_2_model = {
#include "shared/tank_art/four_valve_tank_piece_2/four_valve_tank_piece_2_model.inc"
};

static NJS_CNK_OBJECT four_valve_tank_piece_2_object = {
#include "shared/tank_art/four_valve_tank_piece_2/four_valve_tank_piece_2_object.inc"
};

#endif
