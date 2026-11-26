#ifndef SHARED_ORBITAL_RAIL_200_H
#define SHARED_ORBITAL_RAIL_200_H

#include <ninja.h>
#include <sg_xpt.h>

/* 200-unit grind rail, its scrolling crown and the light beside the rail. */

static Sint16 grind_rail_2_light_polygons[127] = {
#include "shared/orbital/rail_200/grind_rail_2_light_polygons.inc"
};

static Sint32 grind_rail_2_light_vertices[147] = {
#include "shared/orbital/rail_200/grind_rail_2_light_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_2_light_mesh = {
#include "shared/orbital/rail_200/grind_rail_2_light_mesh.inc"
};

static NJS_CNK_OBJECT grind_rail_2_light = {
#include "shared/orbital/rail_200/grind_rail_2_light.inc"
};

static Sint16 grind_rail_2_scroll_polygons[63] = {
#include "shared/orbital/rail_200/grind_rail_2_scroll_polygons.inc"
};

static Sint32 grind_rail_2_scroll_vertices[51] = {
#include "shared/orbital/rail_200/grind_rail_2_scroll_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_2_scroll_mesh = {
#include "shared/orbital/rail_200/grind_rail_2_scroll_mesh.inc"
};

static NJS_CNK_OBJECT grind_rail_2_scroll = {
#include "shared/orbital/rail_200/grind_rail_2_scroll.inc"
};

static Sint16 grind_rail_2_polygons[903] = {
#include "shared/orbital/rail_200/grind_rail_2_polygons.inc"
};

static Sint32 grind_rail_2_vertices[779] = {
#include "shared/orbital/rail_200/grind_rail_2_vertices.inc"
};

static NJS_CNK_MODEL grind_rail_2_mesh = {
#include "shared/orbital/rail_200/grind_rail_2_mesh.inc"
};

#endif
