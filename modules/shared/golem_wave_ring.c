/* the shock wave's ring. no code reads ring_object, the node over its model */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/golem_material/names.h"
#include "shared/golem_wave.h"

const char golem_wave_texture[12] = "sonicwave01";

static NJS_TEXNAME ring_texture[1] = {
#include "shared/golem_wave_ring/ring_texture.inc"
};

NJS_TEXLIST golem_wave_ring_texlist = {
#include "shared/golem_wave_ring/golem_wave_ring_texlist.inc"
};

static Sint16 ring_polygons[205] = {
#include "shared/golem_wave_ring/ring_polygons.inc"
};

static Sint32 ring_vertices[189] = {
#include "shared/golem_wave_ring/ring_vertices.inc"
};

NJS_CNK_MODEL golem_wave_ring_model = {
#include "shared/golem_wave_ring/golem_wave_ring_model.inc"
};

static NJS_CNK_OBJECT ring_object = {
#include "shared/golem_wave_ring/ring_object.inc"
};
