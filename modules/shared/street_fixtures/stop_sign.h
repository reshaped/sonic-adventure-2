/* Stop sign in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_STOP_SIGN_H
#define SHARED_STREET_FIXTURES_STOP_SIGN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME stop_sign_textures[3] = {
#include "shared/street_fixtures/stop_sign/stop_sign_textures.inc"
};

static NJS_TEXLIST stop_sign_texlist = {
#include "shared/street_fixtures/stop_sign/stop_sign_texlist.inc"
};

static Sint16 stop_sign_polygons[119] = {
#include "shared/street_fixtures/stop_sign/stop_sign_polygons.inc"
};

static Sint32 stop_sign_vertices[107] = {
#include "shared/street_fixtures/stop_sign/stop_sign_vertices.inc"
};

static NJS_CNK_MODEL stop_sign_mesh = {
#include "shared/street_fixtures/stop_sign/stop_sign_mesh.inc"
};

static NJS_CNK_OBJECT stop_sign_object = {
#include "shared/street_fixtures/stop_sign/stop_sign_object.inc"
};

#endif
