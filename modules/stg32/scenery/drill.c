#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "stg32/scenery/drill/text.inc"

static NJS_TEXNAME herd_drill_names[4] = {
#include "stg32/scenery/drill/herd_drill_names.inc"
};

NJS_TEXLIST herd_drill_texlist = {
#include "stg32/scenery/drill/herd_drill_texlist.inc"
};

static Sint16 herd_drill_head_polygons[313] = {
#include "stg32/scenery/drill/herd_drill_head_polygons.inc"
};

static Sint32 herd_drill_head_vertices[231] = {
#include "stg32/scenery/drill/herd_drill_head_vertices.inc"
};

static NJS_CNK_MODEL herd_drill_head_model = {
#include "stg32/scenery/drill/herd_drill_head_model.inc"
};

NJS_CNK_OBJECT herd_drill_head[1] = {
#include "stg32/scenery/drill/herd_drill_head.inc"
};

static Sint16 herd_drill_base_polygons[57] = {
#include "stg32/scenery/drill/herd_drill_base_polygons.inc"
};

static Sint32 herd_drill_base_vertices[39] = {
#include "stg32/scenery/drill/herd_drill_base_vertices.inc"
};

static NJS_CNK_MODEL herd_drill_base_model = {
#include "stg32/scenery/drill/herd_drill_base_model.inc"
};

NJS_CNK_OBJECT herd_drill_base[1] = {
#include "stg32/scenery/drill/herd_drill_base.inc"
};
