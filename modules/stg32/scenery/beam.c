#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "stg32/scenery/beam/text.inc"

static NJS_TEXNAME herd_beam_names[1] = {
#include "stg32/scenery/beam/herd_beam_names.inc"
};

NJS_TEXLIST herd_beam_texlist = {
#include "stg32/scenery/beam/herd_beam_texlist.inc"
};

static Sint16 herd_beam_object_polygons[31] = {
#include "stg32/scenery/beam/herd_beam_object_polygons.inc"
};

static Sint32 herd_beam_object_vertices[27] = {
#include "stg32/scenery/beam/herd_beam_object_vertices.inc"
};

static NJS_CNK_MODEL herd_beam_object_model = {
#include "stg32/scenery/beam/herd_beam_object_model.inc"
};

NJS_CNK_OBJECT herd_beam_object[1] = {
#include "stg32/scenery/beam/herd_beam_object.inc"
};
