#ifndef SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_4_H
#define SHARED_TANK_ART_FOUR_VALVE_TANK_PIECE_4_H

/* Tank debris, piece 4. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME four_valve_tank_piece_4_names[3] = {
#include "shared/tank_art/four_valve_tank_piece_4/four_valve_tank_piece_4_names.inc"
};

static NJS_TEXLIST four_valve_tank_piece_4_texlist = {
#include "shared/tank_art/four_valve_tank_piece_4/four_valve_tank_piece_4_texlist.inc"
};

static Sint16 four_valve_tank_piece_4_polygons[175] = {
#include "shared/tank_art/four_valve_tank_piece_4/four_valve_tank_piece_4_polygons.inc"
};

static Sint32 four_valve_tank_piece_4_vertices[127] = {
#include "shared/tank_art/four_valve_tank_piece_4/four_valve_tank_piece_4_vertices.inc"
};

static NJS_CNK_MODEL four_valve_tank_piece_4_model = {
#include "shared/tank_art/four_valve_tank_piece_4/four_valve_tank_piece_4_model.inc"
};

static NJS_CNK_OBJECT four_valve_tank_piece_4_object = {
#include "shared/tank_art/four_valve_tank_piece_4/four_valve_tank_piece_4_object.inc"
};

#endif
