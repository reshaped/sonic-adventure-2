#ifndef SHARED_PRISON_MACHINERY_MACHINE_LAMP_H
#define SHARED_PRISON_MACHINERY_MACHINE_LAMP_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 machine_lamp_polygons[25] = {
#include "shared/prison_machinery/machine_lamp/machine_lamp_polygons.inc"
};

static Sint32 machine_lamp_vertices[19] = {
#include "shared/prison_machinery/machine_lamp/machine_lamp_vertices.inc"
};

static NJS_CNK_MODEL machine_lamp_model = {
#include "shared/prison_machinery/machine_lamp/machine_lamp_model.inc"
};

static NJS_CNK_OBJECT machine_lamp = {
#include "shared/prison_machinery/machine_lamp/machine_lamp.inc"
};

#endif
