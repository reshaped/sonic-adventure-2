/* the model and collision hull of STEPB, the wider of engine_step.h's two
 * moving platforms. no code reads deck_object */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/step_models.h"

#include "shared/steps/wide_deck_mesh.h"

NJS_CNK_MODEL step_b_model[1] = {
#include "stg24/step_b_art/step_b_model.inc"
};

static NJS_CNK_OBJECT deck_object = {
#include "stg24/step_b_art/deck_object.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL deck_material[1] = {
#include "stg24/step_b_art/deck_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 deck_strips[97] = {
#include "stg24/step_b_art/deck_strips.inc"
};

static NJS_MESHSET deck_faces[1] = {
#include "stg24/step_b_art/deck_faces.inc"
};

static NJS_POINT3 deck_points[40] = {
#include "stg24/step_b_art/deck_points.inc"
};

static NJS_VECTOR deck_normals[40] = {
#include "stg24/step_b_art/deck_normals.inc"
};

static NJS_MODEL deck_hull = {
#include "stg24/step_b_art/deck_hull.inc"
};

NJS_OBJECT step_b_collision = {
#include "stg24/step_b_art/step_b_collision.inc"
};
