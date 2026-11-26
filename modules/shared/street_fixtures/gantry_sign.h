/* Gantry sign in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_GANTRY_SIGN_H
#define SHARED_STREET_FIXTURES_GANTRY_SIGN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gantry_sign_textures[4] = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_textures.inc"
};

static NJS_TEXLIST gantry_sign_texlist = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_texlist.inc"
};

static Sint16 gantry_sign_object_node_1_polygons[25] = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_object_node_1_polygons.inc"
};

static Sint32 gantry_sign_object_node_1_vertices[19] = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_object_node_1_vertices.inc"
};

static NJS_CNK_MODEL gantry_sign_object_node_1_mesh = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_object_node_1_mesh.inc"
};

static NJS_CNK_OBJECT gantry_sign_object_node_1 = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_object_node_1.inc"
};

static Sint16 gantry_sign_polygons[87] = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_polygons.inc"
};

static Sint32 gantry_sign_vertices[67] = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_vertices.inc"
};

static NJS_CNK_MODEL gantry_sign_mesh = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_mesh.inc"
};

static NJS_CNK_OBJECT gantry_sign_object = {
#include "shared/street_fixtures/gantry_sign/gantry_sign_object.inc"
};

#endif
