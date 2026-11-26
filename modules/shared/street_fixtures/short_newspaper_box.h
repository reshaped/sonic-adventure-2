/* Short newspaper box in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_SHORT_NEWSPAPER_BOX_H
#define SHARED_STREET_FIXTURES_SHORT_NEWSPAPER_BOX_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME short_newspaper_box_textures[1] = {
#include "shared/street_fixtures/short_newspaper_box/short_newspaper_box_textures.inc"
};

static NJS_TEXLIST short_newspaper_box_texlist = {
#include "shared/street_fixtures/short_newspaper_box/short_newspaper_box_texlist.inc"
};

static Sint16 short_newspaper_box_polygons[129] = {
#include "shared/street_fixtures/short_newspaper_box/short_newspaper_box_polygons.inc"
};

static Sint32 short_newspaper_box_vertices[147] = {
#include "shared/street_fixtures/short_newspaper_box/short_newspaper_box_vertices.inc"
};

static NJS_CNK_MODEL short_newspaper_box_mesh = {
#include "shared/street_fixtures/short_newspaper_box/short_newspaper_box_mesh.inc"
};

static NJS_CNK_OBJECT short_newspaper_box_object = {
#include "shared/street_fixtures/short_newspaper_box/short_newspaper_box_object.inc"
};

#endif
