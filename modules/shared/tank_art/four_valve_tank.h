#ifndef SHARED_TANK_ART_FOUR_VALVE_TANK_H
#define SHARED_TANK_ART_FOUR_VALVE_TANK_H

/* Tank body and sprung lid. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME four_valve_tank_names[10] = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_names.inc"
};

static NJS_TEXLIST four_valve_tank_texlist = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_texlist.inc"
};

static Sint16 four_valve_tank_lid_polygons[285] = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_lid_polygons.inc"
};

static Sint32 four_valve_tank_lid_vertices[259] = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_lid_vertices.inc"
};

static NJS_CNK_MODEL four_valve_tank_lid_model = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_lid_model.inc"
};

static NJS_CNK_OBJECT four_valve_tank_lid = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_lid.inc"
};

static Sint16 four_valve_tank_polygons[575] = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_polygons.inc"
};

static Sint32 four_valve_tank_vertices[611] = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_vertices.inc"
};

static NJS_CNK_MODEL four_valve_tank_model = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_model.inc"
};

static NJS_CNK_OBJECT four_valve_tank_object[1] = {
#include "shared/tank_art/four_valve_tank/four_valve_tank_object.inc"
};

#endif
