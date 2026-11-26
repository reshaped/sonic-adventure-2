#ifndef SHARED_PRISON_MACHINERY_GAS_CYLINDER_BASE_H
#define SHARED_PRISON_MACHINERY_GAS_CYLINDER_BASE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gas_cylinder_base_names[1] = {
#include "shared/prison_machinery/gas_cylinder_base/gas_cylinder_base_names.inc"
};

static NJS_TEXLIST gas_cylinder_base_texlist = {
#include "shared/prison_machinery/gas_cylinder_base/gas_cylinder_base_texlist.inc"
};

static Sint16 gas_cylinder_base_polygons[117] = {
#include "shared/prison_machinery/gas_cylinder_base/gas_cylinder_base_polygons.inc"
};

static Sint32 gas_cylinder_base_vertices[75] = {
#include "shared/prison_machinery/gas_cylinder_base/gas_cylinder_base_vertices.inc"
};

static NJS_CNK_MODEL gas_cylinder_base_model = {
#include "shared/prison_machinery/gas_cylinder_base/gas_cylinder_base_model.inc"
};

static NJS_CNK_OBJECT gas_cylinder_base_object = {
#include "shared/prison_machinery/gas_cylinder_base/gas_cylinder_base_object.inc"
};

#endif
