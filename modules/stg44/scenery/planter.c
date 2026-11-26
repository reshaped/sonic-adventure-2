#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

static Sint16 planter_object_1_polygons[181] = {
#include "stg44/scenery/planter/planter_object_1_polygons.inc"
};

static Sint32 planter_object_1_vertices[135] = {
#include "stg44/scenery/planter/planter_object_1_vertices.inc"
};

static NJS_CNK_MODEL planter_object_1_model = {
#include "stg44/scenery/planter/planter_object_1_model.inc"
};

static NJS_CNK_OBJECT planter_object_1 = {
#include "stg44/scenery/planter/planter_object_1.inc"
};

NJS_CNK_OBJECT planter_object = {
#include "stg44/scenery/planter/planter_object.inc"
};
