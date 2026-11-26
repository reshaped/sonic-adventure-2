/* Tall newspaper box in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_TALL_NEWSPAPER_BOX_H
#define SHARED_STREET_FIXTURES_TALL_NEWSPAPER_BOX_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME tall_newspaper_box_textures[1] = {
#include "shared/street_fixtures/tall_newspaper_box/tall_newspaper_box_textures.inc"
};

static NJS_TEXLIST tall_newspaper_box_texlist = {
#include "shared/street_fixtures/tall_newspaper_box/tall_newspaper_box_texlist.inc"
};

static Sint16 tall_newspaper_box_polygons[207] = {
#include "shared/street_fixtures/tall_newspaper_box/tall_newspaper_box_polygons.inc"
};

static Sint32 tall_newspaper_box_vertices[243] = {
#include "shared/street_fixtures/tall_newspaper_box/tall_newspaper_box_vertices.inc"
};

static NJS_CNK_MODEL tall_newspaper_box_mesh = {
#include "shared/street_fixtures/tall_newspaper_box/tall_newspaper_box_mesh.inc"
};

static NJS_CNK_OBJECT tall_newspaper_box_object = {
#include "shared/street_fixtures/tall_newspaper_box/tall_newspaper_box_object.inc"
};

#endif
