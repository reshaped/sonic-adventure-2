/* the model and collision hull of STEP, the narrower of engine_step.h's two
 * moving platforms. no code reads deck_object */
#include <ninja.h>

#include "shared/step_models.h"

#include "shared/steps/deck_mesh.h"

NJS_CNK_MODEL step_model[1] = {
#include "stg24/step_art/step_model.inc"
};

static NJS_CNK_OBJECT deck_object = {
#include "stg24/step_art/deck_object.inc"
};

#include "shared/steps/rail_collision.h"

NJS_OBJECT step_collision = {
#include "stg24/step_art/step_collision.inc"
};
