/* Roadside dome in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_ROADSIDE_DOME_H
#define SHARED_STREET_FIXTURES_ROADSIDE_DOME_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME roadside_dome_textures[1] = {
#include "shared/street_fixtures/roadside_dome/roadside_dome_textures.inc"
};

static NJS_TEXLIST roadside_dome_texlist = {
#include "shared/street_fixtures/roadside_dome/roadside_dome_texlist.inc"
};

static Sint16 roadside_dome_polygons[109] = {
#include "shared/street_fixtures/roadside_dome/roadside_dome_polygons.inc"
};

static Sint32 roadside_dome_vertices[95] = {
#include "shared/street_fixtures/roadside_dome/roadside_dome_vertices.inc"
};

static NJS_CNK_MODEL roadside_dome_mesh = {
#include "shared/street_fixtures/roadside_dome/roadside_dome_mesh.inc"
};

static NJS_CNK_OBJECT roadside_dome_object = {
#include "shared/street_fixtures/roadside_dome/roadside_dome_object.inc"
};

#endif
