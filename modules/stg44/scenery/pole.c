#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

static Sint16 mad_space_pole_object_1_polygons[91] = {
#include "stg44/scenery/pole/mad_space_pole_object_1_polygons.inc"
};

static Sint32 mad_space_pole_object_1_vertices[51] = {
#include "stg44/scenery/pole/mad_space_pole_object_1_vertices.inc"
};

static NJS_CNK_MODEL mad_space_pole_object_1_model = {
#include "stg44/scenery/pole/mad_space_pole_object_1_model.inc"
};

static NJS_CNK_OBJECT mad_space_pole_object_1 = {
#include "stg44/scenery/pole/mad_space_pole_object_1.inc"
};

static Sint16 mad_space_pole_object_polygons[371] = {
#include "stg44/scenery/pole/mad_space_pole_object_polygons.inc"
};

static Sint32 mad_space_pole_object_vertices[311] = {
#include "stg44/scenery/pole/mad_space_pole_object_vertices.inc"
};

static NJS_CNK_MODEL mad_space_pole_object_model = {
#include "stg44/scenery/pole/mad_space_pole_object_model.inc"
};

NJS_CNK_OBJECT mad_space_pole_object = {
#include "stg44/scenery/pole/mad_space_pole_object.inc"
};
