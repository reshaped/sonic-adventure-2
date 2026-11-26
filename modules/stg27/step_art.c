/* the model and collision hull of STEP in Lost Colony, and its lights, which
 * draw with fog off. no code reads deck_object */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/step_models.h"

static Sint16 running_lights_polygons[399] = {
#include "stg27/step_art/running_lights_polygons.inc"
};

static Sint32 running_lights_vertices[391] = {
#include "stg27/step_art/running_lights_vertices.inc"
};

NJS_CNK_MODEL step_light_model = {
#include "stg27/step_art/step_light_model.inc"
};

static NJS_CNK_OBJECT running_lights_object = {
#include "stg27/step_art/running_lights_object.inc"
};

static Sint16 deck_polygons[707] = {
#include "stg27/step_art/deck_polygons.inc"
};

static Sint32 deck_vertices[555] = {
#include "stg27/step_art/deck_vertices.inc"
};

NJS_CNK_MODEL step_model[1] = {
#include "stg27/step_art/step_model.inc"
};

static NJS_CNK_OBJECT deck_object = {
#include "stg27/step_art/deck_object.inc"
};

#include "shared/steps/rail_collision.h"

NJS_OBJECT step_collision = {
#include "stg27/step_art/step_collision.inc"
};
