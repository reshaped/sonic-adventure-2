/* Stacked road signs in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_STACKED_ROAD_SIGNS_H
#define SHARED_STREET_FIXTURES_STACKED_ROAD_SIGNS_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME stacked_road_signs_textures[4] = {
#include "shared/street_fixtures/stacked_road_signs/stacked_road_signs_textures.inc"
};

static NJS_TEXLIST stacked_road_signs_texlist = {
#include "shared/street_fixtures/stacked_road_signs/stacked_road_signs_texlist.inc"
};

static Sint16 stacked_road_signs_polygons[137] = {
#include "shared/street_fixtures/stacked_road_signs/stacked_road_signs_polygons.inc"
};

static Sint32 stacked_road_signs_vertices[139] = {
#include "shared/street_fixtures/stacked_road_signs/stacked_road_signs_vertices.inc"
};

static NJS_CNK_MODEL stacked_road_signs_mesh = {
#include "shared/street_fixtures/stacked_road_signs/stacked_road_signs_mesh.inc"
};

static NJS_CNK_OBJECT stacked_road_signs_object = {
#include "shared/street_fixtures/stacked_road_signs/stacked_road_signs_object.inc"
};

#endif
