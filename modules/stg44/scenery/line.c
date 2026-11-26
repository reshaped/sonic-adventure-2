#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

static Sint16 mad_space_line_object_polygons[37] = {
#include "stg44/scenery/line/mad_space_line_object_polygons.inc"
};

static Sint32 mad_space_line_object_vertices[35] = {
#include "stg44/scenery/line/mad_space_line_object_vertices.inc"
};

static NJS_CNK_MODEL mad_space_line_object_model = {
#include "stg44/scenery/line/mad_space_line_object_model.inc"
};

NJS_CNK_OBJECT mad_space_line_object = {
#include "stg44/scenery/line/mad_space_line_object.inc"
};
