/* Tall newspaper box piece in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_TALL_NEWSPAPER_BOX_PIECE_H
#define SHARED_STREET_FIXTURES_TALL_NEWSPAPER_BOX_PIECE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME tall_newspaper_box_piece_textures[1] = {
#include "shared/street_fixtures/tall_newspaper_box_piece/tall_newspaper_box_piece_textures.inc"
};

static NJS_TEXLIST tall_newspaper_box_piece_texlist = {
#include "shared/street_fixtures/tall_newspaper_box_piece/tall_newspaper_box_piece_texlist.inc"
};

static Sint16 tall_newspaper_box_piece_polygons[31] = {
#include "shared/street_fixtures/tall_newspaper_box_piece/tall_newspaper_box_piece_polygons.inc"
};

static Sint32 tall_newspaper_box_piece_vertices[27] = {
#include "shared/street_fixtures/tall_newspaper_box_piece/tall_newspaper_box_piece_vertices.inc"
};

static NJS_CNK_MODEL tall_newspaper_box_piece_model = {
#include "shared/street_fixtures/tall_newspaper_box_piece/tall_newspaper_box_piece_model.inc"
};

static NJS_CNK_OBJECT tall_newspaper_box_piece_object = {
#include "shared/street_fixtures/tall_newspaper_box_piece/tall_newspaper_box_piece_object.inc"
};

#endif
