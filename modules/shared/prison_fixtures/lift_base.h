#ifndef SHARED_PRISON_FIXTURES_LIFT_BASE_H
#define SHARED_PRISON_FIXTURES_LIFT_BASE_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 lift_polygons[135] = {
#include "shared/prison_fixtures/lift_base/lift_polygons.inc"
};

static Sint32 lift_vertices[115] = {
#include "shared/prison_fixtures/lift_base/lift_vertices.inc"
};

static NJS_CNK_MODEL lift_model = {
#include "shared/prison_fixtures/lift_base/lift_model.inc"
};

static NJS_CNK_OBJECT lift_object = {
#include "shared/prison_fixtures/lift_base/lift_object.inc"
};

static NJS_MATERIAL lift_solid_materials = {
#include "shared/prison_fixtures/lift_base/lift_solid_materials.inc"
};

static Sint16 lift_solid_strips[19] = {
#include "shared/prison_fixtures/lift_base/lift_solid_strips.inc"
};

static NJS_MESHSET lift_solid_faces = {
#include "shared/prison_fixtures/lift_base/lift_solid_faces.inc"
};

static NJS_POINT3 lift_solid_points[8] = {
#include "shared/prison_fixtures/lift_base/lift_solid_points.inc"
};

static NJS_VECTOR lift_solid_normals[8] = {
#include "shared/prison_fixtures/lift_base/lift_solid_normals.inc"
};

static NJS_MODEL lift_solid_model = {
#include "shared/prison_fixtures/lift_base/lift_solid_model.inc"
};

static NJS_OBJECT lift_collision = {
#include "shared/prison_fixtures/lift_base/lift_collision.inc"
};

#endif
