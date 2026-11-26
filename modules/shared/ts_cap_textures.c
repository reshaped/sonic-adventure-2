#include <ninja.h>
#include <sg_xpt.h>

#include "shared/time_switch.h"
#include "shared/time_switch_art.h"

#include "shared/ts_cap_textures/text.inc"

static NJS_TEXNAME textures[4] = {
#include "shared/ts_cap_textures/textures.inc"
};
NJS_TEXLIST time_switch_cap_texlist = {
#include "shared/ts_cap_textures/time_switch_cap_texlist.inc"
};

static Sint16 time_switch_cap_polygons[363] = {
#include "shared/ts_cap_textures/time_switch_cap_polygons.inc"
};

struct time_switch_vertices time_switch_cap_vertices = {
#include "shared/ts_cap_textures/time_switch_cap_vertices.inc"
};

static NJS_CNK_MODEL time_switch_cap_model = {
#include "shared/ts_cap_textures/time_switch_cap_model.inc"
};

NJS_CNK_OBJECT time_switch_cap = {
#include "shared/ts_cap_textures/time_switch_cap.inc"
};

static Sint16 time_switch_fast_ring_polygons[25] = {
#include "shared/ts_cap_textures/time_switch_fast_ring_polygons.inc"
};

static Sint32 time_switch_fast_ring_vertices[19] = {
#include "shared/ts_cap_textures/time_switch_fast_ring_vertices.inc"
};

static NJS_CNK_MODEL time_switch_fast_ring_model = {
#include "shared/ts_cap_textures/time_switch_fast_ring_model.inc"
};

NJS_CNK_OBJECT time_switch_fast_ring = {
#include "shared/ts_cap_textures/time_switch_fast_ring.inc"
};

static Sint16 time_switch_ring_polygons[25] = {
#include "shared/ts_cap_textures/time_switch_ring_polygons.inc"
};

static Sint32 time_switch_ring_vertices[19] = {
#include "shared/ts_cap_textures/time_switch_ring_vertices.inc"
};

static NJS_CNK_MODEL time_switch_ring_model = {
#include "shared/ts_cap_textures/time_switch_ring_model.inc"
};

NJS_CNK_OBJECT time_switch_ring = {
#include "shared/ts_cap_textures/time_switch_ring.inc"
};

static Sint16 time_switch_slow_ring_polygons[25] = {
#include "shared/ts_cap_textures/time_switch_slow_ring_polygons.inc"
};

static Sint32 time_switch_slow_ring_vertices[19] = {
#include "shared/ts_cap_textures/time_switch_slow_ring_vertices.inc"
};

static NJS_CNK_MODEL time_switch_slow_ring_model = {
#include "shared/ts_cap_textures/time_switch_slow_ring_model.inc"
};

NJS_CNK_OBJECT time_switch_slow_ring = {
#include "shared/ts_cap_textures/time_switch_slow_ring.inc"
};

static NJS_CNK_OBJECT ring_assembly = {
#include "shared/ts_cap_textures/ring_assembly.inc"
};
