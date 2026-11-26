#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "stg32/scenery/pole/text.inc"

static NJS_TEXNAME herd_pole_names[6] = {
#include "stg32/scenery/pole/herd_pole_names.inc"
};

NJS_TEXLIST herd_pole_texlist = {
#include "stg32/scenery/pole/herd_pole_texlist.inc"
};

static Sint16 herd_pole_object_2_polygons[51] = {
#include "stg32/scenery/pole/herd_pole_object_2_polygons.inc"
};

static Sint32 herd_pole_object_2_vertices[51] = {
#include "stg32/scenery/pole/herd_pole_object_2_vertices.inc"
};

static NJS_CNK_MODEL herd_pole_object_2_model = {
#include "stg32/scenery/pole/herd_pole_object_2_model.inc"
};

static NJS_CNK_OBJECT herd_pole_object_2 = {
#include "stg32/scenery/pole/herd_pole_object_2.inc"
};

static Sint16 herd_pole_object_1_polygons[393] = {
#include "stg32/scenery/pole/herd_pole_object_1_polygons.inc"
};

static Sint32 herd_pole_object_1_vertices[347] = {
#include "stg32/scenery/pole/herd_pole_object_1_vertices.inc"
};

static NJS_CNK_MODEL herd_pole_object_1_model = {
#include "stg32/scenery/pole/herd_pole_object_1_model.inc"
};

static NJS_CNK_OBJECT herd_pole_object_1 = {
#include "stg32/scenery/pole/herd_pole_object_1.inc"
};

static Sint16 herd_pole_object_polygons[51] = {
#include "stg32/scenery/pole/herd_pole_object_polygons.inc"
};

static Sint32 herd_pole_object_vertices[51] = {
#include "stg32/scenery/pole/herd_pole_object_vertices.inc"
};

static NJS_CNK_MODEL herd_pole_object_model = {
#include "stg32/scenery/pole/herd_pole_object_model.inc"
};

NJS_CNK_OBJECT herd_pole_object[1] = {
#include "stg32/scenery/pole/herd_pole_object.inc"
};
