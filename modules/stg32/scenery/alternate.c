#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"

static Sint16 meteor_platform_alt_object_polygons[917] = {
#include "stg32/scenery/alternate/meteor_platform_alt_object_polygons.inc"
};

static Sint32 meteor_platform_alt_object_vertices[595] = {
#include "stg32/scenery/alternate/meteor_platform_alt_object_vertices.inc"
};

static NJS_CNK_MODEL meteor_platform_alt_object_model = {
#include "stg32/scenery/alternate/meteor_platform_alt_object_model.inc"
};

NJS_CNK_OBJECT meteor_platform_alt_object[1] = {
#include "stg32/scenery/alternate/meteor_platform_alt_object.inc"
};
