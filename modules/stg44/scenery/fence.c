#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

static Sint16 mad_space_fence_object_1_polygons[39] = {
#include "stg44/scenery/fence/mad_space_fence_object_1_polygons.inc"
};

static Sint32 mad_space_fence_object_1_vertices[35] = {
#include "stg44/scenery/fence/mad_space_fence_object_1_vertices.inc"
};

static NJS_CNK_MODEL mad_space_fence_object_1_model = {
#include "stg44/scenery/fence/mad_space_fence_object_1_model.inc"
};

static NJS_CNK_OBJECT mad_space_fence_object_1 = {
#include "stg44/scenery/fence/mad_space_fence_object_1.inc"
};

static Sint16 mad_space_fence_object_polygons[353] = {
#include "stg44/scenery/fence/mad_space_fence_object_polygons.inc"
};

static Sint32 mad_space_fence_object_vertices[367] = {
#include "stg44/scenery/fence/mad_space_fence_object_vertices.inc"
};

static NJS_CNK_MODEL mad_space_fence_object_model = {
#include "stg44/scenery/fence/mad_space_fence_object_model.inc"
};

NJS_CNK_OBJECT mad_space_fence_object = {
#include "stg44/scenery/fence/mad_space_fence_object.inc"
};
