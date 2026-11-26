/* Palm planter in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_PALM_PLANTER_H
#define SHARED_STREET_FIXTURES_PALM_PLANTER_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME palm_planter_textures[2] = {
#include "shared/street_fixtures/palm_planter/palm_planter_textures.inc"
};

static NJS_TEXLIST palm_planter_texlist = {
#include "shared/street_fixtures/palm_planter/palm_planter_texlist.inc"
};

static Sint16 palm_planter_polygons[307] = {
#include "shared/street_fixtures/palm_planter/palm_planter_polygons.inc"
};

static Sint32 palm_planter_vertices[255] = {
#include "shared/street_fixtures/palm_planter/palm_planter_vertices.inc"
};

static NJS_CNK_MODEL palm_planter_mesh = {
#include "shared/street_fixtures/palm_planter/palm_planter_mesh.inc"
};

static NJS_CNK_OBJECT palm_planter_object = {
#include "shared/street_fixtures/palm_planter/palm_planter_object.inc"
};

#endif
