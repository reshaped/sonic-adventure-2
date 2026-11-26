/* Gantry sign wreck in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_GANTRY_SIGN_WRECK_H
#define SHARED_STREET_FIXTURES_GANTRY_SIGN_WRECK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gantry_sign_wreck_textures[4] = {
#include "shared/street_fixtures/gantry_sign_wreck/gantry_sign_wreck_textures.inc"
};

static NJS_TEXLIST gantry_sign_wreck_texlist = {
#include "shared/street_fixtures/gantry_sign_wreck/gantry_sign_wreck_texlist.inc"
};

static Sint16 gantry_sign_wreck_polygons[123] = {
#include "shared/street_fixtures/gantry_sign_wreck/gantry_sign_wreck_polygons.inc"
};

static Sint32 gantry_sign_wreck_vertices[99] = {
#include "shared/street_fixtures/gantry_sign_wreck/gantry_sign_wreck_vertices.inc"
};

static NJS_CNK_MODEL gantry_sign_wreck_mesh = {
#include "shared/street_fixtures/gantry_sign_wreck/gantry_sign_wreck_mesh.inc"
};

static NJS_CNK_OBJECT gantry_sign_wreck_object = {
#include "shared/street_fixtures/gantry_sign_wreck/gantry_sign_wreck_object.inc"
};

#endif
