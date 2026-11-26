#ifndef STG11_FIXTURES_MACHINE_H
#define STG11_FIXTURES_MACHINE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME machine_names[6] = {
#include "stg11/fixtures/machine/machine_names.inc"
};

static NJS_TEXLIST machine_texlist = {
#include "stg11/fixtures/machine/machine_texlist.inc"
};

#include "shared/prison_machinery/machine_lamp.h"

static Sint16 machine_polygons[241] = {
#include "stg11/fixtures/machine/machine_polygons.inc"
};

static Sint32 machine_vertices[243] = {
#include "stg11/fixtures/machine/machine_vertices.inc"
};

static NJS_CNK_MODEL machine_model = {
#include "stg11/fixtures/machine/machine_model.inc"
};

static NJS_CNK_OBJECT machine_object = {
#include "stg11/fixtures/machine/machine_object.inc"
};

#endif
