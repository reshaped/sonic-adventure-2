#ifndef STG09_FIXTURES_GAS_CYLINDER_SIDE_H
#define STG09_FIXTURES_GAS_CYLINDER_SIDE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gas_cylinder_side_names[2] = {
#include "stg09/fixtures/gas_cylinder_side/gas_cylinder_side_names.inc"
};

static NJS_TEXLIST gas_cylinder_side_texlist = {
#include "stg09/fixtures/gas_cylinder_side/gas_cylinder_side_texlist.inc"
};

static Sint16 gas_cylinder_side_polygons[95] = {
#include "stg09/fixtures/gas_cylinder_side/gas_cylinder_side_polygons.inc"
};

static Sint32 gas_cylinder_side_vertices[55] = {
#include "stg09/fixtures/gas_cylinder_side/gas_cylinder_side_vertices.inc"
};

static NJS_CNK_MODEL gas_cylinder_side_model = {
#include "stg09/fixtures/gas_cylinder_side/gas_cylinder_side_model.inc"
};

static NJS_CNK_OBJECT gas_cylinder_side_object = {
#include "stg09/fixtures/gas_cylinder_side/gas_cylinder_side_object.inc"
};

#endif
