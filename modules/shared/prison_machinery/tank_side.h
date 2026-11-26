#ifndef SHARED_PRISON_MACHINERY_TANK_SIDE_H
#define SHARED_PRISON_MACHINERY_TANK_SIDE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME tank_side_names[2] = {
#include "shared/prison_machinery/tank_side/tank_side_names.inc"
};

static NJS_TEXLIST tank_side_texlist = {
#include "shared/prison_machinery/tank_side/tank_side_texlist.inc"
};

static Sint16 tank_side_polygons[189] = {
#include "shared/prison_machinery/tank_side/tank_side_polygons.inc"
};

static Sint32 tank_side_vertices[195] = {
#include "shared/prison_machinery/tank_side/tank_side_vertices.inc"
};

static NJS_CNK_MODEL tank_side_model = {
#include "shared/prison_machinery/tank_side/tank_side_model.inc"
};

static NJS_CNK_OBJECT tank_side_object = {
#include "shared/prison_machinery/tank_side/tank_side_object.inc"
};

#endif
