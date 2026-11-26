#include <ninja.h>
#include <sg_xpt.h>

#include "shared/time_switch.h"
#include "shared/time_switch_art.h"

#include "shared/ts_base_textures/text.inc"

static NJS_TEXNAME textures[3] = {
#include "shared/ts_base_textures/textures.inc"
};
NJS_TEXLIST time_switch_base_texlist = {
#include "shared/ts_base_textures/time_switch_base_texlist.inc"
};

static Sint16 time_switch_base_polygons[539] = {
#include "shared/ts_base_textures/time_switch_base_polygons.inc"
};

static Sint32 time_switch_base_vertices[247] = {
#include "shared/ts_base_textures/time_switch_base_vertices.inc"
};

NJS_CNK_MODEL time_switch_base = {
#include "shared/ts_base_textures/time_switch_base.inc"
};

static NJS_CNK_OBJECT pedestal = {
#include "shared/ts_base_textures/pedestal.inc"
};
