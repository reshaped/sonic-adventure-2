#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"

static Sint16 herd_fence_object_1_polygons[105] = {
#include "stg32/scenery/fence/herd_fence_object_1_polygons.inc"
};

static Sint32 herd_fence_object_1_vertices[99] = {
#include "stg32/scenery/fence/herd_fence_object_1_vertices.inc"
};

static NJS_CNK_MODEL herd_fence_object_1_model = {
#include "stg32/scenery/fence/herd_fence_object_1_model.inc"
};

static NJS_CNK_OBJECT herd_fence_object_1 = {
#include "stg32/scenery/fence/herd_fence_object_1.inc"
};

static Sint16 herd_fence_object_polygons[387] = {
#include "stg32/scenery/fence/herd_fence_object_polygons.inc"
};

static Sint32 herd_fence_object_vertices[347] = {
#include "stg32/scenery/fence/herd_fence_object_vertices.inc"
};

static NJS_CNK_MODEL herd_fence_object_model = {
#include "stg32/scenery/fence/herd_fence_object_model.inc"
};

NJS_CNK_OBJECT herd_fence_object = {
#include "stg32/scenery/fence/herd_fence_object.inc"
};
