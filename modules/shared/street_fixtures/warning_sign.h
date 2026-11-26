/* Warning sign in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_WARNING_SIGN_H
#define SHARED_STREET_FIXTURES_WARNING_SIGN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME warning_sign_textures[4] = {
#include "shared/street_fixtures/warning_sign/warning_sign_textures.inc"
};

static NJS_TEXLIST warning_sign_texlist = {
#include "shared/street_fixtures/warning_sign/warning_sign_texlist.inc"
};

static Sint16 warning_sign_object_node_2_polygons[25] = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_2_polygons.inc"
};

static Sint32 warning_sign_object_node_2_vertices[19] = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_2_vertices.inc"
};

static NJS_CNK_MODEL warning_sign_object_node_2_mesh = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_2_mesh.inc"
};

static NJS_CNK_OBJECT warning_sign_object_node_2 = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_2.inc"
};

static Sint16 warning_sign_object_node_1_polygons[25] = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_1_polygons.inc"
};

static Sint32 warning_sign_object_node_1_vertices[19] = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_1_vertices.inc"
};

static NJS_CNK_MODEL warning_sign_object_node_1_mesh = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_1_mesh.inc"
};

static NJS_CNK_OBJECT warning_sign_object_node_1 = {
#include "shared/street_fixtures/warning_sign/warning_sign_object_node_1.inc"
};

#include "shared/road_furniture/warning_sign_body.h"

static NJS_CNK_OBJECT warning_sign_object = {
#include "shared/street_fixtures/warning_sign/warning_sign_object.inc"
};

#endif
