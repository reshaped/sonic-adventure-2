#ifndef SHARED_STEPS_RAIL_COLLISION_H
#define SHARED_STEPS_RAIL_COLLISION_H

#include <ninja.h>

/* The two rail hulls sit 19 units to either side of the deck. */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL right_rail_material[1] = {
#include "shared/steps/rail_collision/right_rail_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 right_rail_strips[21] = {
#include "shared/steps/rail_collision/right_rail_strips.inc"
};

static NJS_MESHSET right_rail_faces[1] = {
#include "shared/steps/rail_collision/right_rail_faces.inc"
};

static NJS_POINT3 right_rail_points[8] = {
#include "shared/steps/rail_collision/right_rail_points.inc"
};

static NJS_VECTOR right_rail_normals[8] = {
#include "shared/steps/rail_collision/right_rail_normals.inc"
};

static NJS_MODEL right_rail_hull = {
#include "shared/steps/rail_collision/right_rail_hull.inc"
};

static NJS_OBJECT right_rail_collision = {
#include "shared/steps/rail_collision/right_rail_collision.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL left_rail_material[1] = {
#include "shared/steps/rail_collision/left_rail_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 left_rail_strips[21] = {
#include "shared/steps/rail_collision/left_rail_strips.inc"
};

static NJS_MESHSET left_rail_faces[1] = {
#include "shared/steps/rail_collision/left_rail_faces.inc"
};

static NJS_POINT3 left_rail_points[8] = {
#include "shared/steps/rail_collision/left_rail_points.inc"
};

static NJS_VECTOR left_rail_normals[8] = {
#include "shared/steps/rail_collision/left_rail_normals.inc"
};

static NJS_MODEL left_rail_hull = {
#include "shared/steps/rail_collision/left_rail_hull.inc"
};

static NJS_OBJECT left_rail_collision = {
#include "shared/steps/rail_collision/left_rail_collision.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL deck_material[1] = {
#include "shared/steps/rail_collision/deck_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 deck_strips[46] = {
#include "shared/steps/rail_collision/deck_strips.inc"
};

static NJS_MESHSET deck_faces[1] = {
#include "shared/steps/rail_collision/deck_faces.inc"
};

static NJS_POINT3 deck_points[16] = {
#include "shared/steps/rail_collision/deck_points.inc"
};

static NJS_VECTOR deck_normals[16] = {
#include "shared/steps/rail_collision/deck_normals.inc"
};

static NJS_MODEL deck_hull = {
#include "shared/steps/rail_collision/deck_hull.inc"
};

#endif
