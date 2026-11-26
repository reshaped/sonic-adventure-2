/* Tall newspaper box wreck in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_TALL_NEWSPAPER_BOX_WRECK_H
#define SHARED_STREET_FIXTURES_TALL_NEWSPAPER_BOX_WRECK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME tall_newspaper_box_wreck_textures[1] = {
#include "shared/street_fixtures/tall_newspaper_box_wreck/tall_newspaper_box_wreck_textures.inc"
};

static NJS_TEXLIST tall_newspaper_box_wreck_texlist = {
#include "shared/street_fixtures/tall_newspaper_box_wreck/tall_newspaper_box_wreck_texlist.inc"
};

static Sint16 tall_newspaper_box_wreck_polygons[201] = {
#include "shared/street_fixtures/tall_newspaper_box_wreck/tall_newspaper_box_wreck_polygons.inc"
};

static Sint32 tall_newspaper_box_wreck_vertices[219] = {
#include "shared/street_fixtures/tall_newspaper_box_wreck/tall_newspaper_box_wreck_vertices.inc"
};

static NJS_CNK_MODEL tall_newspaper_box_wreck_mesh = {
#include "shared/street_fixtures/tall_newspaper_box_wreck/tall_newspaper_box_wreck_mesh.inc"
};

static NJS_CNK_OBJECT tall_newspaper_box_wreck_object = {
#include "shared/street_fixtures/tall_newspaper_box_wreck/tall_newspaper_box_wreck_object.inc"
};

#endif
