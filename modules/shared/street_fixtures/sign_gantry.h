/* Sign gantry in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_SIGN_GANTRY_H
#define SHARED_STREET_FIXTURES_SIGN_GANTRY_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME sign_gantry_textures[5] = {
#include "shared/street_fixtures/sign_gantry/sign_gantry_textures.inc"
};

static NJS_TEXLIST sign_gantry_texlist = {
#include "shared/street_fixtures/sign_gantry/sign_gantry_texlist.inc"
};

static Sint16 sign_gantry_polygons[557] = {
#include "shared/street_fixtures/sign_gantry/sign_gantry_polygons.inc"
};

static Sint32 sign_gantry_vertices[539] = {
#include "shared/street_fixtures/sign_gantry/sign_gantry_vertices.inc"
};

static NJS_CNK_MODEL sign_gantry_mesh = {
#include "shared/street_fixtures/sign_gantry/sign_gantry_mesh.inc"
};

static NJS_CNK_OBJECT sign_gantry_object = {
#include "shared/street_fixtures/sign_gantry/sign_gantry_object.inc"
};

#endif
