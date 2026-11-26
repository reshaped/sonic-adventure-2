#ifndef SHARED_PRISON_MACHINERY_TANK_H
#define SHARED_PRISON_MACHINERY_TANK_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 tank_polygons[269] = {
#include "shared/prison_machinery/tank/tank_polygons.inc"
};

static Sint32 tank_vertices[207] = {
#include "shared/prison_machinery/tank/tank_vertices.inc"
};

static NJS_CNK_MODEL tank_model = {
#include "shared/prison_machinery/tank/tank_model.inc"
};

static NJS_CNK_OBJECT tank_object[1] = {
#include "shared/prison_machinery/tank/tank_object.inc"
};

#endif
