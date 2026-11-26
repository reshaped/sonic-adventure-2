#ifndef SHARED_PRISON_MACHINERY_TANK_TEXTURES_H
#define SHARED_PRISON_MACHINERY_TANK_TEXTURES_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

static NJS_TEXNAME tank_names[7] = {
#include "shared/prison_machinery/tank_textures/tank_names.inc"
};

static NJS_TEXLIST tank_texlist = {
#include "shared/prison_machinery/tank_textures/tank_texlist.inc"
};

#endif
