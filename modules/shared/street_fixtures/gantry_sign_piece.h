/* Gantry sign piece in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_GANTRY_SIGN_PIECE_H
#define SHARED_STREET_FIXTURES_GANTRY_SIGN_PIECE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gantry_sign_piece_textures[1] = {
#include "shared/street_fixtures/gantry_sign_piece/gantry_sign_piece_textures.inc"
};

static NJS_TEXLIST gantry_sign_piece_texlist = {
#include "shared/street_fixtures/gantry_sign_piece/gantry_sign_piece_texlist.inc"
};

static Sint16 gantry_sign_piece_polygons[25] = {
#include "shared/street_fixtures/gantry_sign_piece/gantry_sign_piece_polygons.inc"
};

static Sint32 gantry_sign_piece_vertices[19] = {
#include "shared/street_fixtures/gantry_sign_piece/gantry_sign_piece_vertices.inc"
};

static NJS_CNK_MODEL gantry_sign_piece_model = {
#include "shared/street_fixtures/gantry_sign_piece/gantry_sign_piece_model.inc"
};

static NJS_CNK_OBJECT gantry_sign_piece_object = {
#include "shared/street_fixtures/gantry_sign_piece/gantry_sign_piece_object.inc"
};

#endif
