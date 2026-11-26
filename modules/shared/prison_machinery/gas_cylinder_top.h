#ifndef SHARED_PRISON_MACHINERY_GAS_CYLINDER_TOP_H
#define SHARED_PRISON_MACHINERY_GAS_CYLINDER_TOP_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gas_cylinder_top_names[2] = {
#include "shared/prison_machinery/gas_cylinder_top/gas_cylinder_top_names.inc"
};

static NJS_TEXLIST gas_cylinder_top_texlist = {
#include "shared/prison_machinery/gas_cylinder_top/gas_cylinder_top_texlist.inc"
};

static Sint16 gas_cylinder_top_polygons[189] = {
#include "shared/prison_machinery/gas_cylinder_top/gas_cylinder_top_polygons.inc"
};

static Sint32 gas_cylinder_top_vertices[203] = {
#include "shared/prison_machinery/gas_cylinder_top/gas_cylinder_top_vertices.inc"
};

static NJS_CNK_MODEL gas_cylinder_top_model = {
#include "shared/prison_machinery/gas_cylinder_top/gas_cylinder_top_model.inc"
};

static NJS_CNK_OBJECT gas_cylinder_top_object = {
#include "shared/prison_machinery/gas_cylinder_top/gas_cylinder_top_object.inc"
};

#endif
