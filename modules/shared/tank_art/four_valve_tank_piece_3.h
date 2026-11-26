#ifndef SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_3_H
#define SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_3_H

/* Tank debris, piece 3. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME four_valve_tank_piece_3_names[2] = {
#include "shared/tank_art/four_valve_tank_piece_3/four_valve_tank_piece_3_names.inc"
};

static NJS_TEXLIST four_valve_tank_piece_3_texlist = {
#include "shared/tank_art/four_valve_tank_piece_3/four_valve_tank_piece_3_texlist.inc"
};

static Sint16 four_valve_tank_piece_3_polygons[63] = {
#include "shared/tank_art/four_valve_tank_piece_3/four_valve_tank_piece_3_polygons.inc"
};

static Sint32 four_valve_tank_piece_3_vertices[43] = {
#include "shared/tank_art/four_valve_tank_piece_3/four_valve_tank_piece_3_vertices.inc"
};

static NJS_CNK_MODEL four_valve_tank_piece_3_model = {
#include "shared/tank_art/four_valve_tank_piece_3/four_valve_tank_piece_3_model.inc"
};

static NJS_CNK_OBJECT four_valve_tank_piece_3_object = {
#include "shared/tank_art/four_valve_tank_piece_3/four_valve_tank_piece_3_object.inc"
};

#endif
