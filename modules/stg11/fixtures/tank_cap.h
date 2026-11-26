#ifndef STG11_FIXTURES_TANK_CAP_H
#define STG11_FIXTURES_TANK_CAP_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 tank_cap_polygons[373] = {
#include "stg11/fixtures/tank_cap/tank_cap_polygons.inc"
};

static Sint32 tank_cap_vertices[367] = {
#include "stg11/fixtures/tank_cap/tank_cap_vertices.inc"
};

static NJS_CNK_MODEL tank_cap_model = {
#include "stg11/fixtures/tank_cap/tank_cap_model.inc"
};

static NJS_CNK_OBJECT tank_cap_object = {
#include "stg11/fixtures/tank_cap/tank_cap_object.inc"
};

#endif
