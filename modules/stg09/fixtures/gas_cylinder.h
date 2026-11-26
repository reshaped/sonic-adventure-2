#ifndef STG09_FIXTURES_GAS_CYLINDER_H
#define STG09_FIXTURES_GAS_CYLINDER_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gas_cylinder_names[6] = {
#include "stg09/fixtures/gas_cylinder/gas_cylinder_names.inc"
};

static NJS_TEXLIST gas_cylinder_texlist = {
#include "stg09/fixtures/gas_cylinder/gas_cylinder_texlist.inc"
};

static Sint16 gas_cylinder_polygons[559] = {
#include "stg09/fixtures/gas_cylinder/gas_cylinder_polygons.inc"
};

static Sint32 gas_cylinder_vertices[399] = {
#include "stg09/fixtures/gas_cylinder/gas_cylinder_vertices.inc"
};

static NJS_CNK_MODEL gas_cylinder_model = {
#include "stg09/fixtures/gas_cylinder/gas_cylinder_model.inc"
};

static NJS_CNK_OBJECT gas_cylinder_object[1] = {
#include "stg09/fixtures/gas_cylinder/gas_cylinder_object.inc"
};

#endif
