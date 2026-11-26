/* Both LASTSTEP models use this stationary floor while their panels move. */
#ifndef SHARED_STEPS_LAST_STEP_COLLISION_H
#define SHARED_STEPS_LAST_STEP_COLLISION_H
#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL deck_material[1] = {
#include "shared/steps/last_step_collision/deck_material.inc"
};

static Sint16 deck_strips[49] = {
#include "shared/steps/last_step_collision/deck_strips.inc"
};

static NJS_MESHSET deck_faces[1] = {
#include "shared/steps/last_step_collision/deck_faces.inc"
};

static NJS_POINT3 deck_points[16] = {
#include "shared/steps/last_step_collision/deck_points.inc"
};

static NJS_VECTOR deck_normals[16] = {
#include "shared/steps/last_step_collision/deck_normals.inc"
};

static NJS_MODEL deck_hull = {
#include "shared/steps/last_step_collision/deck_hull.inc"
};

#endif
