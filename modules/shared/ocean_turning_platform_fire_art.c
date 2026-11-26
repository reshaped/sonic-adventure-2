#include <ninja.h>
#include <sg_xpt.h>

#include "shared/turning_platform.h"

static Sint16 turning_platform_fire_polygons[77] = {
#include "shared/ocean_turning_platform_fire_art/turning_platform_fire_polygons.inc"
};

static Sint32 turning_platform_fire_vertices[55] = {
#include "shared/ocean_turning_platform_fire_art/turning_platform_fire_vertices.inc"
};

NJS_CNK_MODEL turning_platform_fire_model[1] = {
#include "shared/ocean_turning_platform_fire_art/turning_platform_fire_model.inc"
};

/* R13 also stores the object containing the flame model. */
static NJS_CNK_OBJECT turning_platform_fire_object = {
#include "shared/ocean_turning_platform_fire_art/turning_platform_fire_object.inc"
};
