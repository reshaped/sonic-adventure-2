#ifndef SHARED_PRISON_MACHINERY_TANK_RIM_H
#define SHARED_PRISON_MACHINERY_TANK_RIM_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME tank_rim_names[2] = {
#include "shared/prison_machinery/tank_rim/tank_rim_names.inc"
};

static NJS_TEXLIST tank_rim_texlist = {
#include "shared/prison_machinery/tank_rim/tank_rim_texlist.inc"
};

static Sint16 tank_rim_polygons[109] = {
#include "shared/prison_machinery/tank_rim/tank_rim_polygons.inc"
};

static Sint32 tank_rim_vertices[83] = {
#include "shared/prison_machinery/tank_rim/tank_rim_vertices.inc"
};

static NJS_CNK_MODEL tank_rim_model = {
#include "shared/prison_machinery/tank_rim/tank_rim_model.inc"
};

static NJS_CNK_OBJECT tank_rim_object = {
#include "shared/prison_machinery/tank_rim/tank_rim_object.inc"
};

#endif
