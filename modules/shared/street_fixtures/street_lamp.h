/* Street lamp in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_STREET_LAMP_H
#define SHARED_STREET_FIXTURES_STREET_LAMP_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_lamp_textures[1] = {
#include "shared/street_fixtures/street_lamp/street_lamp_textures.inc"
};

static NJS_TEXLIST street_lamp_texlist = {
#include "shared/street_fixtures/street_lamp/street_lamp_texlist.inc"
};

static Sint16 street_lamp_polygons[349] = {
#include "shared/street_fixtures/street_lamp/street_lamp_polygons.inc"
};

static Sint32 street_lamp_vertices[171] = {
#include "shared/street_fixtures/street_lamp/street_lamp_vertices.inc"
};

static NJS_CNK_MODEL street_lamp_mesh = {
#include "shared/street_fixtures/street_lamp/street_lamp_mesh.inc"
};

static NJS_CNK_OBJECT street_lamp_object = {
#include "shared/street_fixtures/street_lamp/street_lamp_object.inc"
};

#endif
