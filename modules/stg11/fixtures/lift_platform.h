#ifndef STG11_FIXTURES_LIFT_PLATFORM_H
#define STG11_FIXTURES_LIFT_PLATFORM_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 lift_platform_polygons[401] = {
#include "stg11/fixtures/lift_platform/lift_platform_polygons.inc"
};

static Sint32 lift_platform_vertices[395] = {
#include "stg11/fixtures/lift_platform/lift_platform_vertices.inc"
};

static NJS_CNK_MODEL lift_platform_model = {
#include "stg11/fixtures/lift_platform/lift_platform_model.inc"
};

static NJS_CNK_OBJECT lift_platform_object = {
#include "stg11/fixtures/lift_platform/lift_platform_object.inc"
};

#endif
