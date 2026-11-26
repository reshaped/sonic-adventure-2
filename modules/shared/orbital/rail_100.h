#ifndef SHARED_ORBITAL_RAIL_100_H
#define SHARED_ORBITAL_RAIL_100_H

#include <ninja.h>
#include <sg_xpt.h>

/* 100-unit grind rail, its scrolling crown and the light beside the rail. */

static Sint16 grind_rail_0_light_polygons[89] = {
#include "shared/orbital/rail_100/grind_rail_0_light_polygons.inc"
};

static Sint32 grind_rail_0_light_vertices[99] = {
#include "shared/orbital/rail_100/grind_rail_0_light_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_0_light_mesh = {
#include "shared/orbital/rail_100/grind_rail_0_light_mesh.inc"
};

static NJS_CNK_OBJECT grind_rail_0_light = {
#include "shared/orbital/rail_100/grind_rail_0_light.inc"
};

static Sint16 grind_rail_0_scroll_polygons[63] = {
#include "shared/orbital/rail_100/grind_rail_0_scroll_polygons.inc"
};

static Sint32 grind_rail_0_scroll_vertices[51] = {
#include "shared/orbital/rail_100/grind_rail_0_scroll_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_0_scroll_mesh = {
#include "shared/orbital/rail_100/grind_rail_0_scroll_mesh.inc"
};

static NJS_CNK_OBJECT grind_rail_0_scroll = {
#include "shared/orbital/rail_100/grind_rail_0_scroll.inc"
};

static Sint16 grind_rail_0_polygons[649] = {
#include "shared/orbital/rail_100/grind_rail_0_polygons.inc"
};

static Sint32 grind_rail_0_vertices[547] = {
#include "shared/orbital/rail_100/grind_rail_0_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_0_mesh = {
#include "shared/orbital/rail_100/grind_rail_0_mesh.inc"
};

#endif
