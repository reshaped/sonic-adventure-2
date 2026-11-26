/* Roadside signs in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_ROADSIDE_SIGNS_H
#define SHARED_STREET_FIXTURES_ROADSIDE_SIGNS_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME roadside_signs_textures[4] = {
#include "shared/street_fixtures/roadside_signs/roadside_signs_textures.inc"
};

static NJS_TEXLIST roadside_signs_texlist = {
#include "shared/street_fixtures/roadside_signs/roadside_signs_texlist.inc"
};

static Sint16 roadside_signs_polygons[611] = {
#include "shared/street_fixtures/roadside_signs/roadside_signs_polygons.inc"
};

static Sint32 roadside_signs_vertices[615] = {
#include "shared/street_fixtures/roadside_signs/roadside_signs_vertices.inc"
};

static NJS_CNK_MODEL roadside_signs_mesh = {
#include "shared/street_fixtures/roadside_signs/roadside_signs_mesh.inc"
};

static NJS_CNK_OBJECT roadside_signs_object = {
#include "shared/street_fixtures/roadside_signs/roadside_signs_object.inc"
};

#endif
