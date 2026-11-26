/* Short newspaper box wreck in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_SHORT_NEWSPAPER_BOX_WRECK_H
#define SHARED_STREET_FIXTURES_SHORT_NEWSPAPER_BOX_WRECK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME short_newspaper_box_wreck_textures[1] = {
#include "shared/street_fixtures/short_newspaper_box_wreck/short_newspaper_box_wreck_textures.inc"
};

static NJS_TEXLIST short_newspaper_box_wreck_texlist = {
#include "shared/street_fixtures/short_newspaper_box_wreck/short_newspaper_box_wreck_texlist.inc"
};

static Sint16 short_newspaper_box_wreck_polygons[129] = {
#include "shared/street_fixtures/short_newspaper_box_wreck/short_newspaper_box_wreck_polygons.inc"
};

static Sint32 short_newspaper_box_wreck_vertices[131] = {
#include "shared/street_fixtures/short_newspaper_box_wreck/short_newspaper_box_wreck_vertices.inc"
};

static NJS_CNK_MODEL short_newspaper_box_wreck_mesh = {
#include "shared/street_fixtures/short_newspaper_box_wreck/short_newspaper_box_wreck_mesh.inc"
};

static NJS_CNK_OBJECT short_newspaper_box_wreck_object = {
#include "shared/street_fixtures/short_newspaper_box_wreck/short_newspaper_box_wreck_object.inc"
};

#endif
