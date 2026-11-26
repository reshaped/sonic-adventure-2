#ifndef STG40_CYLINDER_ART_LID_H
#define STG40_CYLINDER_ART_LID_H

#include <ninja.h>
#include <sg_xpt.h>

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL lid_material = {
#include "stg40/cylinder_art/lid/lid_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 lid_indices[54] = {
#include "stg40/cylinder_art/lid/lid_indices.inc"
};

static NJS_MESHSET lid_faces = {
#include "stg40/cylinder_art/lid/lid_faces.inc"
};

static NJS_POINT3 lid_points[20] = {
#include "stg40/cylinder_art/lid/lid_points.inc"
};

static NJS_VECTOR lid_normals[20] = {
#include "stg40/cylinder_art/lid/lid_normals.inc"
};

static NJS_MODEL lid_model = {
#include "stg40/cylinder_art/lid/lid_model.inc"
};

static NJS_OBJECT gravity_cylinder_1_lid = {
#include "stg40/cylinder_art/lid/gravity_cylinder_1_lid.inc"
};

#endif
