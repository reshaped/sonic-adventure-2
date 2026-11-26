#ifndef SHARED_PRISON_MACHINERY_TANK_BASE_H
#define SHARED_PRISON_MACHINERY_TANK_BASE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME tank_base_names[3] = {
#include "shared/prison_machinery/tank_base/tank_base_names.inc"
};

static NJS_TEXLIST tank_base_texlist = {
#include "shared/prison_machinery/tank_base/tank_base_texlist.inc"
};

static Sint16 tank_base_polygons[225] = {
#include "shared/prison_machinery/tank_base/tank_base_polygons.inc"
};

static Sint32 tank_base_vertices[159] = {
#include "shared/prison_machinery/tank_base/tank_base_vertices.inc"
};

static NJS_CNK_MODEL tank_base_model = {
#include "shared/prison_machinery/tank_base/tank_base_model.inc"
};

static NJS_CNK_OBJECT tank_base_object = {
#include "shared/prison_machinery/tank_base/tank_base_object.inc"
};

#endif
