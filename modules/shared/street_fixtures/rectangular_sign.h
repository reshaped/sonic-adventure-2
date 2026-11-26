/* Rectangular sign in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_RECTANGULAR_SIGN_H
#define SHARED_STREET_FIXTURES_RECTANGULAR_SIGN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME rectangular_sign_textures[3] = {
#include "shared/street_fixtures/rectangular_sign/rectangular_sign_textures.inc"
};

static NJS_TEXLIST rectangular_sign_texlist = {
#include "shared/street_fixtures/rectangular_sign/rectangular_sign_texlist.inc"
};

static Sint16 rectangular_sign_polygons[219] = {
#include "shared/street_fixtures/rectangular_sign/rectangular_sign_polygons.inc"
};

static Sint32 rectangular_sign_vertices[207] = {
#include "shared/street_fixtures/rectangular_sign/rectangular_sign_vertices.inc"
};

static NJS_CNK_MODEL rectangular_sign_mesh = {
#include "shared/street_fixtures/rectangular_sign/rectangular_sign_mesh.inc"
};

static NJS_CNK_OBJECT rectangular_sign_object = {
#include "shared/street_fixtures/rectangular_sign/rectangular_sign_object.inc"
};

#endif
