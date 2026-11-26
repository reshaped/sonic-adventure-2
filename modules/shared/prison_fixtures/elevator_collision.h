#ifndef SHARED_PRISON_FIXTURES_ELEVATOR_COLLISION_H
#define SHARED_PRISON_FIXTURES_ELEVATOR_COLLISION_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL elevator_solid_materials = {
#include "shared/prison_fixtures/elevator_collision/elevator_solid_materials.inc"
};

static Sint16 elevator_solid_strips[37] = {
#include "shared/prison_fixtures/elevator_collision/elevator_solid_strips.inc"
};

static NJS_MESHSET elevator_solid_faces = {
#include "shared/prison_fixtures/elevator_collision/elevator_solid_faces.inc"
};

static NJS_POINT3 elevator_solid_points[16] = {
#include "shared/prison_fixtures/elevator_collision/elevator_solid_points.inc"
};

static NJS_VECTOR elevator_solid_normals[16] = {
#include "shared/prison_fixtures/elevator_collision/elevator_solid_normals.inc"
};

static NJS_MODEL elevator_solid_model = {
#include "shared/prison_fixtures/elevator_collision/elevator_solid_model.inc"
};

static NJS_OBJECT elevator_collision = {
#include "shared/prison_fixtures/elevator_collision/elevator_collision.inc"
};

#endif
