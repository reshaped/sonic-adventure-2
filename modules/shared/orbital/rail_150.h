#ifndef SHARED_ORBITAL_RAIL_150_H
#define SHARED_ORBITAL_RAIL_150_H

#include <ninja.h>
#include <sg_xpt.h>

/* 150-unit grind rail, its scrolling crown and the light beside the rail. */

static Sint16 grind_rail_1_light_polygons[89] = {
#include "shared/orbital/rail_150/grind_rail_1_light_polygons.inc"
};

static Sint32 grind_rail_1_light_vertices[99] = {
#include "shared/orbital/rail_150/grind_rail_1_light_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_1_light_mesh = {
#include "shared/orbital/rail_150/grind_rail_1_light_mesh.inc"
};

static NJS_CNK_OBJECT grind_rail_1_light = {
#include "shared/orbital/rail_150/grind_rail_1_light.inc"
};

static Sint16 grind_rail_1_scroll_polygons[63] = {
#include "shared/orbital/rail_150/grind_rail_1_scroll_polygons.inc"
};

static Sint32 grind_rail_1_scroll_vertices[51] = {
#include "shared/orbital/rail_150/grind_rail_1_scroll_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_1_scroll_mesh = {
#include "shared/orbital/rail_150/grind_rail_1_scroll_mesh.inc"
};

static NJS_CNK_OBJECT grind_rail_1_scroll = {
#include "shared/orbital/rail_150/grind_rail_1_scroll.inc"
};

static Sint16 grind_rail_1_polygons[649] = {
#include "shared/orbital/rail_150/grind_rail_1_polygons.inc"
};

static Sint32 grind_rail_1_vertices[547] = {
#include "shared/orbital/rail_150/grind_rail_1_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_1_mesh = {
#include "shared/orbital/rail_150/grind_rail_1_mesh.inc"
};

#endif
