/* Three frames of the player's wave and three gunfire flashes. Their
 * vertices carry colors, and the untextured strips blend by alpha. */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/powder.h"

static Sint16 wave_a_polygons[33] = {
#include "effect/powder_models/wave_a_polygons.inc"
};

static Sint32 wave_a_vertices[51] = {
#include "effect/powder_models/wave_a_vertices.inc"
};

static NJS_CNK_MODEL wave_a_model = {
#include "effect/powder_models/wave_a_model.inc"
};

NJS_CNK_OBJECT wave_object_a = {
#include "effect/powder_models/wave_object_a.inc"
};

static Sint16 wave_b_polygons[33] = {
#include "effect/powder_models/wave_b_polygons.inc"
};

static Sint32 wave_b_vertices[51] = {
#include "effect/powder_models/wave_b_vertices.inc"
};

static NJS_CNK_MODEL wave_b_model = {
#include "effect/powder_models/wave_b_model.inc"
};

NJS_CNK_OBJECT wave_object_b = {
#include "effect/powder_models/wave_object_b.inc"
};

static Sint16 wave_c_polygons[33] = {
#include "effect/powder_models/wave_c_polygons.inc"
};

static Sint32 wave_c_vertices[51] = {
#include "effect/powder_models/wave_c_vertices.inc"
};

static NJS_CNK_MODEL wave_c_model = {
#include "effect/powder_models/wave_c_model.inc"
};

NJS_CNK_OBJECT wave_object_c = {
#include "effect/powder_models/wave_object_c.inc"
};

static Sint16 gunfire_a_polygons[73] = {
#include "effect/powder_models/gunfire_a_polygons.inc"
};

static Sint32 gunfire_a_vertices[131] = {
#include "effect/powder_models/gunfire_a_vertices.inc"
};

static NJS_CNK_MODEL gunfire_a_model = {
#include "effect/powder_models/gunfire_a_model.inc"
};

NJS_CNK_OBJECT gunfire_object_a = {
#include "effect/powder_models/gunfire_object_a.inc"
};

static Sint16 gunfire_b_polygons[69] = {
#include "effect/powder_models/gunfire_b_polygons.inc"
};

static Sint32 gunfire_b_vertices[123] = {
#include "effect/powder_models/gunfire_b_vertices.inc"
};

static NJS_CNK_MODEL gunfire_b_model = {
#include "effect/powder_models/gunfire_b_model.inc"
};

NJS_CNK_OBJECT gunfire_object_b[1] = {
#include "effect/powder_models/gunfire_object_b.inc"
};

static Sint16 gunfire_c_polygons[73] = {
#include "effect/powder_models/gunfire_c_polygons.inc"
};

static Sint32 gunfire_c_vertices[131] = {
#include "effect/powder_models/gunfire_c_vertices.inc"
};

static NJS_CNK_MODEL gunfire_c_model = {
#include "effect/powder_models/gunfire_c_model.inc"
};

NJS_CNK_OBJECT gunfire_object_c = {
#include "effect/powder_models/gunfire_object_c.inc"
};
